#ifndef OB_COLLISION_ELEMENT_HPP
#define OB_COLLISION_ELEMENT_HPP

#include "math/rect.hpp"

#include <list>

namespace ob
{
  class Octree;
  class OctreeArea;

  /** \brief Collision element type.
   *
   * Limited range of possibilities.
   */
  enum CollisionElementType
  {
    STATIONARY = 0, /** Stationary point. */
    POINT, /** Ball-style object. */
    LINE /** Line-style object. */
  };

  /** \brief Generic collision element.
   *
   * This is tailored for the simple collision system required for the game.
   * A more complex system would be a waste of space and introduce additional
   * needless complexity.
   */
  class CollisionElement
  {
    private:
      /** Collision area this is in. */
      OctreeArea *m_area;

      /** Worst-case collision box. */
      math::rect3f m_box;

      /** Collision points now. */
      math::vec3f m_pnts_current[2];

      /** Collision points now. */
      math::vec3f m_pnts_former[2];

      /** Collision radius (for points and lines). */
      float m_radius;

      /** Collision type. */
      CollisionElementType m_type;

      /** \brief Faction.
       *
       * If OR with another faction is non-zero, objects can not collide.
       */
      uint32_t m_faction;

    public:
      /** \brief Accessor.
       *
       * \return Collision worst-case box.
       */
      const math::rect3f& getArea()
      {
        return m_box;
      }

    public:
      /** \brief Constructor.
       *
       * \param radius Radius associated with this element.
       * \param type Type of this element.
       * \param faction Faction this belongs in.
       */
      CollisionElement(float radius, CollisionElementType type,
          uint32_t faction);

      /** \brief Destructor.
       */
      virtual ~CollisionElement();

    public:
      /** \brief Gamistic effect for inheritance.
       *
       * Could be called, for example, after collision.
       *
       * All parameters are left for the user to fill.
       *
       * \param args Arguments for gamistic effect.
       */
      virtual void gamisticEffect(void *args) = 0;

    private:
      /** \brief Check for collisions within a single collision area.
       *
       * \param area Area to check.
       * \return A collision element if found or NULL.
       */
      CollisionElement* checkCollisions(const OctreeArea *area) const;

      /** \brief Invokes all collisions in a single collision area.
       *
       * \param dst Store collision elements here.
       * \param area Area to check.
       */
      void getAllCollisions(std::list<CollisionElement*> &dst,
          const OctreeArea *area) const;

      /** \brief Invokes all collisions in a single collision area.
       *
       * Recurses down.
       *
       * \param dst Store collision elements here.
       * \param area Area to check.
       */
      void getAllCollisionsRecursive(std::list<CollisionElement*> &dst,
          const OctreeArea *area) const;

    public:
      /** \brief Check for collisions.
       *
       * Must be called after the physics updates have already been done and
       * consequently area list updated.
       *
       * The first collision counts.
       *
       * Additionally, a non-issue collision element may be passed to the
       * function. If done, this element is excluded from testing.
       *
       * \return First collision found or NULL.
       */
      CollisionElement* checkCollisions() const;

      /** \brief Check for collision to a singular another element.
       *
       * \param other Other element.
       * \return True if collision, false if not.
       */
      bool collides(const CollisionElement &other) const;

      /** \brief Invokes all collisions for this.
       *
       * \param dst Destination list.
       */
      void getAllCollisions(std::list<CollisionElement*> &dst) const;

      /** \brief Initialize area content in this.
       *
       * To be called before updateData in construction.
       *
       * \param pa First point.
       */
      void initCollisionData(const math::vec3f &pa);

      /** \brief Update the contents in this.
       *
       * To be called before updateData in construction.
       *
       * \param pa First point.
       * \param pb Second point.
       */
      void initCollisionData(const math::vec3f &pa, const math::vec3f &pb);

      /** \brief Insert this into a specific collision area.
       *
       * Will work recursively down.
       *
       * \param tree Area to search from.
       */
      void insert(OctreeArea *tree);

      /** \brief Remove from any collision areas this currently is in.
       */
      void removeFromAreas();

      /** \brief Add this to collision areas.
       *
       * Removes this element from all collision areas it's in, then re-adds
       * it to the tree, repopulating the area list.
       *
       * The encompassing worst-case box must be calculated before this.
       *
       * \param tree Original tree to add into.
       */
      void updateAreas(Octree &tree);

      /** \brief Update the contents in this.
       *
       * Recalculates the worst-case box after modifying the point data.
       *
       * \param pa First point.
       */
      void updateCollisionData(const math::vec3f &pa);

      /** \brief Update the contents in this.
       *
       * Recalculates the worst-case box after modifying the point data.
       *
       * \param pa First point.
       * \param pb Second point.
       */
      void updateCollisionData(const math::vec3f &pa, const math::vec3f &pb);

    private:
      /** \brief Expand the collision box withe the collision radius of this.
       */
      void expandBoxWithRadius()
      {
        m_box.x1() -= m_radius;
        m_box.y1() -= m_radius;
        m_box.z1() -= m_radius;
        m_box.x2() += m_radius;
        m_box.y2() += m_radius;
        m_box.z2() += m_radius;
      }

    public:
      /** \brief Set the radius of this.
       *
       * To be used when for example exploding.
       *
       * \param op New radius.
       */
      void setRadius(float op)
      {
        m_radius = op;
      }

      /** \brief Set the collision type of this.
       *
       * To be used when for example exploding.
       *
       * \param op New radius.
       */
      void setType(CollisionElementType op)
      {
        m_type = op;
      }

    public:
      /** \cond */
      friend std::ostream& operator<<(std::ostream &lhs, const CollisionElement &rhs);
      /** \endcond */
  };

  /** \brief Output collision element to a stream.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Stream after input.
   */
  std::ostream& operator<<(std::ostream &lhs, const CollisionElement &rhs);
}

#endif
