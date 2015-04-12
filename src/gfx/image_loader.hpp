#ifndef GFX_IMAGE_LOADER_HPP
#define GFX_IMAGE_LOADER_HPP

#include "data/loader_settings.hpp"

namespace gfx
{
  /** \brief Image loader settings. */
  class ImageLoader :
    public data::LoaderSettings
  {
    private:
      /** Clamp (instead of wrap). */
      static const uint32_t CLAMP = 0x1;

      /** Automatically generate mipmaps whenever content changes? */
      static const uint32_t GENERATE_MIPMAPS = 0x2;

      /** Nearest-neighbor interpolation (as opposed to trilinear). */
      static const uint32_t NEAREST_NEIGHBOR = 0x4;

      /** Premultiply alpha off. */
      static const uint32_t NO_PREMULTIPLY_ALPHA = 0x8;

    private:
      /** Anisotropy requested from the image, default: 2.0f. */
      float m_anisotropy;

    public:
      /** \brief Default constructor. */
      ImageLoader() :
        m_anisotropy(2.0f) { }

    public:
      /** \brief Set anisotropy.
       *
       * \param op New anisotropy.
       * \return Copy of this.
       */
      ImageLoader& anisotropy(float op)
      {
        m_anisotropy = op;
        return *this;
      }

      /** \brief Get anisotropy.
       *
       * \return Anisotropy as float.
       */
      float getAnisotropy() const
      {
        return m_anisotropy;
      }

      /** \brief Is clamp on?
       *
       * \return True if yes, false if no.
       */
      bool hasClamp() const
      {
        return this->hasFlag(CLAMP);
      }

      /** \brief Is generate mipmaps on?
       *
       * \return True if yes, false if no.
       */
      bool hasGenerateMipmaps() const
      {
        return this->hasFlag(GENERATE_MIPMAPS);
      }

      /** \brief Is nearest-neighbor on?
       *
       * \return True if yes, false if no.
       */
      bool hasNearest() const
      {
        return this->hasFlag(NEAREST_NEIGHBOR);
      }

      /** \brief Is premultiply-alpha on?
       *
       * \return True if yes, false if no.
       */
      bool hasPremultiplyAlpha() const
      {
        return !this->hasFlag(NO_PREMULTIPLY_ALPHA);
      }

      /** \brief Is wrap on?
       *
       * \return True if yes, false if no.
       */
      bool hasWrap() const
      {
        return !this->hasFlag(CLAMP);
      }

      /** \brief Turn clamp on.
       *
       * \return A copy of this with clamp turned on.
       */
      ImageLoader& clamp()
      {
        this->setFlag(CLAMP);
        return *this;
      }

      /** \brief Turn generate mipmaps on.
       *
       * \return Reference to this with generate mipmaps turned on.
       */
      ImageLoader& generateMipmaps()
      {
        this->setFlag(GENERATE_MIPMAPS);
        return *this;
      }

      /** \brief Turn nearest-neighbor on.
       *
       * \return Reference to this with nearest-neighbor turned on.
       */
      ImageLoader& nearestNeighbor()
      {
        this->setFlag(NEAREST_NEIGHBOR);
        return *this;
      }

      /** \brief Turn off anisotropy.
       *
       * \return Reference to this with anisotropy turned off.
       */
      ImageLoader& noAnisotropy()
      {
        m_anisotropy = 1.0f;
        return *this;
      }

      /** \brief Turn premultiply alpha off.
       *
       * \return Reference to this with premultiply alpha turned off.
       */
      ImageLoader& noPremultiplyAlpha()
      {
        this->setFlag(NO_PREMULTIPLY_ALPHA);
        return *this;
      }
  };
}

#endif
