#include "ui/text_word.hpp"

using namespace ui;

TextWord::TextWord() :
  m_width(0.0f) { }

TextWord::TextWord(wchar_t cc, float cwid) :
  m_width(cwid)
{
  m_word << cc;
}

float TextWord::add(wchar_t cc, float cwid)
{
  m_word << cc;
  m_width += cwid;
  
  return m_width;
}

std::ostream& TextWord::put(std::ostream &ss) const
{
  std::wstring content = this->getContent();

  return ss << str_utf8(content);
}

