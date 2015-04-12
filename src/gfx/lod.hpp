#ifndef GFX_LOD_HPP
#define GFX_LOD_HPP

#include "gfx/buffer.hpp"
#include "math/mat.hpp"
#include "math/rect.hpp"

#include <map>

namespace gfx
{
  class Lod;

  /** Convenience typedef. */
  typedef boost::shared_ptr<Lod> LodSptr;

  /** Convenience typeded. */
  typedef std::vector<Triangle> TriVec;

  /** Convenience typeded. */
  typedef boost::shared_ptr<TriVec> TriVecSptr;

  /** \brief LOD-level calculation and visibility for a mesh.
   *
   * This does not exist independently from a mesh, the vertices are always
   * referred from outside.
   */
  class Lod
  {
    protected:
      /** Polygon count for bookkeeping. */
      static unsigned g_poly_count;

    protected:
      /** Recursive LOD levels within this. */
      std::vector<LodSptr> m_recursive;

      /** Faces on this LOD level. */
      std::vector<Triangle> m_faces;

      /** Element buffer. */
      BufferSubElem m_elem;

      /** Center position. */
      math::vec3f m_pos;

      /** Boundary value. */
      float m_dist;

      /** Boundary value. */
      float m_dist2;

      /** Boundary positions. */
      std::vector<math::vec3f> m_boundary;

    public:
      /** \brief Accessor.
       *
       * \return Face table.
       */
      TriVec& getFaces()
      {
        return m_faces;
      }

      /** \brief Accessor.
       *
       * \return Face table.
       */
      const TriVec& getFaces() const
      {
        return m_faces;
      }

      /** \brief Accessor.
       *
       * \return Recursion table.
       */
      std::vector<LodSptr>& getRecursive()
      {
        return m_recursive;
      }

      /** \brief Accessor.
       *
       * \return Recursion table.
       */
      const std::vector<LodSptr>& getRecursive() const
      {
        return m_recursive;
      }

    public:
      /** \brief Constructor. */
      Lod() { }

      /** \brief Destructor. */
      virtual ~Lod() { }

    public:
      /** \brief Coalesce triangles from below.
       *
       * Collect all triangles downward from this, up to level given. Then
       * recurse down and coalesce all the children.
       *
       * If a level is reached, where there is no subsequent LODs below the level given,
       * wipe all the children.
       *
       * \param op Coalesce level.
       */
      void coalesce(unsigned op);

      /** \brief Collect all lowest-level faces.
       *
       * \param pfvec Vector to add into.
       * \return Vector that was modified.
       */
      TriVec& collect(TriVec &pfvec) const;

      /** \brief Collect all faces at given level.
       *
       * \param pfvec Vector to add into.
       * \param op Level to collect at.
       * \return True if there exists levels below op.
       */
      bool collect(TriVec &pfvec, unsigned op) const;

      /** \brief Collect all LODs.
       *
       * \param pfvec Vector to add into.
       */
      void collectAll(std::vector<Lod*> &plvec);

      /** \brief Collect all lowest-level LODs.
       *
       * \param pfvec Vector to add into.
       */
      void collectLowest(std::vector<Lod*> &plvec);

      /** \brief Draw this.
       *
       * Vertex buffer must be already reserved.
       *
       * \param pmat Matrix to use for LOD calculation.
       * \return True if this or some recursive element was drawn, false otherwise.
       */
      bool draw(const math::mat4f &pmat) const;

      /** \brief Unreserve. */
      void unreserve();

    protected:
      /** \brief Calculate minimum and maximum corners of bounding rectangle.
       *
       * Implicitly store the distances and centerpoint within this.
       *
       * \param pvvec Vertex data.
       * \return Axis-aligned boundary.
       */
      math::rect3f calcBoundary(const std::vector<math::vec3f> &pvvec);

      /** \brief Collect all vertex references recursively.
       *
       * All LOD levels are accounted for.
       *
       * \param refmap Map to store references into.
       */
      void collect(std::map<unsigned, bool> &refmap) const;

      /** \brief Check culling from boundary points.
       *
       * Assumes the matrix stack is defined in Surface.
       *
       * To be done after other cull checking already complete.
       *
       * \return True if should draw, false if not.
       */
      bool cullBoundary() const;

      /** \brief Set boundary.
       *
       * All previous boundary information is erased.
       *
       * \param brect Boundary rectangle.
       */
      void setBoundary(const math::rect3f brect);

      /** \brief Trim excess unused data. */
      void trim();

    public:
      /** \brief Compile this.
       *
       * \param pvvec Vertex data.
       */
      virtual void compile(const std::vector<math::vec3f> &pvvec);

      /** \brief Culling.
       *
       * The transformed position of this should always be written, at least
       * if the object is going to be drawn.
       *
       * \param tpos Save slot for transformed position of this.
       * \param pmat View matrix.
       * \return True if draw, false if no.
       */
      virtual bool cull(math::vec3f &tpos, const math::mat4f &pmat) const;

    protected:
      /** \brief Check whether to descend down into a more detailed view.
       *
       * Default implementation always descends.
       *
       * \param pmat View matrix.
       * \param tpos Transformed position of this.
       */
      virtual bool checkDescend(const math::mat4f &pmat, const math::vec3f &tpos) const;

    public:
      /** \brief Add a triangle to this.
       *
       * \param ca Corner a.
       * \param cb Corner b.
       * \param cc Corner c.
       */
      void addFace(unsigned ca, unsigned cb, unsigned cc)
      {
        m_faces.push_back(Triangle(ca, cb, cc));
      }

      /** \brief Add a triangle to this.
       *
       * \param op Triangle to add.
       */
      void addFace(const Triangle &op)
      {
        m_faces.push_back(op);
      }

      /** \brief Add a recursive lod to this.
       *
       * \param op LOD to add.
       */
      void addRecursive(Lod *op)
      {
        m_recursive.push_back(LodSptr(op));
      }

      /** \brief Update the element buffer in this.
       *
       * \param idx Index in the host element buffer.
       */
      void assignElem(unsigned idx)
      {
        m_elem.assign(idx, static_cast<unsigned>(m_faces.size()) * 3);
      }

    public:
      /** \brief Clear polygon count.
       *
       * \return Previous polygon count.
       */
      static unsigned poly_count_clear()
      {
        unsigned ret = g_poly_count;
        g_poly_count = 0;
        return ret;
      }

      /** \brief Get polygon count.
       *
       * \return Current polygon count.
       */
      static unsigned get_poly_count()
      {
        return g_poly_count;
      }

      /** \brief Increment polygon count.
       *
       * \param op Increment count.
       * \return Current polygon count after increment.
       */
      static unsigned poly_count_increment(unsigned op)
      {
        g_poly_count += op;
        return g_poly_count;        
      }
  };
}

#endif
