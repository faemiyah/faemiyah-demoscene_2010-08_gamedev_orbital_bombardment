#include "snd/audio_device.hpp"

#include "defaults.hpp"

#include <sstream>

using namespace snd;

AudioDevice::AudioDevice() :
  m_device(alcOpenDevice(NULL)),
  m_context(NULL)
{
  const ALCchar *dev = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
#if defined(DEBUG)
  {
    std::cout << "Using audio device: " << dev << std::endl;
  }
#endif

  if(NULL == m_device)
  {
    std::stringstream sstr;
    sstr << "could not open default audio device '" << dev << "' (" <<
      alGetError()  << "): ";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  this->createContext();

}

AudioDevice::AudioDevice(const std::string &pfname) :
  m_device(alcOpenDevice(pfname.c_str())),
  m_context(NULL)
{
  if(NULL == m_device)
  {
    std::stringstream sstr;
    sstr << "could not open audio device '" << pfname << "' (" <<
      alGetError() << ")";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  this->createContext();
}

AudioDevice::~AudioDevice()
{
  if(m_context)
  {
    alcDestroyContext(m_context);
  }
  if(m_device)
  {
    if(ALC_TRUE != alcCloseDevice(m_device))
    {
      std::stringstream sstr;
      sstr << "could not close audio device (" << alGetError() << ")";
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }
}

void AudioDevice::createContext()
{
  m_context = alcCreateContext(m_device, NULL);
  if(m_context == NULL)
  {
    alcCloseDevice(m_device);
    m_device = NULL;
    std::stringstream sstr;
    sstr << "could not create context for device";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  if(ALC_FALSE == alcMakeContextCurrent(m_context))
  {
    alcDestroyContext(m_context);
    m_context = NULL;
    alcCloseDevice(m_device);
    m_device = NULL;
    std::stringstream sstr;
    sstr << "could not make context current";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
}

