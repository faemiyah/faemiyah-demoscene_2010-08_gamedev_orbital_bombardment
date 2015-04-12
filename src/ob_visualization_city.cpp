#include "ob_visualization_city.hpp"

#include "thr/dispatch.hpp"
#include "ob_constants.hpp"

using namespace ob;

/** Size of one strip (as percentage of whole city size. */
static const float STRIP_WIDTH = 0.24f;

/** City circle detail. */
static const unsigned CV_DETAIL = 24;

/** Marker distance (from surface) for cities. */
static const float OB_CITY_SIZE = 120.0;

VisualizationCity::VisualizationCity()
{
#if 1
  for(unsigned ii = 0; (ii <= CV_DETAIL); ++ii)
  {
    float fi = static_cast<float>(ii) / static_cast<float>(CV_DETAIL),
          ti = fi * 4.0f;

    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));
    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));
    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));
    m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));
    m_texcoord.push_back(math::vec2f(0.0f, ti));
    m_texcoord.push_back(math::vec2f(1.0f, ti));
    m_texcoord.push_back(math::vec2f(0.0f, 1.0f - ti));
    m_texcoord.push_back(math::vec2f(1.0f, 1.0f - ti));

    float cr = math::cos(fi * static_cast<float>(M_PI) * 2.0f),
          sr = math::sin(fi * static_cast<float>(M_PI) * 2.0f),
          r1 = OB_CITY_SIZE,
          r2 = OB_CITY_SIZE * (1.0f - STRIP_WIDTH),
          r3 = OB_CITY_SIZE * (1.0f - STRIP_WIDTH),
          r4 = OB_CITY_SIZE * (1.0f - STRIP_WIDTH * 2.0f);
    math::vec3f rr(cr, sr, 0.0f);

    m_vertex.push_back(rr * r1);
    m_vertex.push_back(rr * r2);
    m_vertex.push_back(rr * r3);
    m_vertex.push_back(rr * r4);
  }

  for(unsigned ii = 0; (ii < CV_DETAIL * 4); ii += 4)
  {
    m_lod.addFace(ii + 0, ii + 4, ii + 1);
    m_lod.addFace(ii + 1, ii + 4, ii + 5);
    m_lod.addFace(ii + 2, ii + 6, ii + 3);
    m_lod.addFace(ii + 3, ii + 6, ii + 7);
  }
#else
  float r1 = OB_CITY_SIZE,
        r2 = OB_CITY_SIZE * (1.0f - STRIP_WIDTH);
  m_vertex.push_back(math::vec3f(-r1, -r1, 0.0f));
  m_texcoord.push_back(math::vec2f(0.0f, 0.0f));
  m_vertex.push_back(math::vec3f(r1, -r1, 0.0f));
  m_texcoord.push_back(math::vec2f(1.0f, 0.0f));
  m_vertex.push_back(math::vec3f(r1, r1, 0.0f));
  m_texcoord.push_back(math::vec2f(1.0f, 1.0f));
  m_vertex.push_back(math::vec3f(-r1, r1, 0.0f));
  m_texcoord.push_back(math::vec2f(0.0f, 1.0f));
  /*m_vertex.push_back(math::vec3f(-r2, -r2, 1.0f));
  m_texcoord.push_back(math::vec2f(-1.0f, -1.0f));
  m_vertex.push_back(math::vec3f(r2, -r2, 1.0f));
  m_texcoord.push_back(math::vec2f(0.0f, -1.0f));
  m_vertex.push_back(math::vec3f(r2, r2, 1.0f));
  m_texcoord.push_back(math::vec2f(0.0f, 0.0f));
  m_vertex.push_back(math::vec3f(-r2, r2, 1.0f));
  m_texcoord.push_back(math::vec2f(-1.0f, 0.0f));*/
  m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, STRIP_ALPHA));
  m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, STRIP_ALPHA));
  m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, STRIP_ALPHA));
  m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, STRIP_ALPHA));
  /*m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, STRIP_ALPHA));
  m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, STRIP_ALPHA));
  m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, STRIP_ALPHA));
  m_color.push_back(gfx::Color(1.0f, 1.0f, 1.0f, STRIP_ALPHA));*/
  m_lod.addFace(0, 1, 2);
  m_lod.addFace(2, 3, 0);
  m_lod.addFace(4, 5, 6);
  m_lod.addFace(6, 7, 4);
#endif

  this->addTextureFile(std::string("texture"),
      std::string("gfx/textures/texture_city_stripe.png"));
  this->compile();
}

