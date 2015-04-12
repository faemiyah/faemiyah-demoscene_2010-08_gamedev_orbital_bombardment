#ifndef MATH_VEC_HPP
#define MATH_VEC_HPP

#include "math/generic.hpp"

namespace math
{
  /** \brief Generic vector type. */
  template<size_t S, typename T> struct vec
  {
    /** STL lookalike. */
    typedef T value_type;

    /** STL lookalike. */
    typedef T* iterator;

    /** Internal information. */
    static const unsigned static_size = S;

    /** Actual data. */
    T m_array[S];

    /** \brief STL lookalike.
     *
     * \return Size of this.
     */
    static size_t size()
    {
      return S;
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
      return m_array + S;
    }

    /** \brief STL lookalike.
     *
     * \return Const end iterator.
     */
    iterator end() const
    {
      return m_array + S;
    }

    /** \brief Empty constructor. */
    vec() { }

    /** \brief Fill constructor.
     *
     * \param v Source.
     */
    vec(const T &v)
    {
      std::fill(m_array, m_array + S, v);
    }

    /** \brief Copy constructor.
     *
     * \param v Source.
     */
    vec(const T *v)
    {
      std::copy(v, v + S, m_array);
    }

    /** \brief Copy constructor.
     *
     * \param other Source.
     */
    vec(const vec<S,T> &other)
    {
      std::copy(other.m_array, other.m_array + S, m_array);
    }

    /** \brief Copy from vector of same type but different length.
     *
     * \param other Source.
     */
    template<size_t S2> vec(const vec<S2, T> &other)
    {
      if(S <= S2)
      {
        for(unsigned i = 0; (i != S); ++i)
        {
          std::copy(other.m_array, other.m_array + S, m_array);
        }
      }
      else
      {
        std::copy(other.m_array, other.m_array + S2, m_array);
        std::fill(m_array + S2, m_array + S, static_cast<T>(0));
      }
    }

    /** \brief Copy from vector of same length but different type.
     *
     * \param other Source.
     */
    template<typename T2> vec(const vec<S, T2> &other)
    {
      for(unsigned ii = 0; (ii < S); ++ii)
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
      if(ii >= S)
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
      if(ii >= S)
      {
        BOOST_THROW_EXCEPTION(std::range_error("index out of range"));
      }
      return m_array[ii];
    }

    /** \brief Component-wise multiplication assign
     *
     * \param other Other operand.
     * \return This after operation.
     */
    vec<S,T>& operator*=(const vec<S,T> &other)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[ii] *= other.m_array[ii];
      }
      return *this;
    }

    /** \brief Scalar multiplication.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    vec<S,T>& operator*=(const T &scalar)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[ii] *= scalar;
      }
      return *this;
    }

    /** \brief Component-wise division assign.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    vec<S,T>& operator/=(const vec<S,T> &other)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[ii] /= other.m_array[ii];
      }
      return *this;
    }

    /** \brief Scalar division assign.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    vec<S,T>& operator/=(const T &scalar)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[ii] /= scalar;
      }
      return *this;
    }

    /** \brief Component-wise addition assign.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    vec<S,T>& operator+=(const vec<S,T> &other)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[ii] += other.m_array[ii];
      }
      return *this;
    }

    /** \brief Scalar addition assign.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    vec<S,T>& operator+=(const T &scalar)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[ii] += scalar;
      }
      return *this;
    }

    /** \brief Component-wise substraction assign.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    vec<S,T>& operator-=(const vec<S,T> &other)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[ii] -= other.m_array[ii];
      }
      return *this;
    }

    /** \brief Scalar substraction assign.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    vec<S,T>& operator-=(const T &scalar)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[ii] -= scalar;
      }
      return *this;
    }

    /** \brief Negate.
     *
     * \return Result.
     */
    vec<S,T> operator-() const
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        ret.m_array[ii] = -m_array[ii];
      }
      return ret;
    }

    /** \brief Component-wise addition.
     *
     * \param other Other operand.
     * \return Result.
     */
    vec<S,T> operator+(const vec<S,T> &other) const
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        ret.m_array[ii] = m_array[ii] + other.m_array[ii];
      }
      return ret;    
    }

    /** \brief Scalar addition.
     *
     * \param scalar Other operand.
     * \return Result.
     */
    vec<S,T> operator+(const T &scalar) const
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        ret.m_array[ii] = m_array[ii] + scalar;
      }
      return ret;
    }

    /** \brief Component-wise substraction.
     *
     * \param other Other operand.
     * \return Result.
     */
    vec<S,T> operator-(const vec<S,T> &other) const
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        ret.m_array[ii] = m_array[ii] - other.m_array[ii];
      }
      return ret;
    }

    /** \brief Scalar substraction.
     *
     * \param other Other operand.
     * \return Result.
     */
    vec<S,T> operator-(const T &scalar) const
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        ret.m_array[ii] = m_array[ii] - scalar;
      }
      return ret;
    }

    /** \brief Component-wise multiplication.
     *
     * \param other Other operand.
     * \return Result.
     */
    vec<S,T> operator*(const vec<S,T> &other) const
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        ret.m_array[ii] = m_array[ii] * other.m_array[ii];
      }
      return ret;
    }

    /** \brief Scalar multiplication.
     *
     * \param scalar Other operand.
     * \return Result.
     */
    vec<S,T> operator*(const T &scalar)
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        ret.m_array[ii] = m_array[ii] * scalar;
      }
      return ret;
    }

    /** \brief Component-wise division.
     *
     * If other has zeroes will generate an error.
     *
     * \param other Other operand.
     * \return Result.
     */
    vec<S,T> operator/(const vec<S,T> &other) const
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        ret.m_array[ii] = m_array[ii] / other.m_array[ii];
      }
      return ret;
    }

    /** \brief Scalar division.
     *
     * \param scalar Other operand.
     * \return Result.
     */
    vec<S,T> operator/(const T &scalar) const
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        ret.m_array[ii] = m_array[ii] / scalar;
      }
      return ret;
    }

    /** \brief Equality comparison.
     *
     * \param other Other operand.
     * \return True if equal, false if not.
     */
    bool operator==(const vec<S,T> &other) const
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        if(m_array[ii] != other.m_array[ii])
        {
          return false;
        }
      }
      return true;
    }

    /** \brief Inequality comparison.
     *
     * \param other Other operand.
     * \return True if equal, false if not.
     */
    bool operator!=(const vec<S,T> & other) const
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        if(m_array[ii] == other.m_array[ii])
        {
          return false;
        }
      }
      return true;
    }

    /** \brief Output a vector into a stream.
     *
     * \param lhs Stream to output to.
     * \param rhs Vector to print.
     */
    friend std::ostream& operator<<(std::ostream &lhs, const vec<S, T> &rhs)
    {
      lhs << "(";
      for(unsigned ii = 0; (ii < S - 1); ++ii)
      {
        lhs << rhs.m_array[ii] << " ; ";
      }
      return lhs << rhs.m_array[S - 1] << ")";
    }
  };

  /** \brief Scalar added to a vector.
   *
   * \param scalar Scalar.
   * \param rhs Vector.
   * \return Result.
   */
  template<unsigned int S, typename T> inline vec<S,T> operator+(const T &scalar, const vec<S,T> &rhs)
  {
    return rhs + scalar;
  }

  /** \brief Scalar from which a vector is substracted from.
   *
   * \param scalar Scalar.
   * \param rhs Vector.
   * \return Result.
   */
  template<unsigned int S, typename T> inline vec<S,T> operator-(const T &scalar, const vec<S,T> &rhs)
  {
    vec<S,T> ret;
    for(unsigned i = 0; (i != S); ++i)
    {
      ret.m_array[i] = scalar - rhs[i];
    }
    return ret;
  }

  /** \brief Scalar multiplied by vector.
   *
   * \param scalar Scalar.
   * \param rhs Vector.
   * \return Result.
   */
  template<size_t S, typename T> inline vec<S,T> operator*(const T &scalar, const vec<S,T> &rhs)
  {
    return rhs * scalar;
  }

  /** \brief Two-component vector with symbolic accessors. */
  template<typename T> struct vec2 : vec<2,T>
  {
    private:
      /** Parent type for internal use. */
      typedef vec<2,T> parent;

    public:
      /** \brief Empty constructor.
      */
      vec2() : parent() { }

      /** \brief Initializing constructor.
       *
       * \param px First component.
       * \param py Second component.
       */
      vec2(const T &px, const T &py)
      {
        parent::m_array[0] = px;
        parent::m_array[1] = py;
      }

      /** \brief Copy constructor.
       *
       * \param other Copied object.
       */
      vec2(const parent &other) : parent(other) { }

    public:
      /** \brief Set contents.
       *
       * \param px First component.
       * \param py Second component.
       */
      void set(const T &px, const T &py)
      {
        parent::m_array[0] = px;
        parent::m_array[1] = py;
      }

    public:
      /** \brief Accessor.
       *
       * \return Vector component.
       */
      T& x()
      {
        return parent::m_array[0];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      const T& x() const
      {
        return parent::m_array[0];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      T& y()
      {
        return parent::m_array[1];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      const T& y() const
      {
        return parent::m_array[1];
      }
  };

  /** \brief Three-component vector with symbolic accessors. */
  template<typename T> struct vec3 : vec<3,T>
  {
    private:
      /** Parent type for internal use. */
      typedef vec<3,T> parent;

    public:
      /** \brief Empty constructor.
      */
      vec3() : parent() { }

      /** \brief Initializing constructor.
       *
       * \param px First component.
       * \param py Second component.
       * \param pz Third component.
       */
      vec3(const T &px, const T &py, const T &pz)
      {
        parent::m_array[0] = px;
        parent::m_array[1] = py;
        parent::m_array[2] = pz;
      }

      /** \brief Copy constructor.
       *
       * \param other Copied object.
       */
      vec3(const parent &other) : parent(other) { }

    public:
      /** \brief Set contents.
       *
       * \param px First component.
       * \param py Second component.
       * \param pz Third component.
       */
      void set(const T &px, const T &py, const T &pz)
      {
        parent::m_array[0] = px;
        parent::m_array[1] = py;
        parent::m_array[2] = pz;
      }

    public:
      /** \brief Accessor.
       *
       * \return Vector component.
       */
      T& x()
      {
        return parent::m_array[0];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      const T& x() const
      {
        return parent::m_array[0];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      T& y()
      {
        return parent::m_array[1];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      const T& y() const
      {
        return parent::m_array[1];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      T& z()
      {
        return parent::m_array[2];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      const T& z() const
      {
        return parent::m_array[2];
      }
  };

  /** Four-component vector with symbolic accessors. */
  template<typename T> struct vec4 : vec<4,T>
  {
    private:
      /** Parent type for internal use. */
      typedef vec<4,T> parent;

    public:
      /** \brief Empty constructor. */
      vec4() : parent() { }

      /** \brief Initializing constructor.
       *
       * \param px First component.
       * \param py Second component.
       * \param pz Third component.
       * \param pw Fourth component.
       */
      vec4(const T &px, const T &py, const T &pz, const T &pw)
      {
        parent::m_array[0] = px;
        parent::m_array[1] = py;
        parent::m_array[2] = pz;
        parent::m_array[3] = pw;
      }

      /** \brief Copy constructor.
       *
       * \param other Copied object.
       */
      vec4(const parent &other) : parent(other) { }

    public:
      /** \brief Set contents.
       *
       * \param px First component.
       * \param py Second component.
       * \param pz Third component.
       * \param pw Fourth component.
       */
      void set(const T &px, const T &py, const T &pz, const T &pw)
      {
        parent::m_array[0] = px;
        parent::m_array[1] = py;
        parent::m_array[2] = pz;
        parent::m_array[3] = pw;
      }

    public:
      /** \brief Accessor.
       *
       * \return Vector component.
       */
      T& x()
      {
        return parent::m_array[0];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      const T& x() const
      {
        return parent::m_array[0];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      T& y()
      {
        return parent::m_array[1];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      const T& y() const
      {
        return parent::m_array[1];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      T& z()
      {
        return parent::m_array[2];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      const T& z() const
      {
        return parent::m_array[2];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      T& w()
      {
        return parent::m_array[3];
      }

      /** \brief Accessor.
       *
       * \return Vector component.
       */
      const T& w() const
      {
        return parent::m_array[3];
      }
  };

  /** Convenience typedef. */
  typedef vec2<double> vec2d;

  /** Convenience typedef. */
  typedef vec3<double> vec3d;

  /** Convenience typedef. */
  typedef vec2<float> vec2f;

  /** Convenience typedef. */
  typedef vec3<float> vec3f;

  /** Convenience typedef. */
  typedef vec4<float> vec4f;

  /** Convenience typedef. */
  typedef vec2<int> vec2i;

  /** Convenience typedef. */
  typedef vec3<int> vec3i;

  /** Convenience typedef. */
  typedef vec4<int> vec4i;

  /** Convenience typedef. */
  typedef vec4<unsigned> vec4u;

  /** \brief Dot product.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Result.
   */
  template<typename T> inline typename T::value_type dot(const T &lhs, const T &rhs)
  {
    typename T::value_type ret = static_cast<typename T::value_type>(0);
    for(unsigned ii = 0; (ii < T::static_size); ++ii)
    {
      ret += lhs[ii] * rhs[ii];
    }
    return ret;
  }

  /** \brief Square of the length of a vector.
   *
   * \param op Vector.
   * \return Result.
   */
  template<typename T> inline typename T::value_type length2(const T &op)
  {
    return dot(op, op);
  }

  /** \brief Length of a vector.
   *
   * \param op Vector.
   * \return Result.
   */
  template<typename T> inline typename T::value_type length(const T &op)
  {
    return sqrt(length2(op));
  }

  /** \brief Normalize to unit vector.
   *
   * \param op Vector.
   * \return Result.
   */
  template<typename T> inline T normalize(const T &op)
  {
    typename T::value_type inv_length = 1.0f / length(op);
    return op * inv_length;
  }

  /** \brief Component-wise min.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Result.
   */
  template<typename T> inline T vec_min(const T &lhs, const T &rhs)
  {
    T ret;
    for(unsigned i = 0; (i != T::static_size); ++i)
    {
      ret[i] = std::min(lhs[i], rhs[i]);
    }
    return ret;
  }
  /** \cond */
  template<> inline math::vec2f min(const vec2f &lhs, const vec2f &rhs)
  {
    return vec_min(lhs, rhs);
  }
  template<> inline math::vec3f min(const vec3f &lhs, const vec3f &rhs)
  {
    return vec_min(lhs, rhs);
  }
  template<> inline math::vec4f min(const vec4f &lhs, const vec4f &rhs)
  {
    return vec_min(lhs, rhs);
  }
  template<> inline math::vec2i min(const vec2i &lhs, const vec2i &rhs)
  {
    return vec_min(lhs, rhs);
  }
  template<> inline math::vec3i min(const vec3i &lhs, const vec3i &rhs)
  {
    return vec_min(lhs, rhs);
  }
  template<> inline math::vec4i min(const vec4i &lhs, const vec4i &rhs)
  {
    return vec_min(lhs, rhs);
  }
  template<> inline math::vec4u min(const vec4u &lhs, const vec4u &rhs)
  {
    return vec_min(lhs, rhs);
  }
  /** \endcond */

  /** \brief Component-wise max.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Result.
   */
  template<typename T> inline T vec_max(const T &lhs, const T &rhs)
  {
    T ret;
    for(unsigned i = 0; (i != T::static_size); ++i)
    {
      ret[i] = std::max(lhs[i], rhs[i]);
    }
    return ret;
  }
  /** \cond */
  template<> inline math::vec2f max(const vec2f &lhs, const vec2f &rhs)
  {
    return vec_max(lhs, rhs);
  }
  template<> inline math::vec3f max(const vec3f &lhs, const vec3f &rhs)
  {
    return vec_max(lhs, rhs);
  }
  template<> inline math::vec4f max(const vec4f &lhs, const vec4f &rhs)
  {
    return vec_max(lhs, rhs);
  }
  template<> inline math::vec2i max(const vec2i &lhs, const vec2i &rhs)
  {
    return vec_max(lhs, rhs);
  }
  template<> inline math::vec3i max(const vec3i &lhs, const vec3i &rhs)
  {
    return vec_max(lhs, rhs);
  }
  template<> inline math::vec4i max(const vec4i &lhs, const vec4i &rhs)
  {
    return vec_max(lhs, rhs);
  }
  template<> inline math::vec4u max(const vec4u &lhs, const vec4u &rhs)
  {
    return vec_max(lhs, rhs);
  }
  /** \endcond */

  /** \brief Get the angle of a vector in assumed plane.
   *
   * Positive x, zero y is 0, positive y, zero x is M_PI / 2.
   *
   * \param op Vector to calculate angle on.
   * \return Angle in the range [0, 2PI[.
   */
  template<typename T> inline T angle(const vec2<T> &op)
  {
    if(static_cast<T>(0) < op.x())
    {
      T ret = atan(op.y() / op.x());
      if(ret < static_cast<T>(0))
      {
        ret += static_cast<T>(M_PI) * static_cast<T>(2);
      }
      return ret;
    }
    if(static_cast<T>(0) > op.x())
    {
      return static_cast<T>(M_PI) + atan(op.y() / op.x());
    }
    return (static_cast<T>(0) < op.y()) ?
      static_cast<T>(M_PI_2) :
      static_cast<T>(M_PI_2 + M_PI_4);
  }

  /** \brief Cross product of 3-dimensional vectors.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Result.
   */
  template<typename T> inline vec<3, T> cross(const vec<3, T> &lhs, const vec<3, T> &rhs)
  {
    vec<3, T> ret;
    ret[0] = (lhs[1] * rhs[2]) - (lhs[2] * rhs[1]);
    ret[1] = (lhs[2] * rhs[0]) - (lhs[0] * rhs[2]);
    ret[2] = (lhs[0] * rhs[1]) - (lhs[1] * rhs[0]);
    return ret;
  }

  /** \brief Distance squared between two points.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Result.
   */
  template<typename T> inline typename T::value_type dist2_point_point(const T &lhs, const T &rhs)
  {
    return length2(lhs - rhs);
  }

  /** \brief Distance Between two points.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Result.
   */
  template<typename T> inline typename T::value_type dist_point_point(const T &lhs, const T &rhs)
  {
    return math::sqrt(dist2_point_point(lhs, rhs));
  }

  /** \brief Squared distance between a line and a point.
   *
   * Algorithm from Wolfram MathWorld:
   * http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
   *
   * \param l1 First line point 1.
   * \param l2 First line point 2.
   * \param pp Point.
   * \return True if collide, false if not.
   */
  template <typename T> inline T dist2_line_point(const math::vec3<T> &l1,
      const math::vec3<T> &l2, const math::vec3<T> &pp)
  {
    math::vec3<T> l2l1 = l2 - l1;
    math::vec3<T> l1pp = l1 - pp;
    T len2 = math::length2(l2l1);

    // Reduction to point-point.
    if(len2 <= static_cast<T>(FLT_EPSILON))
    {
      return math::length2(l1pp);
    }

    T tt = math::dot(l1pp, l2l1) / (-len2);

    // Closer to l1.
    if(tt < static_cast<T>(0))
    {
      return math::length2(l1pp);
    }
    // Closer to l2.
    if(tt > static_cast<T>(1))
    {
      return math::length2(l2 - pp);
    }
    // General case.
    return math::length2(math::cross(l2l1, l1pp)) / len2;
  }

  /** \brief Distance between a line and a and point.
   *
   * \param l1 First line point 1.
   * \param l2 First line point 2.
   * \param pp Point.
   * \return True if collide, false if not.
   */
  template <typename T> inline T dist_line_point(const math::vec3<T> &l1,
      const math::vec3<T> &l2, const math::vec3<T> &pp)
  {
    return math::sqrt(dist2_line_point(l1, l2, pp));
  }

  /** \brief Calculate reflection vector.
   *
   * \param v Ray direction (coming towards origo).
   * \param n Normal (starting from origo).
   * \return Result.
   */
  template<typename T> inline T reflect(const T &v, const T &n)
  {
    return v - 2 * dot(n, v) * n;
  }

  /** \brief Ray intersection.
   *
   * Returns the first intersection (not second).
   *
   * Algorithm from Scott Owen / SIGGRAPH:
   * http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter1.htm
   *
   * \param dst Intersect position.
   * \param pos Position.
   * \param dir Direction.
   * \param bpos Ball position.
   * \param brad Ball radius
   * \return True if intersected, false if not.
   */
  template<typename T> bool intersect_ray_ball(vec3<T> &intersect,
      const vec3<T> &pos, const vec3<T> &dir, const vec3<T> &bpos,
      const T &brad)
  {
    vec3<T> p_b = pos - bpos;
    T aa = length2(dir);
    T bb = dot(dir, p_b) * static_cast<T>(2);
    T cc = length2(p_b) - brad * brad;

    T disc = bb * bb - static_cast<T>(4) * aa * cc;

    // No roots -> no solutions.
    if(static_cast<T>(0) > disc)
    {
      return false;
    }

    // One root -> one solution.
    if(static_cast<T>(0) == disc)
    {
      T tt = -bb * static_cast<T>(0.5);
      if(tt >= static_cast<T>(0))
      {
        intersect = pos + dir * tt;
        return true;
      }
      return false;
    }

    // Two roots -> two solutions.
    disc = sqrt(disc);
    T tt1 = (-bb - disc) * static_cast<T>(0.5);
    T tt2 = (-bb + disc) * static_cast<T>(0.5);
    if(tt1 >= static_cast<T>(0))
    {
      if(tt2 >= static_cast<T>(0))
      {
        intersect = pos + dir * min(tt1, tt2);
        return true;
      }
      intersect = pos + dir * tt1;
      return true;
    }
    if(tt2 >= static_cast<T>(0))
    {
      intersect = pos + dir * tt2;
      return true;
    }
    return false;
  }
}

#endif
