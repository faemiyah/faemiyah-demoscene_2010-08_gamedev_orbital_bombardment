#ifndef GFX_FONT_HPP
#define GFX_FONT_HPP

#include "data/store.hpp"
#include "gfx/font_loader.hpp"
#include "gfx/glyph.hpp"

#include <list>

namespace gfx
{
  class Texture2D;

  /** Convenience typedef. */
  typedef std::map<wchar_t, Glyph> GlyphMap;

  /** Represents one font.
   *
   * The font is loaded from a pregenerated image files and handled as a set
   * of textures.
   */
  class Font :
    public data::Storable<Font, FontLoader>
  {
    public:
      /** \brief Font width / height in pixels.
       *
       * Change this if font generation specifics change.
       */
      static const int GLYPH_SIZE_PIXELS = 32;

      /** \brief Size of one font page in pixels.
       *
       * Change this if font generation specifics change.
       */
      static const int FONT_PAGE_SIZE = 2048;

      /** Number of glyphs per one dimension of font page. */
      static const int GLYPHS_PER_FONT_SIDE = FONT_PAGE_SIZE / GLYPH_SIZE_PIXELS;

      /** Number of glyphs per font page. */
      static const int GLYPHS_PER_FONT_PAGE = GLYPHS_PER_FONT_SIDE * GLYPHS_PER_FONT_SIDE;

      /** Coordinate size of one glyph in font texture space. */
      static const float GLYPH_SIDE_LENGTH;

    private:
      /** Textures of this font file. */
      std::vector<Texture2DSptr> m_textures;

      /** Dictionary of glyphs per their unicode number. */
      GlyphMap m_glyphs;

      /** Width of one space character. */
      float m_space_width;

    public:
      /** \brief Empty constructor. */
      Font();

      /** \brief Constructor.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      Font(const boost::filesystem::path &pfname, const FontLoader &loader = FontLoader());

      /** \brief Destructor. */
      ~Font();

    public:
      /** Gets the length of a given text line.
       *
       * The next line start index will be zero if end of string is
       * reached.
       *
       * \param fs Font size.
       * \param text Text to calculate.
       * \param idx Start calculating from this index.
       * \return Tuple of next line start index and width.
       */
      boost::tuple<unsigned, float> calcLineWidth(float fs, const std::wstring &text, unsigned idx = 0) const;

      /** \brief Gets the dimensions of a given string.</summary>
       * 
       * The dimensions returned herein are close, but not necessarily
       * pixel-perfect exact.
       *
       * \param fs Font size.
       * \param text Text to calculate dimensions for.
       * \return Vector of dimensions.
       */
      math::vec2f calcTextDimensions(float fs, const std::wstring &text) const;

      /** \brief Gets all the row lengths from a given string.
       *
       * \param fs Font size.
       * \param text Text to calculate.
       * \return List of row lengths.
       */
      std::list<float>* calcTextLengths(float fs, const std::wstring &text) const;

      /** \brief Load from file.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      void load(const boost::filesystem::path &pfname, const FontLoader &loader);

      /** \brief Free all resources required by the font.
       * 
       * Should be done before deinitializing OpenGL.
       */
      void unreserve();

    private:
      /** \brief Add one texture to this.
       *
       * Loads an image and appends it to the end of the texture list.
       *
       * \param pfname Texture filename.
       */
      void addTexture(const std::string &pfname);

      /** \brief Compile for use.
       *
       * To be done after a successful load.
       */
      void compile();

    public:
      /** \brief Add one glyph to this.
       *
       * Stores a glyph by it's code.
       *
       * \param gly Glyph to save.
       */
      void addGlyph(const Glyph &gly)
      {
        m_glyphs[gly.getCode()] = gly;
      }

      /** \brief Accessor.
       *
       * This function is NOT checked. To check for the existence of a glyph,
       * use hasGlyph(wchar_t).
       *
       * \param idx Index to get (code).
       * \return Glyph specified by index or NULL.
       */
      const Glyph& getGlyph(wchar_t idx) const
      {
        GlyphMap::const_iterator iter = m_glyphs.find(idx);

        if(m_glyphs.end() == iter)
        {
          // Unknown glyphs are represented by unicode symbol 'WHITE VERTICAL RECTANGLE'.
          return m_glyphs.find(0x25af)->second;
        }
        return iter->second;
      }

      /** \brief Accessor.
       *
       * \return Space character width.
       */
      float getSpaceWidth() const
      {
        return m_space_width;
      }

      /** \brief Accessor.
       *
       * This function is NOT checked. To check for the existence of a
       * texture index, use hasTexture(unsigned).
       *
       * \param idx Index to get.
       * \return Texture in index.
       */
      const Texture2D& getTexture(unsigned idx) const
      {
        return *(m_textures[idx]);
      }

      /** \brief Tell if this has a glyph.
       *
       * \param idx Index to get (code).
       * \return True if yes, false if no.
       */
      bool hasGlyph(wchar_t idx) const
      {
        return (m_glyphs.find(idx) != m_glyphs.end());
      }

      /** \brief Tell if this has a texture.
       *
       * \param idx Texture index.
       * \return True if yes, false if no.
       */
      bool hasTexture(unsigned idx) const
      {
        return (idx < m_textures.size());
      }

    public:
      /** \brief Create implementation.
       *
       * Despite being public, not to be called by user.
       *
       * \param pfname File to load.
       * \param loader Loader settings.
       * \return Container for all created instances.
       */
      static container_type createImplementation(const boost::filesystem::path &pfname,
          const FontLoader &loader);

    public:
      /** \cond */
      friend std::ostream& operator<<(std::ostream &lhs, const Font &rhs);
      /** \endcond */
  };

  /** \brief Output font to a stream.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Stream after input.
   */
  std::ostream& operator<<(std::ostream &lhs, const Font &rhs);
}

#endif
