#ifndef MATH_QUAT_HPP
#define MATH_QUAT_HPP

#include "math/generic.hpp"

namespace math
{
  /** \brief Generic quaternion type.
   *
   * Sir William Rowan Hamilton:
   * i^2 = j^2 = k^2 = ijk = -1
   */
  template<typename T> struct quat
  {
    /** STL lookalike. */
    typedef T value_type;

    /** STL lookalike. */
    typedef T* iterator;
  
    /** Internal information. */
    static const unsigned static_size = 4;

    /** \brief Actual data.
     *
     * Order: a, i, j, k
     */
    T m_array[4];

    /** \brief STL lookalike.
     *
     * \return Size of this.
     */
    static size_t size()
    {
      return 4;
    }

    /** \brief STL lookalike.
     *
     * \return Begin iterator.
     */
    iterator begin()
    {
      return m_array;
    }

    /** \brief STL lookalike.
     *
     * \return Const begin iterator.
     */
    iterator begin() const
    {
      return m_array;
    }

    /** \brief STL lookalike.
     *
     * \return End iterator.
     */
    iterator end()
    {
      return m_array + 4;
    }

    /** \brief STL lookalike.
     *
     * \return Const end iterator.
     */
    iterator end() const
    {
      return m_array + 4;
    }

    /** \brief Empty constructor.
     */
    quat() { }

    /** \brief Initializing constuctor.
     *
     * \param pa A component.
     * \param pi I component.
     * \param pj J component.
     * \param pk K component.
     */
    quat(const T &pa, const T &pi, const T &pj, const T &pk)
    {
      m_array[0] = pa;
      m_array[1] = pi;
      m_array[2] = pj;
      m_array[3] = pk;
    }

    /** \brief Fill constructor.
     *
     * \param v Source.
     */
    quat(const T &v)
    {
      std::fill(m_array, m_array + 4, v);
    }

    /** \brief Copy constructor.
     *
     * \param v Source.
     */
    quat(const T *v)
    {
      std::copy(v, v + 4, m_array);
    }

    /** \brief Copy constructor.
     *
     * \param other Source.
     */
    quat(const quat<T> &other)
    {
      std::copy(other.m_array, other.m_array + 4, m_array);
    }

    /** \brief Copy from quaternion of another type.
     *
     * \param other Source.
     */
    template <typename T2> quat(const quat<T2> &other)
    {
      for(unsigned ii = 0; (ii < 4); ++ii)
      {
        m_array[ii] = static_cast<T>(other.m_array[ii]);
      }
    }

    /** \brief Access operator.
     *
     * \param ii Index to fetch.
     * \return ii:th value.
     */
    value_type& operator[](unsigned ii) 
    {
      return m_array[ii];
    }

    /** \brief Access operator.
     *
     * \param ii Index to fetch.
     * \return ii:th value.
     */
    const value_type& operator[](unsigned ii) const
    {
      return m_array[ii];
    }

    /** \brief Access operator.
     *
     * Same as operator[], but throws an error if out of range.
     * 
     * \param ii Index to fetch.
     * \return ii:th value.
     */
    value_type & at(unsigned ii)
    {
      if(ii >= 4)
      {
        BOOST_THROW_EXCEPTION(std::range_error("index out of range"));
      }
      return m_array[ii];
    }

    /** \brief Access operator.
     *
     * Same as operator[], but throws an error if out of range.
     * 
     * \param ii Index to fetch.
     * \return ii:th value.
     */
    const value_type& at(unsigned ii) const
    {
      if(ii >= 4)
      {
        BOOST_THROW_EXCEPTION(std::range_error("index out of range"));
      }
      return m_array[ii];
    }

    /** \brief Multiplication assing.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    quat<T>& operator*=(const quat<T> &other)
    {
      T aa = m_array[0],
        ii = m_array[1],
        jj = m_array[2],
        kk = m_array[3];
      m_array[0] = aa * other.m_array[0] - ii * other.m_array[1] - jj * other.m_array[2] - kk * other.m_array[3];
      m_array[1] = aa * other.m_array[1] + ii * other.m_array[0] + jj * other.m_array[3] - kk * other.m_array[2];
      m_array[2] = aa * other.m_array[2] - ii * other.m_array[4] + jj * other.m_array[0] + kk * other.m_array[1];
      m_array[3] = aa * other.m_array[3] + ii * other.m_array[2] - jj * other.m_array[1] + kk * other.m_array[0];
    }
    
    /** \brief Scalar multiplication assing.
     *
     * \param scalar Other operand.
     * \return This after operation.
     */
    quat<T>& operator*=(const T& scalar)
    {
      m_array[0] *= scalar;
      m_array[1] *= scalar;
      m_array[2] *= scalar;
      m_array[3] *= scalar;
    }

    /** \brief Addition assing.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    quat<T>& operator+=(const quat<T> &other)
    {
      m_array[0] += other.m_array[0];
      m_array[1] += other.m_array[1];
      m_array[2] += other.m_array[2];
      m_array[3] += other.m_array[3];
      return *this;
    }

    /** \brief Substraction assing.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    quat<T>& operator-=(const quat<T> &other)
    {
      m_array[0] -= other.m_array[0];
      m_array[1] -= other.m_array[1];
      m_array[2] -= other.m_array[2];
      m_array[3] -= other.m_array[3];
      return *this;
    }

    /** \brief Multiplication.
     *
     * \param other Other operand.
     * \return Result quaternion.
     */
    quat<T> operator*(const quat<T> &other) const
    {
      T aa = m_array[0],
        ii = m_array[1],
        jj = m_array[2],
        kk = m_array[3];
      return quat<T>(
          aa * other.m_array[0] - ii * other.m_array[1] - jj * other.m_array[2] - kk * other.m_array[3],
          aa * other.m_array[1] + ii * other.m_array[0] + jj * other.m_array[3] - kk * other.m_array[2],
          aa * other.m_array[2] - ii * other.m_array[4] + jj * other.m_array[0] + kk * other.m_array[1],
          aa * other.m_array[3] + ii * other.m_array[2] - jj * other.m_array[1] + kk * other.m_array[0]);
    }

    /** \brief Scalar multiplication.
     *
     * \param scalar Other operand.
     * \return Result quaternion.
     */
    quat<T> operator*(const T& scalar) const
    {
      return quat<T>(m_array[0] * scalar,
          m_array[1] * scalar,
          m_array[2] * scalar,
          m_array[3] * scalar);
    }

    /** \brief Addition.
     *
     * \param other Other operand.
     * \return Result quaternion.
     */
    quat<T> operator+(const quat<T> &other) const
    {
      return quat<T>(m_array[0] + other.m_array[0],
          m_array[1] + other.m_array[1],
          m_array[2] + other.m_array[2],
          m_array[3] + other.m_array[3]);
    }

    /** \brief Substraction.
     *
     * \param other Other operand.
     * \return Result quaternion.
     */
    quat<T> operator-(const quat<T> &other) const
    {
      return quat<T>(m_array[0] - other.m_array[0],
          m_array[1] - other.m_array[1],
          m_array[2] - other.m_array[2],
          m_array[3] - other.m_array[3]);
    }
  };

  /** Convenience typedef. */
  typedef quat<float> quatf;

  /** Convenience typedef. */
  typedef quat<double> quatd;
}

#endif
