#include "ob_console_state.hpp"

#include "ui/ui_stack.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"
#include "ob_menu_state.hpp"

using namespace ob;

ConsoleState::ConsoleState(ui::Console &op) :
  ui::ConsoleState(op, false) { }

void ConsoleState::update(ui::UiStack &st)
{
  if(glob->isDone())
  {
    st.pushStateAfter(new MenuState(glob->getMenuMain()));
    this->die();
  }
  else
  {
    this->ui::ConsoleState::update(st);
  }
}

