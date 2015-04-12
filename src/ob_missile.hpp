#ifndef OB_MISSILE_HPP
#define OB_MISSILE_HPP

#include "gfx/entity_object.hpp"
#include "gfx/texture_2d.hpp"
#include "ob_billboard.hpp"
#include "ob_collision_element.hpp"
#include "ob_lifetime.hpp"
#include "ob_space_element.hpp"
#include "ob_visualization.hpp"

namespace ob
{
  /** \brief General missile class.
   *
   * Nukes, anti-nuke missiles and anti-ship missiles are inherited from
   * this class.
   */
  class Missile :
    public gfx::EntityObject,
    public Billboard,
    public CollisionElement,
    public Lifetime,
    public Visualization,
    public SpaceElement
  {
    protected:
      /** Has this missile ringed the alarm? */
      bool m_alarm_sounded;

    public:
      /** \brief Tell if this has warned.
       *
       * \return True if yes, false if no.
       */
      bool hasWarned() const
      {
        return m_alarm_sounded;
      }

    public:
      /** \brief Constructor.
       *
       * \param pos Position.
       * \param dir Direction.
       * \param rad Collision element radius.
       * \param faction Faction.
       * \param billboard Billboard to use.
       */
      Missile(const math::vec3d pos, const math::vec3d dir,
          float rad, uint32_t faction, const gfx::Texture2D *billboard);

      /** \brief Destructor. */
      virtual ~Missile() { }

    protected:
      /** \brief Spawn an explosion for anti-missile.
       */
      void spawnExplosion() const;

      /** \brief Update done when dead.
       *
       * \return Should this be removed from the list?
       */
      bool updateDead();

    public:
      /** \brief Get the position of the target of this.
       *
       * If no target, returns vector directly forward.
       */
      virtual math::vec3d getTargetPos() const = 0;

      /** \brief Update missile path.
       *
       * \return Should this still be alive?
       */
      virtual bool update() = 0;

    public:
      /** \cond */
      virtual const gfx::Texture2D* fillBillboard(BillboardArray &parray,
          const gfx::Shader &sh, const gfx::Texture2D *tex) const;
      virtual void gamisticEffect(void *args);
      /** \endcond */
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Missile> MissileSptr;
}

#endif
