#ifndef OB_SILO_HPP
#define OB_SILO_HPP

#include "gfx/entity_object.hpp"
#include "ob_billboard.hpp"
#include "ob_collision_element.hpp"
#include "ob_lifetime.hpp"
#include "ob_surface_element.hpp"
#include "ob_visualization.hpp"

namespace ob
{
  class City;
  class HeightMapPlanet;

  /** \brief Missile silo on planet surface.
   *
   * Silos spam missiles against the ship and nukes.
   */
  class Silo :
    public gfx::EntityObject,
    public Billboard,
    public CollisionElement,
    public Lifetime,
    public Visualization,
    public SurfaceElement
  {
    private:
      /** Reload time left. */
      int m_reload_time;

      /** True at the beginning, set false after first shot*/
      bool m_is_new_silo;

      /** Host city, may be null. */
      City *m_city;

      /** \brief Anti of this.
       *
       * Possibilities are:
       *
       * 1 = ANTI-NUKE
       * 2 = ANTI-SHIP
       * 3 = BOTH
       */
      uint8_t m_anti;

    public:
      /** \brief Empty constructor.
       *
       * This will create the silo in a random position, not necessarily in the
       * proximity of a city.
       *
       * \param hmap Height map to extract desired altitude from.
       * \param refpos Reference pos to start deviating from.
       */
      Silo(const HeightMapPlanet &hmap, const math::vec3d &refpos);

      /** \brief Constructor.
       *
       * \param hmap Height map to extract desired altitude from.
       * \param city City to create in proximity to.
       */
      Silo(const HeightMapPlanet &hmap, City *city);

      /** \brief Destructor. */
      virtual ~Silo();

    private:
      /** \brief Fire against a designated target,
       *
       * \return True if fired, false if not.
       */
      bool fire();

      /** \brief Initialize silo type. */
      void initType();

      /** \brief Spawn explosions. */
      void spawnExplosion() const;

    public:
      /** \brief Gamistic update.
       *
       * \return True if silo still exists after this.
       */
      bool update();

    public:
      /** \cond */
      virtual const gfx::Texture2D* fillBillboard(BillboardArray &parray,
          const gfx::Shader &sh, const gfx::Texture2D *tex) const;
      virtual void gamisticEffect(void *args);
      /** \endcond */

    public:
      /** \brief Tell if this silo shoots missiles.
       *
       * \return True if yes, false if no.
       */
      bool shootsNukes() const
      {
        return ((m_anti & 1) != 0);
      }

      /** \brief Tell if this silo shoots the player.
       *
       * \return True if yes, false if no.
       */
      bool shootsShip() const
      {
        return ((m_anti & 2) != 0);
      }

      /** \brief Clear the referring city.
       *
       * Should only be called by the city itself, and only when dying.
       */
      void clearCity()
      {
        m_city = NULL;
      }
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Silo> SiloSptr;
}

#endif
