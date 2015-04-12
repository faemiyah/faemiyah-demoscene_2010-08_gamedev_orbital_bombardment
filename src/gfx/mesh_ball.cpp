#include "gfx/mesh_ball.hpp"

#include "gfx/shader.hpp"

using namespace gfx;

MeshBall::MeshBall(unsigned subdivision, unsigned subdivision_coalesce)
{
  this->compile(subdivision, subdivision_coalesce);
}

void MeshBall::compile()
{
  this->compile(0);
}

void MeshBall::compile(unsigned subdivision, unsigned subdivision_coalesce)
{
  this->subdivide(subdivision);

  this->coalesce(subdivision_coalesce, subdivision);

  this->calcNormals();

  m_lod.compile(m_vertex);

  thr::wait_privileged(&Mesh::upload, this);
}

void MeshBall::draw(const Shader &psh, const math::mat4f &pmat) const
{
  if(!m_textures.empty())
  {
    m_textures[0]->bind(psh.getUniformTex(), 0);
  }
  m_buf.bind(psh.getAttrTexCoord(), psh.getAttrColor(), psh.getAttrNormal(), psh.getAttrVertex());
  m_elem.bind();
  m_lod.draw(pmat);
}

void MeshBall::unreserve()
{
  m_buf.unreserve();
  this->Mesh::unreserve();
}

void MeshBall::upload()
{
  m_buf.upload(m_texcoord, m_color, m_normal, m_vertex);
  m_elem.upload(m_lod);
}
