#ifndef GFX_GEOMETRY_ARRAY_HPP
#define GFX_GEOMETRY_ARRAY_HPP

#include <boost/shared_array.hpp>

namespace gfx
{
  template<class T> class GeometryArrayIterator
  {
    private:
      /** Pointer parameter to value. */
      T* m_ptr;

      /** Comparison parameter, point to this or to end-of-array. */
      T** m_comparison;

    public:
      /** Accessor.
       *
       * \return Pointer parameter.
       */
      T* ptr() const
      {
        return m_ptr;
      }

    public:
      /** \brief Constructor.
       *
       * \param pptr Pointer parameter.
       */
      GeometryArrayIterator(T* pptr) :
        m_ptr(pptr),
        m_comparison(&m_ptr) { }

      /** \brief Constructor.
       *
       * \param pcomparison Comparison parameter.
       */
      GeometryArrayIterator(T** pcomparison) :
        m_ptr(NULL),
        m_comparison(pcomparison) { }

      /** \brief Constructor.
       *
       * \param pcomparison Comparison parameter.
       */
      GeometryArrayIterator(const T** pcomparison) :
        m_ptr(NULL),
        m_comparison(pcomparison) { }

      /** \brief Copy constructor.
       *
       * \param pother Other iterator.
       */
      GeometryArrayIterator(const GeometryArrayIterator<T> &pother) :
        m_ptr(pother.m_ptr),
        m_comparison((pother.m_comparison == &pother.m_ptr) ? &m_ptr : pother.m_comparison) { }

    public:
      /* \brief Dereference operator.
       *
       * \return Reference to object at this position.
       */
      T& operator*()
      {
        return *m_ptr;
      }

      /* \brief Const dereference operator.
       *
       * \return Const reference to object at this position.
       */
      const T& operator*() const
      {
        return *m_ptr;
      }

      /** \brief Pre-increment operator.
       *
       * \return Reference to this after increment.
       */
      GeometryArrayIterator<T>& operator++()
      {
        BOOST_ASSERT(NULL != m_ptr);
        ++m_ptr;
        return *this;
      }

      /** \brief Post-increment operator.
       *
       * \return Reference to this before increment.
       */
      GeometryArrayIterator<T>& operator++(int)
      {
        GeometryArrayIterator<T> ret(*this);
        ++(*this);
        return ret;
      }

      /** \brief Assignment operator.
       *
       * Carries a slight cost due to comparison pointer.
       *
       * \param rhs Right-hand-side operand.
       */
      GeometryArrayIterator<T>& operator=(const GeometryArrayIterator<T> &rhs)
      {
        m_ptr = rhs.m_ptr;
        m_comparison = (rhs.m_comparison == &rhs.m_ptr) ? &m_ptr : rhs.m_comparison;
        return *this;
      }

      /** \brief Equals operator.
       *
       * \param rhs Right-hand-side operand.
       */
      bool operator==(const GeometryArrayIterator<T> &rhs) const
      {
        return (*m_comparison == *rhs.m_comparison);
      }

      /** \brief Inequals operator.
       *
       * \param rhs Right-hand-side operand.
       */
      bool operator!=(const GeometryArrayIterator<T> &rhs) const
      {
        return (*m_comparison != *rhs.m_comparison);
      }
  };

  /** \brief Point sprite array.
   *
   * Holds an array of point sprites and provides a way to add or remove single
   * items or feed the whole array to graphics cards easily.
   *
   * For rendering performance reasons, the contained elements must be
   * copyable.
   */
  template<class T> class GeometryArray
  {
    public:
      /** Iterator type. */
      typedef GeometryArrayIterator<T> iterator;

    private:
      /** Container for all point sprites. */
      boost::shared_array<T> m_array;

      /** End pointer (for iterator references). */
      T* m_end;

      /** Point sprite count. */
      unsigned m_count;

      /** Array size. */
      unsigned m_size;

      /** Maximum array size. */
      unsigned m_max_size;

    protected:
      /** Maximum size (screen coordinates). */
      float m_billboard_size_min;

      /** Maximum size (screen coordinates). */
      float m_billboard_size_max;

      /** Size multiplier. */
      float m_billboard_size_mul;

    public:
      /** \brief Begin iterator.
       *
       * \return Get an iterator to the beginning of the array.
       */
      iterator begin()
      {
        return GeometryArrayIterator<T>(m_array.get());
      }

      /** \brief End iterator.
       *
       * \return Get iterator to the end of array.
       */
      iterator end()
      {
        return GeometryArrayIterator<T>(&m_end);
      }

      /** \brief Erase an element under iterator.
       *
       * Results undefined if iterator is not valid.
       *
       * \param piter Iterator.
       * \return Iterator after the removed element.
       */
      iterator erase(const iterator &piter)
      {
        T* ptr = piter.ptr();

        BOOST_ASSERT(NULL != ptr);
        BOOST_ASSERT(ptr != m_end);

        --m_end;
        --m_count;

        if(ptr != m_end)
        {
          *ptr = *m_end;
        }
        return piter;
      }

    public:
      /** \brief Tell if this is empty.
       *
       * \return True if yes, false if no.
       */
      bool empty() const
      {
        return (0 >= m_count);
      }

      /** \brief Get direct pointer to the array.
       *
       * \param Array pointer.
       */
      T* getArray()
      {
        return m_array.get();
      }

      /** \brief Get direct const pointer to the array.
       *
       * \param Array pointer.
       */
      const T* getArray() const
      {
        return m_array.get();
      }

      /** \brief Get current size.
       *
       * \return Currently used size.
       */
      unsigned size() const
      {
        return m_count;
      }

    public:
      /** \brief Constructor.
       *
       * \param psize Initial size (default: 64).
       * \param pmax Maximum size (default: 8192), zero for unlimited.
       * \param pbillboardmin Minimum billboard size (default: 1/50 of screen).
       * \param pbillboardmax Maximum billboard size (default: 2/5 of screen).
       * \param pbillboardmul Billboard size multiplier (default: 1).
       */
      GeometryArray(unsigned psize = 64, unsigned pmax = 8192,
          float pbillboardmin = 0.02f, float pbillboardmax = 0.4f,
          float pbillboardmul = 1.0f) :
        m_array(new T[psize]),
        m_end(m_array.get()),
        m_count(0),
        m_size(psize),
        m_max_size(pmax),
        m_billboard_size_min(pbillboardmin),
        m_billboard_size_max(pbillboardmax),
        m_billboard_size_mul(pbillboardmul) { }

    public:
      /** \brief Clear the array.
       *
       * Does not change the size of the array.
       */
      void clear()
      {
        m_end = m_array.get();
        m_count = 0;
      }

      /** \brief Add one billboard to the array.
       *
       * If size increase is no longer allowed, the billboard is simply not added.
       *
       * \param op Point sprite to add.
       */
      void push_back(const T &op)
      {
        if(m_count == m_size)
        {
          unsigned new_size = m_size * 3 / 2;

          if(m_max_size > 0)
          {
            new_size = std::min(new_size, m_max_size);

            if(new_size == m_size)
            {
              return;
            }
          }

          boost::shared_array<T> new_array(new T[new_size]);
          T* src = m_array.get();
          T* dst = new_array.get();

          while(src != m_end)
          {
            *dst = *src;

            ++src;
            ++dst;
          }

          m_array = new_array;
          m_size = new_size;
          m_end = dst;
        }

        *m_end = op;
        ++m_end;
        ++m_count;
      }

      /** \brief Set billboard params.
       *
       * \param pmin Minimum size.
       * \param pmax Maximum size.
       * \param pmul Size multiplier.
       */
      void setParams(float pmin, float pmax, float pmul = 1.0f)
      {
        m_billboard_size_min = pmin;
        m_billboard_size_max = pmax;
        m_billboard_size_mul = pmul;
      }
  };
}

#endif
