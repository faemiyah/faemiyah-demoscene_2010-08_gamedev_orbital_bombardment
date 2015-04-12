#ifndef GFX_GLYPH_HPP
#define GFX_GLYPH_HPP

#include "gfx/texture_2d.hpp"

namespace gfx
{
  class Font;

  /** \brief One glyph contained in a font.
   *
   * Mostly invisible to the user due to being actually accessed by the font
   * class internally.
   *
   * All the measurements are expressed in floating-point values that are
   * fractions of the full font size.
   */
  class Glyph
  {
    private:
      /** The unicode number. */
      wchar_t m_code;

      /** Width of the character. */
      float m_width;

      /** Height of the character. */
      float m_height;

      /** Distance from the current write coordinates to the left border of the
       * glyph. */
      float m_left;

      /** Distance from the current write coordinates to the top border of the
       * glyph. */
      float m_top;

      /** Advance value after writing this glyph. */
      math::vec2f m_advance;

      /** Lower left corner. */
      math::vec4f m_corner1;

      /** Upper right corner. */
      math::vec4f m_corner2;

      /** Font page this glyph is contained in in the font. */
      unsigned m_page;

      /** Texture reference for easy access. */
      const Texture2D *m_texture;

    public:
      /** Constructor. */
      Glyph() { }

      /** Constructor.
       *
       * Glyphs should generally not be generated manually.
       * 
       * \param pcode Code.
       * \param pwidth Width.
       * \param pheight Height.
       * \param pleft Left.
       * \param ptop Top.
       * \param padvx X advance.
       * \param padvy Y advance.
       * \param pos1 Bottom left corner (x1,y1,s1,t1).
       * \param pos2 Top right corner (x2,y2,s2,t2).
       * \param ppage Page in font.
       */
      Glyph(wchar_t pcode, float pwidth, float pheight, float pleft, float ptop, float padvx, float padvy,
          const math::vec4f &pos1, const math::vec4f &pos2, unsigned ppage);

      /** Destructor. */
      ~Glyph() { }

    public:
      /** \brief Calculate for use.
       *
       * Will fill the font-dependant fields of this.
       *
       * \param fnt Host font.
       */
      void compile(const Font &fnt);

    public:
      /** \brief Bind the texture used in this glyph into use.
       */
      void bind() const
      {
        m_texture->bind();          
      }

      /** \brief Accessor.
       *
       * \return Advance of this.
       */
      const math::vec2f& getAdvance() const
      {
        return m_advance;
      }

      /** \brief Accessor.
       *
       * \return X advance of this.
       */
      float getAdvanceX() const
      {
        return m_advance.x();
      }

      /** \brief Accessor.
       *
       * \return Advance of this.
       */
      float getAdvanceY() const
      {
        return m_advance.y();
      }

      /** \brief Accessor.
       *
       * \return Code of this.
       */
      wchar_t getCode() const
      {
        return m_code;
      }

      /** \brief Accessor.
       *
       * \return Height of this.
       */
      float getHeight() const
      {
        return m_height;
      }

      /** \brief Accessor.
       *
       * \return Left of this.
       */
      float getLeft() const
      {
        return m_left;
      }

      /** \brief Accessor.
       *
       * \return Texture of this.
       */
      const Texture2D* getTexture() const
      {
        return m_texture;
      }

      /** \brief Accessor.
       *
       * \return st1 of this.
       */
      const math::vec4f& getCorner1() const
      {
        return m_corner1;
      }

      /** \brief Accessor.
       *
       * \return st2 of this.
       */
      const math::vec4f& getCorner2() const
      {
        return m_corner2;
      }

      /** \brief Accessor.
       *
       * \return Top of this.
       */
      float getTop() const
      {
        return m_top;
      }

      /** \brief Accessor.
       *
       * \return Width of this.
       */
      float getWidth() const
      {
        return m_width;
      }

    public:
      /** \brief Output this to a stream.
       *
       * \param ss Target stream.
       * \return Stream after input.
       */
      std::ostream& put(std::ostream &ss) const;

      /** \brief Output to stream.
       *
       * \param lhs Left-hand-side operand.
       * \param rhs Right-hand-side operand.
       * \return Stream after output.
       */
      friend std::ostream& operator<<(std::ostream &lhs, const Glyph &rhs)
      {
        return rhs.put(lhs);
      }
  };
}

#endif
