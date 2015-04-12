#ifndef MATH_RECT_HPP
#define MATH_RECT_HPP

#include "math/vec.hpp"

namespace math
{
  /** \brief Generic rectangle type.
   *
   * Note, that for speed reasons the validity of size is not checked. If the
   * size is smaller than zero, all operations will not work.
   */
  template<typename T> struct rect2
  {
    private:
      /** Lower-left corner position. */
      vec2<T> m_position;

      /** Width and height. */
      vec2<T> m_size;

    public:
      /** \brief Empty constructor. */
      rect2() { }

      /** \brief Constructor.
       *
       * \param ppos Position.
       * \param psiz Size
       */
      rect2(const vec2<T> &ppos, const vec2<T> &psiz) :
        m_position(ppos),
        m_size(psiz) { }

      /** \brief Constructor.
       *
       * \param px X position.
       * \param py Y position.
       * \param psiz Size
       */
      rect2(const T &px, const T &py, const vec2<T> &psiz) :
        m_position(px, py),
        m_size(psiz) { }

      /** \brief Constructor.
       *
       * \param ppos Position.
       * \param pw Width.
       * \param ph Height.
       */
      rect2(const vec2<T> &ppos, const T &pw, const T &ph) :
        m_position(ppos),
        m_size(pw, ph) { }

      /** \brief Constructor.
       *
       * \param px X position.
       * \param py Y position.
       * \param pw Width.
       * \param ph Height.
       */
      rect2(const T &px, const T &py, const T &pw, const T &ph) :
        m_position(px, py),
        m_size(pw, ph) { }

    public:
      /** \brief Tell if two rectangles overlap.
       *
       * \param op Another rectangle.
       */
      bool overlaps(const rect2<T> &op) const
      {
        return !((op.x2() <= this->x1()) || (this->x2() <= op.x1()) ||
            (op.y2() <= this->y1()) || (this->y2() <= op.y1()));
      }

    public:
      /** \brief Assignment operator.
       *
       * \param op Rectangle to assign.
       * \return This after assignment.
       */
      const math::rect2<T>& operator=(const math::rect2<T> &op)
      {
        m_position = op.pos();
        m_size = op.size();
        return *this;
      }

    public:
      /** \brief Accessor.
       *
       * \return Position of this.
       */
      vec2<T>& pos()
      {
        return m_position;
      }

      /** \brief Accessor.
       *
       * \return Position of this.
       */
      const vec2<T>& pos() const
      {
        return m_position;
      }

      /** \brief Accessor.
       *
       * \return First X coordinate.
       */
      T& x1()
      {
        return m_position.x();
      }

      /** \brief Accessor.
       *
       * \return First X coordinate.
       */
      const T& x1() const
      {
        return m_position.x();
      }

      /** \brief Accessor.
       *
       * \return First Y coordinate.
       */
      T& y1()
      {
        return m_position.y();
      }

      /** \brief Accessor.
       *
       * \return First Y coordinate.
       */
      const T& y1() const
      {
        return m_position.y();
      }

      /** \brief Accessor.
       *
       * \return Size.
       */
      vec2<T>& size()
      {
        return m_size;
      }

      /** \brief Accessor.
       *
       * \return Size.
       */
      const vec2<T>& size() const
      {
        return m_size;
      }

      /** \brief Accessor.
       *
       * \return Width.
       */
      T& w()
      {
        return m_size.x();
      }

      /** \brief Accessor.
       *
       * \return Width.
       */
      const T& w() const
      {
        return m_size.x();
      }

      /** \brief Accessor.
       *
       * \return Height.
       */
      T& h()
      {
        return m_size.y();
      }

      /** \brief Accessor.
       *
       * \return Height.
       */
      const T& h() const
      {
        return m_size.y();
      }

    public:
      /** \brief Getter.
       *
       * \return Second X coordinate.
       */
      T x2() const
      {
        return m_position.x() + m_size.x();
      }

      /** \brief Getter.
       *
       * \return Second Y coordinate.
       */
      T y2() const
      {
        return m_position.y() + m_size.y();
      }

      /** \brief Getter.
       *
       * \return Center position.
       */
      vec2<T> center() const
      {
        return m_position + (m_size * 0.5f);
      }

    public:
      friend std::ostream& operator<<(std::ostream &lhs, const rect2<T> &rhs)
      {
        return lhs << "(" << rhs.x1() << " ; " << rhs.y1() << " ; " <<
          rhs.w() << " ; " << rhs.h() << ")";
      }
  };

  /** Convenience typedef. */
  typedef rect2<float> rect2f;

  /** Convenience typedef. */
  typedef rect2<int> rect2i;

  /** \brief Generic 3D-rectangle type.
   *
   * Used to represent bounding boxes.
   */
  template<typename T> struct rect3
  {
    private:
      /** Maximum point. */
      vec3<T> m_pnt_max;

      /** Minimum point. */
      vec3<T> m_pnt_min;

    public:
      /** \brief Get maximum point.
       *
       * \return Max point.
       */
      const math::vec3<T>& getMax() const
      {
        return m_pnt_max;
      }

      /** \brief Get minimum point.
       *
       * \return Min point.
       */
      const math::vec3<T>& getMin() const
      {
        return m_pnt_min;
      }

      /** \brief Accessor.
       *
       * \return Minimum x coordinate.
       */
      T& x1()
      {
        return m_pnt_min.x();
      }

      /** \brief Accessor.
       *
       * \return Minimum x coordinate.
       */
      T x1() const
      {
        return m_pnt_min.x();
      }

      /** \brief Accessor.
       *
       * \return Maximum x coordinate.
       */
      T& x2()
      {
        return m_pnt_max.x();
      }

      /** \brief Accessor.
       *
       * \return Maximum x coordinate.
       */
      T x2() const
      {
        return m_pnt_max.x();
      }

      /** \brief Accessor.
       *
       * \return Minimum y coordinate.
       */
      T& y1()
      {
        return m_pnt_min.y();
      }

      /** \brief Accessor.
       *
       * \return Minimum y coordinate.
       */
      T y1() const
      {
        return m_pnt_min.y();
      }

      /** \brief Accessor.
       *
       * \return Maximum y coordinate.
       */
      T& y2()
      {
        return m_pnt_max.y();
      }

      /** \brief Accessor.
       *
       * \return Maximum y coordinate.
       */
      T y2() const
      {
        return m_pnt_max.y();
      }

      /** \brief Accessor.
       *
       * \return Minimum z coordinate.
       */
      T& z1()
      {
        return m_pnt_min.z();
      }

      /** \brief Accessor.
       *
       * \return Minimum z coordinate.
       */
      T z1() const
      {
        return m_pnt_min.z();
      }

      /** \brief Accessor.
       *
       * \return Maximum z coordinate.
       */
      T& z2()
      {
        return m_pnt_max.z();
      }

      /** \brief Accessor.
       *
       * \return Maximum z coordinate.
       */
      T z2() const
      {
        return m_pnt_max.z();
      }

    public:
      /** \brief Empty constructor. */
      rect3() { }

      /** \brief Constructor.
       *
       * \param pnt Intial point to start from.
       */
      rect3(const vec3<T> &pnt) :
        m_pnt_max(pnt),
        m_pnt_min(pnt) { }

      /** \brief Constructor.
       *
       * Note that this constructor is not checked, the minimum and maximum
       * points must be correct.
       *
       * \param pnt_min Intial minimum point.
       * \param pnt_max Intial maximum point.
       */
      rect3(const vec3<T> &pnt_min, const vec3<T> &pnt_max) :
        m_pnt_max(pnt_max),
        m_pnt_min(pnt_min) { }

    public:
      /** \brief Calculate the centerpoint.
       *
       * \return Center point.
       */
      math::vec3<T> center() const
      {
        return (m_pnt_min + m_pnt_max) * 0.5f;
      }

      /** \brief Expand the boundary.
       *
       * Uses some manual checking to decrease comparison count instead of pure
       * min/max.
       *
       * \param pnt Point to expand with.
       */
      void expand(const math::vec3<T> &pnt)
      {
        if(pnt.x() > m_pnt_max.x())
        {
          m_pnt_max.x() = pnt.x();
        }
        else if(pnt.x() < m_pnt_min.x())
        {
          m_pnt_min.x() = pnt.x();
        }

        if(pnt.y() > m_pnt_max.y())
        {
          m_pnt_max.y() = pnt.y();
        }
        else if(pnt.y() < m_pnt_min.y())
        {
          m_pnt_min.y() = pnt.y();
        }
        
        if(pnt.z() > m_pnt_max.z())
        {
          m_pnt_max.z() = pnt.z();
        }
        else if(pnt.z() < m_pnt_min.z())
        {
          m_pnt_min.z() = pnt.z();
        }
      }

      /** \brief Expand the boundary.
       *
       * \param other Other boundary.
       */
      void expand(const math::rect3<T> &other)
      {
        if(other.x1() < m_pnt_min.x())
        {
          m_pnt_min.x() = other.x1();
        }
        if(other.x2() < m_pnt_max.x())
        {
          m_pnt_max.x() = other.x2();
        }

        if(other.y1() < m_pnt_min.y())
        {
          m_pnt_min.y() = other.y1();
        }
        if(other.y2() < m_pnt_max.y())
        {
          m_pnt_max.y() = other.y2();
        }

        if(other.z1() < m_pnt_min.z())
        {
          m_pnt_min.z() = other.z1();
        }
        if(other.z2() < m_pnt_max.z())
        {
          m_pnt_max.z() = other.z2();
        }
      }

      /** \brief Check whether this rect is completely inside another.
       *
       * Note that rectangles touching on one side are not considered
       * to not be inside. This means that if the other rectangle is
       * identical to this, this function will return true.
       *
       * \param other Other rectangle.
       * \return True if this is inside the other rect, false otherwise.
       */
      bool isInside(const math::rect3<T> &other) const
      {
        return ((m_pnt_min.x() >= other.m_pnt_min.x()) &&
            (m_pnt_max.x() <= other.m_pnt_max.x()) &&
            (m_pnt_min.y() >= other.m_pnt_min.y()) &&
            (m_pnt_max.y() <= other.m_pnt_max.y()) &&
            (m_pnt_min.z() >= other.m_pnt_min.z()) &&
            (m_pnt_max.z() <= other.m_pnt_max.z()));
      }

      /** \brief Check whether this rect is completely outside another.
       *
       * Note that rectangles touching on one side are not considered
       * intersecting.
       *
       * \param other Other rectangle.
       * \return True if no overlap of any kind, false otherwise.
       */
      bool isOutside(const math::rect3<T> &other) const
      {
        return ((m_pnt_max.x() <= other.m_pnt_min.x()) ||
            (m_pnt_min.x() >= other.m_pnt_max.x()) ||
            (m_pnt_max.y() <= other.m_pnt_min.y()) ||
            (m_pnt_min.y() >= other.m_pnt_max.y()) ||
            (m_pnt_max.z() <= other.m_pnt_min.z()) ||
            (m_pnt_min.z() >= other.m_pnt_max.z()));
      }

      /** \brief Return the size of this.
       *
       * \return Size vector.
       */
      math::vec3f size() const
      {
        return m_pnt_max - m_pnt_min;
      }

    public:
      friend std::ostream& operator<<(std::ostream &lhs, const rect3<T> &rhs)
      {
        return lhs << rhs.getMin() << " -> " << rhs.getMax();
      }
  };

  /** Convenience typedef. */
  typedef rect3<float> rect3f;

  /** Convenience typedef. */
  typedef rect3<int> rect3i;
}

#endif
