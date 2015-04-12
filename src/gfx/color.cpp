#include "gfx/color.hpp"

#include <sstream>

using namespace gfx;

Color::Color(const char *notation)
{
  int pr, pg, pb,
      pa = 255;
  if(4 != sscanf(notation, "%02x%02x%02x%02x", &pr, &pg, &pb, &pa))
  {
    if(3 != sscanf(notation, "%02x%02x%02x", &pr, &pg, &pb))
    {
      std::stringstream sstr;
      sstr << "sring not in HTML notation: " << notation;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }
  this->set(pr, pg, pb, pa);
}

Color Color::fromCMYK(float pc, float pm, float py, float pk, float pa)
{
  float tr = 1.0f - pc,
        tg = 1.0f - pm,
        tb = 1.0f - py,
        maxc = std::max(tr, std::max(tg, tb));
  if(maxc <= 0.0f)
  {
    tr = tg = tb = 1.0f - pk;
  }
  else
  {
    tr = tr / maxc - pk;
    tg = tg / maxc - pk;
    tb = tb / maxc - pk;
  }

  return Color(tr, tg, tb, pa);
}

