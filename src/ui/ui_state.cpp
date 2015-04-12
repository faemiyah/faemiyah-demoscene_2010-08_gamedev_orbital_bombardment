#include "ui/ui_state.hpp"

#include "ui/generic.hpp"

using namespace ui;

UiState::UiState() :
  m_alive(true) { }

bool UiState::handleKey(const EventKey &ev, UiStack &st)
{
  boost::ignore_unused_variable_warning(ev);
  boost::ignore_unused_variable_warning(st);

  return false;
}

bool UiState::handleMisc(const EventMisc &ev, UiStack &st)
{
  boost::ignore_unused_variable_warning(ev);
  boost::ignore_unused_variable_warning(st);

  return false;
}

bool UiState::handleMouseButton(const EventMouseButton &ev, UiStack &st)
{
  boost::ignore_unused_variable_warning(ev);
  boost::ignore_unused_variable_warning(st);

  return false;
}

bool UiState::handleMouseMotion(const EventMouseMotion &ev, UiStack &st)
{
  boost::ignore_unused_variable_warning(ev);
  boost::ignore_unused_variable_warning(st);

  return false;
}

void UiState::reinstate(UiStack &st)
{
  boost::ignore_unused_variable_warning(st);

  key_repeat_off();
}

