#include "ui/generic.hpp"

#include "gfx/generic.hpp"

#include <sstream>

using namespace ui;

/** This much time it takes for key repeat to activate. */
static const int UI_KEY_REPEAT_DELAY = 333;

/** This many milliseconds are counted before next repeat input. */
static const int UI_KEY_REPEAT_COUNT = 33;

bool ui::is_newline(wchar_t cc)
{
  switch(cc)
  {
    case 10:
    case 13:
      return true;

    default:
      return false;
  }
}

bool ui::is_whitespace(wchar_t cc)
{
  switch(cc)
  {
    case 0:
    case 10:
    case 13:
    case ' ':
    case '\t':
      return true;

    default:
      return false;
  }
}

void ui::key_repeat_off()
{
  //std::cout << "Repeat off\n";
  SDL_EnableKeyRepeat(0, 0);
  SDL_EnableUNICODE(0);
}

void ui::key_repeat_on()
{
  //std::cout << "Repeat on\n";
  SDL_EnableKeyRepeat(UI_KEY_REPEAT_DELAY, UI_KEY_REPEAT_COUNT);
  SDL_EnableUNICODE(1);
}

std::string ui::str_utf8(const wchar_t *op)
{
  std::stringstream ret;

  wchar_t *iter = const_cast<wchar_t*>(op);
  for(;;)
  {
    uint32_t cc = static_cast<uint32_t>(*iter++);
    if(0 == cc)
    {
      return ret.str();
    }
    if(cc < 128)
    {
      ret << static_cast<char>(cc);
      continue;
    }

    if(cc < 2048)
    {
      ret << static_cast<char>((((cc & 1984) >> 6) | 192)) <<
        static_cast<char>((cc & 63) | 128);
      continue;
    }

    if(cc < 65536)
    {
      ret << static_cast<char>(((cc & 61440) >> 12) | 224) <<
        static_cast<char>(((cc & 4032) >> 6) | 128) <<
        static_cast<char>((cc & 63) | 128);
      continue;
    }

    if(cc < 1114111)
    {
      ret << static_cast<char>(((cc & 1835008) >> 18) | 240) <<
        static_cast<char>(((cc & 258048) >> 12) | 128) <<
        static_cast<char>(((cc & 4032) >> 6) | 128) <<
        static_cast<char>((cc & 63) | 128);
      continue;
    }

    std::stringstream err;
    err << "invalid unicode character: " << cc;
    BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
  }
}

std::wstring ui::wstr_utf8(const char *op)
{
  std::wstringstream ret;

  uint8_t *iter = reinterpret_cast<uint8_t*>(const_cast<char*>(op));
  for(;;)
  {
    wchar_t cc1 = static_cast<wchar_t>(*iter++);
    if(0 == cc1)
    {
      return ret.str();
    }
    if((cc1 & 128) == 0)
    {
      ret << cc1;
      continue;
    }

    wchar_t cc2 = static_cast<wchar_t>(*iter++);
    if((cc2 & 192) != 128)
    {
      std::stringstream err;
      err << "invalid second byte in sequence: " << static_cast<int>(cc1) <<
        ", " << static_cast<int>(cc2);
      BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
    }

    if((cc1 & 224) == 192) 
    {
      int code = ((cc1 & 31) << 6) | (cc2 & 63);
      ret << static_cast<wchar_t>(code);
      continue;
    }

    wchar_t cc3 = static_cast<wchar_t>(*iter++);
    if((cc3 & 192) != 128)
    {
      std::stringstream err;
      err << "invalid third byte in sequence: " << static_cast<int>(cc1) <<
        ", " << static_cast<int>(cc2) << ", " << static_cast<int>(cc3);
      BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
    }

    if((cc1 & 240) == 224)
    {
      int code = ((cc1 & 15) << 12) | ((cc2 & 63) << 6) | (cc3 & 63);
      ret << static_cast<wchar_t>(code);
      continue;
    }

    wchar_t cc4 = static_cast<wchar_t>(*iter++);
    if((cc4 & 192) != 128)
    {
      std::stringstream err;
      err << "invalid fourth byte in sequence: " << static_cast<int>(cc1) <<
        ", " <<  static_cast<int>(cc2) << ", " << static_cast<int>(cc3) <<
        ", " << static_cast<int>(cc4);
      BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
    }

    if((cc1 & 248) == 240)
    {
      int code = ((cc1 & 7) << 18) | ((cc2 & 63) << 12) | ((cc3 & 63) << 6) |
        (cc4 & 63);
      ret << static_cast<wchar_t>(code);
      continue;
    }

    std::stringstream err;
    err << "invalid first byte in sequence: " << cc1;
    BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
  }
}

