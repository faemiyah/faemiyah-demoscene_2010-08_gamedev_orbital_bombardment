#ifndef UI_CONSOLE_STATE_HPP
#define UI_CONSOLE_STATE_HPP

#include "ui/ui_state.hpp"

namespace ui
{
  class Console;
  class Dispatcher;

  /** \brief Console UI state.
   *
   * This is intendended as the base UI state for use with the console. It
   * includes the basic operations for input, etc.
   */
  class ConsoleState :
    public UiState
  {
    private:
      /** Referenced console. */
      Console &m_console;

      /** Allow quit as last state? */
      bool m_allow_exit_program;

    public:
      /** \brief Constructor.
       *
       * \param pconsole Console to attach to.
       * \param allow_exit Allow ESC if this is the only state?
       */
      ConsoleState(Console &pconsole, bool allow_exit = true);

      /** \brief Destructor. */
      virtual ~ConsoleState() { }

    public:
      /** \cond */
      virtual bool handleKey(const EventKey &ev, UiStack &st);
      virtual void reinstate(UiStack &st);
      virtual void render(UiStack &st, gfx::SurfaceScreen &screen);
      virtual void update(UiStack &st);
      /** \endcond */

    public:
      /** \brief Render task.
       */
      void taskRender(gfx::SurfaceScreen &screen) const;
  };
}

#endif
