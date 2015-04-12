#include "ob_fade.hpp"

#include "gfx/surface_screen.hpp"

using namespace ob;

Fade::Fade() :
  m_level(0.0f),
  m_delta(0.0f) { }

  void Fade::draw(const gfx::SurfaceScreen &pscreen) const
{
  if(m_level <= 0.0)
  {
    return;
  }

  gfx::bind_shader_2d();
  gfx::load_identity();

  gfx::draw_rect(0, 0, static_cast<int>(pscreen.getWidth()), static_cast<int>(pscreen.getHeight()),
      gfx::Color(0.0f, 0.0f, 0.0f, m_level));
}

void Fade::update()
{
  m_level = math::max(0.0f, math::min(1.0f, m_level + m_delta));
}

