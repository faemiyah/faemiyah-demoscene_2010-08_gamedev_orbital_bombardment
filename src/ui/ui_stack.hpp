#ifndef UI_UI_STACK_HPP
#define UI_UI_STACK_HPP

#include "ui/fps_counter.hpp"
#include "ui/event_key.hpp"
#include "ui/event_misc.hpp"
#include "ui/event_mouse_button.hpp"
#include "ui/event_mouse_motion.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

namespace gfx
{
  class SurfaceScreen;
}

namespace ui
{
  class UiState;

  /** \brief The UI state stack.
   *
   * All new UI state handlers enter this stack, preferably on top of it. They
   * remain there until they decide to die by their own volition.
   */
  class UiStack
  {
    public:
      /** Handy typedef */
      typedef boost::shared_ptr<UiState> value_type;

    private:
      /** List (stack) of all UI states. */
      std::list<value_type> m_state_list;

      /** Screen that's being updated. */
      gfx::SurfaceScreen &m_screen;

      /** FPS calulator / bookkeeper. */
      FpsCounter m_fps_counter;

      /** Thread to suspend into. */
      boost::scoped_ptr<boost::thread> m_thread;

    protected:
      /** UI event list. */
      std::list<EventKey> m_events_key;

      /** UI event list. */
      std::list<EventMisc> m_events_misc;

      /** UI event list. */
      std::list<EventMouseButton> m_events_mouse_button;

      /** UI event list. */
      std::list<EventMouseMotion> m_events_mouse_motion;

    public:
      /** \brief Default constructor.
       *
       * \param scr Screen to attach to.
       * \param fps Maximum response frequency (Hz).
       */
      UiStack(gfx::SurfaceScreen &scr, unsigned fps);

      /** \brief Destructor.
       */
      virtual ~UiStack();

    protected:
      /** \brief Virtual method for default keyboard events.
       *
       * This method handles default keyboard events. May be overridden, but this should not be necessary in
       * most cases.
       *
       * Note that the default key handler is executed before pushing the event to the stack, and will happen
       * prior to handling any other events instead of after.
       *
       * \param ev Event struct.
       * \return True if handled, false if not.
       */
      virtual bool handleEventKey(const EventKey &ev);

    private:
      /** \brief Get all pending events into the event lists.
       *
       * Will handle the events on the states, topmost first.
       */
      void handleEvents();

      /** \brief Reinstate first UI state in list.
       *
       * \return True if there was something to reinstate, false if not.
       */
      bool reinstateFirst();

    public:
      /** \brief Join Ui stack thread (that is not yet joined).
       *
       * It is probably not necessary to call this, as UI stacks are joined
       * upon destruction.
       */
      void join();

      /** \brief Kill the topmost state.
       *
       * This state will stand down after the next update() call.
       */
      void killTopState();

      /** \brief Push a state onto the stack.
       *
       * \param state State to push.
       */
      void pushState(value_type &state);

      /** \brief Push a state after the current one onto the stack.
       *
       * States can use this to push new UI states to the stack to be executed
       * immediately after they themselves exit.
       *
       * \param state State to push.
       */
      void pushStateAfter(value_type &state);

      /** \brief Save a screenshot.
       *
       * \param type File ending type of screenshot to save.
       */
      void saveScreen(const char *type);

      /** \brief Suspend.
       *
       * Suspend the UI stack into a thread.
       */
      void suspend();

      /** \brief Switch places of two top states.
       *
       * Useful when switching between, for example, a menu and a game state.
       */
      void switchTopStates();

    private:
      /** \brief Run until the last UI state is removed.
       *
       * All UI states run in a three-phase loop wherein they first wait for
       * input, then are updated, and then rendered if there's still time.
       *
       * The call order of events is: misc, mouse motion, key, mouse button.
       *
       * The states may decide to quit at any point of this cycle by either
       * returning false (from Update or Render) or setting their alive state
       * to false (input). They will be removed before the next phase would
       * begin.
       */
      void run();

    public:
      /** \brief Framerate accessor.
       *
       * \return Current framerate.
       */
      unsigned getFps() const
      {
        return m_fps_counter.getCurrentFps();
      }

      /** \brief Frame count accessor.
       *
       * \return Current frame count.
       */
      uint64_t getFrameCount() const
      {
        return m_fps_counter.getFrameCount();
      }

      /** \brief Get the number of states.
       *
       * \return Number of states currently.
       */
      unsigned getNumStates() const
      {
        return static_cast<unsigned>(m_state_list.size());
      }

      /** \brief Accessor.
       *
       * \return Reference to the screen (to render to).
       */
      gfx::SurfaceScreen& getScreen()
      {
        return m_screen;
      }

      /** \brief Accessor.
       *
       * \return Const reference to the screen (to render to).
       */
      const gfx::SurfaceScreen& getScreen() const
      {
        return m_screen;
      }

      /** \brief Get the topmost state.
       *
       * \return Topmost state pointer.
       */
      const UiState* getStateTop() const
      {
        return m_state_list.empty() ? NULL : m_state_list.front().get();
      }

      /** \brief Wrapper for push state after.
       *
       * \param state State to push.
       */
      void pushStateAfter(UiState *state)
      {
        value_type container(state);
        this->pushStateAfter(container);
      }

      /** \brief Wrapper for push state.
       *
       * \param state State to push.
       */
      void pushState(UiState *state)
      {
        value_type container(state);
        this->pushState(container);
      }

      /** \brief Wrapper for run.
       *
       * \param state Initial state to add.
       */
      void run(value_type &state) 
      {
        this->pushState(state);
        this->run();
      }

      /** \brief Wrapper for run.
       *
       * \param state Initial state to add.
       */
      void run(UiState *state) 
      {
        this->pushState(state);
        this->run();
      }
  };
}

#endif
