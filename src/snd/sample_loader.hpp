#ifndef SND_SAMPLE_LOADER_HPP
#define SND_SAMPLE_LOADER_HPP

#include "data/loader_settings.hpp"

namespace snd
{
  /** \brief Mesh loader settings. */
  class SampleLoader :
    public data::LoaderSettings
  {
    private:
      /** Flat scaling to use. */
      float m_gain;

    public:
      /** \brief Default constructor. */
      SampleLoader() :
        m_gain(1.0f) { }

    public:
      /** \brief Get gain.
       *
       * \return Gain set.
       */
      float getGain() const
      {
        return m_gain;
      }

      /** \brief Set gain.
       *
       * \param op Gain.
       */
      SampleLoader& gain(float op)
      {
        m_gain = op;
        return *this;
      }
  };
}

#endif
