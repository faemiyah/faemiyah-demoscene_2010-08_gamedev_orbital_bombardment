#include "gfx/texture.hpp"

#include "thr/dispatch.hpp"

using namespace gfx;

static void texture_unreserve(GLuint *op)
{
  if(0 != *op)
  {
    glDeleteTextures(1, op);
    *op = 0;
  }
}

void gfx::texture_unreserve_dispatch(GLuint *op)
{
  thr::wait_privileged(&texture_unreserve, op);
}

