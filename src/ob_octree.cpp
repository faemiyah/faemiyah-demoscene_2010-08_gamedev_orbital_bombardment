#include "ob_octree.hpp"

#include <sstream>

using namespace ob;

/** \brief Fill an array with child octree elements.
 *
 * \param tgt Target array.
 * \param box Box to derieve from.
 * \param parent Parent to use.
 */
static void fill_octree_array(boost::array<OctreeAreaSptr, 8> &tgt,
    const math::rect3f &box, OctreeArea *parent)
{
  math::vec3f center = box.center();

  tgt[0] = OctreeAreaSptr(new OctreeArea(math::rect3f(
          box.getMin(),
          center),
        parent));
  tgt[1] = OctreeAreaSptr(new OctreeArea(math::rect3f(
          math::vec3f(center.x(), box.y1(), box.z1()),
          math::vec3f(box.x2(), center.y(), center.z())),
        parent));
  tgt[2] = OctreeAreaSptr(new OctreeArea(math::rect3f(
          math::vec3f(box.x1(), center.y(), box.z1()),
          math::vec3f(center.x(), box.y2(), center.z())),
        parent));
  tgt[3] = OctreeAreaSptr(new OctreeArea(math::rect3f(
          math::vec3f(center.x(), center.y(), box.z1()),
          math::vec3f(box.x2(), box.y2(), center.z())),
        parent));
  tgt[4] = OctreeAreaSptr(new OctreeArea(math::rect3f(
          math::vec3f(box.x1(), box.y1(), center.z()),
          math::vec3f(center.x(), center.y(), box.z2())),
        parent));
  tgt[5] = OctreeAreaSptr(new OctreeArea(math::rect3f(
          math::vec3f(center.x(), box.y1(), center.z()),
          math::vec3f(box.x2(), center.y(), box.z2())),
        parent));
  tgt[6] = OctreeAreaSptr(new OctreeArea(math::rect3f(
          math::vec3f(box.x1(), center.y(), center.z()),
          math::vec3f(center.x(), box.y2(), box.z2())),
        parent));
  tgt[7] = OctreeAreaSptr(new OctreeArea(math::rect3f(
          center,
          box.getMax()),
        parent));
}

OctreeArea::OctreeArea(const math::rect3f &parea, OctreeArea *parent) :
  m_area(parea),
  m_parent(parent),
  m_combined(0)
{
  BOOST_ASSERT(m_parent != this);
}

OctreeArea::~OctreeArea()
{
  if((m_combined > 0) || (m_elements.size() > 0))
  {
    std::stringstream sstr;
    sstr << "elements at exit: combined (" << m_combined << "), elements (" <<
      m_elements.size() << ")";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
}

void OctreeArea::add(CollisionElement *element)
{
  m_elements.push_back(element);

  OctreeArea *area = this;
  do {
    ++(area->m_combined);
    area = area->m_parent;
  } while(area);
}

void OctreeArea::createChildren()
{
  // Either all children exist or none.
  if(m_recursive[0].get())
  {
    return;
  }
  fill_octree_array(m_recursive, m_area, this);
}

void OctreeArea::remove(CollisionElement *element)
{
  for(std::list<CollisionElement*>::iterator ii = m_elements.begin(),
      ee = m_elements.end(); (ii != ee); ++ii)
  {
    if(*ii == element)
    {
      m_elements.erase(ii);

      OctreeArea *area = this;
      do {
        if(--(area->m_combined) <= 0)
        {
          area->m_recursive[0] = OctreeAreaSptr();
          area->m_recursive[1] = OctreeAreaSptr();
          area->m_recursive[2] = OctreeAreaSptr();
          area->m_recursive[3] = OctreeAreaSptr();
          area->m_recursive[4] = OctreeAreaSptr();
          area->m_recursive[5] = OctreeAreaSptr();
          area->m_recursive[6] = OctreeAreaSptr();
          area->m_recursive[7] = OctreeAreaSptr();
        }
        area = area->m_parent;
      } while(area);
      // Only one instance of same element can exist.
      return;
    }
  }

  BOOST_THROW_EXCEPTION(std::runtime_error(
        "asked to remove an element, but none existed"));
}

Octree::Octree(float rr) :
  OctreeArea(math::rect3f(math::vec3f(-rr, -rr, -rr), math::vec3f(rr, rr, rr)), NULL) { }

std::ostream& ob::operator<<(std::ostream &lhs, const OctreeArea &rhs)
{
  return lhs << "OctreeArea:\n" << "  area: " << rhs.m_area <<
    "  \nContains: " << rhs.m_elements.size() << " immediate, " <<
    rhs.m_combined << " total";
}

