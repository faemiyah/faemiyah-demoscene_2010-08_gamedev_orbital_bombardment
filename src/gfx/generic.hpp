#ifndef GFX_GENERIC_HPP
#define GFX_GENERIC_HPP

#include "defaults.hpp"

#if defined(__APPLE__)
#include "GL/glew.h"
#include "OpenGL/GL.h"
#include "SDL/SDL.h"
#elif defined(WIN32)
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#else // General *nix such as FreeBSD or Linux.
#include "GL/glew.h"
#include "SDL.h"
#endif

namespace gfx
{
  /** \brief Interpolation mode.
   *
   * Determines interpolation for frames.
   */
  enum FrameInterpolation
  {
    /** Linear interpolation between frames. */
    LINEAR
  };

  /** Mode setting for both states and modes. */
  enum Mode
  {
    /** Mode off. */
    NONE,

    /** Mode enabled. */
    ENABLED,

    /** Premultiplied alpha blend (default). */
    PREMULTIPLIED,

    /** Alpha blend. */
    ALPHA,

    /** Additive blend. */
    ADDITIVE,

    /** Cull back face (default). */
    BACK,

    /** Cull front face. */
    FRONT,

    /** Cull both faces (lines/points can be drawn). */
    BACK_AND_FRONT,

    /** Test less or equal but do not write. */
    TEST_LEQUAL,

    /** Test less or equal and write. */
    TEST_LEQUAL_AND_WRITE,

    /** Write only. */
    WRITE,

    /** Invalid mode. */
    INVALID
  };

  /** \brief Text justification schema.
   *
   * Determines how the text is drawn.
   */
  enum TextJustify
  {
    /** Text is drawn from left to right starting from the lower left corner
     * of the first row. */
    LEFT,

    /** Text is drawn aligned to the lower right corner of the first row. */
    CENTER,

    /** Text is drawn as if the coordinates were at the center of the whole
     * text block, every line centered. */
    RIGHT
  };

  /** \brief Retrieve GL type.
   *
   * Static method for retrieving the OpenGL type.
   *
   * \return GLenum corresponding to the instantiated type.
   */
  template <typename T> inline GLenum get_opengl_type();
  /** \cond */
  template<> inline GLenum get_opengl_type<float>()
  {
    return GL_FLOAT;
  }
  template<> inline GLenum get_opengl_type<int>()
  {
    return GL_INT;
  }
  template<> inline GLenum get_opengl_type<GLuint>()
  {
    return GL_UNSIGNED_INT;
  }
  /** \endcond */

  /** \brief Get OpenGL errors.
   */
  extern void check_opengl_errors();

  /** \brief Perform OpenGL framebuffer clear.
   *
   * \param op Bitfield.
   */
  extern void clear_framebuffer(GLbitfield op);

  /** \brief Set blend mode.
   *
   * \param mode New blend mode. */
  extern void mode_blend(Mode mode);

  /** \brief Set cull mode.
   *
   * \param mode New cull mode. */
  extern void mode_cull(Mode mode);

  /** \brief Set depth mode.
   *
   * \param mode New depth mode.
   */
  extern void mode_depth(Mode mode);

  /** Set scissor mode on/off.
   *
   * \param state New scissor state.
   */
  extern void mode_scissor(bool state);

  /** \brief Set scissor mode on with given area.
   *
   * \param px X coordinate.
   * \param py Y coordinate.
   * \param pwidth Width.
   * \param pheight Height.
   */
  extern void mode_scissor(int px, int py, unsigned pwidth, unsigned pheight);
}

#endif
