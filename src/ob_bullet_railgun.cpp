#include "ob_bullet_railgun.hpp"

#include "math/random.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** Color of particles. */
static const gfx::Color RAILGUN_PARTICLE_COLOR(0.7f, 0.7f, 1.0f, 0.7f);

/** Explosion size. */
static const float OB_RAILGUN_EXPLOSION_SIZE = 100.0f;

/** Explosion time. */
static const int OB_RAILGUN_EXPLOSION_TIME = 75;

/** Billboard size. */
static const float OB_BILLBOARD_SIZE_MUZZLE_RAILGUN = 2.0f;

/** Billboard size. */
static const float OB_BILLBOARD_SIZE_RAILGUN = 7.0f;

/** Particle lifetime. */
static const int OB_PARTICLE_TIME_RAILGUN = 200;

/** Railgun bullet speed. */
static const float OB_RAILGUN_BULLET_SPEED = 800.0f;

/** Railgun bullet lifetime. */
static const int OB_RAILGUN_BULLET_LIFETIME = 1200;

/** Bullet collision area. */
static const float OB_COLLISION_BULLET_RAILGUN = 1.0f;

BulletRailgun::BulletRailgun(const math::vec3d &ppos,
    const math::vec3d &pdir, const math::vec3d &momentum,
    const EntityObject *target) :
  gfx::EntityObjectOverlay(ppos, pdir, glob->getMeshBulletRailgun()),
  CollisionElement(OB_COLLISION_BULLET_RAILGUN, POINT, OB_FACTION_PLAYER_BULLET),
  Lifetime(OB_RAILGUN_BULLET_LIFETIME)
{
  math::vec3d real_dir;
  math::vec3f up(math::mrand(-1.0f, 1.0f),
      math::mrand(-1.0f, 1.0f),
      math::mrand(-1.0f, 1.0f));

  // Launch offset
  float SCALE = 0.01f;
  math::mat4f rotm = game->getView().getWm();
  rotm(0, 3) = 0;
  rotm(1, 3) = 0;
  rotm(2, 3) = 0;
  rotm(3, 3) = 0;
  math::vec3f launch_offset(rotm*math::vec4f(0.0f*SCALE, -54.0f*SCALE, -102.0f*SCALE, 1.0f));
  
  m_pos += launch_offset;

  if(!target)
  {
    real_dir = pdir;
    m_rot = m_rot * OB_RAILGUN_BULLET_SPEED + momentum * OB_ORBIT_SPEED;
    m_wm.loadLookAt(math::vec3f(m_pos), math::vec3f(m_pos + pdir), up);
  }
  else
  {
    real_dir = calculateBulletAimDirection(target->getPos(),
        target->getRot(), m_pos, momentum * OB_ORBIT_SPEED,
        OB_RAILGUN_BULLET_SPEED);
    m_rot = momentum * OB_ORBIT_SPEED + real_dir * OB_RAILGUN_BULLET_SPEED;
    //std::cout << "Aimed shot!\n";
    m_wm.loadLookAt(math::vec3f(m_pos), math::vec3f(m_pos + real_dir), up);
  }

  this->initCollisionData(math::vec3f(m_pos));

  snd::play(glob->getSampleRailgun(), math::vec3f(m_pos));

  // Fire effect - Lessening particle line upon spawn
  for(int ii=0; ii<20; ii++)
  {
    float randvel = math::mrand(0.0f, 1.0f);
    randvel *= randvel;
    game->addParticle(Particle::randomSparkleParticle(),
        Particle(RAILGUN_PARTICLE_COLOR,
          math::vec3f(m_pos), OB_BILLBOARD_SIZE_MUZZLE_RAILGUN,
          math::vec3f(m_rot * randvel),
          OB_PARTICLE_TIME_MUZZLE_EFFECT));
  }
  game->addParticle(Particle::randomSparkleParticle(),
      Particle(gfx::Color(0.7f, 0.7f, 1.0f, 1.0f),
        math::vec3f(m_pos), OB_BILLBOARD_SIZE_MUZZLE_RAILGUN,
        math::vec3f(momentum * OB_ORBIT_SPEED),
        OB_PARTICLE_TIME_MUZZLE_EFFECT));
}

void BulletRailgun::gamisticEffect(void *args)
{
  boost::ignore_unused_variable_warning(args);

  game->removeBulletRailgun(this);
  this->spawnExplosion();
}

void BulletRailgun::spawnExplosion()
{
  // Explosion effect - Large triple shockwave and crackling in the center
  for(int ii=0; ii<2; ii++)
  {
    game->addParticle(Particle::randomCrackleParticle(),
        Particle(RAILGUN_PARTICLE_COLOR,
          math::vec3f(m_pos), OB_RAILGUN_EXPLOSION_SIZE/2.0f,
          math::vec3f(math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f)),
          OB_RAILGUN_EXPLOSION_TIME - 10, -0.5f*OB_RAILGUN_EXPLOSION_SIZE));
    game->addParticle(Particle::randomCrackleParticle(),
        Particle(RAILGUN_PARTICLE_COLOR,
          math::vec3f(m_pos), OB_RAILGUN_EXPLOSION_SIZE/4.0f,
          math::vec3f(math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f)),
          OB_RAILGUN_EXPLOSION_TIME, -OB_RAILGUN_EXPLOSION_SIZE));
  }
  for(int ii=0; ii<3; ii++)
  {
    game->addParticle(Particle::randomShockwaveNarrowParticle(),
        Particle(RAILGUN_PARTICLE_COLOR,
          math::vec3f(m_pos) + math::vec3f(math::mrand(-5.0f, 5.0f), math::mrand(-5.0f, 5.0f), math::mrand(-5.0f, 5.0f)),
          0.0f,
          math::vec3f(math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f), math::mrand(-2.0f, 2.0f)),
          OB_RAILGUN_EXPLOSION_TIME, (static_cast<float>(ii+2)/4.0f)*OB_RAILGUN_EXPLOSION_SIZE));
  }
}

bool BulletRailgun::update()
{
  //glob->addParticle(new Particle(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f), math::vec3f(m_pos), 5.0f, math::vec3f(0.0f, 0.0f, 0.0f), 100));

  m_pos += m_rot * game->getTimestep();
  m_wm(0, 3) = static_cast<float>(m_pos.x());
  m_wm(1, 3) = static_cast<float>(m_pos.y());
  m_wm(2, 3) = static_cast<float>(m_pos.z());

  // Update the collision stuff.
  this->updateCollisionData(math::vec3f(m_pos));
  this->updateAreas(game->getOctree());

  CollisionElement *other = this->checkCollisions();
  if(other)
  {
    //std::cout << "collision!\n";
    other->gamisticEffect(NULL);
    this->spawnExplosion();
    return false;
  }

  // 20% chance of particle trail.
  //if(math::mrand(0, 4) < 1)
  //{
    // old
    /*
    Particle *par = new Particle(gfx::Color(0.7f, 0.7f, 1.0f, 0.7f),
        math::vec3f(m_pos), OB_BILLBOARD_SIZE_RAILGUN,
        math::vec3f(m_rot*0.5f), OB_PARTICLE_TIME_RAILGUN);
    game->addParticle(Particle::randomSparkleParticle(), par);
  }*/
  //if(m_age%2==0)
  //{
    math::vec3f norm1,norm2;
    math::vec3f orgrot(m_rot);
    // Calculate vectors normal to the movement vector.
    if(m_rot.x()!=0.0f)
    {
      norm1 = math::vec3f((-orgrot.y()-orgrot.z())/orgrot.x(), 1.0f, 1.0f);
      norm2 = math::cross(orgrot, norm1);
      norm1 = math::normalize(norm1);
      norm2 = math::normalize(norm2);
    }
    else
    {
      norm1 = math::vec3f(0.0f, -orgrot.z()/orgrot.y(), 1.0f);
      norm2 = math::cross(orgrot, norm1);
      norm1 = math::normalize(norm1);
      norm2 = math::normalize(norm2);
    }
    float rot = static_cast<float>(m_age)*static_cast<float>(0.10f*M_PI);
    game->addParticle(Particle::randomSparkleParticle(),
        Particle(RAILGUN_PARTICLE_COLOR,
          math::vec3f(m_pos) + 2.0f*(math::cos(rot)*norm1+math::sin(rot)*norm2) + math::vec3f(math::mrand(-0.5f, 0.5f), math::mrand(-0.5f, 0.5f), math::mrand(-0.5f, 0.5f)),
          OB_BILLBOARD_SIZE_RAILGUN,
          2.0f*(math::cos(rot)*norm1+math::sin(rot)*norm2) + math::vec3f(math::mrand(-0.25f, 0.25f), math::mrand(-0.25f, 0.25f), math::mrand(-0.25f, 0.25f)),
          OB_PARTICLE_TIME_RAILGUN));
  //}

  //std::cout << "bullet position: " << m_pos << std::endl;

  bool ret = this->incrementAge(OB_BULLET_DEATH_PROBABILITY);
  if(!ret)
  {
    this->spawnExplosion();
  }
  return ret;
}

