#include "ob_visualization_distort.hpp"

#include "thr/dispatch.hpp"
#include "ob_constants.hpp"

using namespace ob;

/** Maximum size of sun billboards. */
static const float DISTORT_BILLBOARD_SIZE = 1600.0f;

/** Sun color. */
static const gfx::Color DISTORT_COLOR(0.65f, 0.6f, 1.0f, 1.0f);

VisualizationDistort::VisualizationDistort()
{
  m_color.push_back(DISTORT_COLOR);
  m_color.push_back(DISTORT_COLOR);
  m_color.push_back(DISTORT_COLOR);
  m_color.push_back(DISTORT_COLOR);

  m_texcoord.push_back(math::vec2f(0.0f, 0.0f));
  m_texcoord.push_back(math::vec2f(1.0f, 0.0f));
  m_texcoord.push_back(math::vec2f(1.0f, 1.0f));
  m_texcoord.push_back(math::vec2f(0.0f, 1.0f));

  m_vertex.push_back(math::vec3f(-DISTORT_BILLBOARD_SIZE, -DISTORT_BILLBOARD_SIZE, 0.0f));
  m_vertex.push_back(math::vec3f(DISTORT_BILLBOARD_SIZE, -DISTORT_BILLBOARD_SIZE, 0.0f));
  m_vertex.push_back(math::vec3f(DISTORT_BILLBOARD_SIZE, DISTORT_BILLBOARD_SIZE, 0.0f));
  m_vertex.push_back(math::vec3f(-DISTORT_BILLBOARD_SIZE, DISTORT_BILLBOARD_SIZE, 0.0f));

  m_lod.addFace(0, 1, 2);
  m_lod.addFace(2, 3, 0);

  this->addTextureFile(std::string("texture"),
      std::string("gfx/textures/texture_distort.png"));
  this->compile();
}

/*void VisualizationSun::draw(const gfx::Shader &pshader,
    const math::mat4f &pmat) const
{
  pshader.getUniform("sun_params")->update(0.1f, 0.1f, 0.1f, 0.5f); //static_cast<float>(UiStack::getCurrentFrame()%500)/500.0f);

  this->VisualizationMesh::draw(pshader, pmat);
}*/

