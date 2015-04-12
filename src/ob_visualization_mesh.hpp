#ifndef OB_VISUALIZATION_MESH
#define OB_VISUALIZATION_MESH

#include "data/store.hpp"
#include "gfx/mesh.hpp"

namespace ob
{
  class VisualizationMesh :
    public gfx::Mesh
  {
    private:
      /** Array for meshes of this type. */
      gfx::BufferInterleavedTCV m_buf;

    public:
      /** \brief Constructor. */
      VisualizationMesh() { }

      /** \brief Constructor.
       *
       * \param pfname File to load from.
       * \param root Property tree root.
       * \param loader Loader settigns.
       */
      VisualizationMesh(const boost::filesystem::path &pfname, const boost::property_tree::ptree &root,
          const gfx::MeshLoader &loader);

      /** \brief Destructor. */
      virtual ~VisualizationMesh() { }

    public:
      /** \cond */
      virtual void compile();
      virtual void draw(const gfx::Shader &pshader, const math::mat4f &pmat) const;
      virtual void upload();
      /** \endcond */
  };
}

#endif
