#include "gfx/generic.hpp"

#include <sstream>

using namespace gfx;

/** Current depth write state. Needed to be accessible to multiple functions. */
static Mode current_depth_write_state = gfx::INVALID;

/** \brief Turn blending on/off.
 *
 * \param state New state. */
static inline void state_blend(bool state)
{
  static Mode current_state = INVALID;

  if(state)
  {
    if(current_state != ENABLED)
    {
      glEnable(GL_BLEND);
      current_state = ENABLED;
    }
  }
  else
  {
    if(current_state != NONE)
    {
      glDisable(GL_BLEND);
      current_state = NONE;
    }
  }
}

/** \brief Turn culling on/off.
 *
 * \param state New state. */
static inline void state_cull(bool state)
{
  static Mode current_state = INVALID;

  if(state)
  {
    if(current_state != ENABLED)
    {
      glEnable(GL_CULL_FACE);
      current_state = ENABLED;
    }
  }
  else
  {
    if(current_state != NONE)
    {
      glDisable(GL_CULL_FACE);
      current_state = NONE;
    }
  }
}

/** \brief Turn depth test on/off.
 *
 * \param state New state.
 */
static inline void state_depth_test(bool state)
{
  static Mode current_state = INVALID;

  if(state)
  {
    if(current_state != ENABLED)
    {
      glEnable(GL_DEPTH_TEST);
      current_state = ENABLED;
    }
  }
  else
  {
    if(current_state != NONE)
    {
      glDisable(GL_DEPTH_TEST);
      current_state = NONE;
    }
  }
}

/** \brief Turn depth write on/off.
 *
 * \param state New state.
 */
static inline void state_depth_write(bool state)
{
  if(state)
  {
    if(current_depth_write_state != ENABLED)
    {
      glDepthMask(GL_TRUE);
      current_depth_write_state = ENABLED;
    }
  }
  else
  {
    if(current_depth_write_state != NONE)
    {
      glDepthMask(GL_FALSE);
      current_depth_write_state = NONE;
    }
  }
}

/** \brief Turn scissor on/off.
 *
 * \param state New state. */
static inline void state_scissor(bool state)
{
  static Mode current_state = INVALID;

  if(state)
  {
    if(current_state != ENABLED)
    {
      glEnable(GL_SCISSOR_TEST);
      current_state = ENABLED;
    }
  }
  else
  {
    if(current_state != NONE)
    {
      glDisable(GL_SCISSOR_TEST);
      current_state = NONE;
    }
  }
}

void gfx::check_opengl_errors()
{
  GLenum glerr = glGetError();
  if(glerr != GL_NO_ERROR)
  {
    std::stringstream err;
    err << "OpenGL error: " <<
      reinterpret_cast<const char*>(gluErrorString(glerr));
    BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
  }
}

void gfx::clear_framebuffer(GLbitfield op)
{
  if((op & GL_DEPTH_BUFFER_BIT) && (current_depth_write_state != ENABLED))
  {
    state_depth_write(true);
    glClear(op);
    state_depth_write(false);
  }
  else
  {
    glClear(op);
  }
}

void gfx::mode_blend(Mode mode)
{
  static Mode current_mode = INVALID;

  switch(mode)
  {
    case NONE:
      state_blend(false);
      break;

    case PREMULTIPLIED:
      state_blend(true);
      if(current_mode != mode)
      {
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        current_mode = mode;
      }
      break;

    case ALPHA:
      state_blend(true);
      if(current_mode != mode)
      {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        current_mode = mode;
      }
      break;

    case ADDITIVE:
      state_blend(true);
      if(current_mode != mode)
      {
        glBlendFunc(GL_ONE, GL_ONE);
        current_mode = mode;
      }
      break;

    default:
      {
        std::stringstream err;
        err << "invalid blend mode: " << static_cast<int>(mode);
        BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
      }
      break;
  }
}

void gfx::mode_cull(Mode mode)
{
  static Mode current_mode = INVALID;

  switch(mode)
  {
    case NONE:
      state_cull(false);
      break;

    case BACK:
      state_cull(true);
      if(current_mode != mode)
      {
        glCullFace(GL_BACK);
        current_mode = mode;
      }
      break;

    case ALPHA:
      state_cull(true);
      if(current_mode != mode)
      {
        glCullFace(GL_FRONT);
        current_mode = mode;
      }
      break;

    case BACK_AND_FRONT:
      state_cull(true);
      if(current_mode != mode)
      {
        glCullFace(GL_FRONT_AND_BACK);
        current_mode = mode;
      }
      break;

    default:
      {
        std::stringstream err;
        err << "invalid cull mode: " << static_cast<int>(mode);
        BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
      }
      break;
  }
}

void gfx::mode_depth(Mode mode)
{
  static Mode current_mode = INVALID;

  switch(mode)
  {
    case NONE:
      state_depth_test(false);
      state_depth_write(false);
      break;

    case TEST_LEQUAL:
      state_depth_test(true);
      state_depth_write(false);
      if(current_mode != TEST_LEQUAL)
      {
        glDepthFunc(GL_LEQUAL);
        current_mode = TEST_LEQUAL;
      }
      break;
      
    case TEST_LEQUAL_AND_WRITE:
      state_depth_test(true);
      state_depth_write(true);
      if(current_mode != TEST_LEQUAL)
      {
        glDepthFunc(GL_LEQUAL);
        current_mode = TEST_LEQUAL;
      }
      break;
      
    case WRITE:
      state_depth_test(true);
      state_depth_write(false);
      if(current_mode != mode)
      {
        glDepthFunc(GL_ALWAYS);
        current_mode = mode;
      }
      break;

    default:
      {
        std::stringstream err;
        err << "invalid depth mode: " << static_cast<int>(mode);
        BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
      }
  }
}

void gfx::mode_scissor(bool state)
{
  state_scissor(state);
}

void gfx::mode_scissor(int px, int py, unsigned pwidth, unsigned pheight)
{
  state_scissor(true);

  glScissor(px, py, static_cast<GLsizei>(pwidth), static_cast<GLsizei>(pheight));
}
