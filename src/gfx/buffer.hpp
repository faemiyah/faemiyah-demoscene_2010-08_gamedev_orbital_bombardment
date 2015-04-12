#ifndef GFX_BUFFER_HPP
#define GFX_BUFFER_HPP

#include "gfx/array.hpp"
#include "gfx/color.hpp"
#include "gfx/triangle.hpp"
#include "math/vec.hpp"

#include <vector>

namespace gfx
{
  class Lod;

  /** Buffer unreserve dispatcher.
   *
   * \param op Buffer to unreserve.
   */
  extern void buffer_unreserve_dispatch(GLuint *op);

  /** \brief Buffer base class.
   *
   * This class is just used for containing the OpenGL vertex array buffer.
   *
   * There are two main inheritors to Buffer base class, element buffer and
   * different interleaved buffers.
   */
  template<GLenum B> class Buffer
  {
    protected:
      GLuint m_buffer;

    public:
      /** \brief Constructor. */
      Buffer() : m_buffer(0)  { }

      /** \brief Destructor. */
      ~Buffer()
      {
        this->unreserve();
      }

    protected:
      /** \brief Reserve the OpenGL buffer.
       *
       * Intentionally protected, the buffer should not be reserved without
       * something to explicitly put in it.
       *
       * Should only be called from main GL thread.
       */
      void reserve()
      {
        if(0 == m_buffer)
        {
          glGenBuffers(1, &m_buffer);
        }
      }

    public:
      /** \brief Bind this buffer.
       *
       * The user should not be required to call this method.
       */
      void bind() const
      {
        glBindBuffer(B, m_buffer);
      }

      /** \brief Get the OpenGL buffer of this.
       *
       * \return Buffer index.
       */
      GLuint getBuffer() const
      {
        return m_buffer;
      }

      /** \brief Release the OpenGL buffer.
       */
      void unreserve()
      {
        buffer_unreserve_dispatch(&m_buffer);
      }

    public:
      /** Get the buffer type.
       *
       * \return Buffer type enum.
       */
      static GLenum get_buffer_type()
      {
        return B;
      }
  };

  /** \brief Element buffer.
   *
   * Used to feed element arrays into the GPU.
   *
   * After creating an element buffer, upload it to the GPU by readng the contents of an array. You may later
   * update subsegments of the buffer, but unless you do it from the same array, the results might be
   * unexpected.
   *
   * The buffer may at any time be updated to match the contents of any given element array.
   */
  template <typename T> class BufferE : public Buffer<GL_ELEMENT_ARRAY_BUFFER>
  {
    protected:
      /** Size of the contents saved in the last full update. */
      unsigned m_array_size;

    public:
      /** \brief Get size of this element buffer.
       *
       * \return Size in elements (not in primitives).
       */
      unsigned getSize() const
      {
        return m_array_size;
      }

    public:
      /** \brief Constructor. */
      BufferE() { }

      /** \brief Constructor.
       *
       * \param array Array to immediately feed.
       */
      BufferE(const ArrayE<T> &array)
      {
        this->upload(array);
      }

      /** \brief Constructor.
       *
       * \param array Vector of triangles to immediately feed.
       */
      BufferE(const std::vector<Triangle> &array)
      {
        this->upload(array);
      }

      /** \brief Destructor. */
      ~BufferE() { }

    public:
      /** \brief Collect elements.
       *
       * Recursively collects all elements from given LOD. Then write back the
       * element lists in them.
       */
      void upload(Lod &lod);

      /** \brief Upload data to the GPU.
       *
       * Updates all data within an array.
       *
       * In most cases, this should be the only function that is necessary to be
       * called for an element buffer after creating it.
       *
       * \param first First element.
       * \param last Last element.
       */
      void upload(const ArrayE<T> &array)
      {
        m_array_size = array.getSize();
        this->reserve();
        this->bind();
        glBufferData(get_buffer_type(), m_array_size * sizeof(T), array.getData(), GL_STATIC_DRAW);
      }

      /** \brief Upload data to the GPU.
       *
       * Updates all data within an array.
       *
       * In most cases, this should be the only function that is necessary to be
       * called for an element buffer after creating it.
       *
       * \param array Array of triangles.
       */
      void upload(const std::vector<Triangle> &triangle_array)
      {
        ArrayE<T> array(static_cast<unsigned>(triangle_array.size()) * 3);
        unsigned ii = 0;

        BOOST_FOREACH(const Triangle &vv, triangle_array)
        {
          array[ii + 0] = vv.a();
          array[ii + 1] = vv.b();
          array[ii + 2] = vv.c();
          ii += 3;
        }

        this->reserve();
        this->bind();
        glBufferData(get_buffer_type(), array.getSize() * sizeof(T), array.getData(), GL_STATIC_DRAW);
      }

      /** \brief Upload a subsegment of data to the GPU.
       *
       * Numbers are given as full elements, so updating cannot be done for element boundaries.
       *
       * In most situations, the user should have no need to call this function, as it specifically updates a
       * subsegment of the data within a buffer. If the full data within an array has not been updated yet,
       * this may have weird consequences.
       *
       * \param first First element.
       * \param last Last element.
       */
      void upload(const ArrayE<T> &array, unsigned first, unsigned last)
      {
        this->reserve();
        this->bind();
        glBufferSubData(get_buffer_type(), first * sizeof(T), (last - first) * sizeof(T), array.getData() + first);
      }

    public:
      /** \brief Draw according to the elements in this.
       *
       * \param type What kind of elements are we drawing?
       * \param cnt How many elements to feed.
       */
      void draw(unsigned type, unsigned cnt) const
      {
        this->bind();
        glDrawElements(type, cnt, get_opengl_type<T>(), NULL);
      }

      /** \brief Draw according to the elements in this.
       *
       * Like draw(unsigned, unsigned), but feeds all elements.
       *
       * \param type What kind of elements are we drawing?
       */
      void draw(unsigned type) const
      {
        this->draw(type, m_array_size);
      }
  };

  /** \brief Convenience typedef. */
  typedef BufferE<GLuint> BufferElem;

  /** \brief Child version of element buffer.
   *
   * Instead of an element buffer, this is a sub-buffer for elements. It does
   * not contain a buffer instance per se, but is instead initialized into an
   * index pointer and a size.
   */
  template <typename T> class BufferSubE
  {
    protected:
      /** Index (in bytes) in the host buffer. */
      unsigned m_idx;

      /** Number of elements for drawing. */
      unsigned m_count;

    public:
      /** \brief Accessor.
       *
       * \return Number of elements in this buffer.
       */
      unsigned getCount() const
      {
        return m_count;
      }

      /** \brief Accessor.
       *
       * \return Drawing index.
       */
      unsigned getIdx() const
      {
        return m_idx;
      }

    public:
      /** \brief Empty constructor. */
      BufferSubE() { }

      /** \brief Constructor.
       *
       * \param idx Buffer index.
       * \param count Number of elements.
       * */
      BufferSubE(unsigned idx, unsigned count) :
        m_idx(idx),
        m_count(count) { }

      /** \brief Destructor. */
      ~BufferSubE() { }

    public:
      /** \brief Upload the corresponding subsegment into the GPU.
       *
       * This should probably not be called usually. It is provided for academic interest.
       *
       * \param data Pointer to the data from which to update.
       */
      void upload(void *data)
      {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, m_idx * sizeof(T), m_count * sizeof(T), data);
      }

    public:
      /** \brief Assign new values for this buffer.
       *
       * \param idx Buffer index.
       * \param count Number of elements.
       */
      void assign(unsigned idx, unsigned count)
      {
        m_idx = idx;
        m_count = count;
      }

      /** \brief Draw according to the elements in this.
       *
       * \param type What kind of elements are we drawing?
       * \param cnt How many elements to feed.
       */
      void draw(unsigned type, unsigned cnt) const
      {
        glDrawElements(type, static_cast<GLsizei>(cnt), get_opengl_type<T>(),
            static_cast<uint8_t*>(NULL) + m_idx);
      }

      /** \brief Draw according to the elements in this.
       *
       * Like draw(unsigned, unsigned), but feeds all elements.
       *
       * \param type What kind of elements are we drawing?
       */
      void draw(unsigned type) const
      {
        this->draw(type, m_count);
      }
  };

  /** \brief Convenience typedef. */
  typedef BufferSubE<GLuint> BufferSubElem;

  /** \brief An interleaved buffer for typical static data.
   */
  class BufferInterleavedRWTCNV : public Buffer<GL_ARRAY_BUFFER>
  {
    public:
      /** Block size of one element in the GPU buffer. */
      static const unsigned BLOCKSIZE = 4 + 16 + 8 + 4 + 12 + 12;

    public:
      /** \brief Constructor. */
      BufferInterleavedRWTCNV() { }

      /** \brief Destructor. */
      ~BufferInterleavedRWTCNV() { }

    public:
      /** \brief Bind buffers for drawing.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param rr Attribute for 4-component reference data.
       * \param ww Attribute for 4-component weight data.
       * \param tt Attribute for 2-component texture coordinate data.
       * \param cc Attribute for 4-component color data.
       * \param nn Attribute for 3-component normal data.
       * \param vv Attribute for 3-component vertex data.
       */
      void bind(const Attribute &rr, const Attribute &ww, const Attribute &tt, const Attribute &cc,
          const Attribute &nn, const Attribute &vv) const;

      /** \brief Upload data to the GPU.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param rr Attribute for 4-component reference data.
       * \param ww Attribute for 4-component weight data.
       * \param tt 2-component texture coordinate data.
       * \param cc 4-component color data.
       * \param nn 3-component normal data.
       * \param vv 3-component vertex data.
       */
      void upload(const std::vector<math::vec4u> &rr, const std::vector<math::vec4f> &ww,
          const std::vector<math::vec2f> &tt, const std::vector<Color> &cc, const std::vector<math::vec3f> &nn,
          const std::vector<math::vec3f> &vv);
  };

  /** \brief An interleaved buffer for typical static data.
   */
  class BufferInterleavedTCNV : public Buffer<GL_ARRAY_BUFFER>
  {
    public:
      /** Block size of one element in the GPU buffer. */
      static const unsigned BLOCKSIZE = 8 + 4 + 12 + 12;

    public:
      /** \brief Constructor. */
      BufferInterleavedTCNV() { }

      /** \brief Destructor. */
      ~BufferInterleavedTCNV() { }

    public:
      /** \brief Bind buffers for drawing.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param tt Attribute for 2-component texture coordinate data.
       * \param cc Attribute for 4-component color data.
       * \param nn Attribute for 3-component normal data.
       * \param vv Attribute for 3-component vertex data.
       */
      void bind(const Attribute &tt, const Attribute &cc, const Attribute &nn, const Attribute &vv) const;

      /** \brief Upload data to the GPU.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param tt 2-component texture coordinate data.
       * \param cc 4-component color data.
       * \param nn 3-component normal data.
       * \param vv 3-component vertex data.
       */
      void upload(const std::vector<math::vec2f> &tt, const std::vector<Color> &cc,
          const std::vector<math::vec3f> &nn, const std::vector<math::vec3f> &vv);
  };

  /** \brief An interleaved buffer for typical static data.
   */
  class BufferInterleavedCNV : public Buffer<GL_ARRAY_BUFFER>
  {
    public:
      /** Block size of one element in the GPU buffer. */
      static const unsigned BLOCKSIZE = 4 + 12 + 12;

    public:
      /** \brief Constructor. */
      BufferInterleavedCNV() { }

      /** \brief Destructor. */
      ~BufferInterleavedCNV() { }

    public:
      /** \brief Bind buffers for drawing.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param cc Attribute for 4-component color data.
       * \param nn Attribute for 3-component normal data.
       * \param vv Attribute for 3-component vertex data.
       */
      void bind(const Attribute &cc, const Attribute &nn, const Attribute &vv) const;

      /** \brief Upload data to the GPU.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param cc 4-component color data.
       * \param nn 3-component normal data.
       * \param vv 3-component vertex data.
       */
      void upload(const std::vector<Color> &cc, const std::vector<math::vec3f> &nn,
          const std::vector<math::vec3f> &vv);
  };

  /** \brief An interleaved buffer for overlay graphics (or the like).
   */
  class BufferInterleavedTCV : public Buffer<GL_ARRAY_BUFFER>
  {
    public:
      /** Block size of one element in the GPU buffer. */
      static const unsigned BLOCKSIZE = 8 + 4 + 12;

    public:
      /** \brief Constructor. */
      BufferInterleavedTCV() { }

      /** \brief Destructor. */
      ~BufferInterleavedTCV() { }

    public:
      /** \brief Bind buffers for drawing.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param tt Attribute for 2-component texture coordinate data.
       * \param cc Attribute for 4-component color data.
       * \param vv Attribute for 3-component vertex data.
       */
      void bind(const Attribute &tt, const Attribute &cc, const Attribute &vv) const;

      /** \brief Upload data to GPU.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param tt 2-component texture coordinate data.
       * \param cc 4-component color data.
       * \param vv 3-component vertex data.
       */
      void upload(const std::vector<math::vec2f> &tt, const std::vector<Color> &cc,
          const std::vector<math::vec3f> &vv);
  };

  /** \brief An interleaved buffer for static data.
   *
   * This assumes most of the data is contained in other sources, for example
   * textures and normalmaps.
   *
   * The texture coordinates are assumed to be non-repeating and clamped.
   */
  class BufferInterleavedTV : public Buffer<GL_ARRAY_BUFFER>
  {
    public:
      /** Block size of one element in the GPU buffer. */
      static const unsigned BLOCKSIZE = 8 + 12;

    public:
      /** \brief Constructor. */
      BufferInterleavedTV() { }

      /** \brief Destructor. */
      ~BufferInterleavedTV() { }

    public:
      /** \brief Bind buffers for drawing.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param tt Attribute for 2-component texture coordinate data.
       * \param vv Attribute for 3-component vertex data.
       */
      void bind(const Attribute &tt, const Attribute &vv) const;

      /** \brief Upload data to the GPU.
       *
       * Passing order borrows from glInterleavedArrays.
       *
       * \param tt 2-component texture coordinate data.
       * \param vv 3-component vertex data.
       */
      void upload(const std::vector<math::vec2f> &tt, const std::vector<math::vec3f> &vv);
  };

  /** Enable buffer(s), disable all other buffers.
   *
   * \param attr1 First attribute.
   */
  void enable_buffers(const Attribute &attr1);

  /** Enable buffer(s), disable all other buffers.
   *
   * \param attr1 First attribute.
   * \param attr2 Second attribute.
   */
  void enable_buffers(const Attribute &attr1, const Attribute &attr2);

  /** Enable buffer(s), disable all other buffers.
   *
   * \param attr1 First attribute.
   * \param attr2 Second attribute.
   * \param attr3 Third attribute.
   */
  void enable_buffers(const Attribute &attr1, const Attribute &attr2,
      const Attribute &attr3);

  /** Enable buffer(s), disable all other buffers.
   *
   * \param attr1 First attribute.
   * \param attr2 Second attribute.
   * \param attr3 Third attribute.
   * \param attr4 Fourth attribute.
   */
  void enable_buffers(const Attribute &attr1, const Attribute &attr2,
      const Attribute &attr3, const Attribute &attr4);

  /** Enable buffer(s), disable all other buffers.
   *
   * \param attr1 First attribute.
   * \param attr2 Second attribute.
   * \param attr3 Third attribute.
   * \param attr4 Fourth attribute.
   * \param attr5 Fifth attribute.
   */
  void enable_buffers(const Attribute &attr1, const Attribute &attr2,
      const Attribute &attr3, const Attribute &attr4, const Attribute &attr5);

  /** Enable buffer(s), disable all other buffers.
   *
   * \param attr1 First attribute.
   * \param attr2 Second attribute.
   * \param attr3 Third attribute.
   * \param attr4 Fourth attribute.
   * \param attr5 Fifth attribute.
   * \param attr6 Sixth attribute.
   */
  void enable_buffers(const Attribute &attr1, const Attribute &attr2,
      const Attribute &attr3, const Attribute &attr4, const Attribute &attr5,
      const Attribute &attr6);
}

#endif
