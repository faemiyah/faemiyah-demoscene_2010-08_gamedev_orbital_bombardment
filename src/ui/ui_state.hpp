#ifndef UI_UI_STATE_HPP
#define UI_UI_STATE_HPP

#include "ui/event_key.hpp"
#include "ui/event_misc.hpp"
#include "ui/event_mouse_button.hpp"
#include "ui/event_mouse_motion.hpp"

namespace gfx
{
  class SurfaceScreen;
}

namespace ui
{
  class UiStack;

  /** \brief UI state class.
   *
   * Implements behavior in a certain UI state.
   */
  class UiState
  {
    protected:
      /** Is this alive? */
      bool m_alive;

    public:
      /** \brief Constructor. */
      UiState();

      /** \brief Destructor. */
      virtual ~UiState() { }

    public:
      /** \brief Virtual method for keyboard events.
       *
       * \param ev Event struct.
       * \param st UI stack reference.
       * \return True if handled, false if not.
       */
      virtual bool handleKey(const EventKey &ev, UiStack &st);

      /** \brief Virtual method for misc events.
       *
       * \param ev Event struct.
       * \param st UI stack reference.
       * \return True if handled, false if not.
       */
      virtual bool handleMisc(const EventMisc &ev, UiStack &st);

      /** \brief Virtual method for mouse button events.
       *
       * \param ev Event struct.
       * \param st UI stack reference.
       * \return True if handled, false if not.
       */
      virtual bool handleMouseButton(const EventMouseButton &ev, UiStack &st);

      /** \brief Virtual method for mouse motion events.
       *
       * \param ev Event struct.
       * \param st UI stack reference.
       * \return True if handled, false if not.
       */
      virtual bool handleMouseMotion(const EventMouseMotion &ev, UiStack &st);

      /** \brief Called whenever this UI state gets focus.
       *
       * Called when the UI state gets focus for the first time after being
       * out of focus for some time. This means both getting initialized and
       * being rerevealed from upper UI states.
       *
       * For practical purposes, the number of times this method gets called
       * should have no effect.
       *
       * The default implementation simply turns key repeat off. For the most
       * part, users will probably not want to change this.
       */
      virtual void reinstate(UiStack &st);

      /** \brief Render method definition.
       *
       * Called if there was time to render.
       *
       * \param st UI stack reference.
       * \param pscreen Screen to render to.
       */
      virtual void render(UiStack &st, gfx::SurfaceScreen &pscreen) = 0;

      /** \brief Update method definition.
       *
       * If a state decides to stop executing, it should turn alive flag to
       * false (or call die) before returning from an implementation of this
       * method.
       *
       * \param st UI stack reference.
       */
      virtual void update(UiStack &st) = 0;

    public:
      /** \brief Tell if this is alive.
       *
       * \return True if yes, false if no.
       */
      bool isAlive() const
      {
        return m_alive;
      }

      /** \brief Turn alive flag off.
       */
      void die()
      {
        m_alive = false;
      }
  };
}

#endif
