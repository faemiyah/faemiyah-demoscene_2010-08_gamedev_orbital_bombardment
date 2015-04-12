#ifndef OB_CONSOLE_STATE_HPP
#define OB_CONSOLE_STATE_HPP

#include "ui/console_state.hpp"

namespace ob
{
  class ConsoleState :
    public ui::ConsoleState
  {
    public:
      /** \brief Constructor. */
      ConsoleState(ui::Console &op);

      /** \brief Destructor. */
      virtual ~ConsoleState() { }

    public:
      /** \cond */
      virtual void update(ui::UiStack&);
      /** \endcond */
  };
}

#endif
