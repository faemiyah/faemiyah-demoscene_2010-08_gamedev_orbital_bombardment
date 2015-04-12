#include "ob_visualization_mesh.hpp"

#include "gfx/shader.hpp"
#include "thr/dispatch.hpp"

namespace fs = boost::filesystem;
using namespace ob;

static data::Registration<gfx::Mesh, gfx::MeshLoader, VisualizationMesh> reg("ob_visualization");

VisualizationMesh::VisualizationMesh(const boost::filesystem::path &pfname,
    const boost::property_tree::ptree &root, const gfx::MeshLoader &loader)
{
  this->load(pfname, root, loader);
}

void VisualizationMesh::compile()
{
  m_lod.compile(m_vertex);

  thr::wait_privileged(&Mesh::upload, this);
}

void VisualizationMesh::draw(const gfx::Shader &psh, const math::mat4f &pmat) const
{
  if(!m_textures.empty())
  {
    m_textures[0]->bind(psh.getUniformTex(), 0);
  }

  m_buf.bind(psh.getAttrTexCoord(), psh.getAttrColor(), psh.getAttrVertex());
  m_elem.bind();
  m_lod.draw(pmat);
}

void VisualizationMesh::upload()
{
  m_buf.upload(m_texcoord, m_color, m_vertex);
  m_elem.upload(m_lod);
}

