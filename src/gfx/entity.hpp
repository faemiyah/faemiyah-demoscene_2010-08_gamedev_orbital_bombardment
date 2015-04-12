#ifndef GFX_ENTITY_HPP
#define GFX_ENTITY_HPP

#include "gfx/generic.hpp"
#include "math/mat.hpp"

namespace gfx
{
  /** \brief 3D entity.
   *
   * This class encompasses the basic properties of a 3D entity. In practice,
   * this means the orientation and the methods used to manipulate that
   * orientation.
   */
  class Entity
  {
    protected:
      /** Entity position. */
      math::vec3d m_pos;

      /** \brief Entity angles (radians).
       *
       * May be, and will mostly be reused as direction. */
      math::vec3d m_rot;

      /** \brief Entity modelview matrix.
       *
       * Note that the modelview matrix is single-precision even though the
       * position and direction are double-precision. This is just because
       * the matrix is intended to be created from the more exact values.
       */
      math::mat4f m_wm;

    public:
      /** \brief Empty constructor.
       * 
       * Note that this will leave the positional data in unspecified values.
       */
      Entity() { }

      /** \brief Constructor. */
      Entity(const math::vec3d &pos, const math::vec3d &rot);

      /** \brief Destructor. */
      ~Entity() { }

    public:
      /** \brief Project the position of this into 2D space.
       *
       * Uses the transform matrix from the surface.
       *
       * \return Projected position.
       */
      math::vec2f project() const;

      /** \brief Project the position of this into 2D space.
       *
       * \return Projected position.
       */
      math::vec2f project(const math::mat4f &mat) const;

    public:
      /** \brief Get position.
       *
       * \return Position vector.
       */
      const math::vec3d& getPos() const
      {
        return m_pos;
      }

      /** \brief Get rotation.
       *
       * \return Rotation vector.
       */
      const math::vec3d& getRot() const
      {
        return m_rot;
      }

      /** \brief Get the object matrix.
       *
       * \return Reference to the matrix.
       */
      const math::mat4f getWm() const
      {
        return m_wm;
      }

      /** \brief Set the internal matrix as identity.
       *
       * This happens regardless of what the position and rotation actually are.
       */
      const math::mat4f& loadIdentity()
      {
        m_wm.loadIdentity();
        return m_wm;
      }

      /** \brief Set position.
       *
       * \param op New position.
       */
      void setPos(const math::vec3d &op)
      {
        m_pos = op;
      }

      /** \brief Set rotation.
       *
       * Does not apply modulo to the values.
       *
       * \param op New rotation.
       */
      void setRot(const math::vec3d &op)
      {
        m_rot = op;
      }

      /** \brief Set rotation and mod to legal values.
       *
       * Use this if you're using rotation a basis for creating a rotation
       * matrix as opposed to direction.
       *
       * The values will be clamped into a reasonable range.
       *
       * \param op New rotation.
       */
      void setRotCongr(const math::vec3d &op)
      {
        m_rot.x() = math::congr(op.x(), static_cast<double>(M_PI) * 2.0);
        m_rot.y() = math::congr(op.y(), static_cast<double>(M_PI) * 2.0);
        m_rot.z() = math::congr(op.z(), static_cast<double>(M_PI) * 2.0);
      }

      /** \brief Set the world matrix of this.
       *
       * \param op New wm.
       */
      void setWm(const math::mat4f &op)
      {
        m_wm = op;
      }
  };
}

#endif
