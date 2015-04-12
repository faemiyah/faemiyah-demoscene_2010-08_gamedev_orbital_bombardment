#include "ob_missile_anti.hpp"

#include "math/random.hpp"
#include "gfx/mesh_static.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** Aim blink (inverse probability for blink). */
static const int AIM_BLINK = 75;

/** Aim tolerance value for determining when it's safe to fire. */
static const double AIM_TOLERANCE = 0.01;

/** Missile collision area. */
static const float OB_COLLISION_MISSILE_ANTI = 1.0f;

/** Missile main acceleration. */
static const float OB_MISSILE_ACCELERATION = 10.0f;

/** Missile lateral acceleration. */
static const float OB_MISSILE_LATERAL_ACCELERATION = 0.1f;

/** Anti-nuke alarm proximity. */
static const float OB_MISSILE_ANTI_NUKE_ALARM_PROXIMITY = 800.0f;

/** Convenience. */
static const float OB_MISSILE_ANTI_NUKE_ALARM_PROXIMITY_2 = OB_MISSILE_ANTI_NUKE_ALARM_PROXIMITY * OB_MISSILE_ANTI_NUKE_ALARM_PROXIMITY;

/** Missile alarm when closer than this. */
static const float OB_MISSILE_ANTI_SHIP_ALARM_PROXIMITY = 800.0f;

/** Convenience. */
static const float OB_MISSILE_ANTI_SHIP_ALARM_PROXIMITY_2 = OB_MISSILE_ANTI_SHIP_ALARM_PROXIMITY * OB_MISSILE_ANTI_SHIP_ALARM_PROXIMITY;

/** Warning blinking level. */
static const int OB_WARNING_BLINK_LEVEL = 3;

/** Missile speed. */
static const float OB_MISSILE_SPEED = 90.0f;

/** Underground level. */
static const float MISSILE_UNDERGROUND = OB_PLANET_RADIUS - 8.0f;

MissileAnti::MissileAnti(const math::vec3d &pos, const math::vec3d &dir,
    const Missile *tgt) :
  Missile(pos, dir, OB_COLLISION_MISSILE_ANTI, OB_FACTION_ENEMY_MISSILE,
      ((NULL != tgt) ? &(glob->getTextureMissileAntiNuke()) : &(glob->getTextureMissileAntiShip()))),
  m_target(tgt)
#if (0 != MISSILE_ANTI_NEED_PREV_DIR)
  ,m_prev_dir(m_rot)
#endif
{
  this->addMesh(glob->getMeshMissileAnti());
  gainingheight = (tgt==NULL ? false : true);
  raisephaseend = (tgt==NULL ? 0 : 1000);
}

math::vec3d MissileAnti::getTargetPos() const
{
  if(m_target)
  {
    if(game->hasMissileNuke(m_target))
    {
      return m_target->getPos();
    }
    else
    {
      return m_rot;
    }
  }
  return game->getView().getPos();
}

bool MissileAnti::update()
{
  if(this->isDead())
  {
    return this->updateDead();
  }

  math::vec3d tgt_pos;
  math::vec3d tgt_dir;
  if(m_target)
  {
    const MissileNuke *ntarget = static_cast<const MissileNuke*>(m_target);
    if(game->hasMissileNuke(ntarget) && !ntarget->isDead())
    {
      tgt_pos = ntarget->getPos();
      tgt_dir = ntarget->getRot();

      if(math::length2(tgt_pos - m_pos) <= OB_MISSILE_ANTI_NUKE_ALARM_PROXIMITY_2)
      {
        this->setBillboard(&(glob->getTextureMissileAntiNukeWarning()));
        this->enableRandom(OB_WARNING_BLINK_LEVEL);
      }
    }
    else
    {
      this->gamisticEffect(NULL);
      return this->updateDead();
    }
  }
  else
  {
    const GameView &gv = game->getView();
    tgt_pos = gv.getPos();
    tgt_dir = gv.getDir() * OB_ORBIT_SPEED;

    // Play alarm if dangerously close.
    if(math::length2(tgt_pos - m_pos) <= OB_MISSILE_ANTI_SHIP_ALARM_PROXIMITY_2)
    {
      if(!m_alarm_sounded)
      {
        this->setBillboard(&(glob->getTextureMissileAntiShipWarning()));
        snd::play(glob->getSampleAlarm(), math::vec3f(m_pos));
        m_alarm_sounded = true;
      }
      this->enableRandom(OB_WARNING_BLINK_LEVEL);
    }
  }

  math::vec3d aimtarget = calculateMissileAimTarget(tgt_pos, tgt_dir, m_pos, m_rot);
  //math::vec3d aimtarget = SpaceElement::calculateBulletAimTarget(tgt_pos, tgt_dir, m_pos, math::vec3d(0.0,0.0,0.0), OB_MISSILE_SPEED);

  // The purpose of this senseless looking part is to have the missiles gain enough height while aiming at the nuke. Don't do anything for anti-ship missiles.
  if(gainingheight)
  {
    math::vec3d nuketarget = m_target->getTargetPos();
    //std::cout<< "Target: " << aimtarget << " Speed: " << m_rot << " Pos: " << m_pos << "\n";
    gainingheight = dist2_line_point(math::vec3d(m_pos + math::normalize(m_pos)), math::vec3d(nuketarget + math::normalize(nuketarget)), math::vec3d(0.0, 0.0, 0.0)) < OB_PLANET_RADIUS*OB_PLANET_RADIUS;
    //std::cout<< "Line intersection: " << (gainingheight ? "YES" : "NO") << "\n";
    if(!gainingheight)
      raisephaseend = 3*m_age;
  }

  updatePosDir(m_pos, m_rot, aimtarget-m_pos, OB_MISSILE_SPEED,
      OB_MISSILE_ACCELERATION, OB_MISSILE_LATERAL_ACCELERATION, (m_age < raisephaseend ? 0.01f : 1.0f));
  // FIXME: more reasonable up vector?
  m_wm.loadLookAt(math::vec3f(m_pos), math::vec3f(m_pos + m_rot), math::vec3f(tgt_pos));

#if (0 != MISSILE_ANTI_NEED_PREV_DIR)
  m_prev_dir -= m_rot;
  {
    double atol = math::abs(m_prev_dir.x()) + math::abs(m_prev_dir.y()) + math::abs(m_prev_dir.z());
    //std::cout << atol << std::endl;
    if(AIM_TOLERANCE < atol)
    {
      if(this->getVisibility() > AIM_BLINK)
      {
        this->setVisibility(AIM_BLINK);
      }
    }
    this->updateVisibility(true);
  }
  m_prev_dir = m_rot;
#endif

  // Sanity check to ensure that the missile does not go underground.
  if(math::length2(m_pos) <= MISSILE_UNDERGROUND * MISSILE_UNDERGROUND)
  {
    this->gamisticEffect(NULL);
    return true;
  }

  // Normal collision check.
  this->updateCollisionData(math::vec3f(m_pos));
  this->updateAreas(game->getOctree());
  CollisionElement *other = this->checkCollisions();
  if(other)
  {
#if 0
    std::cout << "missile:\n" << *static_cast<CollisionElement*>(this) << std::endl;

    if(other == &(game->getView()))
    {
      std::cout << "cruiser:\n" << *static_cast<CollisionElement*>(&(game->getView())) << std::endl;
    }
    else if(game->hasMissileNuke(static_cast<Missile*>(other)))
    {
      std::cout << "nuke:\n" << *static_cast<CollisionElement*>(other) << std::endl;
    }
#endif

    //std::cout << "collision!\n";
    other->gamisticEffect(NULL);
    this->gamisticEffect(NULL);
    return true;
  }

  // 50% chance of either particle trail.
  if(m_age%3==0)
  {
    if(!math::mrand(0, 2))
    {
      game->addParticle(SMOKE_SOFT_1,
          Particle(gfx::Color(0.5f, 0.55f, 0.55f, 0.7f),
            math::vec3f(m_pos)-math::normalize(m_rot)*0.7f, OB_BILLBOARD_SIZE_SMOKE,
            math::mrand(0.0f, 1.0f) * 0.1f * OB_MISSILE_SPEED * math::vec3f(math::normalize(m_rot)),
            OB_PARTICLE_TIME_SMOKE, -OB_BILLBOARD_SIZE_SMOKE/2.0f));
    }
    else
    {
      game->addParticle(SMOKE_HARD_1,
          Particle(gfx::Color(1.0f, 1.0f, 0.5f, 0.8f),
            math::vec3f(m_pos)-math::normalize(m_rot)*0.7f, OB_BILLBOARD_SIZE_SMOKE,
            math::mrand(0.0f, 1.0f) * 0.1f * OB_MISSILE_SPEED * math::vec3f(math::normalize(m_rot)),
            OB_PARTICLE_TIME_SMOKE, -OB_BILLBOARD_SIZE_SMOKE));
    }
  }

  if(!this->incrementAge(OB_BULLET_DEATH_PROBABILITY))
  {
    this->gamisticEffect(NULL);
  }
  return true;
}

