#include "ui/console_state.hpp"

#include "math/random.hpp"
#include "thr/dispatch.hpp"
#include "ui/console.hpp"
#include "ui/ui_stack.hpp"

using namespace ui;

ConsoleState::ConsoleState(Console &pconsole, bool allow_exit) :
  m_console(pconsole),
  m_allow_exit_program(allow_exit) { }

bool ConsoleState::handleKey(const EventKey &ev, UiStack &st)
{
  if(ev.isPress())
  {
    switch(ev.getCode())
    {
      case SDLK_ESCAPE:
      case SDLK_WORLD_7:
        // Only allow exit if we're not the only state.
        if((st.getStateTop() != this) || (st.getNumStates() > 1) || m_allow_exit_program)
        {
          this->die();
        }
        break;

      default:
        if(0 > m_console.getInput().process(ev))
        {
          m_console.execute();
        }
        break;
    }
  }

  return true;
}

void ConsoleState::reinstate(UiStack &st)
{
  boost::ignore_unused_variable_warning(st);

  key_repeat_on();
}

void ConsoleState::render(UiStack &st, gfx::SurfaceScreen &screen)
{
  boost::ignore_unused_variable_warning(st);

  screen.clear(true, true);
  m_console.render(screen);
  screen.update();
}

void ConsoleState::update(UiStack &st)
{
  boost::ignore_unused_variable_warning(st);
}

