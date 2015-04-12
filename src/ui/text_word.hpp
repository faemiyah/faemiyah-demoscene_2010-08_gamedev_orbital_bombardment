#ifndef UI_TEXT_WORD_HPP
#define UI_TEXT_WORD_HPP

#include "ui/generic.hpp"

#include <sstream>

namespace ui
{
  /** \brief Represents one word within a text.
   */
  class TextWord
  {
    private:
      /** Width of this word. */
      float m_width;

      /** Word content. */
      std::wstringstream m_word;

    public:
      /** \brief Constructor. */
      TextWord();

      /** \brief Constructor.
       *
       * \param cc Character to add.
       * \param cwid Character width.
       */
      TextWord(wchar_t cc, float cwid);

    public:
      /** \brief Add a character to this word.
       *
       * \param cc Character to add.
       * \param cwid Character width.
       * \return The new word length.
       */
      float add(wchar_t cc, float cwid);

    public:
      /** \brief Clear all content of this word.
       */
      void clear()
      {
        m_width = 0.0f;
        m_word.str(std::wstring());
      }

      /** \brief Accessor.
       *
       * \return Content.
       */
      std::wstring getContent() const
      {
        return m_word.str();
      }

      /** \brief Tell if this word is empty.
       * 
       * \return True if yes, false if not.
       */
      bool isEmpty() const
      {
        return (m_word.str().length() <= 0);
      }

      /** \brief Accessor.
       *
       * \return Length of the word in characters.
       */
      unsigned getLength() const
      {
        return static_cast<unsigned>(this->getContent().length());
      }

      /** \brief Accessor.
       *
       * \return Width.
       */
      float getWidth() const
      {
        return m_width;
      }

    public:
      /** \brief Assignment operator.
       *
       * \param op Source.
       */
      const TextWord& operator=(const TextWord &op)
      {
        m_width = op.m_width;
        m_word.str(op.m_word.str());
        return *this;
      }

    public:
      /** \brief Output this to a stream.
       *
       * \param ss Target stream.
       * \return Stream after input.
       */
      std::ostream& put(std::ostream &ss) const;

      /** \brief Output a word into a stream.
       *
       * \param lhs Left-hand-side operand.
       * \param rhs Right-hand-side operand.
       * \return Modified stream.
       */
      friend std::ostream& operator<<(std::ostream &lhs, const TextWord &rhs)
      {
        return rhs.put(lhs);
      }
  };
}
#endif
