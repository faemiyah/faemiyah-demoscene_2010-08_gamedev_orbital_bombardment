#include "ob_visualization.hpp"

#include "math/random.hpp"

using namespace ob;

Visualization::Visualization() :
  m_visibility(0),
  m_visible(false) { }

bool Visualization::updateVisibility(bool flag, int fadeout)
{
  if(flag)
  {
    if(++m_visibility > fadeout)
    {
      m_visibility = fadeout;
      m_visible = true;
      return true;
    }
  }
  else
  {
    if(--m_visibility < 0)
    {
      m_visibility = 0;
      m_visible = false;
      return false;
    }
  }

  return this->randomizeVisibility(fadeout);
}

bool Visualization::randomizeVisibility(int fadeout)
{
  m_visible = (math::mrand(0, fadeout) < m_visibility);
  return m_visible;
}

