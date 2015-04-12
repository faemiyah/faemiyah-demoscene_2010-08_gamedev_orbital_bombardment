#ifndef OB_PLANET_HPP
#define OB_PLANET_HPP

#include "gfx/mesh_planet.hpp"

namespace ob
{
  class HeightMapPlanet;

  /** Name of the planet (in filesystem). */
  extern const std::string PLANET_FILENAME;

  /** Orbital bombardment planet mesh.
   */
  class Planet :
    public gfx::MeshPlanet
  {
    public:

    public:
      /** \brief Constructor.
       *
       * \param ps Subdivide.
       * \param pc Coalesce.
       * \param pt Texture size.
       * \param pv Volume size.
       */
      Planet(unsigned ps, unsigned pc, unsigned pt, unsigned pv, const HeightMapPlanet *hmap = NULL,
          bool generate = false);

      /** \brief Destructor. */
      ~Planet() { }

    public:
      /** \cond */
      virtual void createVolumes(const std::string &fname_header, unsigned vside, bool psave = false);
      virtual void draw(const gfx::Shader &psh, const math::mat4f &op) const;
      /** \endcond */
  };
}

#endif
