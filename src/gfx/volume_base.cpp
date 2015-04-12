#include "gfx/volume_base.hpp"

#include "defaults.hpp"

using namespace gfx;

VolumeBase::VolumeBase(unsigned px, unsigned py, unsigned pz, unsigned pb)
{
  this->setInternalState(px, py, pz, pb);
}

void VolumeBase::setInternalState(unsigned pw, unsigned ph, unsigned pd,
    unsigned pb)
{
  if(pd <= 1)
  {
    BOOST_THROW_EXCEPTION(std::runtime_error("depth <= 1"));
  }

  this->SurfaceBase::setInternalState(pw, ph, pb);
  m_d = pd;
}

