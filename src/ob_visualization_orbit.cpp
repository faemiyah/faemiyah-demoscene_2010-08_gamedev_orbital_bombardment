#include "ob_visualization_orbit.hpp"

#include "thr/dispatch.hpp"
#include "ob_constants.hpp"

using namespace ob;

/** Orbit visualization arc (in radians). */
static const float OV_ARC = 0.8f;

/** Height of orbit visualization. */
static const float OV_ORBIT = 1.0f + OB_PLANET_RADIUS_DIFF * 1.01f;

/** Texture loop count. */
static const float OV_TEX_LOOP = 32.0f;

/** Width of orbit visualization. */
static const float OV_WIDTH = 0.02f;

/** Number of segments in the orbit visualization. */
static const unsigned OV_DETAIL = 32;

VisualizationOrbit::VisualizationOrbit()
{
  float fdetail = static_cast<float>(OV_DETAIL - 1),
        aiter = -OV_ARC,
        ainc = (OV_ARC * 2.0f) / fdetail;

  for(unsigned ii = 0; (ii < OV_DETAIL); ++ii, aiter += ainc)
  {
    float vy = math::sin(aiter) * OB_PLANET_RADIUS * OV_ORBIT,
          vz = math::cos(aiter) * OB_PLANET_RADIUS * OV_ORBIT,
          tt = static_cast<float>(ii) / fdetail,
          alpha = ((tt < 0.5f) ? (tt * 2.0f) : (2.0f - (tt * 2.0f))) * 0.5f;

    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, alpha));
    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, alpha));
    m_texcoord.push_back(math::vec2f(0.0f, tt * OV_TEX_LOOP));
    m_texcoord.push_back(math::vec2f(1.0f, tt * OV_TEX_LOOP));
    m_vertex.push_back(math::vec3f(-OB_PLANET_RADIUS * OV_WIDTH, vy, vz));
    m_vertex.push_back(math::vec3f(OB_PLANET_RADIUS * OV_WIDTH, vy, vz));
  }

  for(unsigned ii = 0; (ii < OV_DETAIL * 2 - 2); ii += 2)
  {
    m_lod.addFace(ii, ii + 1, ii + 2);
    m_lod.addFace(ii + 1, ii + 3, ii + 2);
  }

  this->addTextureFile(std::string("texture"), std::string("gfx/textures/texture_arrow_stripe.png"));
  this->compile();
}

