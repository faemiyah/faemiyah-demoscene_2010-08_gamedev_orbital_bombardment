#ifndef UI_TEXT_ROW_HPP
#define UI_TEXT_ROW_HPP

#include "gfx/font.hpp"

#include <sstream>

namespace ui
{
  class TextWord;

  /** \brief Text row size information. */
  struct TextRowDimensions
  {
    private:
      /** Width using the parameters specified in the addition. */
      float m_width;

      /** Maximum down distance in this row. */
      float m_down;

      /** Maximum up distance in this row. */
      float m_up;

    public:
      /** \brief Constructor. */
      TextRowDimensions() :
        m_width(0.0f) { }

      /** \brief Constructor.
       *
       * \param width Width.
       * \param down Maximum direction down.
       * \param up Maximum direction up.
       */
      TextRowDimensions(float width, float down, float up) :
        m_width(width),
        m_down(down),
        m_up(up) { }

      ~TextRowDimensions() { }

    public:
      /** \brief Get down direction.
       *
       * \return Maximum up distance. */
      float getDown() const
      {
        return m_down;
      }

      /** \brief Get up direction.
       *
       * \return Maximum up distance. */
      float getUp() const
      {
        return m_up;
      }

      /** \brief Get width.
       *
       * \return Width. */
      float getWidth() const
      {
        return m_width;
      }

      /** \brief Increase distance down if necessary.
       *
       * \param op New distance down.
       */
      void incDown(float op)
      {
        if(op > m_down)
        {
          m_down = op;
        }
      }

      /** \brief Increase distance up if necessary.
       *
       * \param op New distance up.
       */
      void incUp(float op)
      {
        if(op > m_up)
        {
          m_up = op;
        }
      }

      /** \brief Increase width.
       *
       * \param op Width to increase with.
       */
      void incWidth(float op)
      {
        m_width += op;
      }
  };

  /** \brief Represents one text row within a textarea.
  */
  class TextRow
  {
    private:
      TextRowDimensions m_dimensions;

      /** String builder, to be unreferenced when ready. */
      std::wstringstream m_builder;

    public:
      /** \brief Constructor.
       */
      TextRow() { }

      /** \brief Copy constructor.
       *
       * \param op Source.
       */
      TextRow(const TextRow &op) :
        m_dimensions(op.m_dimensions)
      {
        m_builder.str(op.m_builder.str());
      }

    public:
      /** \brief Add one character to this row.
       *
       * \param cc Character to add.
       * \param mwid Maximum width.
       * \param cwid Character width.
       * \return Success value.
       */
      bool add(wchar_t cc, float mwid, float cwid);

      /** \brief Add one word to this row.
       *
       * \param word Word to add.
       * \param mwid Maximum width.
       * \param schar Space character.
       * \param swid Space width.
       * \return Success value.
       */
      bool add(const TextWord &word, float mwid, wchar_t schar, float swid);

      /** \brief Calculate dimensions according to given font and given size.
       *
       * \param fnt Font used.
       * \param font size used.
       * \return Dimensions calculated from this text row.
       */
      TextRowDimensions calcDimensions(const gfx::Font &fnt, float font_size) const;

    public:
      /** \brief Clear all content of this row.
       */
      void clear()
      {
        m_dimensions = TextRowDimensions();
        m_builder.str(std::wstring());
      }

      /** \brief Accessor.
       *
       * \return content.
       */
      std::wstring getContent() const
      {
        return m_builder.str();
      }

      /** \brief Accessor.
       *
       * \return Maximum down distance.
       */
      float getDown() const
      {
        return m_dimensions.getDown();
      }

      /** \brief Accessor.
       *
       * \return Maximum up distance.
       */
      float getUp() const
      {
        return m_dimensions.getUp();
      }

      /** \brief Accessor.
       *
       * \return Width.
       */
      float getWidth() const
      {
        return m_dimensions.getWidth();
      }

      /** \brief Set the dimensions of this text row.
       *
       * \param dimensions New dimensions.
       */
      void setDimensions(const TextRowDimensions& op)
      {
        m_dimensions = op;
      }

    public:
      /** \brief Assignment operator.
       *
       * \param op Source.
       */
      const TextRow& operator=(const TextRow &op)
      {
        m_dimensions = op.m_dimensions;
        m_builder.str(op.m_builder.str());
        return *this;
      }
  };
}

#endif
