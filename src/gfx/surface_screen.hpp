#ifndef GFX_SURFACE_SCREEN_HPP
#define GFX_SURFACE_SCREEN_HPP

#include "gfx/surface.hpp"

namespace gfx
{
  /** \brief Class representing window, that can be drawn into.
   */
  class SurfaceScreen : public Surface
  {
    private:
      /** SDL surface contained. */
      SDL_Surface *m_screen;

    public:
      /** \brief Default constructor.
       *
       * \param pw Width of this surface.
       * \param ph Width of this surface.
       * \param pb Bit depth of this surface.
       * \param fs Fullscreen toggle.
       */
      SurfaceScreen(unsigned pw, unsigned ph, unsigned pb, bool fs = false);

      /** Destructor.
       */
      ~SurfaceScreen();
    
    public:
      /** \cond */
      virtual void clear(bool, bool);
      virtual void setBoundary(int, int, unsigned, unsigned);
      virtual void update();
      /** \endcond */

    public:
      /** \brief Parse a resolution input string into width, height and bit depth.
       *
       * \param op Input string.
       * \return Tuple of screen parameters.
       */
      static boost::tuple<unsigned, unsigned, unsigned> parseResolution(const std::string &op);

      /** \brief Save a screenshot.
       *
       * \param pfname Filename to save into.
       */
      void save(const boost::filesystem::path &pfname);

      /** \brief Wrapper for saving a screenshot.
       *
       * \param pfname Filename to save into.
       */
      void save(const std::string &pfname);

    private:
      /** \brief Create SDL video mode flags for this surface.
       *
       * \param fs Fullscreen toggle.
       * \return Flag integer.
       */
      static unsigned createSdlFlags(bool fs)
      {
        return SDL_OPENGL | (fs ? SDL_FULLSCREEN : 0);
      }

      /** \brief Set OpenGL attributes.
       *
       * Called prior to calling surface creation.
       *
       * \param bit_depth Bit depth to use.
       */
      static void setGlAttributes()
      {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      }
  };
}

#endif
