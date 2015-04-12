#ifndef MATH_MAT_HPP
#define MATH_MAT_HPP

#include "math/quat.hpp"
#include "math/vec.hpp"

namespace math
{
  /** \brief Generic matrix type.
   *
   * Defined using the OpenGL by-column definition.
   */
  template<size_t S, typename T> struct mat
  {
    /** STL lookalike. */
    typedef T value_type;

    /** STL lookalike. */
    typedef T* iterator;

    /** Internal information. */
    static const unsigned static_size = S * S;

    /** Actual data. */
    T m_array[S * S];

    /** \brief STL lookalike.
     *
     * \return Size of this.
     */
    static size_t size()
    {
      return S * S;
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
     * \return Begin iterator.
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
      return m_array + (S * S);
    }

    /** \brief STL lookalike.
     *
     * \return End iterator.
     */
    iterator end() const
    {
      return m_array + (S * S);
    }

    /** \brief Empty constructor. */
    mat() { }

    /** \brief Fill constructor.
     *
     * \param v Source.
     */
    mat(T v)
    {
      for(unsigned i = 0; (i != S * S); ++i)
      {
        m_array[i] = v;
      }
    }

    /** \brief Copy constructor.
     *
     * \param v Source.
     */
    mat(const T *v)
    {
      std::copy(v, v + (S * S), m_array);
    }

    /** \brief Copy constructor.
     *
     * \param other Source.
     */
    mat(const mat<S, T> &other)
    {
      std::copy(other.m_array, other.m_array+(S*S), m_array);
    }

    /** \brief Copy from matrix of another type.
     *
     * \param other Source.
     */
    template<size_t S2> mat(const mat<S2, T> &other)
    {
      for(unsigned c=0; c!=S; ++c)
      {
        for(unsigned r=0; r!=S; ++r)
        {
          m_array[c*S+r] =
            (c<S2 && r<S2) ? other(r, c) : 0;
        }
      }
    }

    /** \brief Access operator.
     *
     * \param r Row.
     * \param c Column
     * \return Value in given position..
     */
    T& operator()(unsigned r, unsigned c)
    {
      return m_array[c * S + r];
    }

    /** \brief Access operator.
     *
     * \param r Row.
     * \param c Column
     * \return Value in given position..
     */
    const T& operator()(unsigned r, unsigned c) const
    {
      return m_array[c * S + r];
    }

    /** \brief Access operator.
     *
     * Same as operator[], but throws an error if out of range.
     * 
     * \param r Row.
     * \param c Column
     * \return Value in given position..
     */
    T& at(unsigned r, unsigned c)
    {
      if((r >= S) || (c >= S))
      {
        BOOST_THROW_EXCEPTION(std::range_error("Boundary check fails"));
      }
      return m_array[c * S + r];
    }

    /** \brief Access operator.
     *
     * Same as operator[], but throws an error if out of range.
     * 
     * \param r Row.
     * \param c Column
     * \return Value in given position..
     */
    const T& at(unsigned r, unsigned c) const
    {
      if((r >= S) || (c >= S))
      {
        BOOST_THROW_EXCEPTION(std::range_error("Boundary check fails"));
      }
      return m_array[c * S + r];
    }

    /** \brief Get column as vector.
     *
     * \param c Column.
     * \return Result.
     */
    vec<S, T> getColumn(unsigned c) const
    {
      return vec<S,T>(m_array + (S * c));
    }

    /** \brief Get row as vector.
     *
     * \param r Row.
     * \return Result.
     */
    vec<S, T> getRow(unsigned r) const
    {
      vec<S,T> v;
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        v[ii] = m_array[ii * S + r];
      }
      return v;
    }

    /** \brief Set column within.
     *
     * \param c Column.
     * \param v New vector value.
     */
    void setColumn(unsigned c, const vec<S,T> &v)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[c * S + ii] = v[ii];
      }
    }

    /** \brief Set row within.
     *
     * \param r Row.
     * \param v New vector value.
     */
    void setRow(unsigned r, const vec<S,T> & v)
    {
      for(unsigned ii = 0; (ii != S); ++ii)
      {
        m_array[ii * S + r] = v[ii];
      }
    }

    /** \brief Component-wise addition.
     *
     * \param other Other operand.
     * \return Result.
     */
    mat<S, T> operator+(const mat<S, T> &other) const
    {
      mat<S, T> ret;
      for(unsigned ii = 0; (ii < (S * S)); ++ii)
      {
        ret.m_array[ii] = m_array[ii] + other.m_array[ii];
      }
      return ret;
    }

    /** \brief Scalar addition.
     *
     * \param op Operand.
     * \return Result.
     */
    mat<S, T> operator+(const T &op) const
    {
      mat<S, T> ret;
      for(unsigned ii = 0; (ii < (S * S)); ++ii)
      {
        ret.m_array[ii] = m_array[ii] + op;
      }
      return ret;
    }

    /** \brief Vector multiplication.
     *
     * \param v Vector operand.
     * \return Result.
     */
    vec<S,T> operator*(const vec<S,T> &v) const
    {
      vec<S,T> ret;
      for(unsigned ii = 0; (ii < S); ++ii)
      {
        ret[ii] = dot(v, getRow(ii));
      }
      return ret;
    }

    /** \brief Component-wise multiplication.
     *
     * \param other Other operand.
     * \return Result.
     */
    mat<S,T> operator*(const mat<S,T> &other) const
    {
      mat<S,T> ret;
      for(unsigned ii = 0; (ii < S); ++ii)
      {
        vec<S,T> v2 = other.getColumn(ii);
        for(unsigned jj = 0; (jj < S); ++jj)
        {
          vec<S,T> v1 = getRow(jj);
          ret.m_array[ii * S + jj] = dot(v1, v2);
        }
      }
      return ret;
    }

    /** \brief Scalar multiplication.
     *
     * \param op Operand.
     * \return Result.
     */
    mat<S, T> operator*(const T &op) const
    {
      mat<S, T> ret;
      for(unsigned ii = 0; (ii < (S * S)); ++ii)
      {
        ret.m_array[ii] = m_array[ii] * op;
      }
      return ret;
    }

    /** \brief Component-wise addition assign.
     *
     * \param other Other operand.
     * \return This after operation.
     */
    mat<S, T>& operator+=(const mat<S, T> &other)
    {
      for(unsigned ii = 0; (ii < (S * S)); ++ii)
      {
        m_array[ii] += other.m_array[ii];
      }
      return *this;
    }

    /** \brief Scalar addition assign.
     *
     * \param op Operand.
     * \return This after operation.
     */
    mat<S, T>& operator+(const T &op)
    {
      for(unsigned ii = 0; (ii < (S * S)); ++ii)
      {
        m_array[ii] += op;
      }
      return *this;
    }

    /** \brief Scalar multiplication assign.
     *
     * \param op Operand.
     * \return This after operation.
     */
    mat<S, T>& operator*(const T &op)
    {
      for(unsigned ii = 0; (ii < (S * S)); ++ii)
      {
        m_array[ii] *= op;
      }
      return *this;
    }

    /** \brief Convert a matrix into an identity matrix.
    */
    void loadIdentity()
    {
      for(unsigned c = 0; (c < S); ++c)
      {
        for(unsigned r = 0; (r < S); ++r)
        {
          m_array[c * S + r] = ((r == c) ?
              static_cast<T>(1) : static_cast<T>(0));
        }
      }
    }

    /** \brief Output a matrix into a stream.
     *
     * \param lhs Stream to output to.
     * \param rhs Vector to print.
     */
    friend std::ostream& operator<<(std::ostream &lhs, const mat<S, T> &rhs)
    {
      lhs << "[ " << rhs.getRow(0) << std::endl;
      for(unsigned ii = 1; (ii < S - 1); ++ii)
      {
        lhs << "  " << rhs.getRow(ii) << std::endl;
      }
      return lhs << "  " << rhs.getRow(S - 1) << " ]";
    }
  };

  /** \brief Transpose a matrix.
   *
   * \param m Matrix.
   * \return Result.
   */
  template<size_t S, typename T> mat<S,T> transpose(const mat<S,T> &m)
  {
    mat<S,T> r;
    for(unsigned i = 0; (i < S); ++i)
    {
      r.setColumn(i, m.getRow(i));
    }
    return r;
  }

  /** \brief Matrix for 2D rotaton.
  */
  template<typename T> struct mat2 : mat<2,T>
  {
    /** Parent type for internal use. */
    typedef mat<2,T> parent;

    /** \brief Empty constructor. */
    mat2() : parent() { }

    /** \brief Copy constructor. */
    mat2(const parent &p) : parent(p) { }
  };

  /** \brief Matrix for 3D rotation or homogenous 2D coordinates.
   */
  template<typename T> struct mat3 : mat<3,T>
  {
    /** Parent type for internal use. */
    typedef mat<3,T> parent;

    /** \brief Empty constructor. */
    mat3() : parent() { }

    /** \brief Copy constructor. */
    mat3(const parent &p) : parent(p) { }

    /** \brief Load rotation matrix.
     *
     * \param rot Rotation to load.
     * \param px X translation.
     * \param py Y translation.
     */
    void loadRotation(const T &rot, const T &px, const T &py)
    {
      T sr = math::sin(rot),
        cr = math::cos(rot);

      parent::m_array[0] = cr;
      parent::m_array[1] = sr;
      parent::m_array[2] = static_cast<T>(0);
      parent::m_array[3] = -sr;
      parent::m_array[4] = cr;
      parent::m_array[5] = static_cast<T>(0);
      parent::m_array[6] = px;
      parent::m_array[7] = py;
      parent::m_array[8] = static_cast<T>(1);
    }

    /** \brief Load rotation matrix.
     *
     * \param rot Rotation to load.
     * \param trans Translation.
     */
    void loadRotation(const T &rot, const math::vec2<T> &trans)
    {
      this->loadRotation(rot, trans.x(), trans.y());
    }

    /** \brief Load rotation matrix.
     *
     * \param rot Rotation to load.
     */
    void loadRotation(const T &rot)
    {
      this->loadRotation(rot, 0.0f, 0.0f);
    }

    /** \brief Load scale matrix.
     *
     * \param px X scale.
     * \param py Y scale.
     */
    void loadScale(const T &px, const T &py)
    {
      parent::m_array[0] = px;
      parent::m_array[1] = static_cast<T>(0);
      parent::m_array[2] = static_cast<T>(0);
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = py;
      parent::m_array[5] = static_cast<T>(0);
      parent::m_array[6] = static_cast<T>(0);
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = static_cast<T>(1);
    }

    /** \brief Load rotation matrix.
     *
     * \param px X translation.
     * \param py Y translation.
     */
    void loadTranslation(const T &px, const T &py)
    {
      parent::m_array[0] = static_cast<T>(1);
      parent::m_array[1] = static_cast<T>(0);
      parent::m_array[2] = static_cast<T>(0);
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = static_cast<T>(1);
      parent::m_array[5] = static_cast<T>(0);
      parent::m_array[6] = px;
      parent::m_array[7] = py;
      parent::m_array[8] = static_cast<T>(1);
    }

    /** \brief Load rotation matrix.
     *
     * \param trans Translation.
     */
    void loadTranslation(const math::vec2<T> &trans)
    {
      this->loadTranslation(trans.x(), trans.y());
    }
  };

  /** Matrix for homogenous 3D coordinates.
   */
  template<typename T> struct mat4 : mat<4,T>
  {
    /** Parent type for internal use. */
    typedef mat<4,T> parent;

    /** \brief Empty constructor. */
    mat4() : parent() { }

    /** \brief Copy constructor. */
    mat4(const parent &p) : parent(p) { }

    /** \brief Transform a matrix into a view matrix.
     *
     * Assumes the matrix contains an object-orientation, this is transformed
     * into a camera view matrix that is located in that object orientation.
     *
     * Procedure:
     * - transpose 3x3 part in upper left
     * - take the negative vector of the upper right 1x3 part
     * - transform the negative vector by the transposed part
     * - leave rest untouched
     */
    void convertToView();

    /** \brief Load an orthogonal projection matrix.
     *
     * \param offsetX Starting X offset.
     * \param offsetY Starting Y offset.
     * \param scaleX X scaling factor.
     * \param scaleY Y scaling factor.
     */
    void loadOrthoProjection(T offsetX, T offsetY, T scaleX, T scaleY)
    {
      parent::m_array[0] = scaleX;
      parent::m_array[1] = static_cast<T>(0);
      parent::m_array[2] = static_cast<T>(0);
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = static_cast<T>(0);
      parent::m_array[5] = scaleY;
      parent::m_array[6] = static_cast<T>(0);
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = static_cast<T>(0);
      parent::m_array[9] = static_cast<T>(0);
      parent::m_array[10] = static_cast<T>(1);
      parent::m_array[11] = static_cast<T>(0);
      parent::m_array[12] = offsetX;
      parent::m_array[13] = offsetY;
      parent::m_array[14] = static_cast<T>(0);
      parent::m_array[15] = static_cast<T>(1);
    }

    /** \brief Load a perspective matrix.
     *
     * \param fovy Field of view in radians.
     * \param ratio Ratio of view in w / h.
     * \param znear Near clip plane.
     * \param zfar Near clip plane.
     */
    void loadPerspective(T fovy, T ratio, T znear, T zfar)
    {
      T ff = math::cot(fovy * static_cast<T>(0.5)),
        n_f = static_cast<T>(1) / (znear - zfar);

      parent::m_array[0] = ff / ratio;
      parent::m_array[1] = static_cast<T>(0);
      parent::m_array[2] = static_cast<T>(0);
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = static_cast<T>(0);
      parent::m_array[5] = ff;
      parent::m_array[6] = static_cast<T>(0);
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = static_cast<T>(0);
      parent::m_array[9] = static_cast<T>(0);
      parent::m_array[10] = (zfar + znear) * n_f;
      parent::m_array[11] = static_cast<T>(-1);
      parent::m_array[12] = static_cast<T>(0);
      parent::m_array[13] = static_cast<T>(0);
      parent::m_array[14] = static_cast<T>(2) * zfar * znear * n_f;
      parent::m_array[15] = static_cast<T>(0);
    }

    /** \brief Load a lookat matrix.
     *
     * Functionally equivalent to OpenGL gluLookAt function.
     *
     * \param eye Eye point.
     * \param tgt Target point.
     * \param pup Up vector.
     */
    void loadLookAt(const vec3<T> &eye, const vec3<T> &tgt,
        const vec3<T> &pup)
    {
      vec3<T> fw(normalize(tgt - eye));
      vec3<T> up(normalize(pup));
      vec3<T> rt(normalize(cross(fw, up)));
      up = cross(rt, fw);

      parent::m_array[0] = rt.x();
      parent::m_array[1] = rt.y();
      parent::m_array[2] = rt.z();
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = up.x();
      parent::m_array[5] = up.y();
      parent::m_array[6] = up.z();
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = -fw.x();
      parent::m_array[9] = -fw.y();
      parent::m_array[10] = -fw.z();
      parent::m_array[11] = static_cast<T>(0);
      parent::m_array[12] = eye.x();
      parent::m_array[13] = eye.y();
      parent::m_array[14] = eye.z();
      parent::m_array[15] = static_cast<T>(1);
    }

    /** \brief Create a X rotation matrix.
     *
     * The matrix looks like this:
     *    1    0    0    0
     *    0   cr  -sr    0
     *    0   sr   cr    0
     *    0    0    0    1
     *
     * \param rot Rotation.
     */
    void loadRotX(const T& rot)
    {
      T sr = math::sin(rot),
        cr = math::cos(rot);

      parent::m_array[0] = static_cast<T>(1);
      parent::m_array[1] = static_cast<T>(0);
      parent::m_array[2] = static_cast<T>(0);
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = static_cast<T>(0);
      parent::m_array[5] = cr;
      parent::m_array[6] = sr;
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = static_cast<T>(0);
      parent::m_array[9] = -sr;
      parent::m_array[10] = cr;
      parent::m_array[11] = static_cast<T>(0);
      parent::m_array[12] = static_cast<T>(0);
      parent::m_array[13] = static_cast<T>(0);
      parent::m_array[14] = static_cast<T>(0);
      parent::m_array[15] = static_cast<T>(1);
    }

    /** \brief Create a Y rotation matrix.
     *
     * The matrix looks like this:
     *   cr    0   sr    0
     *    0    1    0    0
     *  -sr    0   cr    0
     *    0    0    0    1
     *
     * \param rot Rotation.
     */
    void loadRotY(const T& rot)
    {
      T sr = math::sin(rot),
        cr = math::cos(rot);

      parent::m_array[0] = cr;
      parent::m_array[1] = static_cast<T>(0);
      parent::m_array[2] = -sr;
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = static_cast<T>(0);
      parent::m_array[5] = static_cast<T>(1);
      parent::m_array[6] = static_cast<T>(0);
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = sr;
      parent::m_array[9] = static_cast<T>(0);
      parent::m_array[10] = cr;
      parent::m_array[11] = static_cast<T>(0);
      parent::m_array[12] = static_cast<T>(0);
      parent::m_array[13] = static_cast<T>(0);
      parent::m_array[14] = static_cast<T>(0);
      parent::m_array[15] = static_cast<T>(1);
    }

    /** \brief Create a Z rotation matrix.
     *
     * The matrix looks like this:
     *   cr  -sr    0    0
     *   sr   cr    0    0
     *    0    0    1    0
     *    0    0    0    1
     *
     * \param rot Rotation.
     */
    void loadRotZ(const T& rot)
    {
      T sr = math::sin(rot),
        cr = math::cos(rot);

      parent::m_array[0] = cr;
      parent::m_array[1] = sr;
      parent::m_array[2] = static_cast<T>(0);
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = -sr;
      parent::m_array[5] = cr;
      parent::m_array[6] = static_cast<T>(0);
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = static_cast<T>(0);
      parent::m_array[9] = static_cast<T>(0);
      parent::m_array[10] = static_cast<T>(1);
      parent::m_array[11] = static_cast<T>(0);
      parent::m_array[12] = static_cast<T>(0);
      parent::m_array[13] = static_cast<T>(0);
      parent::m_array[14] = static_cast<T>(0);
      parent::m_array[15] = static_cast<T>(1);
    }

    /** \brief Create a Z-X-Y rotation matrix.
     *
     * This rotation schema was used in Quake.
     *
     * The matrix look like this:
     * cy*cz + sx*sy*sz   sx*sy*cz - sz*cy              sy*cx                 px
     *            sz*cx              cx*cz                -sx                 py
     * sx*sz*cy - sy*cz   sy*sz + sx*cy*cz              cx*cy                 pz
     *                0                  0                  0                  1
     *
     * \param rot Rotation.
     * \param pos Translation component.
     */
    void loadRotZXY(const vec3<T> &rot,
        const vec3<T> &pos = vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)))
    {
      T sx = math::sin(rot.x()),
        sy = math::sin(rot.y()),
        sz = math::sin(rot.z()),
        cx = math::cos(rot.x()),
        cy = math::cos(rot.y()),
        cz = math::cos(rot.z());

      parent::m_array[0] = sx * sy * sz + cy * cz;
      parent::m_array[1] = sz * cx;
      parent::m_array[2] = sx * sz * cy - sy * cz;
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = sx * sy * cz - sz * cy;
      parent::m_array[5] = cx * cz;
      parent::m_array[6] = sx * cy * cz + sy * sz;
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = sy * cx;
      parent::m_array[9] = -sx;
      parent::m_array[10] = cx * cy;
      parent::m_array[11] = static_cast<T>(0);
      parent::m_array[12] = pos.x();
      parent::m_array[13] = pos.y();
      parent::m_array[14] = pos.z();
      parent::m_array[15] = static_cast<T>(1);
    }

  /** \brief Create quaternion rotation matrix.
   *
   * Generates a transformation matrix of a quaternion expressed as an orientation.
   *
   * The resulting matrix looks like this:
   * 1 - 2j^2 - 2k^2         2ij + 2ak         2ik - 2aj                px
   *       2ij - 2ak   1 - 2i^2 - 2k^2         2jk + 2ai                py
   *       2ik + 2aj         2jk - 2ai   1 - 2i^2 - 2j^2                pz
   *               0                 0                 0                 1
   *
   * \param rot Rotation.
   * \param pos Translation component.
   */
    void loadRotQuat(const quat<T> rot,
        const vec3<T> &pos = vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)))
    {
      T inv_length = 1.0f / length(rot);
      T aa = rot[0] * inv_length;
      T ii = rot[1] * inv_length;
      T jj = rot[2] * inv_length;
      T kk = rot[3] * inv_length;

      parent::m_array[0] = static_cast<T>(1) - static_cast<T>(2) * (jj * jj + kk * kk);
      parent::m_array[1] = static_cast<T>(2) * (ii * jj - aa * kk);
      parent::m_array[2] = static_cast<T>(2) * (ii * kk + aa * jj);
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = static_cast<T>(2) * (ii * jj + aa * kk);
      parent::m_array[5] = static_cast<T>(1) - static_cast<T>(2) * (ii * ii + kk * kk);
      parent::m_array[6] = static_cast<T>(2) * (jj * kk - aa * ii);
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = static_cast<T>(2) * (ii * kk - aa * jj);
      parent::m_array[9] = static_cast<T>(2) * (jj * kk + aa * ii);
      parent::m_array[10] = static_cast<T>(1) - static_cast<T>(2) * (ii * ii + jj * jj);
      parent::m_array[11] = static_cast<T>(0);
      parent::m_array[12] = pos.x();
      parent::m_array[13] = pos.y();
      parent::m_array[14] = pos.z();
      parent::m_array[15] = static_cast<T>(1);
    }

    /** \brief Load a translation matrix.
     *
     * \param mov Movement vector.
     */
    void loadTranslation(const math::vec3<T> &mov)
    {
      parent::m_array[0] = static_cast<T>(1);
      parent::m_array[1] = static_cast<T>(0);
      parent::m_array[2] = static_cast<T>(0);
      parent::m_array[3] = static_cast<T>(0);
      parent::m_array[4] = static_cast<T>(0);
      parent::m_array[5] = static_cast<T>(1);
      parent::m_array[6] = static_cast<T>(0);
      parent::m_array[7] = static_cast<T>(0);
      parent::m_array[8] = static_cast<T>(0);
      parent::m_array[9] = static_cast<T>(0);
      parent::m_array[10] = static_cast<T>(1);
      parent::m_array[11] = static_cast<T>(0);
      parent::m_array[12] = mov.x();
      parent::m_array[13] = mov.y();
      parent::m_array[14] = mov.z();
      parent::m_array[15] = static_cast<T>(1);
    }

    /** \brief Load a translation matrix.
     *
     * \param px X translation.
     * \param py Y translation.
     * \param pz Z translation.
     */
    void loadTranslation(const T &px, const T &py, const T &pz)
    {
      this->loadTranslation(math::vec3<T>(px, py, pz));
    }
  };

  /** \brief Special case vector multiplication.
   *
   * Multiplies a 3-component vector by the upper left 3x3 part.
   *
   * \param vv Vector operand.
   * \return Result.
   */
  template<typename T> vec3<T> operator*(const mat4<T> &lhs, const vec3<T> &rhs)
  {
    math::vec3<T> ret;
    ret.m_array[0] = lhs.m_array[0] * rhs.m_array[0] + lhs.m_array[4] * rhs.m_array[1] + lhs.m_array[8] * rhs.m_array[2];
    ret.m_array[1] = lhs.m_array[1] * rhs.m_array[0] + lhs.m_array[5] * rhs.m_array[1] + lhs.m_array[9] * rhs.m_array[2];
    ret.m_array[2] = lhs.m_array[2] * rhs.m_array[0] + lhs.m_array[6] * rhs.m_array[1] + lhs.m_array[10] * rhs.m_array[2];
    return ret;
  }

  template<typename T> void mat4<T>::convertToView()
  {
    T f1 = parent::m_array[1],
      f2 = parent::m_array[2],
      f4 = parent::m_array[4],
      f6 = parent::m_array[6],
      f8 = parent::m_array[8],
      f9 = parent::m_array[9];
    parent::m_array[1] = f4;
    parent::m_array[2] = f8;
    parent::m_array[4] = f1;
    parent::m_array[6] = f9;
    parent::m_array[8] = f2;
    parent::m_array[9] = f6;

    math::vec3<T> tt = (*this) * math::vec3<T>(-parent::m_array[12], -parent::m_array[13], -parent::m_array[14]);
    parent::m_array[12] = tt.x();
    parent::m_array[13] = tt.y();
    parent::m_array[14] = tt.z();
  }

  /** Convenience typedef. */
  typedef mat2<float> mat2f;
  
  /** Convenience typedef. */
  typedef mat3<float> mat3f;
  
  /** Convenience typedef. */
  typedef mat4<float> mat4f;

  /** Convenience typedef. */
  typedef mat2<int> mat2i;
  
  /** Convenience typedef. */
  typedef mat3<int> mat3i;
  
  /** Convenience typedef. */
  typedef mat4<int> mat4i;
}

#endif
