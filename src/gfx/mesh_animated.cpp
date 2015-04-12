#include "gfx/mesh_animated.hpp"

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace gfx;

static data::Registration<Mesh, MeshLoader, MeshAnimated> reg("animated");

MeshAnimated::MeshAnimated(const fs::path &pfname, const pt::ptree &root, const MeshLoader &loader)
{
  this->load(pfname, root, loader);
}

void MeshAnimated::compile()
{
  this->calcNormals();

  m_lod.compile(m_vertex);

  thr::wait_privileged(&Mesh::upload, this);
}

void MeshAnimated::draw(const Shader &pshader, const math::mat4f &pmat) const
{
  boost::ignore_unused_variable_warning(pshader);
  boost::ignore_unused_variable_warning(pmat);

  // TODO: implement
}

void MeshAnimated::unreserve()
{
  m_buf.unreserve();
  this->Mesh::unreserve();
}

void MeshAnimated::upload()
{
  m_buf.upload(m_reference, m_weight, m_texcoord, m_color, m_normal, m_vertex);
  m_elem.upload(m_lod);
}
