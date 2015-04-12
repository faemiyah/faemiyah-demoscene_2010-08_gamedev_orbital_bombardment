#include "gfx/entity.hpp"

#include "gfx/surface.hpp"

using namespace gfx;

Entity::Entity(const math::vec3d &pos, const math::vec3d &rot) :
  m_pos(pos),
  m_rot(rot) { }

math::vec2f Entity::project() const
{
  return this->project(Surface::get_matrix_stack());
}

math::vec2f Entity::project(const math::mat4f &stack) const
{
  math::vec4f trans(static_cast<float>(m_pos.x()),
      static_cast<float>(m_pos.y()),
      static_cast<float>(m_pos.z()),
      1.0f);
  trans = stack * trans;
  return math::vec2f(trans.x() / trans.w(), trans.y() / trans.w());
}

