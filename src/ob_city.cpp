#include "ob_city.hpp"

#include "math/random.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"
#include "ob_height_map_planet.hpp"

using namespace ob;

/** Population scale factor (for transforming stuff in volumes). */
static const float OB_POPULATION_CITY_BRUSH = 3.0f;

/** Random population instance count. */
static const int OB_POPULATION_CITY_OUTSKIRTS_MAX = 72;

/** Random population instance count. */
static const int OB_POPULATION_CITY_OUTSKIRTS_MIN = 32;

/** Population scale factor (for transforming stuff in volumes). */
static const float OB_POPULATION_OUTSKIRTS_BRUSH = 1.8f;

/** Limit for 'too close' cities. */
static const float OB_CITY_TOO_CLOSE = 540.0f;

/** City collision area. */
static const float OB_COLLISION_CITY = 1.0f;

/** City death time. */
static const int CITY_DEATH_TIME = 100;

City::City(PopulationMap &vol, const HeightMapPlanet &hmap) :
  gfx::EntityObjectOverlay(gfx::Mesh::locate("city")),
  CollisionElement(OB_COLLISION_CITY, STATIONARY, OB_FACTION_CITY)
{
  while(true)
  {
    m_pos = randomize_pos_on_surface(hmap);
    bool too_close = false;
    BOOST_FOREACH(CityMap::value_type &vv, game->getCities())
    {
      float len2 = static_cast<float>(math::length2(m_pos - vv.second->getPos()));
      if(len2 < OB_CITY_TOO_CLOSE * OB_CITY_TOO_CLOSE)
      {
        too_close = true;
        break;
      }
    }
    if(!too_close)
    {
      break;
    }
  }
  m_wm = lookat_on_surface(m_pos, OB_CITY_MARKER_HEIGHT);

  this->paintCenter(vol, hmap);
  this->paintOutskirts(vol, hmap);

  this->initCollisionData(math::vec3f(m_pos));
  this->updateAreas(game->getOctree());
}

City::~City()
{
#if 0
  if(m_silos.size() > 0)
  {
    std::stringstream sstr;
    sstr << m_silos.size() << " silos remaining at city " << this
      << " destruction";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
#else
  for(std::map<size_t, Silo*>::iterator ii = m_silos.begin(), ee = m_silos.end();
      (ii != ee); ++ii)
  {
    ii->second->clearCity();
  }
#endif
}

void City::draw(const gfx::Shader &sh, const math::mat4f &wm) const
{
  if(!this->isVisible())
  {
    return;
  }

  this->EntityObjectOverlay::draw(sh, wm);
}

void City::gamisticEffect(void *args)
{
  boost::ignore_unused_variable_warning(args);

  if(!this->isDead())
  {
    this->removeFromAreas();
    this->die(CITY_DEATH_TIME);
  }
}

void City::paintCenter(PopulationMap &vol, const HeightMapPlanet &hmap) const
{
  boost::ignore_unused_variable_warning(hmap);

  vol.paint(math::vec3f(m_pos), OB_POPULATION_CITY_BRUSH);
}

void City::paintOutskirts(PopulationMap &vol, const HeightMapPlanet &hmap) const
{
  for(int ii = 0, cap = math::mrand(OB_POPULATION_CITY_OUTSKIRTS_MIN, OB_POPULATION_CITY_OUTSKIRTS_MAX); (ii <= cap); ++ii)
  {
    math::vec3f rndvec(math::mrand(-OB_POPULATION_OUTSKIRTS_PROXIMITY, OB_POPULATION_OUTSKIRTS_PROXIMITY),
        math::mrand(-OB_POPULATION_OUTSKIRTS_PROXIMITY, OB_POPULATION_OUTSKIRTS_PROXIMITY),
        math::mrand(-OB_POPULATION_OUTSKIRTS_PROXIMITY, OB_POPULATION_OUTSKIRTS_PROXIMITY)),
      rndpos = math::vec3f(m_pos) + rndvec;
    if(hmap.calcHeight(rndpos) > OB_TERRAIN_LEVEL)
    {
      vol.paint(rndpos, OB_POPULATION_OUTSKIRTS_BRUSH);
    }
  }
}

void City::removeSilo(const Silo *silo)
{
  size_t sptr = reinterpret_cast<size_t>(silo);
  std::map<size_t, Silo*>::iterator iter = m_silos.find(sptr);
  if(m_silos.end() == iter)
  {
    std::stringstream sstr;
    sstr << "requested to remove non-existing silo " << silo <<
      " from city " << this;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  m_silos.erase(iter);
}

bool City::update(bool tooFewSilos)
{
  if(this->isDead())
  {
    this->updateVisibility(false);
    return this->decrementAge();
  }

  float len2rpos = static_cast<float>(math::length2(m_pos - game->getView().getPos()));
  bool vis = is_surface_visible(len2rpos);
  this->updateVisibility(vis);
  if(!vis && !is_surface_processible(len2rpos))
  {
    return true;
  }

  if((m_silos.size() < OB_SILOS_PER_CITY) &&
      (math::mrand(1, OB_SILO_POPUP_PROBABILITY) == 1) &&
      tooFewSilos)
  {
    //std::cout << "new city silo\n";
    Silo *silo(new Silo(glob->getHeightMapPlanet(), this));
    size_t sptr = reinterpret_cast<size_t>(silo);
    m_silos[sptr] = silo;
    game->addSilo(silo);
    //std::cout << "creating silo " << m_silos.size() << " for city " << this << std::endl;
  }
  return true;
}

