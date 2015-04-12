#ifndef GFX_MESH_ANIMATED_HPP
#define GFX_MESH_ANIMATED_HPP

#include "gfx/buffer.hpp"
#include "gfx/mesh.hpp"

namespace gfx
{
  class MeshAnimated : public Mesh
  {
    private:
      /** Buffer for drawing. */
      BufferInterleavedRWTCNV m_buf;

    public:
      /** \brief Empty constructor. */
      MeshAnimated() { }

      /** \brief Load constructor.
       *
       * \param pfname File to load from.
       * \param root Property tree root.
       * \param loader Loader settings.
       */
      MeshAnimated(const boost::filesystem::path &pfname, const boost::property_tree::ptree &root,
          const MeshLoader &loader);

      /** \brief Destructor. */
      virtual ~MeshAnimated() { }

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
