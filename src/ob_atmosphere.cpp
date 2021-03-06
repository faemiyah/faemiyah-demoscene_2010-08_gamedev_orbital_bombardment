#include "ob_atmosphere.hpp"

#include "data/generic.hpp"
#include "gfx/shader.hpp"
#include "thr/dispatch.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** Atmosphere radius (km). */
static const float OB_ATMOSPHERE_RADIUS = OB_PLANET_RADIUS + 250.0f;

/** Atmosphere mesh color. */
static const gfx::Color ATMOSPHERE_COLOR(1.0f, 0.9f, 0.2f, 0.2f);

Atmosphere::Atmosphere(unsigned subdivision, unsigned subdivision_coalesce)
{
  this->compile(subdivision, subdivision_coalesce);
}

void Atmosphere::compile()
{
  this->compile(0);
}

void Atmosphere::compile(unsigned subdivision, unsigned subdivision_coalesce)
{
  this->subdivide(subdivision);

  this->coalesce(subdivision_coalesce, subdivision);

  this->scale(OB_ATMOSPHERE_RADIUS);
  BOOST_FOREACH(gfx::Color &vv, m_color)
  {
    vv = ATMOSPHERE_COLOR;
  }

  this->calcNormals();

  // Generated by icosahedron mesh, but not needed.
  m_texcoord.clear();
  data::stl_trim(m_texcoord);

  m_lod.compile(m_vertex);

  thr::wait_privileged(&Mesh::upload, this);
}

void Atmosphere::draw(const gfx::Shader &psh, const math::mat4f &pmat) const
{
  psh.getUniformLightDiffuse().update(game->getLightDiffuse());
  psh.getUniformLightDir().update(math::normalize(-(pmat * game->getLightDir())));
  psh.getUniform("atmosphere_step")->update(0.04f, 0.28f, 0.96f);

  m_buf.bind(psh.getAttrColor(), psh.getAttrNormal(), psh.getAttrVertex());
  m_elem.bind();
  m_lod.draw(pmat);
}

void Atmosphere::unreserve()
{
  m_buf.unreserve();
  this->Mesh::unreserve();
}

void Atmosphere::upload()
{
  m_buf.upload(m_color, m_normal, m_vertex);
  m_elem.upload(m_lod);
}
