#ifndef LOD_ICOSAHEDRON_HPP
#define LOD_ICOSAHEDRON_HPP

#include "gfx/lod.hpp"

namespace gfx
{
  /** \brief Additional LOD functionality for icosahedrons.
   */
  class LodIcosahedron :
    public Lod
  {
    private:
      /** LOD descend threshold. */
      static float descend_threshold;

      /** Safety value for view boundary checks. */
      static float safety_boundary;

      /** Safety value for view cull checks. */
      static float safety_view;

    private:
      /** \brief Bounding area cornerpoint index. */
      unsigned m_boundary_index_a;

      /** \brief Bounding area cornerpoint index. */
      unsigned m_boundary_index_b;

      /** \brief Bounding area cornerpoint index. */
      unsigned m_boundary_index_c;

      /** Length ab. */
      float m_len_ab;

      /** Length ac. */
      float m_len_ac;

    public:
      /** \brief Constructor.
       *
       * Besides creating the initial triangle, the construction marks the
       * essential corners that will be used to create the bounding area
       * later.
       *
       * \param pa First corner.
       * \param pb Second corner.
       * \param pc Third corner.
       */
      LodIcosahedron(unsigned pa, unsigned pb, unsigned pc);

      /** \brief Destructor. */
      virtual ~LodIcosahedron() { }

    public:
      /** \cond */
      virtual void compile(const std::vector<math::vec3f> &pvvec);
      virtual bool cull(math::vec3f &tpos, const math::mat4f &pmat) const;
      /** \endcond */

    protected:
      /** \cond */
      virtual bool checkDescend(const math::mat4f &pmat, const math::vec3f &tpos) const;
      /** \endcond */
  };
}

#endif
