#include "gfx/surface_base.hpp"

#include "gfx/generic.hpp"

#include <sstream>

using namespace gfx;

SurfaceBase::SurfaceBase(unsigned pw, unsigned ph, unsigned pb)
{
  this->setInternalState(pw, ph, pb);
}

void SurfaceBase::setInternalState(unsigned pw, unsigned ph, unsigned pb)
{
  if(pw <= 1)
  {
    BOOST_THROW_EXCEPTION(std::runtime_error("width <= 1"));
  }
  if(ph <= 1)
  {
    BOOST_THROW_EXCEPTION(std::runtime_error("height <= 1"));
  }
  if((pb != 8) && (pb != 16) && (pb != 24) && (pb != 32))
  {
    std::stringstream err;
    err << "invalid bpp: " << pb;
    BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
  }

  m_w = pw;
  m_h = ph;
  m_b = pb;
}

