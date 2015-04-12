#include "snd/source.hpp"

#include "snd/sample.hpp"

#include <sstream>

using namespace snd;

Source::Source() :
  m_source(0)
{
  alGenSources(1, &m_source);
  if(0 == m_source)
  {
    std::stringstream sstr;
    sstr << "could not generate source (" << alGetError() << ")";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  alSource3f(m_source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
  alSource3f(m_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(m_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSourcef(m_source, AL_PITCH, 1.0f);
  alSourcef(m_source, AL_GAIN, 1.0f);
}

Source::~Source()
{
  //this->unqueue();

  alDeleteSources(1, &m_source);

#if 0
  ALenum err = alGetError();
  if(err != AL_NO_ERROR)
  {
    std::stringstream sstr;
    sstr << "could not delete source (" << err << ")";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
#endif
}

bool Source::isFree() const
{
  ALint val;
  alGetSourcei(m_source, AL_SOURCE_STATE, &val);
  return ((val == AL_INITIAL) || (val == AL_STOPPED));
}

unsigned Source::numQueuedBuffers() const
{
  ALint ret = -1;
  alGetSourcei(m_source, AL_BUFFERS_QUEUED, &ret);
  if(ret < 0)
  {
    // Should not happen.
    std::stringstream sstr;
    sstr << "error getting number of queued buffers: " << ret;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  return static_cast<unsigned>(ret);
}

void Source::play(const Sample &sample) const
{
  alSource3f(m_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(m_source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
  alSource3f(m_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);

  this->unqueue();

  ALuint buf = sample.getBuffer();
  alSourcei(m_source, AL_BUFFER, static_cast<ALint>(buf));
  alSourcePlay(m_source);
}

void Source::play(const Sample &sample, const math::vec3f &pos) const
{
  alSource3f(m_source, AL_POSITION, pos.x(), pos.y(), pos.z());
  alSource3f(m_source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
  alSource3f(m_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSourcef(m_source, AL_GAIN, sample.getGain() * volume_samples);

  this->unqueue();

  ALuint buf = sample.getBuffer();
  alSourcei(m_source, AL_BUFFER, static_cast<ALint>(buf));
  //alSourceQueueBuffers(m_source, 1, &buf);
  alSourcePlay(m_source);
}

void Source::queue(const Sample &sample) const
{
  ALuint buf = sample.getBuffer();
  alSourceQueueBuffers(m_source, 1, &buf);

  ALint val;
  alGetSourcei(m_source, AL_SOURCE_STATE, &val);
  if(AL_PLAYING != val)
  {
    alSourcePlay(m_source);
  }
}

void Source::stop() const
{
  alSourceStop(m_source);
}

void Source::unqueue() const
{
  ALint val;
  alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &val);

  if(val <= 0)
  {
    return;
  }
  if(val == 1)
  {
    ALuint buf;
    alSourceUnqueueBuffers(m_source, 1, &buf);
    return;
  }
  // Multiple buffers to unqueue, perhaps this is kind of needless.
  ALuint *buffers = new ALuint[val];
  alSourceUnqueueBuffers(m_source, 1, buffers);
  delete[] buffers;
}

ALuint Source::unqueueOne() const
{
  ALint val;
  alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &val);

  if(val <= 0)
  {
    return 0;
  }

  ALuint buf;
  alSourceUnqueueBuffers(m_source, 1, &buf);
  return buf;
}

