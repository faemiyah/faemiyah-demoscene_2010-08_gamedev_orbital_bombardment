#ifndef GFX_TRIANGLE_HPP
#define GFX_TRIANGLE_HPP

namespace gfx
{
  /** \brief Triangle.
   *
   * Contains numeric references, used in meshes as a face elementary type.
   */
  class Triangle
  {
    private:
      /** References. */
      unsigned m_corners[3];

    public:
      /** \brief Empty constructor.
       *
       * Only so that STL vector resize() would work.
       */
      Triangle() { }

      /** \brief Constructor.
       *
       * \param pa First corner.
       * \param pb First corner.
       * \param pc First corner.
       */
      Triangle(unsigned pa, unsigned pb, unsigned pc)
      {
        m_corners[0] = pa;
        m_corners[1] = pb;
        m_corners[2] = pc;
      }

    public:
      /** \brief Get the first corner.
       *
       * \return Corner vertex index.
       */
      unsigned& a()
      {
        return m_corners[0];
      }

      /** \brief Get the first corner.
       *
       * \return Corner vertex index.
       */
      unsigned a() const
      {
        return m_corners[0];
      }

      /** \brief Get the second corner.
       *
       * \return Corner vertex index.
       */
      unsigned& b()
      {
        return m_corners[1];
      }

      /** \brief Get the second corner.
       *
       * \return Corner vertex index.
       */
      unsigned b() const
      {
        return m_corners[1];
      }

      /** \brief Get the third corner.
       *
       * \return Corner vertex index.
       */
      unsigned& c()
      {
        return m_corners[2];
      }

      /** \brief Get the third corner.
       *
       * \return Corner vertex index.
       */
      unsigned c() const
      {
        return m_corners[2];
      }

    public:
      /** \brief Assignment operator.
       *
       * \param rhs Right-hand side operand.
       */
      const Triangle& operator=(const Triangle &rhs)
      {
        m_corners[0] = rhs.a();
        m_corners[1] = rhs.b();
        m_corners[2] = rhs.c();
        return *this;
      }

      /** \brief Indexing operator.
       *
       * Not checked, idx should be [0, 2].
       *
       * \param idx Indexing operator.
       */
      unsigned operator()(unsigned idx)
      {
        return m_corners[idx];
      }
  };
};

#endif
