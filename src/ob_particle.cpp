#include "ob_particle.hpp"

#include "math/random.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

Particle::Particle(const gfx::Color &col, const math::vec3f &pos, float size, const math::vec3f &dir,
    int lifetime, float dsize) :
  gfx::PointSprite(math::mrand0(static_cast<float>(M_2_PI)), math::mrandpn(1.0f), math::mrandpn(1.0f), col, pos, size),
  m_dir(dir),
  m_lifetime(1.0f),
  m_lifetime_dec(1.0f / static_cast<float>(lifetime)),
  m_size_delta(dsize * m_lifetime_dec)
{
  m_alpha = this->getColorAlpha(); // cannot be in initializer list
}

bool Particle::update()
{
  m_lifetime -= m_lifetime_dec;

  this->setColorAlpha(m_alpha * m_lifetime);
  this->setVertex(this->getVertex() + m_dir * game->getTimestep());
  this->setSize(this->getSize() + m_size_delta);

  return (0.0f < m_lifetime);
}

ParticleTypeEnum Particle::randomCrackleParticle()
{
  static const ParticleTypeEnum aa = CRACKLE_1;
  static const ParticleTypeEnum bb = CRACKLE_4;
  return static_cast<ParticleTypeEnum>(
      math::mrand(static_cast<int>(aa), static_cast<int>(bb)));
}

ParticleTypeEnum Particle::randomSmokeHardParticle()
{
  static const ParticleTypeEnum aa = SMOKE_HARD_1;
  static const ParticleTypeEnum bb = SMOKE_HARD_4;
  return static_cast<ParticleTypeEnum>(
      math::mrand(static_cast<int>(aa), static_cast<int>(bb)));
}

ParticleTypeEnum Particle::randomSmokeSoftParticle()
{
  static const ParticleTypeEnum aa = SMOKE_SOFT_1;
  static const ParticleTypeEnum bb = SMOKE_SOFT_4;
  return static_cast<ParticleTypeEnum>(
      math::mrand(static_cast<int>(aa), static_cast<int>(bb)));
}

ParticleTypeEnum Particle::randomShockwaveNarrowParticle()
{
  static const ParticleTypeEnum aa = SHOCKWAVE_NARROW_1;
  static const ParticleTypeEnum bb = SHOCKWAVE_NARROW_4;
  return static_cast<ParticleTypeEnum>(
      math::mrand(static_cast<int>(aa), static_cast<int>(bb)));
}

ParticleTypeEnum Particle::randomShockwaveWideParticle()
{
  static const ParticleTypeEnum aa = SHOCKWAVE_WIDE_1;
  static const ParticleTypeEnum bb = SHOCKWAVE_WIDE_4;
  return static_cast<ParticleTypeEnum>(
      math::mrand(static_cast<int>(aa), static_cast<int>(bb)));
}

ParticleTypeEnum Particle::randomSparkleParticle()
{
  static const ParticleTypeEnum aa = SPARKLE_1;
  static const ParticleTypeEnum bb = SPARKLE_6;
  return static_cast<ParticleTypeEnum>(
      math::mrand(static_cast<int>(aa), static_cast<int>(bb)));
}

