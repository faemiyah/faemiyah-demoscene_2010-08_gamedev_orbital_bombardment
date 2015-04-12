#include "ob_visualization_nuke.hpp"

#include "thr/dispatch.hpp"
#include "ob_constants.hpp"

using namespace ob;

/** Pillar width (relative). */
static const float PILLAR_RADIUS = 0.2f;

/** Pillar texture loop. */
static const float RADIUS_TEXTURE_MUL = 2.0f;

/** Pillar texture loop. */
static const float PILLAR_TEXTURE_MUL = 8.0f;

/** Circle detail. */
static const unsigned NV_DETAIL = 24;

VisualizationNuke::VisualizationNuke()
{
  for(unsigned ii = 0; (ii <= NV_DETAIL); ++ii)
  {
    float fi = static_cast<float>(ii) / static_cast<float>(NV_DETAIL);

    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));
    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));
    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 0.0f));
    m_texcoord.push_back(math::vec2f(fi, 0.0f));
    m_texcoord.push_back(math::vec2f(fi, RADIUS_TEXTURE_MUL));
    m_texcoord.push_back(math::vec2f(fi, RADIUS_TEXTURE_MUL + PILLAR_TEXTURE_MUL));

    float cr = math::cos(fi * static_cast<float>(M_PI) * 2.0f),
          sr = math::sin(fi * static_cast<float>(M_PI) * 2.0f),
          r1 = OB_NUKE_MARKER_RADIUS,
          r2 = OB_NUKE_MARKER_RADIUS * PILLAR_RADIUS;
    math::vec3f rr(cr, sr, 0.0f);

    m_vertex.push_back(rr * r1);
    m_vertex.push_back(rr * r2);
    m_vertex.push_back(math::vec3f(0.0f, 0.0f, OB_NUKE_MARKER_HEIGHT));
  }

  for(unsigned ii = 0; (ii < NV_DETAIL * 3); ii += 3)
  {
    m_lod.addFace(ii + 0, ii + 3, ii + 1);
    m_lod.addFace(ii + 1, ii + 3, ii + 4);
    m_lod.addFace(ii + 1, ii + 4, ii + 2);
    m_lod.addFace(ii + 2, ii + 4, ii + 5);
  }

  this->addTextureFile(std::string("texture"),
      std::string("gfx/textures/texture_nuke_stripe.png"));
  this->compile();
}

