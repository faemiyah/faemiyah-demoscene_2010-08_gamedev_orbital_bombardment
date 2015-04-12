#ifndef GFX_TEXTURE_2D_HPP
#define GFX_TEXTURE_2D_HPP

#include "data/store.hpp"
#include "gfx/image_loader.hpp"
#include "gfx/surface_base.hpp"
#include "gfx/texture.hpp"

namespace gfx
{
  class ImageGray8;
  class ImageGray16;
  class ImageRGB;
  class ImageRGBA;

  /** \brief Regular 2D texture.
   *
   * Textures can be loaded from files and drawn into, they may also be bound for
   * drawing other objects.
   *
   * The class only supports single-color luminance or RGB formats with or without
   * alpha.
   */
  class Texture2D :
    public Texture<GL_TEXTURE_2D>,
    public SurfaceBase,
    public data::Storable<Texture2D, ImageLoader>
  {
    private:
      /** Temporary one-shot texture. */
      static Texture2D* g_temp_texture;

      /** Temporary one-shot texture name. */
      static boost::filesystem::path g_temp_texture_name;

    public:
      /** \brief Empty constructor. */
      Texture2D() { }

      /** \brief Image loading constructor.
       *
       * \param pfname Filename to load the texture from.
       * \param loader Image loading options.
       */
      Texture2D(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Image initializing consturctor.
       *
       * \param pimg Image to load from.
       * \param loader Image loading options.
       */
      Texture2D(const ImageGray8 &pimg, const ImageLoader &loader = ImageLoader());

      /** \brief Image initializing consturctor.
       *
       * \param pimg Image to load from.
       * \param loader Image loading options.
       */
      Texture2D(const ImageGray16 &pimg, const ImageLoader &loader = ImageLoader());

      /** \brief Image initializing consturctor.
       *
       * \param pimg Image to load from.
       * \param loader Image loading options.
       */
      Texture2D(const ImageRGB &pimg, const ImageLoader &loader = ImageLoader());

      /** \brief Image initializing consturctor.
       *
       * \param pimg Image to load from.
       * \param loader Image loading options.
       */
      Texture2D(const ImageRGBA &pimg, const ImageLoader &loader = ImageLoader());

      /** Destructor. */
      ~Texture2D() { }

    private:
      /** \brief Adapt an image.
       *
       * \param pimg Image object.
       * \param loader Image loading options.
       */
      void adapt(const ImageGray8 &pimg, const ImageLoader &loader);

      /** \brief Adapt an image.
       *
       * \param pimg Image object.
       * \param loader Image loading options.
       */
      void adapt(const ImageGray16 &pimg, const ImageLoader &loader);

      /** \brief Adapt an image.
       *
       * \param pimg Image object.
       * \param loader Image loading options.
       */
      void adapt(const ImageRGB &pimg, const ImageLoader &loader);

      /** \brief Adapt an image.
       *
       * \param pimg Image object.
       * \param loader Image loading options.
       */
      void adapt(const ImageRGBA &pimg, const ImageLoader &loader);

      /** \brief Load an image file, replace the current one if it exists.
       *
       * If loading of the image fails, the current texture will not be modified.
       *
       * \param pfname File to load.
       * \param loader Image loading options.
       */
      void load(const boost::filesystem::path &pfname, const ImageLoader &loader);

      /** \brief Adapt a data block.
       *
       * \param pdata Data.
       * \param loader Image loading options.
       */
      void upload(const uint8_t *pdata, const ImageLoader &loader);

    public:
      /** \brief Accessor.
       *
       * \return Current texture.
       */
      static const Texture2D* getTempTexture()
      {
        return g_temp_texture;
      }

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

      /** \brief Replace the temporary texture.
       *
       * \param pfname New filename path to use, NULL to just remove.
       * \param loader Image loading options.
       * \return The texture just replaced.
       */
      static const Texture2D* replaceTempTexture(const char *pfname,
          const ImageLoader &loader = ImageLoader());

    public:
      /** \cond */
      friend std::ostream& operator<<(std::ostream &lhs, const Texture2D &rhs);
      /** \endcond */
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Texture2D> Texture2DSptr;

  /** \brief Output texture to a stream.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Stream after input.
   */
  std::ostream& operator<<(std::ostream &lhs, const Texture2D &rhs);
}

#endif
