#include "ui/text_area.hpp"

#include "gfx/surface.hpp"
#include "ui/text_row.hpp"
#include "ui/text_word.hpp"

using namespace ui;

TextArea::TextArea(const math::rect2f &prestrict, float fs,
    const gfx::Font &fnt, TextGravity grav) :
  TextRect(prestrict.size()),
  m_boundary(prestrict),
  m_margin_left(0.0f),
  m_margin_right(0.0f),
  m_margin_top(0.0f),
  m_margin_bottom(0.0f),
  m_font_size(fs),
  m_font(fnt),
  m_gravity(grav) { }

math::rect2f TextArea::getContentArea() const
{
  float leftOffset, downOffset;

  // left
  switch(m_gravity)
  {
    case GRAVITY_UP_RIGHT:
    case GRAVITY_DOWN_RIGHT:
    case GRAVITY_RIGHT:
      leftOffset = m_boundary.x1() + m_boundary.w() - m_dimensions.x() - m_margin_right;
      break;

    case GRAVITY_UP_LEFT:
    case GRAVITY_DOWN_LEFT:
    case GRAVITY_LEFT:
      leftOffset = m_boundary.x1() + m_margin_left;
      break;

    case GRAVITY_UP:
    case GRAVITY_DOWN:
    case GRAVITY_CENTER:
    default:
      leftOffset = (m_boundary.w() - m_dimensions.x()) * 0.5f;
      if(leftOffset < m_margin_left)
      {
        leftOffset = m_boundary.x1() + m_margin_left;
        break;
      }
      else if(leftOffset < m_margin_right)
      {
        leftOffset = m_boundary.x1() + m_boundary.w() - m_dimensions.x() - m_margin_right;
        break;
      }
      leftOffset += m_boundary.x1();
      break;
  }

  // down
  switch(m_gravity)
  {
    case GRAVITY_UP_LEFT:
    case GRAVITY_UP_RIGHT:
    case GRAVITY_UP:
      downOffset = m_boundary.y1() + m_boundary.h() - m_dimensions.y() - m_margin_top;
      break;

    case GRAVITY_DOWN_LEFT:
    case GRAVITY_DOWN_RIGHT:
    case GRAVITY_DOWN:
      downOffset = m_boundary.y1() + m_margin_bottom;
      break;

    case GRAVITY_LEFT:
    case GRAVITY_RIGHT:
    case GRAVITY_CENTER:
    default:
      downOffset = (m_boundary.h() - m_dimensions.y()) * 0.5f;
      if(downOffset < m_margin_bottom)
      {
        downOffset = m_boundary.y1() + m_margin_bottom;
        break;
      }
      else if(downOffset < m_margin_top)
      {
        downOffset = m_boundary.y1() + m_boundary.h() - m_dimensions.y() - m_margin_top;
        break;
      }
      downOffset += m_boundary.y1();
      break;
  }

  return math::rect2f(leftOffset, downOffset, m_dimensions);
}

void TextArea::render(const gfx::Color &tcol, const gfx::Color &bcol) const
{
  //std::cout << "Contents: " << bord << std::endl;
  math::vec2f margin_lb(m_margin_left, m_margin_bottom),
    margin_rt(m_margin_right, m_margin_top);
  math::rect2f carea = this->getContentArea();
  math::rect2f bord = math::rect2f(
      math::vec2f(carea.pos() - margin_lb),
      math::vec2f(carea.size() + margin_lb + margin_rt));

  //std::cout << "Borders: " << bord << std::endl;

  gfx::bind_shader_2d();
  gfx::load_identity();
  for(unsigned ii = 0; (ii < 8); ++ii)
  {
    gfx::draw_fill(ii, bcol);
  }

  float rs1 = m_font_size * 0.25f;
  gfx::draw_fill(0, bord.x1(), bord.y1() + rs1);
  gfx::draw_fill(1, bord.x1(), bord.y2() - rs1);
  gfx::draw_fill(2, bord.x1() + rs1, bord.y2());
  gfx::draw_fill(3, bord.x2() - rs1, bord.y2());
  gfx::draw_fill(4, bord.x2(), bord.y2() - rs1);
  gfx::draw_fill(5, bord.x2(), bord.y1() + rs1);
  gfx::draw_fill(6, bord.x2() - rs1, bord.y1());
  gfx::draw_fill(7, bord.x1() + rs1, bord.y1());
  glDrawArrays(GL_LINE_LOOP, 0, 8);

  float rs2 = m_font_size * 0.1f;
  gfx::draw_fill(0, bord.x1() + rs2, bord.y1() + rs1);
  gfx::draw_fill(1, bord.x1() + rs2, bord.y2() - rs1);
  gfx::draw_fill(2, bord.x1() + rs1, bord.y2() - rs2);
  gfx::draw_fill(3, bord.x2() - rs1, bord.y2() - rs2);
  gfx::draw_fill(4, bord.x2() - rs2, bord.y2() - rs1);
  gfx::draw_fill(5, bord.x2() - rs2, bord.y1() + rs1);
  gfx::draw_fill(6, bord.x2() - rs1, bord.y1() + rs2);
  gfx::draw_fill(7, bord.x1() + rs1, bord.y1() + rs2);
  glDrawArrays(GL_LINE_LOOP, 0, 8);

  gfx::bind_shader_2d_font();
  gfx::load_identity();
  gfx::draw_fill(0, tcol);
  gfx::draw_fill(1, tcol);
  gfx::draw_fill(2, tcol);
  gfx::draw_fill(3, tcol);
  this->renderText(carea);
}

void TextArea::renderText() const
{
  this->renderText(this->getContentArea());
}

void TextArea::renderText(const math::rect2f &carea) const
{
  gfx::TextJustify justify;
  switch(m_gravity)
  {
    case GRAVITY_UP_LEFT:
    case GRAVITY_DOWN_LEFT:
    case GRAVITY_LEFT:
      justify = gfx::LEFT;
      break;

    case GRAVITY_UP_RIGHT:
    case GRAVITY_DOWN_RIGHT:
    case GRAVITY_RIGHT:
      justify = gfx::RIGHT;
      break;

    case GRAVITY_UP:
    case GRAVITY_DOWN:
    case GRAVITY_CENTER:
    default:
      justify = gfx::CENTER;
      break;
  }

  this->TextRect::renderText(carea, m_font, m_font_size, justify);
}

void TextArea::updateBoundary()
{
  this->setLimits(m_boundary.w() - m_margin_left - m_margin_right,
      m_boundary.h() - m_margin_bottom - m_margin_top);
}

