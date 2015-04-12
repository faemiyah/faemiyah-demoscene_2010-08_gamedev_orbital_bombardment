#include "ob_missile.hpp"

#include "math/random.hpp"
#include "gfx/color.hpp"
#include "gfx/surface.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** Death particle color. */
static const gfx::Color DEATH_PARTICLE_COLOR(1.0f, 0.6f, 0.1f, 1.0f);

/** Death Explosion size. */
static const float MISSILE_EXPLOSION_SIZE = 60.0f;

/** Death Explosion length size. */
static const int MISSILE_EXPLOSION_TIME = 20;

/** Death particle lifetime. */
static const int MISSILE_EXPLOSION_PARTICLE_TIME = 80;

/** Nuke reload time. */
static const int OB_MISSILE_LIFETIME = 12000;

/** Death particles per frame for missile. */
//static const unsigned DEATH_PARTICLE_SPAWN = 1;

/** Death particle travel multiplier. */
//static const float DEATH_PARTICLE_SPEED = 30.0f;

Missile::Missile(const math::vec3d pos, const math::vec3d dir,
    const float rad, uint32_t faction, const gfx::Texture2D *billboard) :
  gfx::EntityObject(pos, dir),
  Billboard(billboard),
  CollisionElement(rad, POINT, faction),
  Lifetime(OB_MISSILE_LIFETIME),
  m_alarm_sounded(false)
{
  //snd::play(glob->getSampleContact(), math::vec3f(m_pos));
  this->initCollisionData(math::vec3f(m_pos));
}

const gfx::Texture2D* Missile::fillBillboard(BillboardArray &parray,
    const gfx::Shader &sh, const gfx::Texture2D *tex) const
{
  if(!this->isVisible())
  {
    return NULL;
  }

  const gfx::Texture2D *ret = this->updateBillboard(parray, sh, tex);

  if(!m_billboard_disabled)
  {
    parray.push_back(gfx::PointSprite(gfx::COLOR_WHITE, math::vec3f(m_pos),
          OB_BILLBOARD_SIZE_MISSILE));
  }
  return ret;
}

void Missile::gamisticEffect(void *args)
{
  boost::ignore_unused_variable_warning(args);

  if(this->die(MISSILE_EXPLOSION_TIME))
  {
    if(m_alarm_sounded)
    {
      snd::play(glob->getSampleAlarmOver(), math::vec3f(m_pos));
    }
    else
    {
      snd::play(glob->getSampleTargetDestroyed(), math::vec3f(m_pos));
    }
    //std::cout << "missile died\n";
    this->removeFromAreas();
    this->updateVisibility(false);
  }
}

void Missile::spawnExplosion() const
{
  // Explosion effect - Multiple large, narrow shockwaves, smoke and crackling in the center
  // First, spawn crackling every frame.
  for(int ii=0; ii<1; ii++)
  {
    game->addParticle(Particle::randomCrackleParticle(),
        Particle(gfx::Color(1.0f, 0.8f, 0.5f, 0.4f),
          math::vec3f(m_pos + math::vec3f(math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f), math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f), math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f))),
          MISSILE_EXPLOSION_SIZE/2.0f,
          math::vec3f(m_rot*0.4 + math::vec3f(math::mrand(-4.0f, 4.0f), math::mrand(-4.0f, 4.0f), math::mrand(-4.0f, 4.0f))),
          MISSILE_EXPLOSION_PARTICLE_TIME / 2, -MISSILE_EXPLOSION_SIZE/2.0f));
    game->addParticle(Particle::randomCrackleParticle(),
        Particle(gfx::Color(0.9f, 0.15f, 0.0f, 0.4f),
          math::vec3f(m_pos + math::vec3f(math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f), math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f), math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f))),
          MISSILE_EXPLOSION_SIZE/4.0f,
          math::vec3f(m_rot*0.4 + math::vec3f(math::mrand(-4.0f, 4.0f), math::mrand(-4.0f, 4.0f), math::mrand(-4.0f, 4.0f))),
          MISSILE_EXPLOSION_PARTICLE_TIME / 2, MISSILE_EXPLOSION_SIZE/2.0f));
  }

  // Spawn smoke every frame
  {
    game->addParticle(Particle::randomSmokeHardParticle(),
        Particle(gfx::Color(0.4f, 0.4f, 0.4f, 0.6f),
          math::vec3f(m_pos + math::vec3f(math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f), math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f), math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f))),
          MISSILE_EXPLOSION_SIZE/4.0f,
          math::vec3f(m_rot*0.4 + math::vec3f(math::mrand(-4.0f, 4.0f), math::mrand(-4.0f, 4.0f), math::mrand(-4.0f, 4.0f))),
          MISSILE_EXPLOSION_PARTICLE_TIME * 2, MISSILE_EXPLOSION_SIZE/2.0f));
  }

  // 1/5 chanse of spawning big shockwave every frame
  if(math::mrand(0,5)<1)
  {
    game->addParticle(Particle::randomShockwaveWideParticle(),
        Particle(gfx::Color(1.0f, 0.8f, 0.5f, 0.8f),
          math::vec3f(m_pos + math::vec3f(math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f), math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f), math::mrand(-MISSILE_EXPLOSION_SIZE/4.0f, MISSILE_EXPLOSION_SIZE/4.0f))),
          0.0f,
          math::vec3f(m_rot*0.4),
          MISSILE_EXPLOSION_PARTICLE_TIME, MISSILE_EXPLOSION_SIZE));
  }
}

bool Missile::updateDead()
{
  m_pos += m_rot * game->getTimestep();
  this->spawnExplosion();
  this->updateVisibility(false);
  return this->decrementAge();
}

