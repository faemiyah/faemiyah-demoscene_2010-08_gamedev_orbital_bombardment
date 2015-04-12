#ifndef GFX_TEXTURE_HPP
#define GFX_TEXTURE_HPP

#include "gfx/uniform.hpp"

namespace gfx
{
  /** Texture unreserve dispatcher.
   *
   * \param op Texture to unreserve.
   */
  extern void texture_unreserve_dispatch(GLuint *op);

  /** \brief Base texture class.
   */
  template <GLenum T> class Texture
  {
    protected:
      /** Texture identifier. */
      GLuint m_id;

    public:
      /** \brief Accessor.
       *
       * \return Id of this.
       */
      unsigned id() const
      {
        return m_id;
      }

    public:
      /** \brief Bind this texture into use. */
      void bind() const
      {
        glBindTexture(T, m_id);
      }

      /** \brief Bind this texture to a specific texture unit.
       *
       * \param op Texture unit index.
       */
      void bind(unsigned op) const
      {
        glActiveTexture(GL_TEXTURE0 + op);
        glBindTexture(T, m_id);
      }

      /** \brief Assign a texture unit and bind this texture to it.
       *
       * \param uform Uniform to assign.
       * \param op Texture unit to use.
       */
      void bind(const Uniform &uform, unsigned op) const
      {
        uform.update(static_cast<int>(op));
        this->bind(op);
      }

      /** Reserve all resources allocated by this. */
      void reserve()
      {
        glGenTextures(1, &m_id);
      }

      /** Unreserve all resources allocated by this. */
      void unreserve()
      {
        texture_unreserve_dispatch(&m_id);
      }

    public:
      /** \brief Empty constructor. */
      Texture() : m_id(0) { }

      /** Destructor. */
      ~Texture()
      {
        this->unreserve();
      }

    public:
      /** \brief Transform a pixel bit depth into pixel format.
       *
       * There are only few allowed pixel formats, so this is okay.
       *
       * \param bpp Bit depth.
       * \return Pixel format.
       */
      static GLenum bpp_to_pformat(unsigned bpp)
      {
        switch(bpp)
        {
          case 8:
            return GL_LUMINANCE;

          case 16:
            return GL_LUMINANCE_ALPHA;

          case 24:
            return GL_RGB;

          case 32:
            return GL_RGBA;

          default:
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("invalid bpp to format conversion")));
        }
      }

      /** Assign texture parameters. 
       *
       * \param parameter Parameter to assign.
       * \param value New value.
       */
      static void texParameter(GLenum parameter, GLfloat value)
      {
        glTexParameterf(T, parameter, value);
      }

      /** Assign texture parameters.
       *
       * \param parameter Parameter to assign.
       * \param value New value.
       */
      static void texParameter(GLenum parameter, GLint value)
      {
        glTexParameteri(T, parameter, value);
      }

      /** Assign texture parameters.
       *
       * Specialization added for ambiguous cases.
       *
       * \param parameter Parameter to assign.
       * \param value New value.
       */
      static void texParameter(GLenum parameter, GLenum value)
      {
        glTexParameteri(T, parameter, static_cast<GLint>(value));
      }

      /** Unbind this texture. */
      static void unbind()
      {
        glBindTexture(T, 0);
      }
  };
}

#endif
