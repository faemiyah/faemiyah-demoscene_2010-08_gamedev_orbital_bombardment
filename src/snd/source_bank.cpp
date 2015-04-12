#include "snd/source_bank.hpp"

#include "snd/sample.hpp"

#include <sstream>

using namespace snd;

SourceBank::SourceBank(unsigned size) :
  m_bank(new Source[size]),
  m_count(size),
  m_next(0)
{
  if(m_count <= 0)
  {
    std::stringstream sstr;
    sstr << "cannot create a source bank of size " << m_count;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
}

Source* SourceBank::findSource(bool force)
{
  unsigned last = m_next;
  do {
    Source *src = &(m_bank[m_next]);
    if(++m_next >= m_count)
    {
      m_next = 0;
    }

    // Try to play in this source.
    if(force || src->isFree())
    {
      return src;
    }
  } while(last != m_next);

  // No free sources found.
  return NULL;
}

Source* SourceBank::play(Sample &sample, bool force)
{
  Source *ret = this->findSource(force);
  if(ret && sample.canPlay())
  {
    ret->play(sample);
  }
  return ret;
}

Source* SourceBank::play(Sample &sample, const math::vec3f &pos, bool force)
{
  Source *ret = this->findSource(force);
  if(ret && sample.canPlay())
  {
    ret->play(sample, pos);
  }
  return ret;
}

