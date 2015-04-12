#include "gfx/entity_camera.hpp"

using namespace gfx;

EntityCamera::EntityCamera(const math::vec3d &pos, const math::vec3d &rot) :
  Entity(pos, rot)
{ }

const math::mat4f& EntityCamera::loadZXY()
{
  m_wm.loadRotZXY(math::vec3f(m_rot), math::vec3f(m_pos));
  m_wm.convertToView();
  return m_wm;
}

