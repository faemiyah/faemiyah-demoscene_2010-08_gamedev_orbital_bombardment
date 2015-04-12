#ifndef OB_HIGH_SCORE_STATE_HPP
#define OB_HIGH_SCORE_STATE_HPP

#include "ui/ui_state.hpp"
#include "ob_appearing_string.hpp"

namespace ob
{
  /** \brief Menu UI state.
  */
  class HighScoreState :
    public ui::UiState
  {
    private:
      /** High score list content. */
      std::vector<AppearingStringSptr> m_names;

      /** High score list content. */
      std::vector<AppearingStringSptr> m_scores;

      /** Current offset. */
      float m_offset;

      /** Current movement. */
      float m_movement;

      /** Movement trigger delay. */
      int m_trigger;

      /** Full height. */
      float m_full_height;

      /** Command down. */
      bool m_command_down;

      /** Command up. */
      bool m_command_up;

      /** Index of the newest highscore entry. */
      int m_newest_entry_ind;

    public:
      /** \brief Constructor.
       *
       * \param offset Starting offset (default: 0.0f).
       */
      HighScoreState(float offset = 0.0f);

      /** \brief Destructor. */
      virtual ~HighScoreState() { }

    public:
      /** \cond */
      virtual bool handleKey(const ui::EventKey &ev, ui::UiStack &st);
      virtual void render(ui::UiStack &st, gfx::SurfaceScreen &screen);
      virtual void update(ui::UiStack &st);
      /** \endcond */

    public:
      /** \brief Render task.
      */
      static void formatHighScore(std::stringstream &sstr, int score);
  };
}

#endif
