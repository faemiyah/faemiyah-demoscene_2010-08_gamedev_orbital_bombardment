#ifndef UI_TEXT_RECT_HPP
#define UI_TEXT_RECT_HPP

#include "gfx/color.hpp"
#include "gfx/font.hpp"
#include "math/rect.hpp"
//#include "math/vec.hpp"
#include "ui/text_row.hpp"

namespace ui
{
  /** \brief Rectangle of text.
   *
   * The higher-class abstraction of text row and word.
   */
  class TextRect
  {
    protected:
      /** Restricting dimensions. */
      math::vec2f m_limits;

      /** Actual text content dimensions (width and height). */
      math::vec2f m_dimensions;

      /** >Text contained within, arranged as per-line-basis. */
      std::vector<TextRow> m_content;

    public:
      /** \brief Constructor.
       *
       * \param plimits Area restriction.
       */
      TextRect(const math::vec2f &plimits);

      /** \brief Destructor. */
      ~TextRect() { }

    public:
      /** \brief Calculates new dimensions for the content.
       *
       * This method should be called prior to rendering the same text
       * rectangle with a new font size and/or a new font.
       *
       * Recalculating the dimensions does not modify the text lining nor will
       * it drop any text already inserted within the text rectangle. This may
       * result in a text dimensions that does not fit within the original
       * restrictions. In this case, generating a new text rectangle from the
       * input text data is probably in order.
       *
       * \param font Font to use for recalculation.
       * \param font_size Font size to use for recalculation.
       * \return True if new dimensions are within the limits, false if not.
       */
      bool calcDimensions(const gfx::Font &font, float font_size);

      /** \brief Clears all content in this.
       */
      void clear();

      /** \brief Renders text in this textarea.
       *
       * Fill color must have been set beforehand.
       *
       * \param carea Content area.
       * \param font Font to use.
       * \param font_size Font size to use.
       * \param justify Justification schema.
       */
      void renderText(const math::rect2f &carea, const gfx::Font &font,
          float font_size, gfx::TextJustify = gfx::LEFT) const;

      /** \brief Sets the contents of this.
       *
       * If the text rectangle can't hold a single character, this will return the
       * string it was passed.
       *
       * \param str Input string to use (as much as possible).
       * \param fnt Font to use.
       * \param fs Font size to use.
       * \return Rest of the string (what was not used), may be empty.
       */
      std::wstring setContent(const std::wstring &str, const gfx::Font &fnt,
          float fs);

    private:
      /** \brief Append a row.
       *
       * Modifies the dimensions accordingly.
       *
       * \param op Textrow to add.
       * \param fnt Font to use.
       * \param fs Font size.
       * \return True if row added (there was room), false if not.
       */
      bool add(const TextRow &op, const gfx::Font &fnt, float fs);

    public:
      /** \brief Accessor.
       *
       * \return Contents of this.
       */
      const std::vector<TextRow>& getContent() const
      {
        return m_content;
      }

      /** \brief Accessor.
       *
       * \return Dimensions vector.
       */
      const math::vec2f getDimensions() const
      {
        return m_dimensions;
      }

      /** \brief Accessor.
       * 
       * \return Boundary rectangle.
       */
      const math::vec2f& getLimits() const
      {
        return m_limits;
      }

      /** \brief Get a text line.
       *
       * NOT checked. Use hasLine() to check.
       *
       * \param idx Line index.
       * \return Line at index.
       */
      TextRow& getLine(unsigned idx)
      {
        return m_content[idx];
      }

      /** \brief Const version of getLine().
       *
       * \param idx Line index.
       * \return Line at index.
       */
      const TextRow& getLine(unsigned idx) const
      {
        return m_content[idx];
      }

      /** \brief Tell if line exists in here.
       *
       * \param idx Line index.
       * \return True if yes, false if no.
       */
      bool hasLine(unsigned idx) const
      {
        return (idx < m_content.size());
      }

      /** \brief Set the boundary area of this.
       *
       * Will implicitly clear the content.
       *
       * \param plimits New limits.
       */
      void setLimits(const math::vec2f &plimits)
      {
        this->clear();
        m_limits = plimits;
      }

      /** \brief Wrapper for limit set.
       *
       * \param pw Width.
       * \param ph Height.
       */
      void setLimits(float pw, float ph)
      {
        this->setLimits(math::vec2f(pw, ph));
      }
  };
}

#endif
