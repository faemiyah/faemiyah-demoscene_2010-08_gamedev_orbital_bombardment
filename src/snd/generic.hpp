#ifndef SND_GENERIC_HPP
#define SND_GENERIC_HPP

#if defined(__APPLE__)
#include <AL/al.h>
#include <AL/alc.h>
#include <SDL/SDL.h>
#elif defined(WIN32)
#include "AL/al.h"
#include "AL/alc.h"
#include "SDL.h"
#else // General *nix such as FreeBSD or Linux.
#include "AL/al.h"
#include "AL/alc.h"
#include "SDL.h"
#endif

#include "math/vec.hpp"

namespace snd
{
  class Sample;
  class Source;
  class Stream;

  /** Global gain. */
  extern float volume_music;

  /** Global gain. */
  extern float volume_samples;

  /** \brief Play one sample on a source.
   *
   * This sample will be played in origo location.
   *
   * \param sample Sample to play.
   * \param force Bypass sample saturation check.
   */
  extern Source* play(Sample &sample, bool force = false);

  /** \brief Play one sample on a source.
   *
   * \param sample Sample to play.
   * \param pos Position.
   * \param force Bypass sample saturation check.
   */
  extern Source* play(Sample &sample, const math::vec3f &pos, bool force = false);

  /** \brief Start playing a stream.
   *
   * \param pfname Filename to open.
   */
  extern Stream* play_stream(const char *pfname);

  /** \brief Start playing a stream.
   *
   * \param pfname Filename to open.
   */
  extern Stream* play_stream(const std::string &pfname);

  /** \brief Set listener attributes.
   *
   * \param pos Position.
   * \param dir Direction.
   * \param up Up vector.
   */
  extern void set_listener(const math::vec3f &pos, const math::vec3f &dir,
      const math::vec3f &up);

  /** \brief Set listener attributes.
   *
   * \param pos Position.
   * \param dir Direction.
   * \param up Up vector.
   * \param vel Velocity.
   */
  extern void set_listener(const math::vec3f &pos, const math::vec3f &dir,
      const math::vec3f &up, const math::vec3f &vel);

  /** \brief Set music volume.
   *
   * \param op New volume.
   */
  extern void set_volume_music(float op);

  /** \brief Set sample volume.
   *
   * \param op New volume.
   */
  extern void set_volume_samples(float op);

  /** \brief Initialize sound system.
   *
   * The user may choose to disregard this altogether and initialize an audio
   * device and potential sound banks independently. This is provided for
   * easy, convenient and simple use.
   *
   * Throws an exception on error. Calling this multiple times counts as an
   * error.
   *
   * Will allocate one sound bank as a side effect.
   *
   * \param size Number of simultaneous samples (default: 16).
   * \param distance_model Distance model for sounds.
   */
  extern void snd_init(unsigned size = 16, ALenum distance_model = AL_NONE);

  /** \brief Deinitialize sound system.
   *
   * If called when snd_init was not already called, this is a silent NOP.
   *
   * \param throw_on_error If true throw an error instead of NOP.
   */
  extern void snd_quit(bool throw_on_error = false);
}

#endif
