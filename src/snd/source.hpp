#ifndef SND_SOURCE_HPP
#define SND_SOURCE_HPP

#include "snd/generic.hpp"

namespace snd
{
  class Sample;

  /** \brief One source playing audio.
   */
  class Source
  {
    private:
      /** Source id. */
      ALuint m_source;

    public:
      /** \brief Accessor.
       *
       * \return Source id.
       */
      ALuint getId() const
      {
        return m_source;
      }

    public:
      /** \brief Constructor. */
      Source();

      /** \brief Destructor. */
      ~Source();

    private:
      /** \brief Unqueue buffers previously assigned to this source.
       */
      void unqueue() const;

    public:
      /** \brief Tell if this source is free.
       *
       * A source is free if no sample is playing.
       *
       * \return True if yes, false if not.
       */
      bool isFree() const;

      /** \brief Get the number of queued buffers.
       *
       * \return Number of queued buffers.
       */
      unsigned numQueuedBuffers() const;

      /** \brief Play a sample in this source.
       *
       * Plays a sample here in non-positional manner.
       *
       * If a sound was already being played here, it is replaced.
       *
       * \param sample Sampel to play.
       * \param pos Position of the source.
       */
      void play(const Sample &sample) const;

      /** \brief Play a sample in this source.
       *
       * This is a simple method that simply plays the sound assuming it was
       * located in given position.
       *
       * If a sound was already being played here, it is replaced.
       *
       * \param sample Sample to play.
       * \param pos Position of the source.
       */
      void play(const Sample &sample, const math::vec3f &pos) const;

      /** \brief Queue buffers into this source.
       *
       * \param sample Sample to queue.
       */
      void queue(const Sample &sample) const;

      /** \brief Stop the sample in this source.
       *
       * Self-evident.
       */
      void stop() const;

      /** \brief Unqueue one buffer from this source.
       *
       * \return Unqueued buffer id or zero if none.
       */
      ALuint unqueueOne() const;
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Source> SourceSptr;
}

#endif
