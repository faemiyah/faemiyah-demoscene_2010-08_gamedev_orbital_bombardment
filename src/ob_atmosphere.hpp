#ifndef OB_ATMOSPHERE_HPP
#define OB_ATMOSPHERE_HPP

#include "gfx/mesh_icosahedron.hpp"

namespace ob
{
  /** Atmosphere overlay for the planet.
   */
   class Atmosphere :
     public gfx::MeshIcosahedron
   {
    private:
      /** Buffer for drawing. */
      gfx::BufferInterleavedCNV m_buf;

    public:
      /** \brief Constructor.
       *
       * If subdivision count is zero, it will be omitted for no subdivision.
       *
       * \param subdivision Maximum subdivision count.
       * \param subdivision_coalesce Subdivision coalesce count.
       * \param hmap heigthmap to use.
       * \param texture_detail Surface texture detail to use.
       */
      Atmosphere(unsigned subdivision, unsigned subdivision_coalesce = 3);

      /** \brief Destructor. */
      virtual ~Atmosphere() { }

    public:
      /** \brief Compile this using the given parameters.
       *
       * A normal compilation would just execute this with subdivision set to
       * zero.
       *
       * \param subdivision Maximum subdivision count.
       * \param subdivision_coalesce Subdivision coalesce count.
       * \param hmap heigthmap to use.
       * \param texture_detail Surface texture detail to use.
       */
      void compile(unsigned subdivision, unsigned subdivision_coalesce = 3);

    public:
      /** \cond */
      virtual void draw(const gfx::Shader &psh, const math::mat4f &op) const;
      /** \endcond */

    protected:
      /** \cond */
      virtual void compile();
      virtual void unreserve();
      virtual void upload();
      /** \endcond */
   };
}

#endif
