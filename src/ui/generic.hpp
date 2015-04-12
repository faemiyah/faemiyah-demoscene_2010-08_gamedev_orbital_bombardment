#ifndef UI_GENERIC_HPP
#define UI_GENERIC_HPP

#include "defaults.hpp"

namespace ui
{
  /** Text gravity enumeration. */
  enum TextGravity
  {
    /** Upper left corner. */
    GRAVITY_UP_LEFT,

    /** Upper right corner. */
    GRAVITY_UP_RIGHT,

    /** Lower left corner. */
    GRAVITY_DOWN_LEFT,

    /** Lower right corner. */
    GRAVITY_DOWN_RIGHT,

    /** Up center. */
    GRAVITY_UP,

    /** Down center. */
    GRAVITY_DOWN,

    /** Left center. */
    GRAVITY_LEFT,

    /** Left center. */
    GRAVITY_RIGHT,

    /** Center of area. */
    GRAVITY_CENTER
  };

  /** \brief Test for a newline character.
   *
   * \param cc Character to test.
   * \return True if newline, false if not.
   */
  extern bool is_newline(wchar_t cc);

  /** \brief Test for a whitespace character.
   *
   * \param cc Character to test.
   * \return True if whitespace, false if not.
   */
  extern bool is_whitespace(wchar_t cc);

  /** \brief Turn off key repeat. */
  extern void key_repeat_off();

  /** \brief Turn on key repeat. */
  extern void key_repeat_on();

  /** \brief Convert a wide string into UTF-8.
   *
   * \param op String to convert.
   * \return Newly created string.
   */
  extern std::string str_utf8(const wchar_t *op);

  /** \brief Convert a wide string into UTF-8.
   *
   * \param op String to convert.
   * \return Newly created string.
   */
  static inline std::string str_utf8(const std::wstring &op)
  {
    return str_utf8(op.c_str());
  }

  /** \brief Convert a string in UTF-8 format into a wide string.
   *
   * \param op String to convert.
   * \return Newly created wide string.
   */
  extern std::wstring wstr_utf8(const char *op);

  /** \brief Convert a string in UTF-8 format into a wide string.
   *
   * \param op String to convert.
   * \return Newly created wide string.
   */
  static inline std::wstring wstr_utf8(const std::string &op)
  {
    return wstr_utf8(op.c_str());
  }
}

#endif
