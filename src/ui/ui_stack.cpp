#include "ui/ui_stack.hpp"

#include "gfx/surface_screen.hpp"
#include "ui/ui_state.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <sstream>

using namespace ui;

/** \brief Convert SDL modifiers to key event modifiers.
 *
 * \param op SDL modifiers.
 * \return Key event modifiers.
 */
static uint8_t sdl_mod_to_mod(SDLMod op)
{
  uint8_t ret = 0;

  if(op & (KMOD_LCTRL | KMOD_RCTRL))
  {
    ret |= EventKey::MODIFIER_CTRL;
  }
  if(op & (KMOD_LSHIFT | KMOD_RSHIFT))
  {
    ret |= EventKey::MODIFIER_SHIFT;
  }

  return ret;
}

/** \brief Render task.
 *
 * Runs render on the topmost state and checks for OpenGL errors.
 *
 * \param ui_stack Stack.
 * \param ui_state State.
 */
static void ui_stack_render_task(UiStack *ui_stack, UiState *ui_state)
{
  ui_state->render(*ui_stack, ui_stack->getScreen());

  gfx::check_opengl_errors();
}

UiStack::UiStack(gfx::SurfaceScreen &scr, unsigned fps) :
  m_screen(scr),
  m_fps_counter(fps) { }

UiStack::~UiStack()
{
  this->join();
}

bool UiStack::handleEventKey(const EventKey &ev)
{
  if(ev.isPress())
  {
    if(ev.hasModCtrl() && ev.hasModShift())
    {
      switch(ev.getCode())
      {
        case SDLK_F10:
          m_events_misc.push_back(EventMisc(QUIT));
          return true;

        case SDLK_F11:
          this->saveScreen("jpg");
          return true;

        case SDLK_F12:
          this->saveScreen("png");
          return true;

        default:
          break;
      }
    }
  }

  return false;
}

void UiStack::handleEvents()
{
  // Clear old events before acquiring new ones.
  m_events_key.clear();
  m_events_misc.clear();
  m_events_mouse_motion.clear();
  m_events_mouse_button.clear();

  // Gather all events to execute them in a correct order.
  SDL_Event sevent;
  while(SDL_PollEvent(&sevent) != 0)
  {
    switch(sevent.type)
    {
      case SDL_QUIT:
        m_events_misc.push_back(EventMisc(QUIT));
        break;

      case SDL_KEYDOWN:
        {
          uint8_t mod = sdl_mod_to_mod(sevent.key.keysym.mod);
  
          if(SDL_EnableUNICODE(-1) && (32 <= sevent.key.keysym.unicode))
          {
            m_events_key.push_back(EventKey(sevent.key.keysym.unicode, true, mod | EventKey::MODIFIER_UNICODE));
          }
          else
          {
            EventKey key_event(sevent.key.keysym.sym, true, mod);

            if(!this->handleEventKey(key_event))
            {
              m_events_key.push_back(key_event);
            }
          }
        }
        break;

      case SDL_KEYUP:
        {
          EventKey key_event(sevent.key.keysym.sym, false, sdl_mod_to_mod(sevent.key.keysym.mod));

          if(!this->handleEventKey(key_event))
          {
            m_events_key.push_back(key_event);
          }
        }
        break;

      case SDL_MOUSEMOTION:
        m_events_mouse_motion.push_back(
          EventMouseMotion(sevent.motion.x, sevent.motion.y, sevent.motion.xrel, sevent.motion.yrel));
        break;

      case SDL_MOUSEBUTTONDOWN:
        m_events_mouse_button.push_back(
          EventMouseButton(sevent.button.button, true, sevent.button.x, sevent.button.y));
        break;

      case SDL_MOUSEBUTTONUP:
        m_events_mouse_button.push_back(
          EventMouseButton(sevent.button.button, false, sevent.button.x, sevent.button.y));
        break;

      default:
        // Unknown event, silently ignore.
        break;
    }
  }

  // If a state returns false, it wants to say it didn't handle an event
  // correctly, in which case the handling is passed onto an event in a
  // deeper level. If no-one returns true to tell they handled the event,
  // it's silently forgotten.
  BOOST_FOREACH(EventMisc &vv, m_events_misc)
  {
    BOOST_FOREACH(value_type &ww, m_state_list)
    {
      if(ww->handleMisc(vv, *this))
      {
        break;
      }
    }
  }
  BOOST_FOREACH(EventMouseMotion &vv, m_events_mouse_motion)
  {
    BOOST_FOREACH(value_type &ww, m_state_list)
    {
      if(ww->handleMouseMotion(vv, *this))
      {
        break;
      }
    }
  }
  BOOST_FOREACH(EventKey &vv, m_events_key)
  {
    BOOST_FOREACH(value_type &ww, m_state_list)
    {
      if(ww->handleKey(vv, *this))
      {
        break;
      }
    }
  }
  BOOST_FOREACH(EventMouseButton &vv, m_events_mouse_button)
  {
    BOOST_FOREACH(value_type &ww, m_state_list)
    {
      if(ww->handleMouseButton(vv, *this))
      {
        break;
      }
    }
  }
}

void UiStack::join()
{
  if(NULL != m_thread.get())
  {
    m_thread->join();
    m_thread.reset();
  }
}

void UiStack::killTopState()
{
  if(!m_state_list.empty())
  {
    m_state_list.front()->die();
  }
}

void UiStack::pushState(value_type &state)
{
  m_state_list.push_front(state);
  state->reinstate(*this);
}

void UiStack::pushStateAfter(value_type &state)
{
  if(m_state_list.empty())
  {
    this->pushState(state);
    return;
  }
  std::list<value_type>::iterator iter = m_state_list.begin();
  ++iter;
  m_state_list.insert(iter, state);
  // No reinstate since not on top.
}

bool UiStack::reinstateFirst()
{
  if(m_state_list.empty())
  {
    return false;
  }
  (m_state_list.front())->reinstate(*this);
  return true;
}

void UiStack::run() 
{
  while(!m_state_list.empty())
  {
    unsigned state = m_fps_counter.check(true);
    // Timing failure.
    if(state <= 0)
    {
      continue;
    }

    UiState *topst = m_state_list.front().get();

    thr::wait_privileged(&UiStack::handleEvents, this);
    // After handling the state may have decided it has died.
    if(!topst->isAlive())
    {
      m_state_list.pop_front();
      this->reinstateFirst();
      continue;
    }

    thr::wait(&UiState::update, topst, boost::ref(*this));
    // State may have decided to die also during update.
    if(!topst->isAlive())
    {
      m_state_list.pop_front();
      this->reinstateFirst();
    }
    // Pass onto render implementation if we are alive and have time.
    else if(1 < state)
    {
      thr::wait_privileged(ui_stack_render_task, this, topst);
    }
  }

  // Destructing UI stack should take threading with it.
  thr::thr_quit();
}

void UiStack::saveScreen(const char *type)
{
  std::ostringstream sstr;

  boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
  boost::gregorian::date current_date(now.date());
  boost::posix_time::time_duration current_time(now.time_of_day());

  sstr.fill('0');
  sstr.width(4);
  sstr << "screenshot_" << current_date.year() << '-';

  sstr.width(2);
  sstr << current_date.month().as_number() << '-' << current_date.day().as_number() << '_' <<
    current_time.hours() << 'h' << current_time.minutes() << 'm' << current_time.seconds() << "s." << type;

  m_screen.save(sstr.str());
}

void UiStack::suspend()
{
  BOOST_ASSERT(NULL == m_thread.get());

  m_thread.reset(new boost::thread(boost::bind(&UiStack::run, this)));
}

void UiStack::switchTopStates()
{
  if(m_state_list.size() >= 2)
  {
    std::list<value_type>::iterator aa = m_state_list.begin(), bb = aa;
    ++bb;
    value_type tmp = *bb;
    *bb = *aa;
    *aa = tmp;
    m_state_list.front()->reinstate(*this);
  }
}

