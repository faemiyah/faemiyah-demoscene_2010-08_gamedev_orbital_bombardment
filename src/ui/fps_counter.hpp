#ifndef UI_FPS_COUNTER_HPP
#define UI_FPS_COUNTER_HPP

#include "ui/generic.hpp"

#include <list>

namespace ui
{
  /** \brier Class to keep track of the current framerate<
   *
   * TODO: Should this be adjusted to work with something else than SDL?
   */
  class FpsCounter
  {
    private:
      /** Frame counter of frames (supposed to be) rendered. */
      uint64_t m_frame_counter;

      /** Ticks when last checked. */
      uint64_t m_last_ticks;;

      /** \brief The desired framerate.
       *
       * Limited to integer values for clarity of framerate calculation.
       */
      unsigned m_desired_framerate;

      /** \brief Minimum framerate.
       *
       * The framerate draw request calculation will not go below this number,
       * instead causing slowdown.
       */
      unsigned m_minimum_framerate;

      /** Microseconds per tick. */
      double m_tick_us;

      /** Excess ticks to use. */
      double m_excess_ticks;

      /** Listing of latest rendered frames to keep track of the framerate. */
      std::list<uint64_t> m_frame_list;

    public:
      /** \brief Default constructor.
       *
       * Per default behavior, the minimum (slowdown) framerate is specified to
       * one fourth of the maximum framerate.
       * 
       * \param maxrate Maximum framerate.
       */
      FpsCounter(unsigned maxrate);

      /** \brief Alternate constructor.
       *
       * \param maxrate Maximum framerate.
       * \param minrate Maximum framerate.
       */
      FpsCounter(unsigned maxrate, unsigned minrate);

    private:
      /** \brief Append a frame.
       *
       * \param stamp Current timestamp.
       */
      void appendFrame(uint64_t stamp);

      /** \brief Assign given framerates and throw an exception on error.
       *
       * \param maxrate Maximum framerate.
       * \param minrate Minimum framerate.
       */
      void assignFramerates(unsigned maxrate, unsigned minrate);

      /** \brief Get the current ticks in a wrap-safe manner.
       *
       * \return Tick count as long.
       */
      uint64_t getCurrentTicks();

    public:
      /** \brief Check if a frame should be drawn.
       *
       * If there is time and it is allowed, this method will suspend.
       *
       * The return value of 0 is system inaccuracies, and perfectly reasonable.
       * 
       * \param allow_suspend True to allow sleeping.
       * \return 0: do nothing, 1: essentials, 2: everything.
       */
      unsigned check(bool allow_suspend);

      /** \brief Reset the frame calculation.
       *
       * Should be done after a long period of inactivity in framerate
       * calculation to prevent catch-up behavior.
       */
      void reset();

    public:
      /** \brief Get the current framerate.
       *
       * \return Current framerate as integer.
       */
      unsigned getCurrentFps() const
      {
        return static_cast<unsigned>(m_frame_list.size());
      }

      /** \brief Returns the number of frames this has allowed to be rendered.
       *
       * \return Frame count.
       */
      uint64_t getFrameCount() const
      {
        return m_frame_counter;
      }

      /** \brief Get the target framerate.
       *
       * \return Target fps.
       */
      unsigned getTargetFps() const
      {
        return m_desired_framerate;
      }
  };
}

#endif
