#include "ui/text_row.hpp"

#include "ui/text_word.hpp"

using namespace ui;

bool TextRow::add(wchar_t cc, float mwid, float cwid)
{
  if(m_dimensions.getWidth() + cwid <= mwid)
  {
    m_builder << cc;
    m_dimensions.incWidth(cwid);
    return true;
  }
  return false;
}

bool TextRow::add(const TextWord &word, float mwid, wchar_t schar, float swid)
{
  if(m_dimensions.getWidth() + swid + word.getWidth() <= mwid)
  {
    if(0 != schar)
    {
      m_builder << schar;
    }
    m_builder << word.getContent();
    m_dimensions.incWidth(swid + word.getWidth());
    return true;
  }
  return false;
}

TextRowDimensions TextRow::calcDimensions(const gfx::Font &fnt, float font_size) const
{
  std::wstring content = this->getContent();
  TextRowDimensions ret(0.0f, 0.0f, 0.0f);

  BOOST_FOREACH(wchar_t cc, content)
  {
    const gfx::Glyph &gly = fnt.getGlyph(cc);

    ret.incWidth(gly.getAdvanceX() * font_size);
    ret.incDown((gly.getHeight() - gly.getTop()) * font_size);
    ret.incUp(gly.getTop() * font_size);
  }

  return ret;
}

