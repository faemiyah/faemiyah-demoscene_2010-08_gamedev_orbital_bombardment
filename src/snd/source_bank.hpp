#ifndef SND_SOURCE_BANK_HPP
#define SND_SOURCE_BANK_HPP

#include "snd/source.hpp"

#include <boost/shared_array.hpp>

namespace snd
{
  class Source;

  /** Source bank holds a certain number of sources.
   *
   * Used as a 'mixer' that can be used to limit simultaneously playing
   * samples.
   */
  class SourceBank
  {
    private:
      /** The source vector. */
      boost::shared_array<Source> m_bank;

      /** Number of elements. */
      unsigned m_count;

      /** Rotating source index. */
      unsigned m_next;

    public:
      /** \brief Constructor.
       *
       * Size should be greater than 1.
       *
       * \param size Size of the array to create.
       */
      SourceBank(unsigned size);

      /** \brief Destructor. */
      ~SourceBank() { }

    private:
      /** \brief Get a free source in this bank.
       *
       * \param force Do not ask nicely, take first slot.
       * \return First free source or NULL if none available.
       */
      Source* findSource(bool force);

    public:
      /** \brief Play a sample.
       *
       * Plays a sample if there is a free source. If not, does nothing.
       *
       * Sample is interpreted as nonpositional.
       *
       * \param sample Sample.
       * \return Source that was used for the sample.
       * \param force Bypass sample saturation check.
       */
      Source* play(Sample &sample, bool force = false);

      /** \brief Play a sample.
       *
       * Plays a sample if there is a free source. If not, does nothing.
       *
       * \param sample Sample.
       * \param pos Position of the sample.
       * \param force Bypass sample saturation check.
       * \return Source that was used for the sample.
       */
      Source* play(Sample &sample, const math::vec3f &pos, bool force = false);
  };
}

#endif
