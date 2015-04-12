#ifndef OB_PARTICLE_HPP
#define OB_PARTICLE_HPP

#include "gfx/point_sprite.hpp"
#include "math/vec.hpp"

namespace ob
{
  /** \brief Different particle types.
   *
   * Particles will be added to different tables.
   */
  enum ParticleTypeEnum
  {
    CRACKLE_1 = 0,
    CRACKLE_2,
    CRACKLE_3,
    CRACKLE_4,
    GLOW_MEDIUM,
    GLOW_SHARP,
    GLOW_SOFT,
    SHOCKWAVE_NARROW_1,
    SHOCKWAVE_NARROW_2,
    SHOCKWAVE_NARROW_3,
    SHOCKWAVE_NARROW_4,
    SHOCKWAVE_WIDE_1,
    SHOCKWAVE_WIDE_2,
    SHOCKWAVE_WIDE_3,
    SHOCKWAVE_WIDE_4,
    SMOKE_HARD_1,
    SMOKE_HARD_2,
    SMOKE_HARD_3,
    SMOKE_HARD_4,
    SMOKE_SOFT_1,
    SMOKE_SOFT_2,
    SMOKE_SOFT_3,
    SMOKE_SOFT_4,
    SPARKLE_1,
    SPARKLE_2,
    SPARKLE_3,
    SPARKLE_4,
    SPARKLE_5,
    SPARKLE_6
  };

  /** Number of different particles. */
  static const unsigned OB_PARTICLE_COUNT = 29; // FIXME: this may change?

  /** \brief One particle.
   *
   * Used for explosisons & such.
   *
   * This corresponds pretty straightforward to the billboard element used
   * in billboard arrays.
   */
  class Particle : public gfx::PointSprite
  {
    private:
      /** Original alpha. */
      float m_alpha;

      /** Movement direction (and speed). */
      math::vec3f m_dir;

      /** \brief Remaining lifetime.
       *
       * Could also use Lifetime class, but this is actually faster & cleaner.
       */
      float m_lifetime;

      /** Lifetime decrement. */
      float m_lifetime_dec;

      /** Size delta. */
      float m_size_delta;

    public:
      /** \brief Empty constructor. */
      Particle() { }

      /** \brief Constructor.
       *
       * \param col Color.
       * \param pos Position.
       * \param size Billboard size.
       * \param dir Direction.
       * \param lifetime Life time in frames.
       * \param dsize Additional size as the function of time.
       */
      Particle(const gfx::Color &col, const math::vec3f &pos, float size, const math::vec3f &m_dir,
          int lifetime, float dsize = 0.0f);

      /** \brief Destructor. */
      ~Particle() { }

    public:
      /** \brief Update this particle.
       *
       * \return True if this is still alive, false if dead.
       */
      bool update();

      /** \brief Return the enumerated index of a random Sparkle particle.
       *
       * \return Random Sparkle particle enumerator.
       */
      static ParticleTypeEnum randomSparkleParticle();
      // More similar functions
      static ParticleTypeEnum randomCrackleParticle();
      static ParticleTypeEnum randomShockwaveNarrowParticle();
      static ParticleTypeEnum randomShockwaveWideParticle();
      static ParticleTypeEnum randomSmokeHardParticle();
      static ParticleTypeEnum randomSmokeSoftParticle();
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Particle> ParticleSptr;
}

#endif
