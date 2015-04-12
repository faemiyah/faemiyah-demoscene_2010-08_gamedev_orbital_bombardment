#ifndef GFX_ENTITY_CAMERA_HPP
#define GFX_ENTITY_CAMERA_HPP

#include "gfx/entity.hpp"

namespace gfx
{
  /** \brief Camera entity.
   */
  class EntityCamera : public Entity
  {
    public:
      /** \brief constructor. */
      EntityCamera() { }

      /** \brief constructor.
       *
       * \param pos Position.
       * \param rot Rotation.
       */
      EntityCamera(const math::vec3d &pos, const math::vec3d &rot);

      /** \brief Destructor. */
      ~EntityCamera() { }

    public:
      /** \brief Load this camera as zxy rotation.
       *
       * \return Reference to the generated matrix.
       */
      const math::mat4f& loadZXY();
  };
}

#endif
