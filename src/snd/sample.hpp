#ifndef SND_SAMPLE_HPP
#define SND_SAMPLE_HPP

#include "data/store.hpp"
#include "snd/generic.hpp"
#include "snd/sample_loader.hpp"

#include <string>

namespace snd
{
  /** \brief One OpenAL sample.
   */
  class Sample :
    public data::Storable<Sample, SampleLoader>
  {
    private:
      /** Buffer for the wave. */
      ALuint m_buffer;

      /** Last play time. */
      uint64_t m_last_play_time;

      /** Format (OpenAL style). */
      ALenum m_format;

      /** Frequency (in Hz). */
      unsigned m_freq;

      /** Size (in bytes). */
      uint32_t m_size;

      /** Sound level. */
      float m_gain;

    public:
      /** \brief Accessor.
       *
       * \return Buffer of this.
       */
      ALuint getBuffer() const
      {
        return m_buffer;
      }

      /** \brief Accessor.
       *
       * \return Sound level.
       */
      float getGain() const
      {
        return m_gain;
      }

      /** \brief Set the level.
       *
       * \param op New level.
       */
      void setGain(float op)
      {
        m_gain = op;
      }

    public:
      /** \brief Empty constructor. */
      Sample();

      /** \brief Constructor.
       *
       * \param pfname Filename to load.
       * \param loader Loader settings.
       */
      Sample(const boost::filesystem::path &pfname, const SampleLoader &loader = SampleLoader());

      /** \brief Constructor.
       *
       * \param data Data count.
       * \param size Size of data.
       * \param channels Number of channels.
       * \param freq Frequency.
       * \param level General level for this sample.
       */
      Sample(const uint8_t *data, unsigned size, unsigned channels, unsigned freq, float level = 1.0f);

      /** \brief Destructor. */
      ~Sample();

    private:
      /** \brief Generate the buffer in this sample.
       */
      void generateBuffer();

    protected:
      /** \brief Release all resources reserved by this.
       */
      void unreserve();

    public:
      /** \brief Check if it's okay to play shis sample.
       *
       * Samples may be denied if they're spammed, that is, constantly
       * requested again and again.
       */
      bool canPlay();

      /** \brief Load a sample.
       *
       * Throws an exception on error.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      void load(const boost::filesystem::path &pfname, const SampleLoader &loader);

    public:
      /** \brief Create implementation.
       *
       * Despite being public, not to be called by user.
       *
       * \param pfname File to load.
       * \param loader Loader settings.
       * \return Container for all created instances.
       */
      static container_type createImplementation(const boost::filesystem::path &pfname,
          const SampleLoader &loader);

    public:
      /** \cond */
      friend std::ostream& operator<<(std::ostream &lhs, const Sample &rhs);
      /** \endcond */
  };

  /** \brief Output sample to a stream.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Stream after input.
   */
  std::ostream& operator<<(std::ostream &lhs, const Sample &rhs);

  /** Convenience typedef. */
  typedef boost::shared_ptr<Sample> SampleSptr;
}

#endif
