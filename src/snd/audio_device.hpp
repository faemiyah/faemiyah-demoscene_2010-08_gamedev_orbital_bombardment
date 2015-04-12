#ifndef SND_AUDIO_DEVICE_HPP
#define SND_AUDIO_DEVICE_HPP

#include "snd/generic.hpp"

#include <string>

namespace snd
{
  /** \brief Class for initializing the audio device.
   *
   * Creating an instance of this will init OpenAL, destroying it will shut
   * it down.
   */
  class AudioDevice
  {
    private:
      /** Device used. */
      ALCdevice *m_device;

      /** Context for device. */
      ALCcontext *m_context;

    public:
      /** \brief Constructor.
       *
       * This will initialize the default audio device.
       */
      AudioDevice();

      /** \brief Constructor.
       *
       * \param dev Device name.
       */
      AudioDevice(const std::string &dev);

      /** \brief Destructor. */
      ~AudioDevice();

    private:
      /** \brief Create a context for the opened device.
       */
      void createContext();
  };
}

#endif
