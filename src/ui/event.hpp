#ifndef UI_EVENT_HPP
#define UI_EVENT_HPP

#include "math/vec.hpp"

namespace ui
{
  /** Event type. */
  enum EventType
  {
    /** Key event. */
    KEY,

    /** Miscellaneous event. */
    MISC,

    /** Mouse button event. */
    MOUSE_BUTTON,

    /** Mouse motion event. */
    MOUSE_MOTION,

    /** Quit event. */
    QUIT
  };

  /** \brief Generic event class.
   *
   * Actually contains all the required elements for all the events. The
   * subsequent events implement a specific type for virtual method call
   * reasons.
   */
  class Event
  {
    protected:
      /** Type of this event. */
      EventType m_type;

      /** Coordinates (if applicable). */
      math::vec2i m_coord;

      /** Delta coordinates (if applicable). */
      math::vec2i m_delta;

      /** Button, keycode or special code (depends on the event). */
      int m_code;

      /** Modifier flags (if applicable). */
      uint8_t m_mod;

      /** Is this a press (instead of a raise)? */
      bool m_press;

    protected:
      /** \brief Constructor.
       *
       * \param ptype Event type.
       */
      Event(EventType ptype)
      {
        m_type = ptype;
      }

    public:
      /** \brief Accessor.
       *
       * \return Event code of this.
       */
      int getCode() const
      {
        return m_code;
      }

      /** \brief Get the coordinates of this.
       *
       * \return Vector reference.
       */
      const math::vec2i& getCoord() const
      {
        return m_coord;
      }

      /** \brief Get the coordinates of this.
       *
       * \return Vector reference.
       */
      const math::vec2i& getDelta() const
      {
        return m_delta;
      }

      /** \brief Accessor.
       *
       * \return Event type of this.
       */
      EventType getType() const
      {
        return m_type;
      }

      /** \brief Tell if this is a press event.
       * 
       * Note that this might not apply unless this is a key or button event.
       *
       * \return True if yes, false if no.
       */
      bool isPress() const
      {
        return m_press;
      }

      /** \brief Tell if this is a raise event.
       * 
       * Note that this might not apply unless this is a key or button event.
       *
       * \return True if yes, false if no.
       */
      bool isRaise() const
      {
        return !m_press;
      }
  };
}

#endif

