#ifndef UI_EVENT_KEY_HPP
#define UI_EVENT_KEY_HPP

#include "ui/event.hpp"

namespace ui
{
  /** \brief Keyboard event.
   */
  class EventKey : public Event
  {
    public:
      /** Modifier flag. */
      static const uint8_t MODIFIER_CTRL = 0x1;

      /** Modifier flag. */
      static const uint8_t MODIFIER_SHIFT = 0x2;

      /** Modifier flag. */
      static const uint8_t MODIFIER_UNICODE = 0x4;

    public:
      /** \brief Constructor.
       *
       * \param pcode Keycode.
       * \param ppress Press flag.
       */
      EventKey(int pcode, bool ppress, uint8_t pmod) :
        Event(KEY)
      {
        m_code = pcode;
        m_press = ppress;
        m_mod = pmod;
      }

    public:
      /** \brief Tell if ctrl modifier is on.
       *
       * \return True if yes, false if no.
       */
      bool hasModCtrl() const
      {
        return (0 != (m_mod & MODIFIER_CTRL));
      }

      /** \brief Tell if shift modifier is on.
       *
       * \return True if yes, false if no.
       */
      bool hasModShift() const
      {
        return (0 != (m_mod & MODIFIER_SHIFT));
      }

      /** \brief Tell if the key event has an unicode character insted of a key symbol.
       *
       * \return True if yes, false if no.
       */
      bool hasUnicode() const
      {
        return (0 != (m_mod & MODIFIER_UNICODE));
      }
  };
}

#endif

