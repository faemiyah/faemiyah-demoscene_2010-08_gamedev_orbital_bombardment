#ifndef OB_TARGET_HPP
#define OB_TARGET_HPP

#include "math/vec.hpp"

namespace gfx
{
  class EntityObject;
  class Texture2D;
}

namespace ob
{
  /** 2D priority (target).
   *
   * Used for selecting targets in the game in 2D space.
   */
  class Target
  {
    private:
      /** Target position in 2D. */
      math::vec2f m_pos;

      /** Reference to target. */
      const gfx::EntityObject *m_target;

      /** Billboard size. */
      float m_billboard_size;

      /** Corresponding target icon. */
      const gfx::Texture2D *m_icon;

      /** Allow lock on this. */
      bool m_allow_lock;

    public:
      /** \brief Accessor.
       *
       * \return Billboard size.
       */
      float getBillboardSize() const
      {
        return m_billboard_size;
      }

      /** \brief Accessor.
       *
       * \return True if can lock, false if no.
       */
      bool canLock() const
      {
        return m_allow_lock;
      }

      /** \brief Accessor.
       *
       * \return Texture pointer.
       */
      const gfx::Texture2D* getIcon() const
      {
        return m_icon;
      }

      /** \brief Accessor.
       *
       * \return Position.
       */
      const math::vec2f& getPos() const
      {
        return m_pos;
      }

      /** \brief Accessor.
       *
       * \return Target.
       */
      const gfx::EntityObject* getTarget() const
      {
        return m_target;
      }

    public:
      /** \brief Constructor.
       *
       * \param pos Position (in 2D).
       * \param target Target.
       * \param billboard_size Billobard size.
       * \param icon Icon to display as target.
       * \param allow_lock True to allow lock-on.
       */
      Target(const math::vec2f &pos, const gfx::EntityObject *target,
          float billboard_size, const gfx::Texture2D &icon, bool allow_lock) :
        m_pos(pos),
        m_target(target),
        m_billboard_size(billboard_size),
        m_icon(&icon),
        m_allow_lock(allow_lock) { }

      /** \brief Destructor. */
      ~Target() { }

    public:
      /** \brief Less than.
       *
       * \param rhs Right-hand-side operand.
       */
      bool operator<(const Target &rhs) const
      {
        return (math::length2(m_pos) > math::length2(rhs.m_pos));
      }
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Target> TargetSptr;
}

#endif
