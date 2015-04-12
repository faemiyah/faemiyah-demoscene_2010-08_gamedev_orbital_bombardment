#include "ob_silo.hpp"

#include "math/random.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** Death particle color. */
static const gfx::Color DEATH_PARTICLE_COLOR(1.0f, 0.3f, 0.1f, 1.0f);

/** Death particle size. */
static const float DEATH_PARTICLE_SIZE = 24.0f;

/** Death particle travel multiplier. */
static const float DEATH_PARTICLE_SPEED = 25.0f;

/** Death particle lifetime. */
static const int DEATH_PARTICLE_LIFETIME = 50;

/** Time it takes to die. */
static const int DEATH_TIME = 60;

/** Silo collision area. */
static const float OB_COLLISION_SILO = 9.0f;

/** Silo popup area. */
static const float OB_SILO_INITIAL_PROXIMITY_COUNTRYSIDE = 1800.0f;

/** Silo popup area. */
static const float OB_SILO_INITIAL_PROXIMITY_CITY = 350.0f;

Silo::Silo(const HeightMapPlanet &hmap, const math::vec3d &refpos) :
  gfx::EntityObject(glob->getMeshSilo()),
  CollisionElement(OB_COLLISION_SILO, STATIONARY, OB_FACTION_ENEMY),
  /*m_reload_time(math::mrand(0, OB_SILO_RELOAD)),*/
  m_reload_time(OB_SILO_RELOAD),
  m_is_new_silo(true),
  m_city(NULL)
{
  m_pos = randomize_pos_on_surface(hmap, refpos,
      OB_SILO_INITIAL_PROXIMITY_COUNTRYSIDE);
  m_rot = math::vec3d(math::mrand(-1.0, 1.0),
      math::mrand(-1.0, 1.0),
      math::mrand(-1.0, 1.0));

  this->initType();
  this->initCollisionData(math::vec3f(m_pos));
  this->updateAreas(game->getOctree());
}

Silo::Silo(const HeightMapPlanet &hmap, City *city) :
  gfx::EntityObject(glob->getMeshSilo()),
  CollisionElement(OB_COLLISION_SILO, STATIONARY, OB_FACTION_ENEMY),
  /*m_reload_time(math::mrand(0, OB_SILO_RELOAD)),*/
  m_reload_time(OB_SILO_RELOAD),
  m_is_new_silo(true),
  m_city(city)
{
  m_pos = randomize_pos_on_surface(hmap, m_city->getPos(),
      OB_SILO_INITIAL_PROXIMITY_CITY);
  m_rot = math::vec3d(math::mrand(-1.0, 1.0),
      math::mrand(-1.0, 1.0),
      math::mrand(-1.0, 1.0));

  this->initType();
  this->initCollisionData(math::vec3f(m_pos));
  this->updateAreas(game->getOctree());
}

Silo::~Silo()
{
  if(m_city)
  {
    m_city->removeSilo(this);
  }
}

const gfx::Texture2D* Silo::fillBillboard(BillboardArray &parray,
    const gfx::Shader &sh, const gfx::Texture2D *tex) const
{
  if(!this->isVisible())
  {
    return NULL;
  }

  const gfx::Texture2D *ret = this->updateBillboard(parray, sh, tex);

  parray.push_back(
      gfx::PointSprite(gfx::COLOR_WHITE, math::vec3f(m_pos), OB_BILLBOARD_SIZE_SILO));
  return ret;
}

bool Silo::fire()
{
  if (m_is_new_silo)
  {
    m_is_new_silo = false;
    switch(m_anti)
    {
      case 1:
        this->setBillboard(&(glob->getTextureSiloAntiNuke()));
        break;

      case 2:
        this->setBillboard(&(glob->getTextureSiloAntiShip()));
        break;

      default:
        this->setBillboard(&(glob->getTextureSiloBoth()));
        break;
    }
  }
  const Missile *tgt = NULL;
  double last_dist = DBL_MAX;

  MissileMap &missiles_nuke = game->getMissilesNuke();
  BOOST_FOREACH(const MissileMap::value_type &vv, missiles_nuke)
  {
    const MissileNuke *nuke = static_cast<const MissileNuke*>(vv.second.get());
    if(nuke->isDead())
    {
      continue;
    }

    if(tgt)
    {
      double new_dist = math::length2(m_pos - nuke->getPos());
      if(new_dist < last_dist)
      {
        tgt = nuke;
        last_dist = new_dist;
      }
    }
    else
    {
      tgt = nuke;
      last_dist = math::length2(m_pos - tgt->getPos());
    }
  }

  if(tgt && (last_dist <= OB_PROXIMITY_NUKE * OB_PROXIMITY_NUKE) &&
      this->shootsNukes())
  {
    game->addMissileAnti(new MissileAnti(m_pos, math::normalize(m_pos), tgt));
    return true;
  }
  else if(this->shootsShip())
  {
    math::vec3d ppos = game->getView().getPos();
    if(math::length2(m_pos - ppos) < OB_PROXIMITY_SHIP * OB_PROXIMITY_SHIP)
    {
      game->addMissileAnti(new MissileAnti(m_pos, math::normalize(m_pos), NULL));
      return true;
    }
  }
  return false;
}

void Silo::gamisticEffect(void *args)
{
  boost::ignore_unused_variable_warning(args);

  if(this->die(DEATH_TIME))
  {
    snd::play(glob->getSampleTargetDestroyed(), math::vec3f(m_pos));
    this->removeFromAreas();
  }
}

void Silo::initType()
{
  m_anti = static_cast<uint8_t>(math::mrand(1, 3));
  switch(m_anti)
  {
    case 1:
      this->setBillboard(&(glob->getTextureSiloAntiNukeInactive()));
      break;

    case 2:
      this->setBillboard(&(glob->getTextureSiloAntiShipInactive()));
      break;

    default:
      this->setBillboard(&(glob->getTextureSiloBothInactive()));
      break;
  }
  //this->setBillboard(&(glob->getTextureMissileAntiShip()));
}

void Silo::spawnExplosion() const
{
  math::vec3f orgpos(m_pos);
  math::vec3f orgnormal(math::normalize(m_pos));

  // One-time shockwave effect
  if(m_age==DEATH_TIME)
  {
    math::vec3f norm1,norm2;
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
    
    math::vec3f playerpos(game->getView().getPos());
    math::vec3f glowspot = math::normalize(playerpos-orgpos)*80;

    // Create the fast, initial planar shockwave
    for(int ii=0; ii<100; ii++)
    {
      float rot = math::mrand(0.0f, static_cast<float>(2.0f*M_PI));
      game->addParticle(GLOW_SHARP,
          Particle(DEATH_PARTICLE_COLOR,
            orgpos, 0.2f*DEATH_PARTICLE_SIZE,
            2.0f*DEATH_PARTICLE_SPEED*(math::cos(rot)*norm1+math::sin(rot)*norm2),
            DEATH_PARTICLE_LIFETIME, 0.8f*DEATH_PARTICLE_SIZE));
    }
    // Create glow
    game->addParticle(GLOW_SOFT,
        Particle(DEATH_PARTICLE_COLOR,
          orgpos + glowspot, 100.0f,
          math::vec3f(0.0f, 0.0f, 0.0f),
          DEATH_TIME, 0));
  }

  if(m_age > DEATH_TIME/2)
  {
  for(unsigned ii = 0; (ii < 2); ++ii)
  {
      math::vec3f pdir = (math::vec3f(math::mrand(-1.0f, 1.0f),
          math::mrand(-1.0f, 1.0f),
          math::mrand(-1.0f, 1.0f)) + 3.0f*orgnormal) *
        DEATH_PARTICLE_SPEED;
      game->addParticle(SPARKLE_5,
          Particle(DEATH_PARTICLE_COLOR,
            math::vec3f(m_pos), DEATH_PARTICLE_SIZE,
            pdir, DEATH_PARTICLE_LIFETIME, 2.0f*DEATH_PARTICLE_SIZE));
    }
  }
}

bool Silo::update()
{
  if(this->isDead())
  {
    this->spawnExplosion();
    this->updateVisibility(false);
    return this->decrementAge();
  }
  else
  {
    float len2rpos = static_cast<float>(
        math::length2(m_pos - game->getView().getPos()));
    bool vis = is_surface_visible(len2rpos);
    this->updateVisibility(vis);
    if(!vis)
    {
      if(!is_surface_processible(len2rpos))
      {
        return true;
      }
    }
    else
    {
      game->incSilosInRange();
    }


    // Collision upward.
    CollisionElement *other = this->checkCollisions();
    if(NULL != other)
    {
      //std::cout << "silo collision!\n";
      other->gamisticEffect(NULL);
      this->gamisticEffect(NULL);
      return true;
    }
  }

  if(--m_reload_time <= 0)
  {
    m_reload_time = this->fire() ? OB_SILO_RELOAD : 0;
  }

  return true;
}

