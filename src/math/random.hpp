#ifndef MATH_RANDOM_HPP
#define MATH_RANDOM_HPP

#include "defaults.hpp"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

namespace math
{
  /** Convenience typedef. */
  typedef boost::mt19937 mrgen;

  /** The global random number generator. */
  extern mrgen global_mrgen;

  /** \brief Generate a random number.
   *
   * Uniform distribution, inclusive.
   *
   * \param rmin Minimum value.
   * \param rmax Maximum value.
   */
  inline int mrand(int rmin, int rmax)
  {
    BOOST_ASSERT(rmin <= rmax);

    boost::uniform_int<> distribution(rmin, rmax);
    boost::variate_generator<mrgen&, boost::uniform_int<> >
      randomize(global_mrgen, distribution);
    return randomize();
  }

  /** \brief Generate a random number.
   *
   * Uniform distribution, min inclusive, max exclusive.
   *
   * The exclusiveness of max is not numerically significant.
   *
   * \param rmin Minimum value.
   * \param rmax Maximum value.
   */
  inline float mrand(float rmin, float rmax)
  {
    BOOST_ASSERT(rmin <= rmax);

    boost::uniform_real<> distribution(static_cast<double>(rmin),
        static_cast<double>(rmax));
    boost::variate_generator<mrgen&, boost::uniform_real<> >
      randomize(global_mrgen, distribution);
    return static_cast<float>(randomize());
  }

  /** \brief Generate a random number.
   *
   * Uniform distribution, min inclusive, max exclusive.
   *
   * The exclusiveness of max is not numerically significant.
   *
   * \param rmin Minimum value.
   * \param rmax Maximum value.
   */
  inline double mrand(double rmin, double rmax)
  {
    BOOST_ASSERT(rmin <= rmax);

    boost::uniform_real<> distribution(rmin, rmax);
    boost::variate_generator<mrgen&, boost::uniform_real<> >
      randomize(global_mrgen, distribution);
    return randomize();
  }

  /** \brief Generate a random number.
   *
   * From 0 to given number. Uniform distribution, inclusive.
   *
   * Value can be positive or negative.
   *
   * \param op Maximum / minimum value.
   */
  template <typename T> inline T mrand0(T op)
  {
    if(op > static_cast<T>(0))
    {
      return mrand(static_cast<T>(0), op);
    }
    else if(op < static_cast<T>(0))
    {
      return mrand(op, static_cast<T>(0));
    }
    return static_cast<T>(0);
  }

  /** Generate a positive or negative random number.
   *
   * Given value is multiplied by 1 or -1.
   *
   * \param op Given value.
   */
  template <typename T> inline T mrandpn(T op)
  {
    return mrand0(1) ? op : -op;
  }
}

#endif
