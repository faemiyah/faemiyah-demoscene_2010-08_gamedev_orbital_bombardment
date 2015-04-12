#ifndef GFX_MESH_PLANET_HPP
#define GFX_MESH_PLANET_HPP

#include "gfx/mesh_icosahedron.hpp"
#include "gfx/texture_3d.hpp"

namespace gfx
{
  class HeightMapBall;

  /** \brief Planet mesh.
   *
   * Actually, this represents an icosahedron ball applicable to complex
   * planet-like objects. An ordinary icosahedron would be a corner case at
   * most.
   *
   * Height volumes:
   * 0 - water
   * 1 - lowlands
   * 2 - mountains
   *
   * Latitude volumes:
   * 3 - arctic
   * 4 - desert
   */
  class MeshPlanet :
    public MeshIcosahedron
  {
    protected:
      /** Buffer for drawing. */
      BufferInterleavedTV m_buf;

    public:
      /** \brief Empty constructor.
       *
       * To be called from inheriting classes that will call the compilation
       * themselves.
       */
      MeshPlanet() { }

      /** \brief Constructor.
       *
       * If subdivision count is zero, it will be omitted for no subdivision.
       *
       * \param subdivision Maximum subdivision count.
       * \param subdivision_coalesce Subdivision coalesce count.
       * \param hmap heigthmap to use.
       * \param texture_detail Surface texture detail to use.
       */
      MeshPlanet(unsigned subdivision, unsigned subdivision_coalesce = 3,
          const HeightMapBall *hmap = NULL, unsigned texture_detail = 256);

      /** \brief Destructor. */
      virtual ~MeshPlanet() { }

    public:
      /** \brief Compile this using the given parameters.
       *
       * A normal compilation would just execute this with subdivision set to zero.
       *
       * \param subdivision Maximum subdivision count.
       * \param subdivision_coalesce Subdivision coalesce count.
       * \param hmap heigthmap to use.
       * \param texture_detail Surface texture detail to use.
       * \param volume_detail Volumetric texture detail to use.
       * \param fname_header Prefix for filenames used for loading.
       * \param psave True to save textures after generation.
       */
      void compile(unsigned subdivision, unsigned subdivision_coalesce = 3, const HeightMapBall *hmap = NULL,
          unsigned texture_detail = 256, unsigned volume_detail = 128,
          const std::string &fname_header = std::string(), bool psave = false);

    protected:
      /** \brief Create volumetric textures.
       *
       * Create the volumetric textures to be used when mapping over the
       * surface.
       *
       * \param fname_header Prefix for filenames used for loading.
       * \param vside Volume side length (default: 128).
       * \param psave True to save textures after generation.
       */
      virtual void createVolumes(const std::string &fname_header, unsigned vside,
          bool psave = false) = 0;

    public:
      /** \cond */
      virtual void compile();
      virtual void upload();
      /** \endcond */

    protected:
      /** \cond */
      virtual void unreserve();
      /** \endcond */
  };
}

#endif
