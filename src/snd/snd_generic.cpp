#include "snd/generic.hpp"

#include "snd/audio_device.hpp"
#include "snd/source_bank.hpp"
#include "snd/stream.hpp"

#include <sstream>

using namespace snd;

/** Global audio device. */
static AudioDevice *device = NULL;

/** Global sound bank. */
static SourceBank *bank = NULL;

/** Global sound stream. */
static StreamSptr stream;

/** Global music volume. */
float snd::volume_music = 1.0f;

/** Global sample volume. */
float snd::volume_samples = 1.0f;

Source* snd::play(Sample &sample, bool force)
{
  return bank->play(sample, force);
}

Source* snd::play(Sample &sample, const math::vec3f &pos, bool force)
{
  return bank->play(sample, pos, force);
}

Stream* snd::play_stream(const char *pfname)
{
  return snd::play_stream(std::string(pfname));
}

Stream* snd::play_stream(const std::string &pfname)
{
  if((NULL == device) || (NULL == bank))
  {
    std::stringstream sstr;
    sstr << "sound system not initialized";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  stream = StreamSptr(new Stream(pfname));
  stream->play();
  return stream.get();
}

void snd::set_listener(const math::vec3f &pos, const math::vec3f &dir,
    const math::vec3f &up)
{
  alListener3f(AL_POSITION, pos.x(), pos.y(), pos.z());
  alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);

  float orientation[] = { dir.x(), dir.y(), dir.z(), up.x(), up.y(), up.z() };
  alListenerfv(AL_ORIENTATION, orientation);
}

void snd::set_listener(const math::vec3f &pos, const math::vec3f &dir,
    const math::vec3f &up, const math::vec3f &vel)
{
  alListener3f(AL_POSITION, pos.x(), pos.y(), pos.z());
  alListener3f(AL_VELOCITY, vel.x(), vel.y(), vel.z());

  float orientation[] = { dir.x(), dir.y(), dir.z(), up.x(), up.y(), up.z() };
  alListenerfv(AL_ORIENTATION, orientation);
}

void snd::set_volume_music(float op)
{
  volume_music = op;
}

void snd::set_volume_samples(float op)
{
  volume_samples = op;
}

void snd::snd_init(unsigned size, ALenum distance_model)
{
  if(device || bank)
  {
    std::stringstream sstr;
    sstr << "sound system already initialized";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  device = new AudioDevice();
  bank = new SourceBank(size);
  alDistanceModel(distance_model);
}

void snd::snd_quit(bool throw_on_error)
{
  if((NULL == device) || (NULL == bank))
  {
    if(throw_on_error)
    {
      std::stringstream sstr;
      sstr << "sound system already deinitialized";
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
    else
    {
      return;
    }
  }
  stream = StreamSptr();
  delete bank;
  bank = NULL;
  delete device;
  device = NULL;
}

