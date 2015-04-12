#include "ob_visualization_sun.hpp"

#include "math/random.hpp"
#include "thr/dispatch.hpp"
#include "ob_constants.hpp"

using namespace ob;

/** Maximum size of sun billboards. */
static const float SUN_BILLBOARD_MAX_SIZE = 120.0f;

/** Minimum size of sun billboards. */
static const float SUN_BILLBOARD_MIN_SIZE = 60.0f;

/** Sun billboard count. */
static const unsigned SUN_BILLBOARD_COUNT = 1;

/** Sun color. */
static const gfx::Color SUN_COLOR(1.0f, 1.0f, 0.28f, 1.0f);

VisualizationSun::VisualizationSun()
{
  unsigned ii = 0;

  for(float rr = SUN_BILLBOARD_MAX_SIZE; rr>1.0f; rr-=SUN_BILLBOARD_MIN_SIZE)
  {
    m_color.push_back(SUN_COLOR);
    m_color.push_back(SUN_COLOR);
    m_color.push_back(SUN_COLOR);
    m_color.push_back(SUN_COLOR);

    if(math::mrand(0, 1))
    {
      m_texcoord.push_back(math::vec2f(0.0f, 0.0f));
      m_texcoord.push_back(math::vec2f(1.0f, 0.0f));
      m_texcoord.push_back(math::vec2f(1.0f, 1.0f));
      m_texcoord.push_back(math::vec2f(0.0f, 1.0f));
    }
    else
    {
      m_texcoord.push_back(math::vec2f(0.0f, 1.0f));
      m_texcoord.push_back(math::vec2f(1.0f, 1.0f));
      m_texcoord.push_back(math::vec2f(1.0f, 0.0f));
      m_texcoord.push_back(math::vec2f(0.0f, 0.0f));
    }

    float rad_offset = math::mrand(0.0f, static_cast<float>(M_PI * 2.0f));
    for(unsigned jj = 0; (jj < 4); ++jj)
    {
      float rad = (static_cast<float>(jj) / 4.0f) * static_cast<float>(M_PI) * 2.0f;
      float cr = math::cos(rad + rad_offset),
            sr = math::sin(rad + rad_offset);
      m_vertex.push_back(math::vec3f(rr * cr, rr * sr, 0.0f));
    }

    unsigned idx = ii * 4;
    m_lod.addFace(idx + 0, idx + 1, idx + 2);
    m_lod.addFace(idx + 2, idx + 3, idx + 0);
    ++ii;
  }

  this->addTextureFile(std::string("texture"),
      std::string("gfx/textures/texture_sun.png"));
  this->compile();
}

/*void VisualizationSun::draw(const gfx::Shader &pshader,
  const math::mat4f &pmat) const
  {
  pshader.getUniform("sun_params")->update(0.1f, 0.1f, 0.1f, 0.5f); //static_cast<float>(UiStack::getCurrentFrame()%500)/500.0f);

  this->VisualizationMesh::draw(pshader, pmat);
  }*/

