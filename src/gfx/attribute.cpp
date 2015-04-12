#include "gfx/attribute.hpp"

using namespace gfx;

/** Bookkeeping for mode changes. */
static Mode vertex_attribute_array[] =
{
  INVALID,
  INVALID,
  INVALID,
  INVALID,
  INVALID,
  INVALID
};

/** Vertex attribute check. */
#define VERTEX_ATTRIBUTE_COUNT (sizeof(vertex_attribute_array) / sizeof(Mode))

void gfx::vertex_attribute_array_disable(GLuint op)
{
  Mode *array_state = vertex_attribute_array + op;

  BOOST_ASSERT(op < VERTEX_ATTRIBUTE_COUNT);

  if(NONE != *array_state)
  {
    glDisableVertexAttribArray(op);
    *array_state = NONE;
  }
}

void gfx::vertex_attribute_array_enable(GLuint op)
{
  Mode *array_state = vertex_attribute_array + op;

  BOOST_ASSERT(op < VERTEX_ATTRIBUTE_COUNT);

  if(ENABLED != *array_state)
  {
    glEnableVertexAttribArray(op);
    *array_state = ENABLED;
  }
}
