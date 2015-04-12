#ifndef GFX_ARRAY_HPP
#define GFX_ARRAY_HPP

#include "gfx/attribute.hpp"
#include "gfx/color.hpp"
#include "math/vec.hpp"

namespace gfx
{
  /** \brief Array base class.
   *
   * Array is a class used to contain data for feeding it to the video card.
   * The array itself does not handle the actual potential vertex buffer
   * object (VBO), instead, Buffer class is used for that.
   *
   * \param T Elementary type.
   * \param C Number of elements within.
   */
  template <typename T, unsigned C> class Array
  {
    protected:
      /** Internal array. */
      T* m_array;

      /** Size of the array (in elements). */
      unsigned m_array_size;

    protected:
      /** \brief Empty constructor.
       *
       * Array created in this manner is not usable right away. It must be
       * reserved first.
       */
      Array() :
        m_array(NULL),
        m_array_size(0) { }

      /** \brief Constructor.
       *
       * \param count Number of elements to reserve.
       * \param buftype Type of buffer, usually GL_ARRAY_BUFFER.
       */
      Array(unsigned count) :
        m_array(NULL),
        m_array_size(0)
      {
        this->reserve(count);
      }

      /** \brief Destructor. */
      ~Array()
      {
        unreserve();
      }

    public:
      /** \brief Reserve data for this array.
       *
       * If array data already exists, it will be deleted beforehand.
       *
       * The size of the array is dependant on the count of elementary units
       * per one logical element for this array.
       *
       * \param count Number of elements to reserve.
       * \return Size of the created array in bytes.
       */
      unsigned reserve(unsigned count)
      {
        this->unreserve();

        m_array_size = count;
        m_array = new T[m_array_size * C];
        return m_array_size * C * static_cast<unsigned>(sizeof(T));
      }

      /** \brief Unreserve all data. */
      void unreserve()
      {
        delete[] m_array;
        m_array = NULL;
      }

    public:
      /** \brief Access operator for elementary type within.
       *
       * \param idx Index to access.
       */
      T& operator[](unsigned idx)
      {
        return m_array[idx];
      }

      /** \brief Access operator for elementary type within.
       *
       * \param idx Index to access.
       */
      const T& operator[](unsigned idx) const
      {
        return m_array[idx];
      }

    public:
      /** \brief Get pointer to the data within this.
       *
       * \return Data pointer.
       */
      const T* getData() const
      {
        return m_array;
      }

      /** \brief Get the size of this.
       *
       * Size is returned in full elements. To get size in elementary types,
       * multiply this with get_elementary_count().
       *
       * \return Size in elements.
       */
      unsigned getSize() const
      {
        return m_array_size;
      }

      /** \brief Get the size of this.
       *
       * \return Size in bytes.
       */
      unsigned getSizeBytes() const
      {
        return m_array_size * sizeof(T) * C;
      }

    public:
      /** \brief Get the number of elementary types per element.
       *
       * \return Elementary type count as unsigned.
       */
      static unsigned getElementaryCount()
      {
        return C;
      }
  };

  /** \brief Element buffer.
   *
   * Used for drawing instead of supplying the data.
   */
  template <typename T> class ArrayE : public Array<T, 1>
  {
    public:
      /** \brief Empty constructor. */
      ArrayE() { }

      /** \brief Constructor.
       *
       * \param count Number of elements to reserve.
       */
      ArrayE(unsigned count) :
        Array<T, 1>(count) { }

    public:
      /** \brief Draw according to the elements in this.
       *
       * \param type What kind of elements are we drawing?
       * \param cnt How many elements to feed.
       */
      void draw(unsigned type, unsigned cnt) const
      {
        glDrawElements(type, cnt, get_opengl_type<T>(),
            Array<T, 1>::m_array);
      }

      /** \brief Draw according to the elements in this.
       *
       * Like draw(unsigned, unsigned), but feeds all elements.
       *
       * \param type What kind of elements are we drawing?
       */
      void draw(unsigned type) const
      {
        this->draw(type, Array<T, 1>::m_array_size);
      }
  };

  /** Convenience typedef. */
  typedef ArrayE<uint32_t> ArrayElem;

  /** \brief Vertex attribute buffer.
   *
   * This class is used to instantiate the distinct vertex attribute arrays.
   */
  template <typename T, unsigned C> class ArrayA : public Array<T, C>
  {
    public:
      /** \brief Empty constructor. */
      ArrayA() :
        Array<T, C>() { }

      /** \brief Constructor.
       *
       * \param count Number of elements to reserve.
       */
      ArrayA(unsigned count) :
        Array<T, C>(count) { }

    public:
      /** \brief Feed this buffer.
       *
       * \param idx Vertex attribute index to feed as.
       */
      void feed(GLuint idx) const
      {
        glVertexAttribPointer(idx, C, get_opengl_type<T>(), GL_FALSE, 0,
            Array<T, C>::m_array);
      }

      /** \brief Feed this buffer.
       *
       * Extract the index from the attribute abstraction.
       *
       * \param op Attribute.
       */
      void feed(const Attribute &op)
      {
        this->feed(op.id());
      }
  };

  /** Convenience typedef. */
  typedef ArrayA<float, 2> ArrayA2f;

  /** Convenience typedef. */
  typedef ArrayA<float, 3> ArrayA3f;

  /** Convenience typedef. */
  typedef ArrayA<float, 4> ArrayA4f;

  /** Convenience typedef. */
  typedef ArrayA<int, 4> ArrayA4i;
}

#endif
