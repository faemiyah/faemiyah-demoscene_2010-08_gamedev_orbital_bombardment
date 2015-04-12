#ifndef GFX_ENTITY_OBJECT_OVERLAY_HPP
#define GFX_ENTITY_OBJECT_OVERLAY_HPP

#include "gfx/entity_object.hpp"

namespace gfx
{
  /** \brief Overlay object.
   *
   * An object that works otherwise just like the entity object, but has no
   * modelview matrix updates. In other words, it's just laid over the world
   * without taking into account the lighting properties of the world.
   */
  class EntityObjectOverlay :
    public EntityObject
  {
    public:
      /** \brief Construtor. */
      EntityObjectOverlay() { }

      /** \brief Constructor.
       *
       * \param ppos Position.
       * \param prot Rotation.
       */
      EntityObjectOverlay(const math::vec3d &ppos, const math::vec3d &prot) :
        EntityObject(ppos, prot) { }

      /** \brief Constructor.
       *
       * \param ppos Position.
       * \param prot Rotation.
       * \param pmesh Mesh to use.
       */
      EntityObjectOverlay(const math::vec3d &ppos, const math::vec3d &prot, const Mesh *pmesh) :
        EntityObject(ppos, prot, pmesh) { }

      /** \brief Constructor.
       *
       * \param ppos Position.
       * \param prot Rotation.
       * \param op Meshes to use.
       */
      EntityObjectOverlay(const math::vec3d &ppos, const math::vec3d &prot, const Mesh::container_type &op) :
        EntityObject(ppos, prot, op) { }

      /** \brief Constructor.
       *
       * \param pmesh Mesh to use.
       */
      EntityObjectOverlay(const Mesh *pmesh) :
        EntityObject(pmesh) { }

      /** \brief Constructor.
       *
       * \param op Meshes to use.
       */
      EntityObjectOverlay(const Mesh::container_type &op) :
        EntityObject(op) { }

      /** \brief Destructor. */
      virtual ~EntityObjectOverlay() { }

    protected:
      /** \cond */
      virtual math::mat4f orient(const math::mat4f &pmat) const;
      /** \endcond */
  };
}

#endif
