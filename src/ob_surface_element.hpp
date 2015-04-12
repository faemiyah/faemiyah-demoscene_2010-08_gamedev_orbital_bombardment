#ifndef OB_SURFACE_ELEMENT_HPP
#define OB_SURFACE_ELEMENT_HPP

#include "math/mat.hpp"

namespace ob
{
  class HeightMapPlanet;

  /** \brief Element on planet surface.
   *
   * Carries very little functionality per se.
   */
  class SurfaceElement
  {
    protected:
      /** \brief Check if this should be processed.
       *
       * \param len2 Length squared;
       * \return True if yes, false if no.
       */
      static bool is_surface_processible(float len2);

      /** \brief Check if this should be visible.
       *
       * \param len2 Length squared;
       * \return True if yes, false if no.
       */
      static bool is_surface_visible(float len2);

      /** \brief Generate a lookat matrix for an element on surface.
       *
       * \param pos Position on surface.
       * \param ht How much to raise the element off the surface.
       * @dir Direction to look at.
       */
      static math::mat4f lookat_on_surface(const math::vec3d &pos, float ht,
          const math::vec3d &dir);

      /** \brief Generate a lookat matrix for an element on surface.
       *
       * Looks at random direction.
       *
       * \param pos Position on surface.
       * \param ht How much to raise the element off the surface.
       */
      static math::mat4f lookat_on_surface(const math::vec3d &pos, float ht);

      /** \brief Randomize a position on the planet that is not in water.
       *
       * \param hmap Height map to randomize from.
       * \return A position on planet surface.
       */
      static math::vec3d randomize_pos_on_surface(const HeightMapPlanet &hmap);

      /** \brief Randomize a position on the planet that is not in water.
       *
       * Uses a base position to differentiate from.
       *
       * \param hmap Height map to randomize from.
       * \param base_position Differentiate from this position.
       * \param offset Initial offset to start randomizing out of.
       * \return A position on planet surface.
       */
      static math::vec3d randomize_pos_on_surface(const HeightMapPlanet &hmap,
          const math::vec3d &base_position, float offset);
  };
}

#endif
