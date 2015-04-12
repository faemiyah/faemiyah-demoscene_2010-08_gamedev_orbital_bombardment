#ifndef UI_INPUT_LINE_HPP
#define UI_INPUT_LINE_HPP

#include "gfx/color.hpp"
#include "math/rect.hpp"

namespace gfx
{
  class Font;
}

namespace ui
{
  class EventKey;

  /** \brief Line input.
   */
  class InputLine
  {
    private:
      /** Area usable for input line. */
      math::rect2f m_area;

      /** Input line. */
      std::wstring m_line;

      /** Cursor position. */
      unsigned m_cursor;

    public:
      /** \brief Empty constructor. */
      InputLine();

      /** \brief Constructor.
       *
       * \param op Initial string.
       */
      InputLine(const std::wstring &op);

      /** \brief Destructor. */
      ~InputLine() { }

    public:
      /** \brief Append a character to the input line.
       *
       * \param code Character code to append.
       */
      void appendChar(wchar_t code);

      /** \brief Delete a character from the input line.
       *
       * \param dir Direction to delete from.
       */
      void deleteChar(int dir);

      /** \brief Draw this line.
       *
       * \param bcol Color (bottom).
       * \param tcol Color (top).
       * \param font_size Font size to use.
       * \param fnt Font to use.
       */
      void draw(const gfx::Color &bcol, const gfx::Color &tcol,
          float font_size, const gfx::Font &fnt) const;

      /** \brief Move in input.
       *
       * \param dir Number of steps to move.
       */
      void move(int dir);

      /** \brief Process input.
       *
       * \return 0 if not processed, -1 if activated, 1 on normal process.
       */
      int process(const EventKey &ev);

    public:
      /** \brief Accessor.
       *
       * \return Area.
       */
      const math::rect2f& getArea()
      {
        return m_area;
      }

      /** \brief Accessor.
       *
       * \return Line.
       */
      const std::wstring& getLine()
      {
        return m_line;
      }

      /** \brief Reset the contents. */
      void reset()
      {
        m_line = std::wstring();
        m_cursor = 0;
      }

      /** \brief Set the area.
       *
       * \param op New area.
       */
      void setArea(const math::rect2f &op)
      {
        m_area = op;
      }
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<InputLine> InputLineSptr;
}

#endif
