#include "gfx/entity_object_overlay.hpp"

#include "gfx/surface.hpp"

using namespace gfx;

math::mat4f EntityObjectOverlay::orient(const math::mat4f &pmat) const
{
  math::mat4f ret = pmat * m_wm;
  load_transform(ret);
  return ret;
}

