#ifndef UI_EVENT_MISC_HPP
#define UI_EVENT_MISC_HPP

#include "ui/event.hpp"

namespace ui
{
  /** \brief Miscellaneous event.
   */
  class EventMisc : public Event
  {
    public:
      /** \brief Constructor.
       *
       * \param pcode Event code.
       */
      EventMisc(EventType pcode) :
        Event(pcode) { }
  };
}

#endif

