#include "ui/text_rect.hpp"

#include "gfx/surface.hpp"
#include "ui/text_word.hpp"

using namespace ui;

TextRect::TextRect(const math::vec2f &plimits) :
  m_limits(plimits),
  m_dimensions(0.0f, 0.0f) { }

bool TextRect::add(const TextRow &op, const gfx::Font &fnt, float fs)
{
  TextRowDimensions trd = op.calcDimensions(fnt, fs);
  float new_height;

  if(m_content.empty())
  {
    new_height = trd.getUp() + trd.getDown();
  }
  else
  {
    new_height = m_content.front().getUp() +
      (static_cast<float>(m_content.size()) * fs) +
      trd.getDown();
  }

  if(new_height > m_limits.y())
  {
    return false;
  }

  m_dimensions.y() = new_height;

  if(m_dimensions.x() < trd.getWidth())
  {
    m_dimensions.x() = trd.getWidth();
  }

  m_content.push_back(op);
  m_content.back().setDimensions(trd);
  return true;
}

void TextRect::clear()
{
  m_dimensions = math::vec2f(0.0f, 0.0f);
  m_content.clear();
}

bool TextRect::calcDimensions(const gfx::Font &font, float font_size)
{
  m_dimensions = math::vec2f(0.0f, 0.0f);

  if(m_content.empty())
  {
    return true;
  }

  BOOST_FOREACH(TextRow &vv, m_content)
  {
    TextRowDimensions trd = vv.calcDimensions(font, font_size);

    m_dimensions.x() = math::max(m_dimensions.x(), trd.getWidth());

    vv.setDimensions(trd);
  }

  m_dimensions.y() = m_content.front().getUp() +
    (static_cast<float>(m_content.size() - 1) * font_size) +
    m_content.back().getDown();

  return (m_dimensions.x() <= m_limits.x()) &&
    (m_dimensions.y() <= m_limits.y());
}

void TextRect::renderText(const math::rect2f &carea, const gfx::Font &font,
    float font_size, gfx::TextJustify justify) const
{
  if(m_content.empty())
  {
    return;
  }

  float cy = carea.y2() - m_content.front().getUp();
  switch(justify)
  {
    case gfx::LEFT:
      BOOST_FOREACH(const TextRow &vv, m_content)
      {
        gfx::draw_text_line(carea.x1(), cy, font_size, vv.getContent(), font);
        cy -= font_size;
      }
      return;

    case gfx::RIGHT:
      BOOST_FOREACH(const TextRow &vv, m_content)
      {
        gfx::draw_text_line(carea.x2() - vv.getWidth(), cy, font_size,
            vv.getContent(), font);
        cy -= font_size;
      }
      return;

    case gfx::CENTER:
    default:
      BOOST_FOREACH(const TextRow &vv, m_content)
      {
        gfx::draw_text_line(carea.x1() + (carea.w() - vv.getWidth()) * 0.5f, cy,
            font_size, vv.getContent(), font);
        cy -= font_size;
      }
      return;
  }
}

std::wstring TextRect::setContent(const std::wstring &str, const gfx::Font &fnt, float fs)
{
  TextRow row;
  TextWord word;
  wchar_t last_space = 0;
  float last_space_width = 0.0f;
  unsigned last_inc = 0;
  unsigned ret_idx = 0;

  for(unsigned ii = 0; (ii <= str.length()); ++ii)
  {
    // Newline is the same as zero here, it also forces commit.
    wchar_t cc = 0;
    float cwid = 0.0f;
    if(ii < str.length())
    {
      cc = str[ii];
      if(is_newline(cc))
      {
        cc = 0;
      }
      else
      {
        cwid = fnt.getGlyph(cc).getAdvanceX() * fs;
      }
    }

    // Whitespace detected, must commit.
    if(is_whitespace(cc))
    {
      // If no word to commit, do nothing.
      if(word.isEmpty())
      {
        last_space = cc;
        last_space_width = cwid;
      }
      // If appending to row succeeds, it seems all good.
      else if(row.add(word, m_limits.x(), last_space, last_space_width))
      {
        last_space = cc;
        last_space_width = cwid;
        last_inc = ii;
        word.clear();
      }
      // If can't add to row, must create a new row. If can't create a new
      // row, abort.
      else if(!this->add(row, fnt, fs))
      {
        return str.substr(ret_idx);
      }
      // Could add new row.
      else
      {
        row.clear();
        // If can't add word into new row, abort.
        if(!row.add(word, m_limits.x(), 0, 0.0f))
        {
          return str.substr(ret_idx);
        }
        last_space = cc;
        last_space_width = cwid;
        ret_idx = last_inc;
        word.clear();
      }

      // If newline, must additionally commit. Word is always empty now.
      if(0 == cc)
      {
        // If can't create a new row, exit.
        if(!this->add(row, fnt, fs))
        {
          return str.substr(ret_idx);
        }
        row.clear();
        last_space = 0;
        last_space_width = 0.0f;
        ret_idx = last_inc = ii;
      }
    }
    // Not whitespace, append to row.
    else
    {
      word.add(cc, cwid);
    }
  }

  return std::wstring();
}

