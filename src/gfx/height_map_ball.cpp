#include "gfx/height_map_ball.hpp"

using namespace gfx;

HeightMapBall::HeightMapBall(const std::string &pbk, const std::string &pdn,
    const std::string &pfw, const std::string &plt,
    const std::string &prt, const std::string &pup, float pmin, float pmax) :
  m_bk(pbk),
  m_dn(pdn),
  m_fw(pfw),
  m_lt(plt),
  m_rt(prt),
  m_up(pup),
  m_min(pmin),
  m_max(pmax) { }

math::vec3f HeightMapBall::calcNormal(const math::vec3f &pnt, float gdist) const
{
  return this->calcNormal(pnt, gdist, this->calcHeight(pnt));
}

math::vec3f HeightMapBall::calcNormal(const math::vec3f &pnt, float gdist, float ht) const
{
  math::vec3d vv(this->normalizeHeight(pnt, ht)),
    up(0.0, 1.0, 0.0), // TODO: fix
    vd1(math::normalize(math::cross(up, vv))),
    vd2(math::normalize(math::cross(vv, vd1))),
    vd3(math::normalize(math::cross(vv, vd2))),
    vd4(math::normalize(math::cross(vv, vd3)));

  vd1 = this->normalizeHeight(vd1 * gdist + pnt) - pnt;
  vd2 = this->normalizeHeight(vd2 * gdist + pnt) - pnt;
  vd3 = this->normalizeHeight(vd3 * gdist + pnt) - pnt;
  vd4 = this->normalizeHeight(vd4 * gdist + pnt) - pnt;

  math::vec3d nn1(math::cross(vd1, vd2)),
    nn2(math::cross(vd2, vd3)),
    nn3(math::cross(vd3, vd4)),
    nn4(math::cross(vd4, vd1));

  return math::vec3f(math::normalize(nn1 + nn2 + nn3 + nn4));
}

float HeightMapBall::calcHeight(const math::vec3f &pnt) const
{
  return this->calcHeightNormalized(math::normalize(pnt));
}

float HeightMapBall::calcHeightNormalized(const math::vec3f &vv) const
{
  math::vec3f off(vv * 0.5f + 0.5f),
    wt,
    rad;

  if(vv.x() < 0.0f)
  {
    wt.x() = -vv.x();
    rad.x() = m_lt.getHeightValue(off.z(), off.y());
  }
  else
  {
    wt.x() = vv.x();
    rad.x() = m_rt.getHeightValue(off.z(), off.y());
  }
  if(vv.y() < 0.0f)
  {
    wt.y() = -vv.y();
    rad.y() = m_dn.getHeightValue(off.x(), off.z());
  }
  else
  {
    wt.y() = vv.y();
    rad.y() = m_up.getHeightValue(off.x(), off.z());
  }
  if(vv.z() < 0.0f)
  {
    wt.z() = -vv.z();
    rad.z() = m_fw.getHeightValue(off.x(), off.y());
  }
  else
  {
    wt.z() = vv.z();
    rad.z() = m_bk.getHeightValue(off.x(), off.y());
  }

  // This will do strange shit on origo, won't happen tho.
  float sum = wt.x() + wt.y() + wt.z(),
        ret = math::dot(wt, rad) / sum;

  ret = math::min(math::max(ret, 0.0f), 1.0f);
#if 0
  if((ret < 0.0f) || (ret > 1.0f))
  {
    std::stringstream sstr;
    sstr << "shit: " << vv << " results in: " << ret;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
#endif

  return ret;
}

math::vec3f HeightMapBall::normalizeHeight(const math::vec3f &vv) const
{
  return this->normalizeHeight(vv, this->calcHeight(vv));
}

math::vec3f HeightMapBall::normalizeHeight(const math::vec3f &vv, float ht) const
{
  return math::vec3f(this->normalizeHeight(math::vec3d(vv), ht));
}

math::vec3d HeightMapBall::normalizeHeight(const math::vec3d &vv) const
{
  return this->normalizeHeight(vv, this->calcHeight(math::vec3f(vv)));
}

math::vec3d HeightMapBall::normalizeHeight(const math::vec3d &vv, float ht) const
{
  return math::normalize(vv) * static_cast<double>(m_min + (m_max - m_min) * ht);
}

