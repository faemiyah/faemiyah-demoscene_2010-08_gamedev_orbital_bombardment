#ifndef OB_POPULATION_MAP_HPP
#define OB_POPULATION_MAP_HPP

#include "gfx/texture_3d.hpp"
#include "gfx/volume.hpp"
#include "math/rect.hpp"

namespace gfx
{
  class Shader;
}

namespace ob
{
  /** \brief Population map in 3D.
  */
  class PopulationMap :
    public gfx::VolumeGray16
  {
    private:
      /** Current population (color units). */
      unsigned m_population;

      /** 3D texture associated with this. */
      gfx::Texture3DSptr m_texture;

    public:
      /** \brief Accessor.
       *
       * \return Current population.
       */
      unsigned getPopulation() const
      {
        return m_population;
      }

      /** \brief Accessor.
       *
       * \return Texture of this.
       */
      const gfx::Texture3D& getTexture() const
      {
        return *m_texture;
      }

    public:
      /** \brief Constructor. */
      PopulationMap();

      /** \brief Destructor. */
      virtual ~PopulationMap();

    private:
      /** Texture task.
       *
       * \param vol Volume to pass (this or NULL).
       */
      void taskTexture(const gfx::VolumeGray16 *vol);

      /** Sub-texture task.
       *
       * \param data Data to update.
       * \param idx Update index.
       * \param size Update size.
       */
      void taskSubTexture(const uint8_t *data, const math::vec3i &idx,
          const math::vec3i &size);

    public:
      /** \brief Feed into shader.
       *
       * \param sh Shader.
       * \param idx Texture unit to bind to.
       */
      void feed(const gfx::Shader &sh, unsigned idx) const;

      /** \brief Filter this.
      */
      void filter();

      /** \brief Collect average value from around a certain pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param pz Z coordinate.
       * \return Average value.
       */
      uint8_t filterCollect(unsigned px, unsigned py, unsigned pz);

      /** \brief Get a value from given coordinates or negative.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param pz Z coordinate.
       * \return Value here or negative.
       */
      int filterGet(unsigned px, unsigned py, unsigned pz);

      /** \brief Paint into the population map.
       *
       * Each paint operation erases as much population as it adds rubble, but
       * not vice versa.
       *
       * To paint population, str is positive.
       *
       * To paint rubble, str is negative.
       *
       * You should not need greater paint values than 1.0.
       *
       * \param pos 3D position to paint into.
       * \param str Brush strength.
       * \param update Set to true to immediately update the sub-texture.
       * \return Amount of population strength removed in total.
       */
      int paint(const math::vec3f &pos, float str, bool update = false);

      /** \brief Scale all population values.
       *
       * \param op Factor.
       */
      void scale(float op);

      /** \brief Refresh the texture in this.
      */
      void refresh();

    public:
      /** \brief Clears this.
      */
      void clear()
      {
        m_population = 0;
        this->gfx::Volume::clear(static_cast<uint8_t>(0));
      }
  };
}

#endif
