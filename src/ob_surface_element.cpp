#include "ob_surface_element.hpp"

#include "math/random.hpp"
#include "ob_constants.hpp"
#include "ob_height_map_planet.hpp"

using namespace ob;

/** Objects farther than this will not be drawn. */
static const float SURFACE_VISIBILITY = 5000.0;

/** Objects farther than this will not be processed. */
static const float SURFACE_PROCESSIBILITY = 7000.0f;

/** \brief Offset escape geometric progression.
 *
 * Should be large enough not to loop forever, but also small enough not to
 * escape to infinity too quickly. */
static const float OFFSET_ESCAPE_SPEED = 1.06f;

bool SurfaceElement::is_surface_processible(float len2)
{
  return (len2 <= SURFACE_PROCESSIBILITY * SURFACE_PROCESSIBILITY);
}

bool SurfaceElement::is_surface_visible(float len2)
{
  return (len2 <= SURFACE_VISIBILITY * SURFACE_VISIBILITY);
}

math::mat4f SurfaceElement::lookat_on_surface(const math::vec3d &pos,
    float ht, const math::vec3d &dir)
{
  math::vec3f fpos(pos);
  math::mat4f ret;
  ret.loadLookAt(fpos + math::normalize(fpos) * ht,
      math::vec3f(0.0f, 0.0f, 0.0f),
      math::vec3f(dir));
  return ret;
}

math::mat4f SurfaceElement::lookat_on_surface(const math::vec3d &pos,
    float ht)
{
  return lookat_on_surface(pos, ht,
      math::vec3d(math::mrand(-1.0, 1.0),
        math::mrand(-1.0, 1.0),
        math::mrand(-1.0, 1.0)));
}

math::vec3d SurfaceElement::randomize_pos_on_surface(
    const HeightMapPlanet &hmap)
{
  // Randomize until on land.
  for(;;)
  {
    math::vec3f candidate(math::mrand(-1.0f, 1.0f),
        math::mrand(-1.0f, 1.0f),
        math::mrand(-1.0f, 1.0f));
    float ht = hmap.calcHeight(candidate);
    
    if(ht > OB_TERRAIN_LEVEL)
    {
      return math::vec3d(hmap.normalizeHeight(candidate, ht));
    }  
  }
}

math::vec3d SurfaceElement::randomize_pos_on_surface(
    const HeightMapPlanet &hmap, const math::vec3d &base_position,
    float offset)
{
  // Randomize until on land.
  for(;; offset *= OFFSET_ESCAPE_SPEED)
  {
    math::vec3d diffvec(math::mrand(-1.0, 1.0),
        math::mrand(-1.0, 1.0),
        math::mrand(-1.0, 1.0));
    math::vec3f candidate(base_position + 
        math::normalize(diffvec) *
        math::mrand(static_cast<double>(0.0), static_cast<double>(offset)));
    float ht = hmap.calcHeight(candidate);
    
    if(ht > OB_TERRAIN_LEVEL)
    {
      return math::vec3d(hmap.normalizeHeight(candidate, ht));
    }
  }
}

