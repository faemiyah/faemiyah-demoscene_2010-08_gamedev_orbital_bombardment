#include "gfx/mesh_static.hpp"

#include "gfx/shader.hpp"
#include "thr/dispatch.hpp"

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace gfx;

static data::Registration<Mesh, MeshLoader, MeshStatic> reg("static");

MeshStatic::MeshStatic(const fs::path &pfname, const pt::ptree &root, const MeshLoader &loader)
{
  this->load(pfname, root, loader);
}

void MeshStatic::compile()
{
  // texcoord array may be uninitialized
  if(m_texcoord.size() != m_vertex.size())
  {
    m_texcoord.resize(m_vertex.size(), math::vec2f(0.0f, 0.0f));
  }

  this->calcNormals();

  m_lod.compile(m_vertex);

  thr::wait_privileged(&Mesh::upload, this);
}

void MeshStatic::draw(const Shader &psh, const math::mat4f &pmat) const
{
  if(!m_textures.empty())
  {
    m_textures[0]->bind(psh.getUniformTex(), 0);
  }
  m_buf.bind(psh.getAttrTexCoord(), psh.getAttrColor(), psh.getAttrNormal(), psh.getAttrVertex());
  m_elem.bind();
  m_lod.draw(pmat);
}

void MeshStatic::unreserve()
{
  m_buf.unreserve();
  this->Mesh::unreserve();
}

void MeshStatic::upload()
{
  m_buf.upload(m_texcoord, m_color, m_normal, m_vertex);
  m_elem.upload(m_lod);
}

