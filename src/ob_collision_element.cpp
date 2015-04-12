#include "ob_collision_element.hpp"

#include "ob_octree.hpp"

#include <map>
#include <sstream>

using namespace ob;

/** \brief Collision between triangle and line.
 *
 * \param t1 Triangle corner 1.
 * \param t2 Triangle corner 2.
 * \param t3 Triangle corner 3.
 * \param l1 Line point 1.
 * \param l2 Line point 2.
 * \param rr Radius squared.
 * \return True if collide, false if not.
 */
static bool collides_triangle_line(const math::vec3f &t1, const math::vec3f &t2, const math::vec3f &t3,
    const math::vec3f &l1, const math::vec3f &l2, float rr)
{
  boost::ignore_unused_variable_warning(t1);
  boost::ignore_unused_variable_warning(t2);
  boost::ignore_unused_variable_warning(t3);
  boost::ignore_unused_variable_warning(l1);
  boost::ignore_unused_variable_warning(l2);
  boost::ignore_unused_variable_warning(rr);

  // TODO: implement
  return false;
}

/** \brief Collision between triangle and point.
 *
 * \param t1 Triangle corner 1.
 * \param t2 Triangle corner 2.
 * \param t3 Triangle corner 3.
 * \param pp Point.
 * \param rr Radius squared.
 * \return True if collide, false if not.
 */
static bool collides_triangle_point(const math::vec3f &t1, const math::vec3f &t2, const math::vec3f &t3,
    const math::vec3f &pp, float rr)
{
  boost::ignore_unused_variable_warning(t1);
  boost::ignore_unused_variable_warning(t2);
  boost::ignore_unused_variable_warning(t3);
  boost::ignore_unused_variable_warning(pp);
  boost::ignore_unused_variable_warning(rr);

  // TODO: implement
  return false;
}

/** \brief Collision between line and line.
 *
 * Algorithm from softSurfer:
 * http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
 *
 * \param l1 First line point 1.
 * \param l2 First line point 2.
 * \param m1 Second line point 1.
 * \param m2 Second line point 2.
 * \param rr Radius squared.
 * \return True if collide, false if not.
 */
static bool collides_line_line(const math::vec3f &l1, const math::vec3f &l2, const math::vec3f &m1,
    const math::vec3f &m2, float rr)
{
  math::vec3f u = l2 - l1,
    v = m2 - m1,
    w = l1 - l2;
  float a = dot(u,u),
        b = dot(u,v),
        c = dot(v,v),
        d = dot(u,w),
        e = dot(v,w),
        D = a*c - b*b,
        sD = D,
        tD = D,
        sc, sN, tc, tN; 

  // paraller lines
  if(D <= FLT_EPSILON)
  {
    // use l1 to prevent division by 0
    sN = 0.0;
    sD = 1.0;
    tN = e;
    tD = c;
  }
  else
  {
    // get the closest points on the infinite lines
    sN = (b*e - c*d);
    tN = (a*e - b*d);
    if(sN < 0.0)
    {
      // sc < 0 => the s=0 edge is visible
      sN = 0.0;
      tN = e;
      tD = c;
    }
    else if(sN > sD)
    {
      // sc > 1 => the s=1 edge is visible
      sN = sD;
      tN = e + b;
      tD = c;
    }
  }

  if(tN < 0.0)
  {
    // tc < 0 => the t=0 edge is visible
    tN = 0.0;
    // recompute sc for this edge
    if(-d < 0.0)
    {
      sN = 0.0;
    }
    else if (-d > a)
    {
      sN = sD;
    }
    else
    {
      sN = -d;
      sD = a;
    }
  }
  else if (tN > tD)
  {
    // tc > 1 => the t=1 edge is visible
    tN = tD;
    // recompute sc for this edge
    if ((-d + b) < 0.0)
    {
      sN = 0;
    }
    else if((-d + b) > a)
    {
      sN = sD;
    }
    else
    {
      sN = (-d + b);
      sD = a;
    }
  }
  // finally do the division to get sc and tc
  sc = ((math::abs(sN) <= FLT_EPSILON) ? 0.0f : (sN / sD));
  tc = ((math::abs(tN) <= FLT_EPSILON) ? 0.0f : (tN / tD));

  // get the difference of the two closest points
  math::vec3f dP = w + (sc * u) - (tc * v);  // = S1(sc) - S2(tc)

  return (math::length2(dP) < rr);
}

/** \brief Collision between line and point.
 *
 * Algorithm from Wolfram MathWorld:
 * http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
 *
 * \param l1 First line point 1.
 * \param l2 First line point 2.
 * \param pp Point.
 * \param rr Radius squared.
 * \return True if collide, false if not.
 */
static inline bool collides_line_point(const math::vec3f &l1, const math::vec3f &l2, const math::vec3f &pp,
    float rr)
{
  return (math::dist2_line_point(l1, l2, pp) < rr);
}

CollisionElement::CollisionElement(float radius, CollisionElementType type,
    uint32_t faction) :
  m_area(NULL),
  m_radius(radius),
  m_type(type),
  m_faction(faction) { }

CollisionElement::~CollisionElement()
{
  this->removeFromAreas();
}

CollisionElement* CollisionElement::checkCollisions() const
{
  if(NULL == m_area)
  {
    return NULL;
  }

  CollisionElement *ret = this->checkCollisions(m_area);
  if(ret)
  {
    //std::cout << "found" << std::endl;
    return ret;
  }

  const OctreeArea *parent = m_area->getParent();
  while(NULL != parent)
  {
    ret = this->checkCollisions(parent);
    if(ret)
    {
      //std::cout << "found" << std::endl;
      return ret;
    }
    parent = parent->getParent();
  }

  return NULL;
}

CollisionElement* CollisionElement::checkCollisions(
    const OctreeArea *area) const
{
  BOOST_FOREACH(CollisionElement *vv, area->getElements())
  {
    if((vv == this) || ((vv->m_faction) & m_faction))
    {
      continue;
    }
    if(this->collides(*vv))
    {
      return vv;
    }
  }
  return NULL;
}

bool CollisionElement::collides(const CollisionElement &other) const
{
  // Trivial exclude via boxes.
  if(m_box.isOutside(other.m_box))
  {
    return false;
  }

  // Squared distance is common for all comparisons.
  float rr = m_radius + other.m_radius;
  rr *= rr;

  if(m_type == other.m_type)
  {
    switch(m_type)
    {
      case LINE:
        return
          collides_triangle_line(
              m_pnts_current[0], m_pnts_current[1], m_pnts_former[0],
              other.m_pnts_current[0], other.m_pnts_former[0], rr) ||
          collides_triangle_line(
              m_pnts_current[1], m_pnts_former[0], m_pnts_former[1],
              other.m_pnts_current[1], other.m_pnts_former[1], rr) ||
          collides_triangle_line(
              other.m_pnts_current[0], other.m_pnts_current[1], other.m_pnts_former[0],
              m_pnts_current[0], m_pnts_former[0], rr) ||
          collides_triangle_line(
              other.m_pnts_current[1], other.m_pnts_former[0], other.m_pnts_former[1],
              m_pnts_current[1], m_pnts_former[1], rr);

      case POINT:
        return collides_line_line(m_pnts_current[0], m_pnts_former[0],
            other.m_pnts_current[0], other.m_pnts_former[0], rr);

      case STATIONARY:
      default:
        {
          float d2 = math::dist2_point_point(m_pnts_current[0],
              other.m_pnts_current[0]);
          return (d2 < rr);
        }
    }
  }

  // Stationary versus moving ball object.
  if((STATIONARY == m_type) && (POINT == other.m_type))
  {
    return collides_line_point(other.m_pnts_current[0], other.m_pnts_former[0],
        m_pnts_current[0], rr);
  }
  if((POINT == m_type) && (STATIONARY == other.m_type))
  {
    return collides_line_point(m_pnts_current[0], m_pnts_former[0],
        other.m_pnts_current[0], rr);
  }

  // Stationary versus moving line object.
  if((STATIONARY == m_type) && (LINE == other.m_type))
  {
    return
      collides_triangle_point(
          other.m_pnts_current[0], other.m_pnts_current[1], other.m_pnts_former[0],
          m_pnts_current[0], rr) ||
      collides_triangle_point(
          other.m_pnts_current[1], other.m_pnts_former[0], other.m_pnts_former[1],
          m_pnts_current[0], rr);
  }
  if((LINE == m_type) && (STATIONARY == other.m_type))
  {
    return
      collides_triangle_point(
          m_pnts_current[0], m_pnts_current[1], m_pnts_former[0],
          other.m_pnts_current[0], rr) ||
      collides_triangle_point(
          m_pnts_current[1], m_pnts_former[0], m_pnts_former[1],
          other.m_pnts_current[0], rr);
  }

  // Moving ball object versus moving line object.
  if((POINT == m_type) && (LINE == other.m_type))
  {
    return
      collides_triangle_line(
          other.m_pnts_current[0], other.m_pnts_current[1], other.m_pnts_former[0],
          m_pnts_current[0], m_pnts_former[0], rr) ||
      collides_triangle_line(
          other.m_pnts_current[1], other.m_pnts_former[0], other.m_pnts_former[1],
          m_pnts_current[0], m_pnts_former[0], rr);
  }
  if((LINE == m_type) && (POINT == other.m_type))
  {
    return
      collides_triangle_line(
          m_pnts_current[0], m_pnts_current[1], m_pnts_former[0],
          other.m_pnts_current[0], other.m_pnts_former[0], rr) ||
      collides_triangle_line(
          m_pnts_current[1], m_pnts_former[0], m_pnts_former[1],
          other.m_pnts_current[0], other.m_pnts_former[0], rr);
  }

  std::stringstream sstr;
  sstr << "unknown collision element types found: " << static_cast<int>(m_type) << " and " <<
    static_cast<int>(other.m_type);
  BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));

  return false;
}

void CollisionElement::getAllCollisions(std::list<CollisionElement*> &dst, const OctreeArea *area) const
{
  BOOST_FOREACH(CollisionElement *vv, area->getElements())
  {
    if((vv == this) || ((vv->m_faction) & m_faction))
    {
      continue;
    }
    if(this->collides(*vv))
    {
      dst.push_back(vv);
    }
  }
}

void CollisionElement::getAllCollisionsRecursive(std::list<CollisionElement*> &dst,
    const OctreeArea *area) const
{
  if(!area)
  {
    return;
  }

  this->getAllCollisions(dst, area);

  BOOST_FOREACH(const OctreeAreaSptr &vv, area->getRecursive())
  {
    this->getAllCollisionsRecursive(dst, vv.get());
  }
}

void CollisionElement::getAllCollisions(std::list<CollisionElement*> &dst) const
{
  if(!m_area)
  {
    return;
  }

  this->getAllCollisionsRecursive(dst, m_area);

  const OctreeArea *parent = m_area->getParent();
  while(parent)
  {
    this->getAllCollisions(dst, parent);
    parent = parent->getParent();
  }
}

void CollisionElement::initCollisionData(const math::vec3f &pa)
{
  m_pnts_current[0] = pa;

  m_box = math::rect3f(m_pnts_current[0]);

  this->expandBoxWithRadius();
}

void CollisionElement::initCollisionData(const math::vec3f &pa, const math::vec3f &pb)
{
  m_pnts_current[0] = pa;
  m_pnts_current[1] = pb;

  m_box = math::rect3f(m_pnts_current[0]);
  m_box.expand(m_pnts_current[1]);

  this->expandBoxWithRadius();
}

void CollisionElement::insert(OctreeArea *tree)
{
  BOOST_ASSERT(tree != m_area);

  tree->createChildren(); // Will NOP if children already there.

  BOOST_FOREACH(OctreeAreaSptr &vv, tree->getRecursive())
  {
    if(m_box.isInside(vv->getArea()))
    {
      this->insert(vv.get());
      return;
    }
  }

  // Could not fit inside children, add to top area instead.
  m_area = tree;
  tree->add(this);
}

void CollisionElement::removeFromAreas()
{
  if(NULL != m_area)
  {
    m_area->remove(this);
    m_area = NULL;
  }
}

void CollisionElement::updateAreas(Octree &tree)
{
  if(NULL == m_area)
  {
    this->insert(&tree);
    return;
  }

  // Check if could be inserted to current children.
  BOOST_FOREACH(OctreeAreaSptr &vv, m_area->getRecursive())
  {
    if(m_box.isInside(vv->getArea()))
    {
      // Insert down.
      OctreeArea *old = m_area;
      this->insert(vv.get());
      old->remove(this);
      return;
    }
  }

  // Trivial case in topmost level. We already tried to insert to current
  // children and it didn't work, so we'll stay here.
  OctreeArea *iter = m_area->getParent();
  if(NULL == iter)
  {
    return;
  }

  // Try to insert into a neighboring areas. This should never work, but
  // sometimes we would go back to where we were, and can skip the rest
  // of the steps.
  BOOST_FOREACH(OctreeAreaSptr &vv, iter->getRecursive())
  {
    if(m_box.isInside(vv->getArea()))
    {
      // Back where we began?
      if(vv.get() == m_area)
      {
        return;
      }
      OctreeArea *old = m_area;
      this->insert(vv.get());
      old->remove(this);
      return;
    }
  }

  // Start ascending.
  for(iter = iter->getParent(); (NULL != iter); iter = iter->getParent())
  {
    BOOST_FOREACH(OctreeAreaSptr &vv, iter->getRecursive())
    {
      if(m_box.isInside(vv->getArea()))
      {
        OctreeArea *old = m_area;
        this->insert(vv.get());
        old->remove(this);
        return;
      }
    }
  }

  // Nothing left, we're fucked on the top level.
  OctreeArea *old = m_area;
  m_area = &tree;
  tree.add(this);
  old->remove(this);
}

void CollisionElement::updateCollisionData(const math::vec3f &pa)
{
  m_pnts_former[0] = m_pnts_current[0];
  m_pnts_current[0] = pa;

  m_box = math::rect3f(m_pnts_current[0]);
  m_box.expand(m_pnts_former[0]);

  this->expandBoxWithRadius();

#if 0
  math::vec3f size = m_box.size();
  if((size.x() > 100.0f) || (size.y() > 100.0f) || (size.z() > 100.0f))
  {
    std::cout << "weird shits: " << (*this);
    std::cout << "\n";
  }
#endif
}

void CollisionElement::updateCollisionData(const math::vec3f &pa,
    const math::vec3f &pb)
{
  m_pnts_former[0] = m_pnts_current[0];
  m_pnts_former[1] = m_pnts_current[1];
  m_pnts_current[0] = pa;
  m_pnts_current[1] = pb;

  m_box = math::rect3f(m_pnts_current[0]);
  m_box.expand(m_pnts_current[1]);
  m_box.expand(m_pnts_former[0]);
  m_box.expand(m_pnts_former[1]);

  this->expandBoxWithRadius();
}

std::ostream& ob::operator<<(std::ostream &lhs, const CollisionElement &rhs)
{
  lhs << "CollisionElement:\n  area: " << rhs.m_box << "\n  curr: " << rhs.m_pnts_current[0] << " ; " <<
    rhs.m_pnts_current[1] << "\n  prev: " << rhs.m_pnts_former[0] << " ; " << rhs.m_pnts_former[1] <<
    "\n  radius: " << rhs.m_radius << "\nTrace:\n";
  for(OctreeArea *iter = rhs.m_area; (NULL != iter); iter = iter->getParent())
  {
    lhs << *iter << "\n";
  }
  return lhs;
}

