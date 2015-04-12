#include "ui/input_line.hpp"

#include "gfx/font.hpp"
#include "gfx/surface.hpp"
#include "ui/event_key.hpp"
#include "ui/generic.hpp"

using namespace ui;

InputLine::InputLine() :
  m_cursor(0) { }

InputLine::InputLine(const std::wstring &op) :
  m_line(op),
  m_cursor(0) { }

void InputLine::appendChar(wchar_t code)
{
  std::wstring add;
  add.append(1, code);
  m_line = m_line.substr(0, m_cursor) + add + m_line.substr(m_cursor);
  ++m_cursor;
}

void InputLine::deleteChar(int dir)
{
  if(dir < 0)
  {
    if(m_cursor <= 0)
    {
      return;
    }
    m_line = m_line.substr(0, m_cursor - 1) +
      m_line.substr(m_cursor);
    --m_cursor;
    return;
  }

  if(m_cursor >= m_line.length())
  {
    return;
  }
  m_line = m_line.substr(0, m_cursor) +
    m_line.substr(m_cursor + 1);
}

void InputLine::draw(const gfx::Color &bcol, const gfx::Color &tcol,
    float font_size, const gfx::Font &fnt) const
{
  gfx::draw_fill(0, bcol);
  gfx::draw_fill(1, bcol);
  gfx::draw_fill(2, tcol);
  gfx::draw_fill(3, tcol);

  std::wstring heading(wstr_utf8("> "));
  int nline;
  float hlen, cpos;
  boost::tie(nline, hlen) = fnt.calcLineWidth(font_size, heading);

  std::wstring line(m_line);
  float ilen;
  boost::tie(nline, ilen) = fnt.calcLineWidth(font_size, line);

  if(hlen + ilen > m_area.w())
  {
    std::wstring period(wstr_utf8("..."));
    float plen;
    boost::tie(nline, plen) = fnt.calcLineWidth(font_size, period);

    int iline_start = static_cast<int>(m_cursor);
    int iline_end = iline_start;
    for(;;)
    {
      if(--iline_start < 0)
      {
        iline_start = 0;
      }
      else
      {
        std::wstring line_new(m_line.substr(static_cast<size_t>(iline_start),
              static_cast<size_t>(iline_end - iline_start)));
        boost::tie(nline, ilen) = fnt.calcLineWidth(font_size, line_new);
        if((hlen + plen * 2.0f + ilen) > m_area.w())
        {
          ++iline_start;
          break;
        }
        line = line_new;
      }

      if(++iline_end > static_cast<int>(m_line.length()))
      {
        iline_end = static_cast<int>(m_line.length());
      }
      else
      {
        std::wstring line_new(m_line.substr(static_cast<size_t>(iline_start),
              static_cast<size_t>(iline_end - iline_start)));
        boost::tie(nline, ilen) = fnt.calcLineWidth(font_size, line_new);
        if((hlen + plen * 2.0f + ilen) > m_area.w())
        {
          --iline_end;
          break;
        }
        line = line_new;
      }
    }

    if(iline_start > 0)
    {
      heading.append(period);
      hlen += plen;
    }
    if(iline_end < static_cast<int>(m_line.length()))
    {
      line.append(period);
    }

    boost::tie(nline, cpos) = fnt.calcLineWidth(font_size,
        line.substr(0, m_cursor - static_cast<unsigned>(iline_start)));
    heading += line;
  }
  else
  {
    heading += line;
    boost::tie(nline, cpos) = fnt.calcLineWidth(font_size,
        m_line.substr(0, m_cursor));
  }

  gfx::bind_shader_2d_font();
  gfx::load_identity();
  
  gfx::draw_text_line(m_area.x1(), m_area.y1(), font_size, heading,
      fnt);
  gfx::draw_text_line(m_area.x1() + hlen + cpos, m_area.y1(),
      font_size, wstr_utf8("_"), fnt);
}

void InputLine::move(int dir)
{
  if(INT_MIN == dir)
  {
    m_cursor = 0;
    return;
  }
  if(INT_MAX == dir)
  {
    m_cursor = static_cast<unsigned>(m_line.length());
    return;
  }

  int new_cursor_pos = static_cast<int>(m_cursor) + dir;

  if(new_cursor_pos > static_cast<int>(m_line.length()))
  {
    m_cursor = static_cast<unsigned>(m_line.length());
  }
  else if(new_cursor_pos < 0)
  {
    m_cursor = 0;
  }
  else
  {
    m_cursor = static_cast<unsigned>(new_cursor_pos);
  }
}

int InputLine::process(const EventKey &ev)
{
  if(!ev.isPress())
  {
    return 0;
  }

  int code = ev.getCode();

  if(ev.hasUnicode() || ((32 <= code) && (127 >= code)))
  {
    this->appendChar(static_cast<wchar_t>(code));
    return 1;
  }

  switch(code)
  {
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
      return -1;

    case SDLK_BACKSPACE:
      this->deleteChar(-1);
      return 1;

    case SDLK_DELETE:
      this->deleteChar(1);
      return 1;

    case SDLK_LEFT:
      this->move(-1);
      return 1;

    case SDLK_HOME:
      this->move(INT_MIN);
      return 1;

    case SDLK_RIGHT:
      this->move(1);
      return 1;

    case SDLK_END:
      this->move(INT_MAX);
      return 1;

    default:
      break;
  }
  return 0;  
}

