#ifndef GFX_FBO_HPP
#define GFX_FBO_HPP

#include "gfx/surface.hpp"
#include "gfx/texture_2d.hpp"

namespace gfx
{
  /** \brief Framebuffer object.
   *
   * Used primarily as render-to-texture.
   */
  class SurfaceFbo : public Surface
  {
    private:
      /** Texture reference. */
      Texture2D m_tex;

      /** Id for the frame buffer object. */
      GLuint m_fbo_id;

      /** Id for the depth texture object. */
      GLuint m_depth_id;

    public:
      /** \brief Constructor. */
      SurfaceFbo(unsigned pw, unsigned ph, unsigned pb = 32, bool depth = true);

      /** \brief Destructor. */
      virtual ~SurfaceFbo();

    public:
      /** \brief Get the 2D texture.
       *
       * \return 2D texture reference.
       */
      const Texture2D& getTexture() const
      {
        return m_tex;
      }

    public:
      /** \cond */
      virtual void clear(bool, bool);
      virtual void setBoundary(int, int, unsigned, unsigned);
      virtual void update();
      /** \endcond */

    private:
      /** \brief Bind this framebuffer object. for use.
       *
       * After this call, all drawing will actually happen to the framebuffer
       * object.
       */
      void bind() const;

      /** \brief Reserve the resources in this.
       *
       * \param pdepth True to reserve a depth buffer.
       */
      void reserve(bool pdepth);

      /** \brief Unbind the buffer object.
       *
       * After calling this, all rendering will happen as normal.
       */
      void unbind() const;

      /** \brief Unreserve the resources in this.
       */
      void unreserve();
  };
}

#endif
