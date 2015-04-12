#include "ob_lifetime.hpp"

#include "math/random.hpp"

#include <limits.h>

using namespace ob;

Lifetime::Lifetime() :
  m_age(0),
  m_lifetime(INT_MAX),
  m_dead(false) { }

Lifetime::Lifetime(int lt) :
  m_age(0),
  m_lifetime(lt),
  m_dead(false) { }

bool Lifetime::decrementAge()
{
  return ((--m_age) >= 0);
}

bool Lifetime::die(int dtime)
{
  if(this->isDead())
  {
    return false;
  }

  m_dead = true;
  m_age = dtime;
  return true;
}

void Lifetime::resetLife(int timetodie)
{
  m_lifetime = timetodie;
}

bool Lifetime::incrementAge(int prob)
{
  ++m_age;
  if(m_age >= m_lifetime)
  {
    if(math::mrand(1, prob) == 1)
    {
      return false;
    }
  }
  return true;
}

