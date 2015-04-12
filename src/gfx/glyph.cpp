#include "gfx/glyph.hpp"
#include "gfx/font.hpp"

using namespace gfx;

Glyph::Glyph(wchar_t pcode, float pwidth, float pheight, float pleft, float ptop, float padvx, float padvy,
    const math::vec4f &pos1, const math::vec4f &pos2, unsigned ppage) :
  m_code(pcode),
  m_width(pwidth),
  m_height(pheight),
  m_left(pleft),
  m_top(ptop),
  m_advance(padvx, padvy),
  m_corner1(pos1),
  m_corner2(pos2),
  m_page(ppage)
{
  // Do nothing.
}

void Glyph::compile(const Font &fnt)
{
  m_texture = &(fnt.getTexture(m_page));
#if 0 //defined(DEBUG)
  std::cout << *this << " ST1: " << m_st1 << " ST2: " << m_st2 << std::endl;
#endif
}

std::ostream& Glyph::put(std::ostream &ss) const
{
  return ss << "Glyph: " << m_code << " / " << m_page << " W/H:" <<
    m_width << "/" << m_height << " L/T:" << m_left << "/" << m_top << " X/Y:" <<
    m_advance.x() << "/" << m_advance.y();
}

