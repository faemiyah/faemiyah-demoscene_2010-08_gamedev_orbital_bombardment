#ifndef OB_HEIGHT_MAP_PLANET_HPP
#define OB_HEIGHT_MAP_PLANET_HPP

#include "gfx/height_map_ball.hpp"

namespace ob
{
  /** \brief Orbital Bombardment planet surface map.
   */
  class HeightMapPlanet :
    public gfx::HeightMapBall
  {
    public:
      /** \brief Constructor. */
      HeightMapPlanet();

      /** \brief Destructor. */
      virtual ~HeightMapPlanet() { }

    public:
      /** \cond */
      virtual float calcHeight(const math::vec3f &pnt) const;
      virtual float calcTerrain(const math::vec3f &pnt) const;
      /** \endcond */
  };
}

#endif
