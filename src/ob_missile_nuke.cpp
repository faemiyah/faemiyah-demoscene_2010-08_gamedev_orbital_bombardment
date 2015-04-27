#include "ob_missile_nuke.hpp"

#include "math/random.hpp"
#include "gfx/mesh_static.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** Missile collision area. */
static const float OB_COLLISION_MISSILE_NUKE = 1.0f;

/** Nuke explosion time. */
static const int NUKE_TIME = 1000;

/** Nuke explosion height. */
static const float OB_NUKE_AIRBURST = 30.0f;

/** Nuke explosion area. */
static const float OB_NUKE_AREA = 300.0f;

/** Nuke explosion area. */
static const float OB_NUKE_BRUSH = -3.2f;

/** Color for initial flash and glow. */
static const gfx::Color NUKE_FLASH_COLOR(1.0f, 0.8f, 0.2f, 0.6f);

/** Lifetime for initial flash shockwave. */ 
static const int NUKE_FLASH_PARTICLE_LIFETIME = 60;

/** Nuke flash particle size. */
static const float NUKE_FLASH_PARTICLE_SIZE = 60.0f;

/** Color for pillar fire. */
static const gfx::Color NUKE_PILLAR_COLOR(1.0f, 0.4f, 0.0f, 0.3f);

/** Color for pillar smoke. */
static const gfx::Color NUKE_PILLAR_SMOKE_COLOR(0.5f, 0.5f, 0.55f, 0.4f);

/** Nuke pillar particle size. */
static const float NUKE_PILLAR_PARTICLE_SIZE = 45.0f;

/** Nuke pillar particle lifetime. */
static const int NUKE_PILLAR_PARTICLE_LIFETIME = 50;

/** Color for nukeboom. */
static const gfx::Color NUKE_SMOKE_COLOR(0.5f, 0.5f, 0.55f, 0.3f);

/** Color for nukeboom. */
static const gfx::Color NUKE_SHOCKWAVE_COLOR(1.0f, 0.2f, 0.0f, 0.5f);

/** Nuke particle distance. */
static const int NUKE_SHOCKWAVE_PARTICLE_LIFETIME = 350;

/** Nuke particle size. */
static const float NUKE_SHOCKWAVE_PARTICLE_SIZE = 65.0f;

/** Color for nuke fireball. */
static const gfx::Color NUKE_FIREBALL_COLOR(1.0f, 0.45f, 0.1f, 0.4f);
//static const gfx::Color NUKE_FIREBALL_COLOR(0.1f, 1.0f, 0.1f, 0.6f);

/** Fireball apex height. */
static const float NUKE_FIREBALL_HEIGHT = 200.0;

/** Fireball apex height. */
static const float NUKE_FIREBALL_SIZE = 120.0;

/** Nuke particle distance. */
static const int NUKE_FIREBALL_PARTICLE_LIFETIME = 250;

/** Nuke particle distance. */
static const float NUKE_FIREBALL_PARTICLE_SIZE = 50.0f;

/** Enable NUKE XIIT here */
#define NUKE_XIIT 0
/** Nuke main acceleration. */
static const float OB_NUKE_ACCELERATION =
#if (NUKE_XIIT != 0)
100.0f;
#else
10.0f;
#endif
/** Nuke lateral acceleration. */
static const float OB_NUKE_LATERAL_ACCELERATION =
#if (NUKE_XIIT != 0)
10.0f;
#else
0.1f;
#endif
/** Nuke speed. */
static const float OB_NUKE_SPEED =
#if (NUKE_XIIT != 0)
700.0f;
#else
75.0f;
#endif

/** Nuke impact in timer. */
static const float OB_NUKE_IMPACT_IN_LENGTH = 8.0f*OB_NUKE_SPEED;

/** Nuke impact in timer. */
static const float OB_NUKE_IMPACT_IN_2 = OB_NUKE_IMPACT_IN_LENGTH * OB_NUKE_IMPACT_IN_LENGTH;

#if 0
/** Non-probability multiplier to "impact in" -sound. */
static const float OB_NUKE_IMPACT_IN_PROB = OB_NUKE_IMPACT_IN_2 * 1000.0f;
#endif

MissileNuke::MissileNuke(const math::vec3d &pos, const math::vec3d &dir, const math::vec3d &target) :
  Missile(pos, dir, OB_COLLISION_MISSILE_NUKE, OB_FACTION_PLAYER_MISSILE, &(glob->getTextureMissileNuke())),
  m_target(target),
  m_nuking(false),
  m_beepingsound(NULL)
{
  this->addMesh(glob->getMeshMissileNuke());

  // Launch offset
  float SCALE = 0.01f;
  math::mat4f rotm = game->getView().getWm();
  rotm(0, 3) = 0;
  rotm(1, 3) = 0;
  rotm(2, 3) = 0;
  rotm(3, 3) = 0;
  math::vec3f launch_offset(rotm*math::vec4f(-90.8f*SCALE, 32.0f*SCALE, 4.0f*SCALE, 1.0f));

  m_pos += launch_offset;

  this->initCollisionData(math::vec3f(m_pos));

  snd::play(glob->getSampleNuke(), math::vec3f(m_pos));
}

float MissileNuke::getDistortAlpha() const
{
  float ret = static_cast<float>(m_age) / static_cast<float>(NUKE_TIME * 2);
  return ret * ret * (ret * 1.5f);
}

math::vec3d MissileNuke::getTargetPos() const
{
  return m_target;
}

bool MissileNuke::update()
{
  if(m_nuking)
  {
    return this->updateNuking();
  }
  if(isDead())
  {
    return this->updateDead();
  }

  math::vec3d dir = calculateMissileAimDirection(m_target, math::vec3d(0.0, 0.0, 0.0),
      m_pos, m_rot);
  math::vec3d udir;
  //if(m_age<500)
  //{
    //udir = updatePosDir(m_pos, m_rot, dir, OB_NUKE_SPEED*4.0f,
        //OB_NUKE_ACCELERATION*4.0f, OB_NUKE_LATERAL_ACCELERATION, static_cast<float>(m_age)/500.0f);
  //}
  //else
  //{
    udir = updatePosDir(m_pos, m_rot, dir, OB_NUKE_SPEED,
        OB_NUKE_ACCELERATION, OB_NUKE_LATERAL_ACCELERATION, 1.0f);
  //}
  // FIXME: more reasonable up vector?
  m_wm.loadLookAt(math::vec3f(m_pos), math::vec3f(m_pos + udir), math::vec3f(m_target));

  double ht2 = math::length2(m_pos - m_target);
  if(ht2 < OB_NUKE_AIRBURST * OB_NUKE_AIRBURST)
  {
    this->setRadius(OB_NUKE_AREA);
    this->setType(STATIONARY);
    this->initCollisionData(math::vec3f(m_pos));
    this->updateAreas(game->getOctree());

    std::list<CollisionElement*> collisions;
    this->getAllCollisions(collisions);
    BOOST_FOREACH(CollisionElement *vv, collisions)
    {
      //std::cout << "found a nuke wipe object " << vv << std::endl;
      vv->gamisticEffect(this);
    }

    snd::play(*(snd::Sample::locate("ob_nuke_explosion").get()), math::vec3f(m_pos));

    game->incSiloMinCountInRange();
    //std::cout << "more silos now popping: " << game->getSiloMinCountInRange() << '\n';

    m_nuking = true;
    this->die(NUKE_TIME);
    this->updateVisibility(false);
    return true;
  }
  else
  {
    if(ht2 < OB_NUKE_IMPACT_IN_2)
    {
      if(m_beepingsound == NULL)
      {
        m_beepingsound = snd::play(*(snd::Sample::locate("ob_impact_in").get()), math::vec3f(m_pos));
      }
      /*// Closer -> more probable that impact sounds come.
      if(math::mrand(0.0f, static_cast<float>(OB_NUKE_IMPACT_IN_2 - ht2)) <
          OB_NUKE_IMPACT_IN_PROB)
      {
        snd::play(glob->getSampleImpactIn(), math::vec3f(m_pos));
      }*/
    }

    this->updateCollisionData(math::vec3f(m_pos));
    this->updateAreas(game->getOctree());
    CollisionElement *other = this->checkCollisions();
    if(other)
    {
#if 0
      std::cout << "nuke: " << *static_cast<CollisionElement*>(this) << std::endl;
      
      std::cout << "opponent: " << *static_cast<CollisionElement*>(other) << std::endl;
#endif
      
      //std::cout << "collision!\n";
      if(m_beepingsound != NULL)
      {
        m_beepingsound->stop();
      }
      other->gamisticEffect(NULL);
      this->gamisticEffect(NULL);
      return true;
    }
  }

  //std::cout << "Nuke pos: " << m_pos << std::endl;

  // 50% chance of either particle trail.
  if(m_age%3==0)
  {
    if(!math::mrand(0, 2))
    {
      game->addParticle(SMOKE_SOFT_1,
          Particle(gfx::Color(0.5f, 0.55f, 0.55f, 0.7f),
            math::vec3f(m_pos)-math::normalize(m_rot)*0.8f, OB_BILLBOARD_SIZE_SMOKE,
            math::mrand(0.0f, 1.0f) * 0.1f * OB_NUKE_SPEED * math::vec3f(math::normalize(m_rot)),
            OB_PARTICLE_TIME_SMOKE, -OB_BILLBOARD_SIZE_SMOKE/2.0f));
    }
    else
    {
      game->addParticle(SMOKE_HARD_1,
          Particle(gfx::Color(1.0f, 1.0f, 0.5f, 0.8f),
            math::vec3f(m_pos)-math::normalize(m_rot)*0.8f, OB_BILLBOARD_SIZE_SMOKE,
            math::mrand(0.0f, 1.0f) * 0.1f * OB_NUKE_SPEED * math::vec3f(math::normalize(m_rot)),
            OB_PARTICLE_TIME_SMOKE, -OB_BILLBOARD_SIZE_SMOKE));
    }
  }

  this->updateVisibility(true);
  if(!this->incrementAge(OB_BULLET_DEATH_PROBABILITY))
  {
    this->gamisticEffect(NULL);
  }
  return true;
}

bool MissileNuke::updateNuking()
{
  math::vec3f norm1,norm2;
  math::vec3f orgpos(m_pos);
  math::vec3f orgnormal(math::normalize(m_pos));
  // Calculate vectors normal to the impact point vector.
  if(m_rot.x()!=0.0f)
  {
    norm1 = math::vec3f((-orgpos.y()-orgpos.z())/orgpos.x(), 1.0f, 1.0f);
    norm2 = math::cross(orgpos, norm1);
    norm1 = math::normalize(norm1);
    norm2 = math::normalize(norm2);
  }
  else
  {
    norm1 = math::vec3f(0.0f, -orgpos.z()/orgpos.y(), 1.0f);
    norm2 = math::cross(orgpos, norm1);
    norm1 = math::normalize(norm1);
    norm2 = math::normalize(norm2);
  }

  // Frame zero, inital one-time effects
  if(m_age == NUKE_TIME)
  {
    math::vec3f playerpos(game->getView().getPos());
    math::vec3f glowspot = math::normalize(playerpos-orgpos)*400;

    // Create the fast, initial planar shockwave
    for(int ii=0; ii<100; ii++)
    {
      float rot = math::mrand(0.0f, static_cast<float>(2.0f*M_PI));
      game->addParticle(GLOW_SHARP,
          Particle(NUKE_FLASH_COLOR,
            orgpos, 0.2f*NUKE_FLASH_PARTICLE_SIZE,
            350.0f*(math::cos(rot)*norm1+math::sin(rot)*norm2),
            NUKE_FLASH_PARTICLE_LIFETIME, 0.8f*NUKE_FLASH_PARTICLE_SIZE));
    }
    // Create large glow
    game->addParticle(GLOW_SOFT,
        Particle(NUKE_FLASH_COLOR,
          orgpos + glowspot, 500.0f,
          math::vec3f(0.0f, 0.0f, 0.0f),
          NUKE_TIME, 100));
  }
  
  // Create the round, enlarging planar shockwave
  for(int ii=0; ii<4; ii++)
  {
    if(ii%2==0)
    {
      float rot = math::mrand(0.0f, static_cast<float>(2.0f*M_PI));
      game->addParticle(Particle::randomSmokeHardParticle(),
          Particle(NUKE_SHOCKWAVE_COLOR,
            orgpos, 0.5f*NUKE_SHOCKWAVE_PARTICLE_SIZE,
            math::mrand(0.8f, 1.0f)*50.0f*(math::cos(rot)*norm1+math::sin(rot)*norm2),
            NUKE_SHOCKWAVE_PARTICLE_LIFETIME, 0.5f*NUKE_SHOCKWAVE_PARTICLE_SIZE));
    }
    else
    {
      float rot = math::mrand(0.0f, static_cast<float>(2.0f*M_PI));
      game->addParticle(Particle::randomCrackleParticle(),
          Particle(NUKE_SMOKE_COLOR,
            orgpos, 0.2f*NUKE_SHOCKWAVE_PARTICLE_SIZE,
            math::mrand(0.8f, 1.0f)*50.0f*(math::cos(rot)*norm1+math::sin(rot)*norm2),
            NUKE_SHOCKWAVE_PARTICLE_LIFETIME, 0.8f*NUKE_SHOCKWAVE_PARTICLE_SIZE));
    }
  }

  float completeratio = 1.0f-static_cast<float>(m_age)/static_cast<float>(NUKE_TIME);
  float raise_speed = 100.0f*static_cast<float>(NUKE_FIREBALL_HEIGHT)/static_cast<float>(NUKE_TIME);

  // Create rising smoke pillar
  for(int ii=0; ii<4; ii++)
  {
    if(ii%2==0)
    {
      game->addParticle(Particle::randomCrackleParticle(),
          Particle(NUKE_PILLAR_COLOR,
            orgpos + math::mrand(0.0f, 1.0f)*(completeratio*NUKE_FIREBALL_HEIGHT-20.0f)*orgnormal + math::vec3f(math::mrand(-25.0f, 25.0f), math::mrand(-25.0f, 25.0f), math::mrand(-25.0f, 25.0f)),
            NUKE_PILLAR_PARTICLE_SIZE,
            math::mrand(0.8f, 1.0f)*50*orgnormal,
            NUKE_PILLAR_PARTICLE_LIFETIME,
            math::mrand(-0.5f, 0.5f)*NUKE_PILLAR_PARTICLE_SIZE));
    }
    else
    {
      game->addParticle(Particle::randomSmokeHardParticle(),
          Particle(NUKE_PILLAR_SMOKE_COLOR,
            orgpos + math::mrand(0.0f, 1.0f)*(completeratio*NUKE_FIREBALL_HEIGHT-20.0f)*orgnormal + math::vec3f(math::mrand(-25.0f, 25.0f), math::mrand(-25.0f, 25.0f), math::mrand(-25.0f, 25.0f)),
            NUKE_PILLAR_PARTICLE_SIZE,
            math::mrand(0.8f, 1.0f)*50*orgnormal,
            NUKE_PILLAR_PARTICLE_LIFETIME,
            math::mrand(-0.5f, 0.5f)*NUKE_PILLAR_PARTICLE_SIZE));
    }
  }
  // Create rising fireball
  for(int ii=0; ii<6; ii++)
  {
    float rot = math::mrand(0.0f, static_cast<float>(2.0f*M_PI));
    if(ii%2==0)
    {
      game->addParticle(Particle::randomCrackleParticle(),
          Particle(NUKE_FIREBALL_COLOR,
            orgpos + completeratio*orgnormal*NUKE_FIREBALL_HEIGHT + math::mrand(0.1f, 1.0f)*0.5f*(1+completeratio)*NUKE_FIREBALL_SIZE*(math::cos(rot)*norm1+math::sin(rot)*norm2) + math::vec3f(math::mrand(-25.0f, 20.0f), math::mrand(-25.0f, 25.0f), math::mrand(-25.0f, 25.0f)),
            NUKE_FIREBALL_PARTICLE_SIZE,
            raise_speed*orgnormal + math::vec3f(math::mrand(-10.0f, 10.0f), math::mrand(-10.0f, 10.0f), math::mrand(-10.0f, 10.0f)),
            NUKE_FIREBALL_PARTICLE_LIFETIME, -0.8f*NUKE_FIREBALL_PARTICLE_SIZE));
    }
    else
    {
      game->addParticle(Particle::randomSmokeHardParticle(),
          Particle(NUKE_SMOKE_COLOR,
            orgpos + completeratio*orgnormal*NUKE_FIREBALL_HEIGHT + math::mrand(0.1f, 1.0f)*0.5f*(1+completeratio)*NUKE_FIREBALL_SIZE*(math::cos(rot)*norm1+math::sin(rot)*norm2) + math::vec3f(math::mrand(-25.0f, 20.0f), math::mrand(-25.0f, 25.0f), math::mrand(-25.0f, 25.0f)),
            0.5f*NUKE_FIREBALL_PARTICLE_SIZE,
            raise_speed*orgnormal + math::vec3f(math::mrand(-10.0f, 10.0f), math::mrand(-10.0f, 10.0f), math::mrand(-10.0f, 10.0f)),
            NUKE_FIREBALL_PARTICLE_LIFETIME, 0.5f*NUKE_FIREBALL_PARTICLE_SIZE));
    }
  }
  
  // Modify the map.
  if(m_age == NUKE_TIME / 4)
  {
    // Score increment is negative because the nuke decrements the population.
    int score = game->getPopulation().paint(math::vec3f(m_pos), OB_NUKE_BRUSH, true);
    score *= SCORE_MULTIPLIER;
    game->incrementScore(-score);
  }

  this->updateVisibility(false);
  return this->decrementAge();
}

