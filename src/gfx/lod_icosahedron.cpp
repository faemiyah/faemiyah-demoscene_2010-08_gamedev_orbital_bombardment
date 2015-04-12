#include "gfx/lod_icosahedron.hpp"

#include "gfx/surface.hpp"

using namespace gfx;

float LodIcosahedron::descend_threshold = 0.02f;
float LodIcosahedron::safety_boundary = 0.15f;
float LodIcosahedron::safety_view = 0.25f;

LodIcosahedron::LodIcosahedron(unsigned pa, unsigned pb, unsigned pc) :
  m_boundary_index_a(pa),
  m_boundary_index_b(pb),
  m_boundary_index_c(pc)
{
  this->addFace(Triangle(pa, pb, pc));
}

bool LodIcosahedron::checkDescend(const math::mat4f &pmat,
    const math::vec3f &tpos) const
{
  boost::ignore_unused_variable_warning(pmat);

  // It does not matter if the calculation is not mathematically accurate, the
  // control constant is all that matters, it can be changed at will.
  math::vec4f proj(Surface::get_projection() *
      math::vec4f(m_len_ab, m_len_ac, math::length(tpos), 1.0f));
  return ((proj.x() * proj.y() / (proj.w() * proj.w())) > descend_threshold);
}

void LodIcosahedron::compile(const std::vector<math::vec3f> &pvvec)
{
  math::rect3f brect = this->calcBoundary(pvvec);

  math::vec3f pa(pvvec[m_boundary_index_a]),
    pb(pvvec[m_boundary_index_b]),
    pc(pvvec[m_boundary_index_c]);

  math::vec3f
    ptoa(pa - m_pos),
    ptob(pb - m_pos),
    ptoc(pc - m_pos);

  m_len_ab = math::length(pb - pa);
  m_len_ac = math::length(pc - pa);

#if 0
  math::vec3f
    maxp1(pa + ptoa * safety_boundary + pa * safety_boundary),
    maxp2(pb + ptob * safety_boundary + pb * safety_boundary),
    maxp3(pc + ptoc * safety_boundary + pc * safety_boundary),
    minp1(pa + ptoa * safety_boundary - pa * safety_boundary),
    minp2(pb + ptob * safety_boundary - pb * safety_boundary),
    minp3(pc + ptoc * safety_boundary - pc * safety_boundary);

  m_boundary.push_back(maxp1);
  m_boundary.push_back(maxp2);
  m_boundary.push_back(maxp3);
  m_boundary.push_back(minp1);
  m_boundary.push_back(minp2);
  m_boundary.push_back(minp3);
#else
  this->setBoundary(brect);
#endif
  
  BOOST_FOREACH(LodSptr &vv, m_recursive)
  {
    (*vv).compile(pvvec);
  }

  this->trim();
}

bool LodIcosahedron::cull(math::vec3f &tpos, const math::mat4f &pmat) const
{
  math::vec4f tp(pmat * math::vec4f(m_pos.x(), m_pos.y(), m_pos.z(), 1.0f));
  
  tpos = math::vec3f(tp.x(), tp.y(), tp.z());
  
  if(math::length2(tpos) <= m_dist2)
  {
    return true;
  }
  if(tpos.z() - m_dist > 0.0f)
  {
    return false;
  }

  math::vec4f cv(pmat * math::vec4f(0.0f, 0.0f, 0.0f, 1.0f));
  math::vec3f  pv(cv.x(), cv.y(), cv.z()),
    nn(tpos - pv);
  if(math::dot(math::normalize(nn), math::normalize(pv)) > safety_view)
  {
    return false;
  }

  return this->cullBoundary();
}  

