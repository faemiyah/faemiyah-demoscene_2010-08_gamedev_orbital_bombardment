#include "ob_appearing_string.hpp"

#include "math/generic.hpp"

using namespace ob;

AppearingString::AppearingString(const std::wstring &op)
{
  this->set(op);
}
  
std::wstring AppearingString::get() const
{
  size_t maxlen = math::max(m_curr.length(), m_prev.length());

  std::wstring ret;
  ret.assign(maxlen, 0);
  
  for(unsigned ii = 0; (ii < maxlen); ++ii)
  {
    if(m_vis[ii].isVisible())
    {
      ret[ii] = (m_curr.length() > ii) ?
        m_curr[ii] :
        static_cast<wchar_t>(' ');
    }
    else
    {
      ret[ii] = (m_prev.length() > ii) ?
        m_prev[ii] :
        static_cast<wchar_t>(' ');
    }
  }
  return ret;
}

void AppearingString::set(const std::wstring &op)
{
  m_prev = m_curr;
  m_curr = op;

  m_vis.resize(math::max(m_curr.length(), m_prev.length()));
  BOOST_FOREACH(Visualization &vv, m_vis)
  {
    vv.setVisibility(0);
  }
}

void AppearingString::update()
{
  BOOST_FOREACH(Visualization &vv, m_vis)
  {
    vv.updateVisibility(true);
  }
}

void AppearingString::updateReset()
{
  m_prev = std::wstring();

  m_vis.resize(m_curr.length());
  BOOST_FOREACH(Visualization &vv, m_vis)
  {
    vv.setVisibility(0);
  }
}

