#ifndef OB_BULLET_RAILGUN_HPP
#define OB_BULLET_RAILGUN_HPP

#include "gfx/entity_object_overlay.hpp"
#include "ob_collision_element.hpp"
#include "ob_lifetime.hpp"
#include "ob_space_element.hpp"

namespace ob
{
  /** \brief Bullet.
   *
   * Represents one straight-moving bullet with limited lifetime.
   */
  class BulletRailgun :
    public gfx::EntityObjectOverlay,
    public CollisionElement,
    public Lifetime,
    public SpaceElement
  {
    public:
      /** \brief Constructor with target.
       *
       * \param ppos Position.
       * \param pdir Direction.
       * \param momentum Direction of momentum.
       * \param tgt Target.
       */
      BulletRailgun(const math::vec3d &ppos, const math::vec3d &pdir,
          const math::vec3d &momentum, const EntityObject *target = NULL);

      /** \brief Destructor. */
      virtual ~BulletRailgun() { }

    public:
      /** \cond */
      virtual void gamisticEffect(void *args);
      /** \endcond */

    public:
      /** \brief Spawn an explosion.
       */
      void spawnExplosion();

      /** \brief Gamistic update.
       *
       * \return True if this still lives after update.
       */
      bool update();
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<BulletRailgun> BulletRailgunSptr;
}

#endif
