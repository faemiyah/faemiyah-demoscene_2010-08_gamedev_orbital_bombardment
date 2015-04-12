#ifndef OB_OCTREE_HPP
#define OB_OCTREE_HPP

#include "math/rect.hpp"

#include <boost/array.hpp>

#include <list>

namespace ob
{
  class CollisionElement;
  class OctreeArea;

  /** Convenience typedef. */
  typedef boost::shared_ptr<OctreeArea> OctreeAreaSptr;

  /** \brief Singular collision area.
   */
  class OctreeArea
  {
    private:
      /** The comprising cube. */
      math::rect3f m_area;

      /** Parent area. */
      OctreeArea *m_parent;

      /** Recursive octrees inside this. */
      boost::array<OctreeAreaSptr, 8> m_recursive;

      /** \brief Collision elements within this.
       *
       * The list makes removals O(N), but considering the potential low
       * number of elements per area, a simple construct like this actually
       * has the least performance impact.
       */
      std::list<CollisionElement*> m_elements;

      /** Combined number of elements in all child boxes. */
      unsigned m_combined;

    public:
      /** \brief Accessor.
       *
       * \return The area associated with this.
       */
      const math::rect3f& getArea() const
      {
        return m_area;
      }

      /** \brief Accessor.
       *
       * \return The element list.
       */
      const std::list<CollisionElement*>& getElements() const
      {
        return m_elements;
      }

      /** \brief Accessor.
       *
       * \return The parent area.
       */
      OctreeArea* getParent()
      {
        return m_parent;
      }

      /** \brief Accessor.
       *
       * \return The parent area.
       */
      const OctreeArea* getParent() const
      {
        return m_parent;
      }

      /** \brief Accessor.
       *
       * \return Recursive area array.
       */
      boost::array<OctreeAreaSptr, 8>& getRecursive()
      {
        return m_recursive;
      }

      /** \brief Accessor.
       *
       * \return Recursive area array.
       */
      const boost::array<OctreeAreaSptr, 8>& getRecursive() const
      {
        return m_recursive;
      }

    public:
      /** \brief Constructor.
       *
       * \param parea The area relevant to this.
       * \param parent Parent area.
       */
      OctreeArea(const math::rect3f &parea, OctreeArea *parent);

      /** \brief Destructor. */
      ~OctreeArea();

    public:
      /** \brief Add a collison element into this.
       *
       * Will increment reference counts in all upper areas.
       */
      void add(CollisionElement *element);

      /** \brief Create empty children for this.
       *
       * Creates the children (empty) including their areas for this area.
       *
       * All of these children will stay existing for as long as any of them
       * or this parent has any remaining collision elements.
       */
      void createChildren();

      /** \brief Remove a collision element from this.
       *
       * Calling this method may delete this and in some cases delete elements
       * recursively upward until root.
       *
       * \param elem Collision element to remove.
       */
      void remove(CollisionElement *element);

    public:
      /** \cond */
      friend std::ostream& operator<<(std::ostream &lhs, const OctreeArea &rhs);
      /** \endcond */
  };

  /** \brief Collision tree model.
   */
  class Octree :
    public OctreeArea
  {
    public:
      /** \brief Constructor.
       *
       * \param rr Initial cube size from origo.
       */
      Octree(float rr);

      /** \brief Destructor. */
      ~Octree() { }
  };

  /** \brief Output collision element to a stream.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Stream after input.
   */
  std::ostream& operator<<(std::ostream &lhs, const OctreeArea &rhs);
}

#endif
