#include "ob_height_map_planet.hpp"

#include "ob_constants.hpp"

using namespace ob;

HeightMapPlanet::HeightMapPlanet() :
  gfx::HeightMapBall("gfx/maps/surf_bk.png", "gfx/maps/surf_dn.png",
      "gfx/maps/surf_fw.png", "gfx/maps/surf_lt.png",
      "gfx/maps/surf_rt.png", "gfx/maps/surf_up.png",
      OB_PLANET_RADIUS * (1.0f - OB_PLANET_RADIUS_DIFF),
      OB_PLANET_RADIUS * (1.0f + OB_PLANET_RADIUS_DIFF)) { }

float HeightMapPlanet::calcHeight(const math::vec3f &vv) const
{
  return std::max(this->gfx::HeightMapBall::calcHeight(vv), 0.45f);
}

float HeightMapPlanet::calcTerrain(const math::vec3f &vv) const
{
  float ht = this->calcHeight(vv);
  if(ht < 0.5f)
  {
    ht = (ht - 0.45f) / 0.05f;
  }
  else
  {
    ht = 1.0f + math::sqrt((ht - 0.5f) / 0.5f);
  }
  return ht * 0.5f;
}

