#include "ob_visualization_railgun.hpp"

#include "thr/dispatch.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** Bullet + trail length (in km). */
static const float BULLET_LENGTH = 55.0f;

/** Bullet + trail width (km). */
static const float BULLET_WIDTH = 0.12f;

/** How many ? of the bullet is in front as opposed to back. */
static const float BULLET_HEADER = 0.1f;

/** How many ? of the bullet is in front as opposed to back. */
static const float BULLET_MASS_CENTER = 0.89f;

/** Facet count. */
static const unsigned FACET_COUNT = 7;

VisualizationRailgun::VisualizationRailgun()
{
  for(unsigned ii = 0; (ii <= FACET_COUNT); ++ii)
  {
    float fi = static_cast<float>(ii) / static_cast<float>(FACET_COUNT);

    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));
    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));
    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));

    m_texcoord.push_back(math::vec2f(0.0f, fi));
    m_texcoord.push_back(math::vec2f(BULLET_HEADER, fi));
    m_texcoord.push_back(math::vec2f(1.0f, fi));

    float pi = fi * static_cast<float>(M_PI) * 2.0f;
    float ci = math::cos(pi) * BULLET_WIDTH,
          si = math::sin(pi) * BULLET_WIDTH;

    m_vertex.push_back(math::vec3f(0.0f, 0.0f, -BULLET_LENGTH * BULLET_MASS_CENTER));
    m_vertex.push_back(math::vec3f(ci, si, BULLET_LENGTH * BULLET_HEADER - BULLET_LENGTH * BULLET_MASS_CENTER));
    m_vertex.push_back(math::vec3f(0.0f, 0.0f, BULLET_LENGTH * (1.0f - BULLET_MASS_CENTER)));

    if(ii > 0)
    {
      unsigned idx = ii * 3;
      m_lod.addFace(idx + 0, idx + 1, idx - 3);
      m_lod.addFace(idx + 1, idx - 3, idx - 2);
      m_lod.addFace(idx + 1, idx + 2, idx - 2);
      m_lod.addFace(idx + 2, idx - 2, idx - 1);
    }
  }

  this->addTextureFile(std::string("texture"),
      std::string("gfx/textures/texture_railgun.png"));
  this->compile();
}

