#ifndef GFX_MESH_STATIC_HPP
#define GFX_MESH_STATIC_HPP

#include "gfx/buffer.hpp"
#include "gfx/mesh.hpp"

namespace gfx
{
  /** \brief Static mesh.
   *
   * i.e. a mesh which does not animate.
   */
  class MeshStatic :
    public Mesh
  {
    protected:
      /** Buffer for drawing. */
      BufferInterleavedTCNV m_buf;

    public:
      /** \brief Empty constructor. */
      MeshStatic() { }

      /** \brief Load constructor.
       *
       * \param pfname File to load from.
       * \param root Property tree root.
       * \param loader Loader settigns.
       */
      MeshStatic(const boost::filesystem::path &pfname, const boost::property_tree::ptree &root,
          const MeshLoader &loader);

      /** \brief Destructor. */
      virtual ~MeshStatic() { }

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
