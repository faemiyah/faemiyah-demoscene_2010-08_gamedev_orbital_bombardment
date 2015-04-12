#ifndef GFX_TEXTURE_3D_HPP
#define GFX_TEXTURE_3D_HPP

#include "data/store.hpp"
#include "gfx/image_loader.hpp"
#include "gfx/volume_base.hpp"
#include "gfx/texture.hpp"

namespace gfx
{
  class VolumeGray8;
  class VolumeGray16;
  class VolumeRGB;
  class VolumeRGBA;

  /** \brief 3D texture construct.
   *
   * 3D volumetric texture.
   *
   * Slight overlap with 2D texturing accounts for some dubious terminology.
   * 
   * As per OpenGL coordinate system, the texture S coordinate is X, T is Y
   * and R is Z. In practice, this means that SR is the plane in texture space
   * just as XZ is the plane in the world space. However, due to the way
   * texture information is stored, the data block to be passed is specified
   * S first, then T, then R.
   */
  class Texture3D :
    public Texture<GL_TEXTURE_3D>,
    public VolumeBase,
    public data::Storable<Texture3D, ImageLoader>
  {
    public:
      /** \brief Empty constructor. */
      Texture3D() { }

      /** \brief Constructor.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       * \param pb Bit depth.
       */
      Texture3D(unsigned pw, unsigned ph, unsigned pd, unsigned pb) :
        VolumeBase(pw, ph, pd, pb) { }

      /** \brief Constructor.
       *
       * Immediately adapts data from given block.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       * \param pb Bit depth.
       * \param pdata Data to adapt.
       * \param loader Loader settings.
       */
      Texture3D(unsigned pw, unsigned ph, unsigned pd, unsigned pb, const void *pdata,
          const ImageLoader &loader = ImageLoader());

      /** \brief Load constructor.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      Texture3D(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Constructor.
       *
       * \param volume Volume to adapt.
       * \param loader Loader settings.
       */
      Texture3D(const VolumeGray8 &volume, const ImageLoader &loader = ImageLoader());

      /** \brief Constructor.
       *
       * \param volume Volume to adapt.
       * \param loader Loader settings.
       */
      Texture3D(const VolumeGray16 &volume, const ImageLoader &loader = ImageLoader());

      /** \brief Constructor.
       *
       * \param volume Volume to adapt.
       * \param loader Loader settings.
       */
      Texture3D(const VolumeRGB &volume, const ImageLoader &loader = ImageLoader());

      /** \brief Constructor.
       *
       * \param volume Volume to adapt.
       * \param loader Loader settings.
       */
      Texture3D(const VolumeRGBA &volume, const ImageLoader &loader = ImageLoader());

      /** \brief Destructor. */
      ~Texture3D() { }

    private:
      /** \brief Adapt a data block.
       *
       * Sets internal parameters before reading the data.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       * \param pb Bit depth.
       * \param data Texture data.
       * \param loader Loader settings.
       */
      void adapt(unsigned pw, unsigned ph, unsigned pd, unsigned pb, const void *data,
          const ImageLoader &loader);

      /** \brief Adapt a volume.
       *
       * \param volume Volume to adapt.
       * \param loader Loader settings.
       */
      void adapt(const VolumeGray8 &volume, const ImageLoader &loader);

      /** \brief Adapt a volume.
       *
       * \param volume Volume to adapt.
       * \param loader Loader settings.
       */
      void adapt(const VolumeGray16 &volume, const ImageLoader &loader);

      /** \brief Adapt a volume.
       *
       * \param volume Volume to adapt.
       * \param loader Loader settings.
       */
      void adapt(const VolumeRGB &volume, const ImageLoader &loader);

      /** \brief Adapt a volume.
       *
       * \param volume Volume to adapt.
       * \param loader Loader settings.
       */
      void adapt(const VolumeRGBA &volume, const ImageLoader &loader);

      /** \brief Adapt a data block.
       *
       * Assumes internal parameters already set.
       *
       * \param data Texture data.
       * \param loader Loader settings.
       */
      void upload(const void *data, const ImageLoader &loader);

    public:
      /** \brief Load texture from file.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      void load(const boost::filesystem::path &pfname, const ImageLoader &loader);

    public:
      /** \brief Create implementation.
       *
       * Despite being public, not to be called by user.
       *
       * \param pfname File to load.
       * \param loader Loader settings.
       * \return Container for all created instances.
       */
      static container_type createImplementation(const boost::filesystem::path &pfname,
          const ImageLoader &loader);

    public:
      /** \cond */
      friend std::ostream& operator<<(std::ostream &lhs, const Texture3D &rhs);
      /** \endcond */
  };

  /** \brief Convenience typedef. */
  typedef boost::shared_ptr<Texture3D> Texture3DSptr;
}

#endif
