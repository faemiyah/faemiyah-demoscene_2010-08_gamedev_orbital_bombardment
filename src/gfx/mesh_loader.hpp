#ifndef GFX_MESH_LOADER_HPP
#define GFX_MESH_LOADER_HPP

#include "data/loader_settings.hpp"

namespace gfx
{
  /** \brief Mesh loader settings. */
  class MeshLoader :
    public data::LoaderSettings
  {
    private:
      /** Scale into a boundary area (instead of just normal scale). */
      static const uint32_t SCALE_UNIFORM = 0x1;

      /** Object is a sub-mesh. */
      static const uint32_t SUB_ELEMENT = 0x2;

      /** Center the object. */
      static const uint32_t TRANSLATE_CENTER = 0x4;

    private:
      /** Flat scaling to use. */
      math::vec3f m_scale_flat;

      /** Scale-into to use. */
      math::vec3f m_scale_uniform;

    public:
      /** \brief Default constructor. */
      MeshLoader() :
        m_scale_flat(1.0f, 1.0f, 1.0f),
        m_scale_uniform(0.0f, 0.0f, 0.0f) { }

    public:
      /** \brief Copy this and turn submesh flag on.
       *
       * \return A copy of this with submesh flag set to on.
       */
      MeshLoader asSubMesh() const
      {
        MeshLoader ret(*this);
        ret.setFlag(SUB_ELEMENT);
        return ret;
      }

      /** \brief Get flat scaling.
       *
       * \return Scale factor set.
       */
      const math::vec3f& getScaleFlat() const
      {
        return m_scale_flat;
      }

      /** \brief Get uniform scaling.
       *
       * \return Scale factor set.
       */
      const math::vec3f& getScaleUniform() const
      {
        return m_scale_uniform;
      }

      /** \brief Has uniform scale been set?
       *
       * \return True if yes, false if no.
       */
      bool hasScaleUniform() const
      {
        return this->hasFlag(SCALE_UNIFORM);
      }

      /** \brief Has translate center been set?
       *
       * \return True if yes, false if no.
       */
      bool hasTranslateCenter() const
      {
        return this->hasFlag(TRANSLATE_CENTER);
      }

      /** \brief Tell if this is a sub-element.
       *
       * \return True if yes, false if no.
       */
      bool isSubElement() const
      {
        return this->hasFlag(SUB_ELEMENT);
      }

      /** \brief Set flat scale.
       *
       * Flat scale scales the object by a factor. It can be used to make the object bigger or smaller, but it
       * should be primarily used to deform or 'shape' an object.
       *
       * To fit an object into a certain size, use uniform scale.
       * 
       * \param pscale Scale to use.
       * \return A copy of this with scale set to given value.
       */
      MeshLoader& scale(const math::vec3f &pscale)
      {
        m_scale_flat = pscale;
        return *this;
      }

      /** \brief Set flat scale.
       *
       * \param px X component of scale.
       * \param py Y component of scale.
       * \param pz Z component of scale.
       * \return A copy of this with scale set to given value.
       */
      MeshLoader& scale(float px, float py, float pz)
      {
        return this->scale(math::vec3f(px, py, pz));
      }

      /** \brief Set uniform scale.
       *
       * If uniform scale is set, the mesh is scaled uniformly to not exceed the limits of given boundary. This
       * may replace an existing uniform scale directive already present in the file.
       *
       * Uniform scaling applies after any flat scaling has already been applied.
       *
       * \param pscale Scale to use.
       * \return A copy of this with uniform scale set to given value.
       */
      MeshLoader& scaleUniform(const math::vec3f &pscale)
      {
        m_scale_uniform = pscale;
        this->setFlag(SCALE_UNIFORM);
        return *this;
      }

      /** \brief Set uniform scale.
       *
       * \param px X component of scale.
       * \param py Y component of scale.
       * \param pz Z component of scale.
       * \return A copy of this with uniform scale set to given value.
       */
      MeshLoader& scaleUniform(float px, float py, float pz)
      {
        return this->scaleUniform(math::vec3f(px, py, pz));
      }

      /** \brief Turn centering translation on.
       *
       * \return A copy of this with centering translation turned on.
       */
      MeshLoader& translateCenter()
      {
        this->setFlag(TRANSLATE_CENTER);
        return *this;
      }
  };
}

#endif
