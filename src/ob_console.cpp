#include "ob_console.hpp"

using namespace ob;

Console::Console(const gfx::Font &fnt, float fs,
    const gfx::SurfaceScreen &screen) :
  ui::Console(fnt, fs, screen)
{
  m_color_background = gfx::Color(1.0f, 1.0f, 1.0f, 1.0f);
  m_color_border = gfx::Color(0.05f, 0.00f, 0.00f, 0.94f);
  m_color_input_top = gfx::Color(0.00f, 0.00f, 0.05f, 0.78f);
  m_color_input_bottom = gfx::Color(0.00f, 0.00f, 0.05f, 0.86f);
  m_color_text_top = gfx::Color(0.04f, 0.00f, 0.00f, 0.86f);
  m_color_text_bottom = gfx::Color(0.04f, 0.00f, 0.00f, 0.94f);
}

void Console::render(gfx::SurfaceScreen &screen)
{
  this->ui::Console::render(screen);
}

