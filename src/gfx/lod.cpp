#include "gfx/lod.hpp"

#include "data/generic.hpp"
#include "gfx/surface.hpp"

using namespace gfx;

unsigned Lod::g_poly_count = 0;

/** Outside flag. */
static const uint8_t OUTSIDE_XMAX = 0x01;

/** Outside flag. */
static const uint8_t OUTSIDE_XMIN = 0x02;

/** Outside flag. */
static const uint8_t OUTSIDE_YMAX = 0x04;

/** Outside flag. */
static const uint8_t OUTSIDE_YMIN = 0x08;

/** Outside flag. */
static const uint8_t OUTSIDE_ZMAX = 0x10;

/** Outside flag. */
static const uint8_t OUTSIDE_ZMIN = 0x20;

/** \brief Get outside mask of given boundary item.
 *
 * \param pmat Matrix to use (full stack).
 * \param pvec Vector to check.
 */
static inline uint8_t check_outside_mask(const math::mat4f &pmat, const math::vec3f &pvec)
{
  math::vec4f pv(pmat * math::vec4f(pvec.x(), pvec.y(), pvec.z(), 1.0f));
  float hx = pv.x() / pv.w(),
        hy = pv.y() / pv.w(),
        hz = pv.z() / pv.w();
  return static_cast<uint8_t>(
      ((hx > 1.0f) ? OUTSIDE_XMAX : ((hx < -1.0f) ? OUTSIDE_XMIN : 0)) |
      ((hy > 1.0f) ? OUTSIDE_YMAX : ((hy < -1.0f) ? OUTSIDE_YMIN : 0)) |
      ((hz > 1.0f) ? OUTSIDE_ZMAX : ((hz < 0.0f) ? OUTSIDE_ZMIN : 0)));
}

math::rect3f Lod::calcBoundary(const std::vector<math::vec3f> &pvvec)
{
  std::map<unsigned, bool> vert_references;
   this->collect(vert_references);

  if(vert_references.empty())
  {
    BOOST_THROW_EXCEPTION(std::runtime_error("LOD entry contains no vertices"));
  }

  std::map<unsigned, bool>::iterator ii = vert_references.begin();
  math::rect3f ret(pvvec[(*ii).first]);
  ++ii;
  for(std::map<unsigned, bool>::iterator ee = vert_references.end();
      (ii != ee); ++ii)
  {
    ret.expand(pvvec[(*ii).first]);
  }

  m_pos = ret.center();
  m_dist = math::length(ret.getMax() - m_pos);
  m_dist2 = m_dist * m_dist;

  return ret;
}

bool Lod::checkDescend(const math::mat4f &pmat, const math::vec3f &tpos) const
{
  boost::ignore_unused_variable_warning(pmat);
  boost::ignore_unused_variable_warning(tpos);
  return true;
}

void Lod::coalesce(unsigned op)
{
  m_faces.clear();
  if(!this->collect(m_faces, op))
  {
    //std::cout << "Clearing children.\n";
    m_recursive.clear();
    return;
  }
  BOOST_FOREACH(const LodSptr &vv, m_recursive)
  {
    (*vv).coalesce(op);
  }
}

void Lod::collect(std::map<unsigned, bool> &refmap) const
{
  BOOST_FOREACH(const Triangle &vv, m_faces)
  {
    refmap[vv.a()] = true;
    refmap[vv.b()] = true;
    refmap[vv.c()] = true;
  }
  BOOST_FOREACH(const LodSptr &vv, m_recursive)
  {
    vv->collect(refmap);
  }
}

TriVec& Lod::collect(TriVec &pfvec) const
{
  if(m_recursive.empty())
  {
    BOOST_FOREACH(const Triangle &vv, m_faces)
    {
      pfvec.push_back(vv);
    }
    return pfvec;
  }
  BOOST_FOREACH(const LodSptr &vv, m_recursive)
  {
    (*vv).collect(pfvec);
  }
  return pfvec;
}

bool Lod::collect(TriVec &pfvec, unsigned op) const
{
  if(op <= 0)
  {
    BOOST_FOREACH(const Triangle &vv, m_faces)
    {
      pfvec.push_back(vv);
    }
    return !m_recursive.empty();
  }
  bool ret = false;
  BOOST_FOREACH(const LodSptr &vv, m_recursive)
  {
    ret |= (*vv).collect(pfvec, op - 1);
  }
  return ret;
}

void Lod::collectAll(std::vector<Lod*> &plvec)
{
  plvec.push_back(this);

  BOOST_FOREACH(const LodSptr &vv, m_recursive)
  {
    vv->collectAll(plvec);
  }
}

void Lod::collectLowest(std::vector<Lod*> &plvec)
{
  if(m_recursive.empty())
  {
    plvec.push_back(this);
    return;
  }

  BOOST_FOREACH(const LodSptr &vv, m_recursive)
  {
    vv->collectLowest(plvec);
  }
}

void Lod::compile(const std::vector<math::vec3f> &pvvec)
{
  this->setBoundary(this->calcBoundary(pvvec));

  BOOST_FOREACH(LodSptr &vv, m_recursive)
  {
    vv->compile(pvvec);
  }

  this->trim();
}

bool Lod::cull(math::vec3f &tpos, const math::mat4f &pmat) const
{
  math::vec4f tp(pmat * math::vec4f(m_pos.x(), m_pos.y(), m_pos.z(), 1.0f));
  
  tpos = math::vec3f(tp.x(), tp.y(), tp.z());
  
  if(math::length2(tpos) <= m_dist2)
  {
    return true;
  }
  else if(tpos.z() - m_dist > 0.0f)
  {
    return false;
  }

  return this->cullBoundary();
}

bool Lod::cullBoundary() const
{
  uint8_t combined = 0xFF;
  BOOST_FOREACH(const math::vec3f &vv, m_boundary)
  {
    uint8_t mask = check_outside_mask(Surface::get_matrix_stack(), vv);
    combined &= mask;
    if(0 == combined)
    {
      return true;
    }
  }
  return false;
}

bool Lod::draw(const math::mat4f &pmat) const
{
  math::vec3f tpos;
  if(!this->cull(tpos, pmat))
  {
    return false;
  }

  if(this->checkDescend(pmat, tpos))
  {
    bool rec_draw = false;
    BOOST_FOREACH(const LodSptr &vv, m_recursive)
    {
      //std::cout << "Recursing!\n";
      rec_draw |= (*vv).draw(pmat);
    }
    if(!rec_draw)
    {
      m_elem.draw(GL_TRIANGLES);
      poly_count_increment(m_elem.getCount() / 3);
    }
  }
  else
  {
    m_elem.draw(GL_TRIANGLES);
    poly_count_increment(m_elem.getCount() / 3);
  }
  return true;
}

void Lod::setBoundary(const math::rect3f brect)
{
  m_boundary.resize(8);
  m_boundary[0] = brect.getMin();
  m_boundary[1] = math::vec3f(brect.x2(), brect.y1(), brect.z1());
  m_boundary[2] = math::vec3f(brect.x1(), brect.y2(), brect.z1());
  m_boundary[3] = math::vec3f(brect.x1(), brect.y1(), brect.z2());
  m_boundary[4] = math::vec3f(brect.x2(), brect.y2(), brect.z1());
  m_boundary[5] = math::vec3f(brect.x2(), brect.y1(), brect.z2());
  m_boundary[6] = math::vec3f(brect.x1(), brect.y2(), brect.z2());
  m_boundary[7] = brect.getMax();
}

void Lod::trim()
{
  data::stl_trim(m_boundary);
  data::stl_trim(m_recursive);
  data::stl_trim(m_faces);
}

void Lod::unreserve()
{
  m_elem.assign(0, 0);
  m_faces.clear();
  m_recursive.clear();
}

