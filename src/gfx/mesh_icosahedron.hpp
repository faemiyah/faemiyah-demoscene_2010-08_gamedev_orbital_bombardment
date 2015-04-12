#ifndef GFX_MESH_ICOSAHEDRON_HPP
#define GFX_MESH_ICOSAHEDRON_HPP

#include "gfx/mesh.hpp"

namespace gfx
{
  /** \brief Icosahedron mesh.
   *
   * Actually, this represents an icosahedron ball applicable to complex
   * planet-like objects. An ordinary icosahedron would be a corner case at
   * most.
   */
  class MeshIcosahedron :
    public Mesh
  {
    public:
      /** \brief Constructor.
       *
       * The icosahedron mesh acts both as grounds for ordinary icosahedrons
       * and balls subdivided from them and full-fledged planets with
       * normalmaps & such derieved from the base icosahedron mesh.
       *
       * The constructor does nothing. Inherited classes must run compile().
       */
      MeshIcosahedron() { }

      /** \brief Destructor. */
      virtual ~MeshIcosahedron() { }

    protected:
      /** \brief Coalesce the icosahedron for LOD levels.
       *
       * The coalesce count should be smaller or equal to the subdivision
       * count or results may be unexpected.
       *
       * Subdivision count may be passed for automatic error checking.
       *
       * \param cnt Coalesce count.
       * \param subdivision Subdivision count.
       */
      void coalesce(unsigned cnt, unsigned subdivision = UINT_MAX);

      /** \brief Reinitializes abse form from the icosahedron data. */
      void init();

      /** \brief Subdivide the base icosahedron into a ball.
       *
       * Silently reinitializes the data.
       *
       * Subdivision of an icosahedron leaves the icosahedron in an unstable state.
       *
       * \param cnt Subdivision count.
       * \param split_for_texturing True if the ball's going to be textured.
       */
      void subdivide(unsigned subdivision, bool split_for_texturing = false);

    public:
      /** \brief Add an additional vertex between two existing ones.
       *
       * \param pa Vertex index a.
       * \param pb Vertex index b.
       * \return Newly created vertex index.
       */
      unsigned createVertex(unsigned pa, unsigned pb);
  };
}

#endif
