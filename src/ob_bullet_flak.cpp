#include "ob_bullet_flak.hpp"

#include "math/random.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** Billboard size. */
static const float OB_BILLBOARD_SIZE_MUZZLE_FLAK = 1.0f;

/* Flak explosion length */
static const int OB_FLAK_EXPLOSION_TIME  = 50;

/* Flak explosion size */
static const float OB_FLAK_EXPLOSION_SIZE = 80.0f;

/** Flak bullet speed. */
static const float OB_FLAK_BULLET_SPEED = 400.0f;

/* Flak bullet lifetime. */
static const int OB_FLAK_BULLET_MAX_LIFETIME = 400;

/* Flak bullet explode time inaccuracy. */
static const int OB_FLAK_DEATH_INACCURACY = 10;

/* Flak bullet death probability (1 per). */
//static const int OB_FLAK_DEATH_PROBABILITY = 15;
static const int OB_FLAK_DEATH_PROBABILITY = 40;

/** Bullet collision area. */
static const float OB_COLLISION_BULLET_FLAK = 2.0f;

/** Flak explosion collision area. */
static const float OB_COLLISION_EXPLOSION_FLAK = 4.0f;

/** Flak explosion collision time. */
static const int OB_COLLISION_TIME_FLAK_EXPLOSION = 2;

BulletFlak::BulletFlak(const math::vec3d &ppos, const math::vec3d &pdir,
    const math::vec3d &momentum, const EntityObject *target) :
  gfx::EntityObjectOverlay(ppos, pdir, glob->getMeshBulletFlak()),
  CollisionElement(OB_COLLISION_BULLET_FLAK, POINT, OB_FACTION_PLAYER_BULLET),
  Lifetime(OB_FLAK_BULLET_MAX_LIFETIME)
{
  math::vec3d real_dir;
  math::vec3f up(math::mrand(-1.0f, 1.0f),
      math::mrand(-1.0f, 1.0f),
      math::mrand(-1.0f, 1.0f));

  // Determine which flak to fire
  math::mat4f inv = game->getView().getWm();
  inv.convertToView();

  math::vec4f dircorrected = inv * math::vec4f(static_cast<float>(pdir.x()),
      static_cast<float>(pdir.y()), static_cast<float>(pdir.z()), 1.0f);
  
  // Launch offset creation
  float SCALE = 0.01f;
  math::mat4f rotm = game->getView().getWm();
  rotm(0, 3) = 0;
  rotm(1, 3) = 0;
  rotm(2, 3) = 0;
  rotm(3, 3) = 0;

  math::vec3f launch_offset;

  // LEFT flak
  if(dircorrected.x() > 0)
  {
    launch_offset = math::vec3f(rotm*math::vec4f(64.0f*SCALE, 0.0f*SCALE, -102.0f*SCALE, 1.0f));
  }
  // RIGHT flak
  else
  {
    launch_offset = math::vec3f(rotm*math::vec4f(-64.0f*SCALE, 0.0f*SCALE, -102.0f*SCALE, 1.0f));
  }

  m_pos += launch_offset;

#if defined(OB_FLAK_USE_TARGET)
  if(!target)
  {
#endif
    real_dir = pdir;
    m_rot = m_rot * OB_FLAK_BULLET_SPEED + momentum * OB_ORBIT_SPEED;
    m_wm.loadLookAt(math::vec3f(m_pos), math::vec3f(m_pos + real_dir), up);
#if defined(OB_FLAK_USE_TARGET)
  }
  else
  {
    real_dir = calculateBulletAimDirection(target->getPos(),
        target->getRot(), m_pos, momentum * OB_ORBIT_SPEED,
        OB_FLAK_BULLET_SPEED);
    m_rot = momentum * OB_ORBIT_SPEED + real_dir * OB_FLAK_BULLET_SPEED;
    m_wm.loadLookAt(math::vec3f(m_pos), math::vec3f(m_pos + real_dir), up);

    // Set flak to explode near target
    int timeofdeath = static_cast<int>(100.0f * calculateBulletTimeOfFlight(target->getPos(), target->getRot(), m_pos, momentum * OB_ORBIT_SPEED, OB_FLAK_BULLET_SPEED)) - OB_FLAK_DEATH_INACCURACY;
    //std::cout << "Time of life: " << timeofdeath << "\n";
    if((timeofdeath<OB_FLAK_BULLET_MAX_LIFETIME)&&(timeofdeath>0))
    {
      resetLife(timeofdeath);
    }
  }
#else
  boost::ignore_unused_variable_warning(target);
#endif

  m_rot += math::vec3f(math::mrand(-2.5f, 2.5f), math::mrand(-2.5f, 2.5f), math::mrand(-2.5f, 2.5f));

  this->initCollisionData(math::vec3f(m_pos));

  // Fire effect - Random sparks around object
  for(int ii=0; ii<10; ii++)
  {
    float randvel = math::mrand(0.0f, 1.0f);
    randvel *= randvel * 0.1f;
    game->addParticle(GLOW_SHARP,
        Particle(gfx::Color(1.0f, 0.9f, 0.5f, 0.7f),
          math::vec3f(m_pos), OB_BILLBOARD_SIZE_MUZZLE_FLAK,
          math::vec3f(momentum * OB_ORBIT_SPEED + real_dir * randvel * OB_FLAK_BULLET_SPEED + math::vec3f(math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f))),
          OB_PARTICLE_TIME_MUZZLE_EFFECT));
  }

  snd::play(glob->getSampleFlak(), math::vec3f(m_pos));
}

void BulletFlak::gamisticEffect(void *args)
{
  boost::ignore_unused_variable_warning(args);

  if(this->die(OB_COLLISION_TIME_FLAK_EXPLOSION))
  {
    this->spawnExplosion();
    this->setRadius(OB_COLLISION_EXPLOSION_FLAK);
    this->disableDraw();
  }
}

void BulletFlak::spawnExplosion()
{
  // Explosion effect - Large shockwave and crackling in the center
  for(int ii=0; ii<4; ii++)
  {
    game->addParticle(Particle::randomCrackleParticle(),
        Particle(gfx::Color(1.0f, 0.8f, 0.5f, 0.4f),
          math::vec3f(m_pos), OB_FLAK_EXPLOSION_SIZE * 0.5f,
          math::vec3f(m_rot) * 0.4f + math::vec3f(math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f)),
          OB_FLAK_EXPLOSION_TIME - 10, -OB_FLAK_EXPLOSION_SIZE * 0.5f));
       
    game->addParticle(Particle::randomCrackleParticle(),
        Particle(gfx::Color(0.9f, 0.15f, 0.0f, 0.4f),
          math::vec3f(m_pos), OB_FLAK_EXPLOSION_SIZE/4.0f,
          math::vec3f(m_rot*0.4 + math::vec3f(math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f))),
          OB_FLAK_EXPLOSION_TIME, OB_FLAK_EXPLOSION_SIZE/3.0f));
  }
  game->addParticle(Particle::randomShockwaveNarrowParticle(),
      Particle(gfx::Color(1.0f, 0.8f, 0.5f, 0.8f),
        math::vec3f(m_pos), 0.0f,
        math::vec3f(m_rot*0.4),
        OB_FLAK_EXPLOSION_TIME, OB_FLAK_EXPLOSION_SIZE));
}

bool BulletFlak::update()
{
  m_pos += m_rot * game->getTimestep();
  m_wm(0, 3) = static_cast<float>(m_pos.x());
  m_wm(1, 3) = static_cast<float>(m_pos.y());
  m_wm(2, 3) = static_cast<float>(m_pos.z());

  //std::cout << "bullet position: " << m_pos << std::endl;

  // Update the collision stuff.
  this->updateCollisionData(math::vec3f(m_pos));
  this->updateAreas(game->getOctree());

  CollisionElement *other = this->checkCollisions();
  if(other)
  {
    //std::cout << "collision!\n";
    other->gamisticEffect(NULL);
    this->gamisticEffect(NULL);
    return true;
  }

  //std::cout << "bullet position: " << m_pos << std::endl;

  if(this->isDead())
  {
    return this->decrementAge();
  }
  else
  {
    bool ret = incrementAge(OB_FLAK_DEATH_PROBABILITY);
    if(!ret)
    {
      this->gamisticEffect(NULL);
    }
    return true;
  }
}

