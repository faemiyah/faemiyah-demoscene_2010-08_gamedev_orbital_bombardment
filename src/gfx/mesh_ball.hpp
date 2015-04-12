#ifndef GFX_MESH_BALL_HPP
#define GFX_MESH_BALL_HPP

#include "gfx/mesh_icosahedron.hpp"

namespace gfx
{
  /** \brief Ball mesh.
   *
   * Simple ball.
   *
   * If specified with no subdivision, a ball mesh is actually an icosahedron.
   * most.
   */
  class MeshBall :
    public MeshIcosahedron
  {
    private:
      /** Buffer for drawing. */
      BufferInterleavedTCNV m_buf;

    public:
      /** \brief Constructor.
       *
       * If subdivision count is zero, it will be omitted for no subdivision.
       *
       * \param subdivision Maximum subdivision count.
       * \param subdivision_coalesce Subdivision coalesce count.
       * \param hmap heigthmap to use.
       * \param texture_detail Surface texture detail to use.
       */
      MeshBall(unsigned subdivision, unsigned subdivision_coalesce = 3);

      /** \brief Destructor. */
      virtual ~MeshBall() { }

    public:
      /** \brief Compile this using the given parameters.
       *
       * A normal compilation would just execute this with subdivision set to
       * zero.
       *
       * \param subdivision Maximum subdivision count.
       * \param subdivision_coalesce Subdivision coalesce count.
       * \param hmap heigthmap to use.
       * \param texture_detail Surface texture detail to use.
       */
      void compile(unsigned subdivision, unsigned subdivision_coalesce = 3);

    public:
      /** \cond */
      virtual void compile();
      virtual void draw(const Shader &pshader, const math::mat4f &pmat) const;
      virtual void upload();
      /** \endcond */

    protected:
      /** \cond */
      virtual void unreserve();
      /** \endcond */
  };
}

#endif
