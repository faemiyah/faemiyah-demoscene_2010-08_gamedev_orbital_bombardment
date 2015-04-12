#include "ob_high_scores.hpp"

/** Number of high scores allowed. */
static const unsigned HIGH_SCORES_MAX = 32;

using namespace ob;

void HighScores::add(int score, const std::wstring &name, bool newest)
{
  m_entries.push_back(HighScoreEntry(score, name, newest));
  std::push_heap(m_entries.begin(), m_entries.end());
  if(m_entries.size() > HIGH_SCORES_MAX)
  {
    std::pop_heap(m_entries.begin(), m_entries.end());
  }
  std::sort(m_entries.begin(), m_entries.end());
}

bool HighScores::check(int score)
{
  if (score == 0) return false;
  if(m_entries.size() < HIGH_SCORES_MAX)
  {
    return true;
  }
  return (m_entries.back().getScore() < score);
}

