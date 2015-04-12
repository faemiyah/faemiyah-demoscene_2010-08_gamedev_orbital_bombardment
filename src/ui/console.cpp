#include "ui/console.hpp"

using namespace ui;

static const float AREA_MARGIN = 0.6f;
static const float AREA_MARGIN_BOTTOM = AREA_MARGIN * 1.125f;
static const float AREA_BORDER = 0.125f;

Console::Console(const gfx::Font &fnt, float fs, const math::rect2f &prestrict, const char *pfname) :
  m_font(fnt),
  m_font_size(fs),
  m_area(prestrict),
  m_background(NULL)
{
  this->setDefaultColors();
  this->updateArea();

  if(NULL != pfname)
  {
    m_background = gfx::Texture2D::create(pfname).get().get();
  }
}

Console::Console(const gfx::Font &fnt, float fs, const gfx::SurfaceScreen &screen, const char *pfname) :
  m_font(fnt),
  m_font_size(fs),
  m_area(screen.getArea()),
  m_background(NULL)
{
  this->setDefaultColors();
  this->updateArea();

  if(NULL != pfname)
  {
    m_background = gfx::Texture2D::create(pfname).get().get();
  }
}

void Console::addRow(const std::wstring &op)
{
  if(0 >= op.length())
  {
    return;
  }

  TextRect row(math::vec2f(m_area_text.w(), m_area_text.h()));
  std::wstring remaining = row.setContent(op, m_font, m_font_size);
  if(0 < remaining.length())
  {
    // TODO: ?
  }

  boost::mutex::scoped_lock scope(m_mut);

  m_lines.push_front(row);

  float total_h = 0.0f;

  for(std::list<TextRect>::iterator ii = m_lines.begin(), ee = m_lines.end();
      (ii != ee);)
  {
    if(total_h > m_area_text.h())
    {
      ii = m_lines.erase(ii);
      continue;
    }
    total_h += ii->getDimensions().y() + m_font_size * AREA_BORDER;
    ++ii;
  }
}

void Console::execute()
{
  this->addRow(m_input.getLine());
  m_input.reset();
}

void Console::render(gfx::SurfaceScreen &screen)
{
  screen.select2D();

  if(NULL != m_background)
  {
    gfx::bind_shader_2d_texture();
    gfx::load_identity();
    gfx::draw_rect_textured_fill(m_area, m_color_background, *m_background);
  }

  m_input.draw(m_color_input_bottom, m_color_input_top, m_font_size, m_font);

  gfx::draw_fill(0, m_color_text_bottom);
  gfx::draw_fill(1, m_color_text_bottom);
  gfx::draw_fill(2, m_color_text_top);
  gfx::draw_fill(3, m_color_text_top);
  this->renderText(screen);
}

void Console::renderText(const gfx::SurfaceScreen &screen)
{
  math::rect2i parea = screen.toPixelArea(m_area);
  int bw = math::lround(m_font_size * AREA_BORDER *
      static_cast<float>(math::max(parea.w(), parea.h())));

  gfx::bind_shader_2d_font();
  gfx::load_identity();
  {
    boost::mutex::scoped_lock scope(m_mut);

    float cy = m_area_text.y1();
    BOOST_FOREACH(const TextRect &vv, m_lines)
    {
      math::rect2f carea(m_area_text.x1(), cy, vv.getDimensions());
      vv.renderText(carea, m_font, m_font_size);
      cy += vv.getDimensions().y() + m_font_size * AREA_BORDER;
    }
  }

  gfx::bind_shader_2d();
  gfx::load_identity();

  // OpenGL rounding system requires +1 here.
  gfx::draw_rect_contour(parea.x1(), parea.y1(),
      parea.w() + 1, parea.h() + 1, bw, m_color_border);
}

void Console::setBackground(const gfx::Texture2D *tex)
{
  m_background = tex;
}

void Console::setDefaultColors()
{
  m_color_background = gfx::Color(0.3f, 0.3f, 0.3f, 1.0f);
  m_color_border = gfx::Color(0.4f, 0.8f, 0.5f, 0.3f);
  m_color_input_bottom = gfx::Color(0.8f, 0.9f, 0.9f, 0.8f);
  m_color_input_top = gfx::Color(0.8f, 0.9f, 0.9f, 0.8f);
  m_color_text_bottom = gfx::Color(0.9f, 0.8f, 0.8f, 0.8f);
  m_color_text_top = gfx::Color(0.9f, 0.8f, 0.8f, 0.8f);
}

void Console::updateArea()
{
  math::rect2f area_input(m_area.x1() + m_font_size * AREA_MARGIN,
      m_area.y1() + m_font_size * AREA_MARGIN_BOTTOM,
      m_area.w() - m_font_size * AREA_MARGIN * 2.0f,
      m_font_size);
  m_input.setArea(area_input);

  m_area_text = math::rect2f(area_input.x1(),
      area_input.y2(),
      area_input.w(),
      m_area.y2() - area_input.y2() - m_font_size * AREA_MARGIN);
}

