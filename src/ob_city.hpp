#ifndef OB_CITY_HPP
#define OB_CITY_HPP

#include "gfx/entity_object_overlay.hpp"
#include "ob_collision_element.hpp"
#include "ob_lifetime.hpp"
#include "ob_surface_element.hpp"
#include "ob_visualization.hpp"

#include <map>

namespace ob
{
  class PopulationMap;
  class HeightMapPlanet;
  class Silo;

  /** \brief Representation of a city.
   */
  class City :
    public gfx::EntityObjectOverlay,
    public CollisionElement,
    public Lifetime,
    public Visualization,
    public SurfaceElement
  {
    private:
      /** \brief Silos for this city.
       *
       * Indexed by the contents of the smart pointer.
       */
      std::map<size_t, Silo*> m_silos;

    public:
      /** \brief Accessor.
       *
       * \return Silo map.
       */
      std::map<size_t, Silo*>& getSilos()
      {
        return m_silos;
      }

      /** \brief Accessor.
       *
       * \return Silo map.
       */
      const std::map<size_t, Silo*>& getSilos() const
      {
        return m_silos;
      }

    public:
      /** \brief Create a new city.
       *
       * Position is randomized from the heightmap.
       *
       * \param vol Volume to store population data into.
       * \param hmap Height map.
       */
      City(PopulationMap &vol, const HeightMapPlanet &hmap);

      /** \brief Destructor. */
      virtual ~City();

    public:
      /** \brief Paint city centrum population.
       *
       * \param vol Volume to paint to.
       * \param hmap Height map.
       */
      void paintCenter(PopulationMap &vol, const HeightMapPlanet &hmap) const;

      /** \brief Paint city outskirts population.
       *
       * \param vol Volume to paint to.
       * \param hmap Height map.
       */
      void paintOutskirts(PopulationMap &vol, const HeightMapPlanet &hmap) const;

      /** \brief Remove a silo from this.
       *
       * \param silo Silo to remove.
       */
      void removeSilo(const Silo *silo);

      /** \brief Gamistic update.
       */
      bool update(bool tooFewSilos);

    public:
      /** \cond */
      virtual void draw(const gfx::Shader &sh, const math::mat4f &wm) const;
      virtual void gamisticEffect(void *args);
      /** \endcond */

    private:
      /** \brief Generate a random position from heightmap.
       *
       * \param hmap.
       * \return Reference to the new position 
       */
      const math::vec3f& randomizePos(const HeightMapPlanet &hmap);
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<City> CitySptr;
}

#endif
