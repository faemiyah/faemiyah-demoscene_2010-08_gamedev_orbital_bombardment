#include "ui/fps_counter.hpp"

#include "thr/generic.hpp"

#include <sstream>

using namespace ui;

FpsCounter::FpsCounter(unsigned maxrate) :
  m_frame_counter(0),
  m_last_ticks(thr::usec_get_timestamp()),
  m_excess_ticks(0.0)
{
  this->assignFramerates(maxrate, std::max(maxrate / 4, 1U));
}

FpsCounter::FpsCounter(unsigned maxrate, unsigned minrate) :
  m_frame_counter(0),
  m_last_ticks(thr::usec_get_timestamp()),
  m_excess_ticks(0.0)
{
  this->assignFramerates(maxrate, minrate);
}

void FpsCounter::appendFrame(uint64_t stamp)
{
  m_frame_list.push_front(stamp);

  while(m_frame_list.back() <= stamp - 1000000)
  {
    m_frame_list.pop_back();
  }
}

void FpsCounter::assignFramerates(unsigned maxrate, unsigned minrate)
{
  if(maxrate <= 0)
  {
    std::ostringstream str;
    str << "invalid maximum framerate " << maxrate << " maxrate";
    BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
  }
  if(minrate > maxrate)
  {
    std::ostringstream str;
    str << "minimum framerate can not be larger than maximum framerate " <<
      minrate << " minrate";
    BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
  }
  if(minrate <= 0)
  {
    std::ostringstream str;
    str << "invalid minimum framerate " << minrate << " minrate";
    BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
  }

  m_desired_framerate = maxrate;
  m_minimum_framerate = minrate;
  m_tick_us = 1000000.0 / static_cast<double>(maxrate);
}

unsigned FpsCounter::check(bool allow_suspend)
{
  uint64_t current_ticks = this->getCurrentTicks();

  if(allow_suspend && (m_excess_ticks < 0.0))
  {
    thr::usec_sleep(static_cast<uint64_t>(-m_excess_ticks));
    current_ticks = this->getCurrentTicks();
  }

  // Below minimum FPS must draw in any case
  if(this->getCurrentFps() <= m_minimum_framerate)
  {
    ++m_frame_counter;
    // Must modify excess tick numeber to prevent later delay rush.
    this->appendFrame(current_ticks);
    m_excess_ticks = m_tick_us;
    return 2;
  }

  // Fallback to check with normal methods.
  if(m_excess_ticks >= 0.0)
  {
    ++m_frame_counter;
    m_excess_ticks -= m_tick_us;
    //std::cout << m_excess_ticks << '\n';
    if(m_excess_ticks > 0.0)
    {
      return 1;
    }
    this->appendFrame(current_ticks);
    return 2;
  }
  return 0;
}

uint64_t FpsCounter::getCurrentTicks()
{
  uint64_t ret = thr::usec_get_timestamp();
  m_excess_ticks += static_cast<double>(ret - m_last_ticks);
  m_last_ticks = ret;
  return ret;
}

void FpsCounter::reset()
{
  this->getCurrentTicks();

  m_frame_list.clear();
  m_excess_ticks = 0.0;
  m_frame_counter = 0;
}

