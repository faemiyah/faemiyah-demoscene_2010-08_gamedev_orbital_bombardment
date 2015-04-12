#ifndef UI_EVENT_MOUSE_BUTTON_HPP
#define UI_EVENT_MOUSE_BUTTON_HPP

#include "ui/event.hpp"

namespace ui
{
  /** \brief Mouse button event.
   */
  class EventMouseButton : public Event
  {
    public:
      /** \brief Constructor.
       *
       * \param pcode Keycode.
       * \param ppress Press flag.
       */
      EventMouseButton(int pcode, bool ppress, int px, int py) :
        Event(MOUSE_BUTTON)
      {
        m_code = pcode;
        m_press = ppress;
        m_coord = math::vec2i(px, py);
      }
  };
}

#endif

