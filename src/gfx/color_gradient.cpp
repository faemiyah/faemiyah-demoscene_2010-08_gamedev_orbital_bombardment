#include "gfx/color_gradient.hpp"

using namespace gfx;

ColorGradient::ColorGradient(float pv, const gfx::Color &pc)
{
  this->add(pv, pc);
}

void ColorGradient::add(float pv, const gfx::Color &pc)
{
  m_colors.push_back(ColorGradientPair(pv, pc));
}

Color ColorGradient::getColor(float pv) const
{
  if(m_colors.size() < 2)
  {
    return m_colors.front().second;
  }

  std::list<ColorGradientPair>::const_iterator ii = m_colors.begin(),
    jj = ii;
  ++jj;
  do {
    if((ii->first <= pv) && (pv <= jj->first))
    {
      const Color &ca = ii->second;
      const Color &cb = jj->second;
      float diff = (pv - ii->first) / (jj->first - ii->first);
      Color ret;
      ret.r() = ca.r() + (cb.r() - ca.r()) * diff;
      ret.g() = ca.g() + (cb.g() - ca.g()) * diff;
      ret.b() = ca.b() + (cb.b() - ca.b()) * diff;
      ret.a() = ca.a() + (cb.a() - ca.a()) * diff;
      return  ret;
    }
    ++ii;
    ++jj;
  } while(m_colors.end() != jj);

  return m_colors.back().second;
}

