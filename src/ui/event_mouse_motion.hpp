#ifndef UI_EVENT_MOUSE_MOTION_HPP
#define UI_EVENT_MOUSE_MOTION_HPP

#include "ui/event.hpp"

namespace ui
{
  /** \brief Mouse motion event.
   */
  class EventMouseMotion : public Event
  {
    public:
      /** \brief Constructor.
       * 
       * \param mx Mouse X position.
       * \param my Mouse Y position.
       * \param dx Difference X position.
       * \param dy Difference Y position.
       */
      EventMouseMotion(int mx, int my, int dx, int dy) :
        Event(MOUSE_MOTION)
      {
        m_coord.set(mx, my);
        m_delta.set(dx, dy);
      }

      /** \brief Alternate constructor.
       * 
       * \param pcoord Coordinates.
       * \param pdelta Delta.
       */
      EventMouseMotion(const math::vec2i &pcoord, const math::vec2i &pdelta) :
        Event(MOUSE_MOTION)
      {
        m_coord = pcoord;
        m_delta = pdelta;
      }
  };
}

#endif

