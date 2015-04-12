#ifndef OB_HIGH_SCORES
#define OB_HIGH_SCORES

#include "ui/generic.hpp"
#include "ob_appearing_string.hpp"

namespace ob
{
  /** \brief High score entry.
   */
  class HighScoreEntry
  {
    private:
      /** Score value. */
      int m_score;

      /** Associated appearing string. */
      std::wstring m_name;

      /** This score was most recently added*/
      bool m_newest;

    public:
      /** \brief Constructor.
       *
       * \param score Score.
       * \param name Name.
       * \param newest Newest.
       */
      HighScoreEntry(int score, const std::wstring &name, bool newest) :
        m_score(score),
        m_name(name),
        m_newest(newest) { }

      /** \brief Constructor.
       *
       * \param other Other high score entry.
       */
      HighScoreEntry(const HighScoreEntry &other) :
        m_score(other.m_score),
        m_name(other.m_name),
        m_newest(other.m_newest) { }

      /** \brief Destructor. */
      ~HighScoreEntry() { }

    public:
      /** \brief Accessor.
       *
       * \return Appearing string.
       */
      const std::wstring& getName() const
      {
        return m_name;
      }

      /** \brief Accessor.
       *
       * \return Score.
       */
      int getScore() const
      {
        return m_score;
      }

      /** \brief Accessor.
       *
       * \return m_newest.
       */
      int isNewest() const
      {
        return m_newest;
      }

      /** \brief Comparison operator.
       *
       * \param rhs Right-hand side operand.
       */
      bool operator<(const HighScoreEntry &rhs) const
      {
        if(m_score == rhs.m_score)
        {
          return (m_name > rhs.m_name);
        }
        return (m_score > rhs.m_score);
      }

      /** \brief Assignment operator.
       *
       * \param rhs Right-hand side operand.
       * \return Reference to this.
       */
      HighScoreEntry& operator=(const HighScoreEntry &rhs)
      {
        m_score = rhs.m_score;
        m_name = rhs.m_name;
        m_newest = rhs.m_newest;
        return *this;
      }
  };

  /** \brief High scores.
   */
  class HighScores
  {
    private:
      /** Map of high scores. */
      std::vector<HighScoreEntry> m_entries;

    public:
      /** \brief Constructor. */
      HighScores() { }

      /** \brief Destructor. */
      ~HighScores() { }

    public:
      /** \brief Append a high score entry.
       *
       * \param score Score.
       * \param name Name.
       * \param newest Newest.
       */
      void add(int score, const std::wstring &name, bool newest);

      /** \brief Check if a high score warrants entry.
       *
       * \param score Score.
       */
      bool check(int score);

    public:
      /** \brief Accessor.
       *
       * \return Array of high scores.
       */
      const std::vector<HighScoreEntry>& getEntries()
      {
        std::sort(m_entries.begin(), m_entries.end());
        return m_entries;
      }
  };
}

#endif
