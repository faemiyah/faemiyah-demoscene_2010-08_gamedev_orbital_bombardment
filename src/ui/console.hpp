#ifndef UI_CONSOLE_HPP
#define UI_CONSOLE_HPP

#include "gfx/surface_screen.hpp"
#include "ui/input_line.hpp"
#include "ui/text_area.hpp"

#include <boost/thread.hpp>

namespace ui
{
  /** \brief Text console class.
   *
   * A (not necessarily interactive) console for displaying text and scrolling it.
   *
   * Does not fully overlap with TextArea functionality, but shares enough of it warrant inheritance.
   */
  class Console
  {
    private:
      /** Font. */
      const gfx::Font &m_font;

      /** Font size. */
      float m_font_size;

      /** View area. */
      math::rect2f m_area;

      /** Area usable for text. */
      math::rect2f m_area_text;

      /** Input line. */
      InputLine m_input;

      /** List of previous lines. */
      std::list<TextRect> m_lines;

      /** Background texture. */
      const gfx::Texture2D *m_background;

      /** Mutex to prevent simultaneous accesses to the log. */
      boost::mutex m_mut;

    protected:
      /** Background color. */
      gfx::Color m_color_background;

      /** Border color. */
      gfx::Color m_color_border;

      /** Text color. */
      gfx::Color m_color_input_bottom;

      /** Text color. */
      gfx::Color m_color_input_top;

      /** Text color. */
      gfx::Color m_color_text_bottom;

      /** Text color. */
      gfx::Color m_color_text_top;

    public:
      /** \brief Constructor.
       *
       * \param fnt Font to use.
       * \param fs Font size for this text box.
       * \param prestrict Text area restriction (in terms of screen size).
       * \param pfname Backround image filename.
       */
      Console(const gfx::Font &fnt, float fs, const math::rect2f &prestrict, const char *pfname = NULL);

      /** \brief Constructor.
       *
       * \param fnt Font to use.
       * \param fs Font size for this text box.
       * \param screen Take restriction area from this screen object.
       * \param pfname Backround image filename.
       */
      Console(const gfx::Font &fnt, float fs, const gfx::SurfaceScreen &screen, const char *pfname = NULL);

      /** \brief Destructor. */
      virtual ~Console() { }

    private:
      /** \brief Set default colors.
       */
      void setDefaultColors();

    public:
      /** \brief Append a row.
       *
       * The row may actually be multiple rows.
       *
       * \param op String (the row) to append.
       */
      void addRow(const std::wstring &op);

      /** \brief Set the background image.
       *
       * \param tex New background texture.
       */
      void setBackground(const gfx::Texture2D *tex);

    protected:
      /** \brief Update the console area.
       *
       * This updates the subsidiary areas derived from font size and the area in
       * use.
       */
      void updateArea();

    public:
      /** \brief Execute and clear the input line.
       */
      virtual void execute();

      /** \brief Render this console.
       *
       * \param screen Screen to render to.
       */
      virtual void render(gfx::SurfaceScreen &screen);

      /** \brief Render the text content in the console.
       *
       * By default, this method should not need to be overwritten. Just remember to set fill colors before
       * calling.
       *
       * \param screen Screen to render to.
       */
      virtual void renderText(const gfx::SurfaceScreen &screen);

    public:
      /** \brief Wrapper for addRow.
       *
       * \param op Row to add.
       */
      void addRow(const std::string &op)
      {
        this->addRow(wstr_utf8(op));
      }

      /** \brief Accessor.
       *
       * \return Input line.
       */
      InputLine& getInput()
      {
        return m_input;
      }

      /** \brief Accessor.
       *
       * \return Input line.
       */
      const InputLine& getInput() const
      {
        return m_input;
      }
  };
}

#endif
