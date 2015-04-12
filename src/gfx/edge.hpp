#ifndef GFX_EDGE_HPP
#define GFX_EDGE_HPP

namespace gfx
{
  /** \brief Edge between triangles.
   *
   * Edges are used as intermediary tool in mesh manipulation operations.
   * Additionally, edges may be used in shadow volume generation.
   */
  class Edge
  {
    private:
      /** Vertex references. */
      unsigned m_array[2];

    public:
      /** \brief Accessor.
       *
       * \return Vertex index 1
       */
      unsigned a() const
      {
        return m_array[0];
      }

      /** \brief Accessor.
       *
       * \return Vertex index 2
       */
      unsigned b() const
      {
        return m_array[1];
      }

      /** \brief Accessor (64-bit).
       *
       * \return Vertex index 1 (64-bit).
       */
      uint64_t a64() const
      {
        return static_cast<uint64_t>(m_array[0]);
      }

      /** \brief Accessor (64-bit).
       *
       * \return Vertex index 2 (64-bit).
       */
      uint64_t b64() const
      {
        return static_cast<uint64_t>(m_array[1]);
      }

    public:
      /** \brief Empty constructor. */
      Edge() { }

      /** \brief Constructor.
       *
       * \param pa Vertex index 1.
       * \param pb Vertex index 2.
       */
      Edge(unsigned pa, unsigned pb)
      {
        m_array[0] = pa;
        m_array[1] = pb;
      }

    public:
      /** \brief Inequality operator.
       *
       * \param rhs Right-hand-side operand.
       * \return True if not equal, false if equal.
       */
      bool operator!=(const Edge &rhs)
      {
        return ((m_array[0] != rhs.a()) || (m_array[1] != rhs.b()));
      }

      /** \brief Equality operator.
       *
       * \param rhs Right-hand-side operand.
       * \return True if equal, false if not.
       */
      bool operator==(const Edge &rhs)
      {
        return ((m_array[0] == rhs.a()) && (m_array[1] == rhs.b()));
      }

      /** \brief Test for same edge (not necessarily same direction).
       *
       * \param rhs Right-hand-side operand.
       * \return True if vertices are same, false if not.
       */
      bool isSameEdge(const Edge &rhs)
      {
        return (((*this) == rhs) ||
            ((m_array[0] == rhs.b()) && (m_array[1] == rhs.a())));
      }

    public:
      /** \brief Create an edge id.
       *
       * \return Edge identifier for this.
       */
      uint64_t id() const
      {
        return ((this->a64() << 32) | this->b64());
      }

      /** \brief Create an edge id that is the same no matter the direction.
       *
       * \return Edge identifier for this.
       */
      uint64_t idSameEdge() const
      {
        return ((m_array[0] < m_array[1]) ? this->id() :
            ((this->b64() << 32) | this->a64()));
      }

  };
}

#endif
