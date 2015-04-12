#ifndef UI_TEXT_AREA_HPP
#define UI_TEXT_AREA_HPP

#include "gfx/color.hpp"
#include "ui/generic.hpp"
#include "ui/text_rect.hpp"

namespace ui
{
  class TextWord;

  /** \brief Portrays a text box.
  */
  class TextArea :
    public TextRect
  {
    protected:
      /** Restrict area. */
      math::rect2f m_boundary;

      /** Margin value. */
      float m_margin_left;

      /** Margin value. */
      float m_margin_right;

      /** Margin value. */
      float m_margin_top;

      /** Margin value. */
      float m_margin_bottom;

      /** General font size of this. */
      float m_font_size;

      /** Font used in this. */
      const gfx::Font &m_font;

      /** Justification mode. */
      TextGravity m_gravity;

    public:
      /** \brief Constructor.
       *
       * \param prestrict Restrictions.
       * \param fs Font size for this text box.
       * \param fnt Font to use.
       * \param pjust Justification schema.
       */
      TextArea(const math::rect2f &prestrict, float fs, const gfx::Font &fnt,
          TextGravity grav = GRAVITY_CENTER);

      /** \brief Destructor.
       */
      virtual ~TextArea() { }

    public:
      /** \brief Calculate the borders of the encapsulated text content.
       *
       * Uses the internal gravity variable to determine the area.
       * 
       * \return Size rectangle including position.
       */
      math::rect2f getContentArea() const;

    protected:
      /** \brief Render text in this.
       *
       * Fill color must have been set beforehand.
       *
       * \param col Color to render with.
       * \param carea Content area calculated beforehand.
       */
      void renderText(const math::rect2f &carea) const;

      /** \brief Wrapper for text render.
       *
       * Calculates the content size by itself.
       *
       * \param col Color to render with.
       */
      void renderText() const;

    private:
      /** \brief Update the text rectangle boundary.
       *
       * Takes the margins into account to update the boudnary.
       */
      void updateBoundary();

    public:
      /** \brief Render the text area.
       *
       * The default render is intentionally crude. It can be used as an
       * example to create better-looking renders (overwriting this method or
       * not).
       *
       * \param tcol Text color.
       * \param bcol Border color.
       */
      virtual void render(const gfx::Color &tcol, const gfx::Color &bcol) const;

    public:
      /** \brief Get the font.
       *
       * \return Font in this.
       */
      const gfx::Font& getFont() const
      {
        return m_font;
      }

      /** \brief Get the font size.
       *
       * \return Font size.
       */
      float getFontSize() const
      {
        return m_font_size;
      }

      /** \brief Get justification mode.
       *
       * \return Justification mode.
       */
      TextGravity getGravity() const
      {
        return m_gravity;
      }

      /** \brief Margin accessor.
       *
       * \return Margin value.
       */
      float getMarginBottom() const
      {
        return m_margin_left;
      }

      /** \brief Margin accessor.
       *
       * \return Margin value.
       */
      float getMarginLeft() const
      {
        return m_margin_left;
      }

      /** \brief Margin accessor.
       *
       * \return Margin value.
       */
      float getMarginRight() const
      {
        return m_margin_right;
      }

      /** \brief Margin accessor.
       *
       * \return Margin value.
       */
      float getMarginTop() const
      {
        return m_margin_left;
      }

      /** \brief Get the position.
       *
       * \return Position vector.
       */
      const math::vec2f& getPosition() const
      {
        return m_boundary.pos();
      }

      /** \brief Get the size.
       *
       * \return Size vector.
       */
      const math::vec2f& getSize() const
      {
        return m_boundary.size();
      }

      /** \brief Move this.
       *
       * Alters the restriction rectangle to move this.
       *
       * Note that the move anchor is always the lower left corner of the
       * textarea.
       *
       * \param mvec New position.
       */
      void move(const math::vec2f &mvec)
      {
        m_boundary = math::rect2f(mvec, m_boundary.size());
      }

      /** \brief Move this.
       *
       * Alters the restriction rectangle to move this.
       *
       * Note that the move anchor is always the lower left corner of the
       * textarea.
       *
       * \param px New x coordinate.
       * \param py New y coordinate.
       */
      void move(float px, float py)
      {
        this->move(math::vec2f(px, py));
      }

      /** \brief Set the text content of this.
       *
       * \param op String to set in.
       * \return Rest of the string that didn't fit.
       */
      std::wstring setContent(const std::wstring &op)
      {
        return this->TextRect::setContent(op, m_font, m_font_size);
      }

      /** \brief Set bottom margin.
       *
       * Note: does not update the boundary.
       *
       * \param op New margin.
       */
      void setMarginBottom(float op)
      {
        m_margin_bottom = op * m_font_size;
      }

      /** \brief Set left margin.
       *
       * Note: does not update the boundary.
       *
       * \param op New margin.
       */
      void setMarginLeft(float op)
      {
        m_margin_left = op * m_font_size;
      }

      /** \brief Set right margin.
       *
       * Note: does not update the boundary.
       *
       * \param op New margin.
       */
      void setMarginRight(float op)
      {
        m_margin_right = op * m_font_size;
      }

      /** \brief Set top margin.
       *
       * Note: does not update the boundary.
       *
       * \param op New margin.
       */
      void setMarginTop(float op)
      {
        m_margin_top = op * m_font_size;
      }

      /** \brief Set margins.
       *
       * Order: left right bottom top.
       *
       * \param pleft Left margin.
       * \param pright Right margin.
       * \param pbottom Bottom margin.
       * \param ptop Top margin.
       */
      void setMargins(float pleft, float pright, float pbottom,
          float ptop)
      {
        this->setMarginLeft(pleft);
        this->setMarginRight(pright);
        this->setMarginBottom(pbottom);
        this->setMarginTop(ptop);
        this->updateBoundary();
      }
  };
}

#endif
