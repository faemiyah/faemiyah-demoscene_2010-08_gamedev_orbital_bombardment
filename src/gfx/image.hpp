#ifndef GFX_IMAGE_HPP
#define GFX_IMAGE_HPP

#include "gfx/color.hpp"
#include "gfx/image_loader.hpp"
#include "gfx/surface_base.hpp"
#include "math/vec.hpp"

#include <boost/filesystem.hpp>

namespace gfx
{
  /** \brief Image, as opposed to texture.
   */
  class Image :
    public SurfaceBase
  {
    protected:
      /** Image data. */
      uint8_t *m_data;

      /** \brief Filename this.
       *
       * Not set by default, only if loaded from somewhere.
       */
      boost::filesystem::path m_filename;

    public:
      /** \brief Get pointer to the data within the image.
       *
       * \return Data pointer.
       */
      uint8_t* getData()
      {
        return m_data;
      }

      /** \brief Get pointer to the data within the image.
       *
       * \return Data pointer.
       */
      const uint8_t* getData() const
      {
        return m_data;
      }

      /** \brief Accessor.
       *
       * \return The filename as a string.
       */
      const boost::filesystem::path& getFilename() const
      {
        return m_filename;
      }

    public:
      /** \brief Get projected data size in bytes.
       *
       * \return Size of data in this.
       */
      unsigned getSizeBytes() const
      {
        return m_w * m_h * (m_b / 8);
      }

    protected:
      /** \brief Empty constructor. */
      Image() :
        m_data(NULL) { }

      /** \brief Create a new image of certain dimensions.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pb Bit depth.
       */
      Image(unsigned pw, unsigned ph, unsigned pb) :
        SurfaceBase(pw, ph, pb),
        m_data(new uint8_t[pw * ph * pb / 8]) { }

    public:
      /** \brief Destructor. */
      virtual ~Image();

    protected:
      /** \brief Premultiply alpha channel.
       *
       * Normally done immediately after loading the image, but may be done
       * at any point manually.
       */
      void premultiplyAlpha();

    protected:
      /** \brief Loader base.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       * \param reqbpp Requested bit depth.
       */
      void loadBase(const boost::filesystem::path &pfname, const ImageLoader &loader, unsigned reqbpp);

      /** \brief Free all resources reserved by this object.
       *
       * Will leave the image in a state ready for loading new data.
       */
      void unreserve();

    public:
      /** \brief Write an image.
       *
       * Writes this image to the given filename.
       *
       * \param pfname Destination filename.
       */
      void write(const boost::filesystem::path &pfname);

    public:
      /** \brief Load an image.
       *
       * Replaces image data with data from given source.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      virtual void load(const boost::filesystem::path &pfname, const ImageLoader &loader) = 0;

    public:
      /** \brief Create an image.
       *
       * Returns an image object of the correct type. Casting to specific type is left to the user.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       * \return Newly created image object.
       */
      static Image* create(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());
  };

  /** \brief GRAY8 image.
   */
  class ImageGray8 :
    public Image
  {
    public:
      /** \brief Empty constructor. */
      ImageGray8() { }

      /** \brief Create a new image of certain dimensions.
       *
       * \param pw Width.
       * \param ph Height.
       */
      ImageGray8(unsigned pw, unsigned ph) :
        Image(pw, ph, 8) { }

      /** \brief Constructor.
       *
       * Will throw an exception on failure.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      ImageGray8(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Destructor. */
      virtual ~ImageGray8() { }

    public:
      /** \brief Get a heightfield value from image.
       *
       * Input coordinates outside [0, 1] are clamped.
       *
       * \param px Image x.
       * \param py Image y.
       * \return Heightfield value [0, 1].
       */
      float getHeightValue(float px, float py) const;

    public:
      /** \cond */
      virtual void load(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());
      /** \endcond */

    public:
      /** \brief Get a color value.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \return Color object.
       */
      uint8_t getPixel(unsigned px, unsigned py) const
      {
        return m_data[py * m_w + px];
      }

      /** \brief Get a color value.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \return Color vector.
       */
      float getLuminance(unsigned px, unsigned py) const
      {
        return static_cast<float>(this->getPixel(px, py)) / 255.0f;
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param pg Gray component.
       */
      void setLuminance(unsigned px, unsigned py, float pg)
      {
        this->setPixel(px, py, static_cast<uint8_t>(math::lround(pg * 255.0f)));
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param pg Gray component.
       */
      void setPixel(unsigned px, unsigned py, uint8_t pg)
      {
        m_data[py * m_w + px] = pg;
      }
  };

  /** \brief GRAY16 image.
   */
  class ImageGray16 :
    public Image
  {
    public:
      /** \brief Empty constructor. */
      ImageGray16() { }

      /** \brief Create a new image of certain dimensions.
       *
       * \param pw Width.
       * \param ph Height.
       */
      ImageGray16(unsigned pw, unsigned ph) :
        Image(pw, ph, 16) { }

      /** \brief Constructor.
       *
       * Will throw an exception on failure.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      ImageGray16(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Destructor. */
      virtual ~ImageGray16() { }

    public:
      /** \brief Premultiply alpha channel of this image. */
      void premultiplyAlpha();

    public:
      /** \cond */
      virtual void load(const boost::filesystem::path &pfname,
          const ImageLoader &loader = ImageLoader());
      /** \endcond */

    public:
      /** \brief Get a color value.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \return Color object.
       */
      Color getPixel(unsigned px, unsigned py) const
      {
        const uint8_t *iter = m_data + ((py * m_w + px) * 2);
        uint8_t gray = iter[0];
        return Color(gray, gray, gray, iter[1]);
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param pg Gray component.
       * \param pa Alpha component.
       */
      void setPixel(unsigned px, unsigned py, uint8_t pg, uint8_t pa)
      {
        uint8_t *iter = m_data + ((py * m_w + px) * 2);
        iter[0] = pg;
        iter[1] = pa;
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param pg Gray component.
       * \param pa Alpha component.
       */
      void setPixel(unsigned px, unsigned py, float pg, float pa)
      {
        this->setPixel(px, py,
            static_cast<uint8_t>(math::lround(pg * 255.0f)),
            static_cast<uint8_t>(math::lround(pa * 255.0f)));
      }

      /** \brief Set a pixel.
       *
       * Red, Green and Blue in Color are averaged.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param col Color component.
       */
      void setPixel(unsigned px, unsigned py, const Color &col)
      {
        this->setPixel(px, py,
            (col.r() + col.g() + col.b()) * (1.0f / 3.0f),
            col.a());
      }
  };

  /** \brief RGB24 image.
   */
  class ImageRGB :
    public Image
  {
    public:
      /** \brief Empty constructor. */
      ImageRGB() { }

      /** \brief Create a new image of certain dimensions.
       *
       * \param pw Width.
       * \param ph Height.
       */
      ImageRGB(unsigned pw, unsigned ph) :
        Image(pw, ph, 24) { }

      /** \brief Constructor.
       *
       * Will throw an exception on failure.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      ImageRGB(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Destructor. */
      virtual ~ImageRGB() { }

    public:
      /** \cond */
      virtual void load(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());
      /** \endcond */

    public:
      /** \brief Get a color value.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \return Color object.
       */
      Color getPixel(unsigned px, unsigned py) const
      {
        const uint8_t *iter = m_data + ((py * m_w + px) * 3);
        return Color(iter[0], iter[1], iter[2], 255);
      }

      /** \brief Get a color value.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \return Color vector.
       */
      math::vec3f getPixelVec(unsigned px, unsigned py)
      {
        const uint8_t *iter = m_data + ((py * m_w + px) * 3);
        return math::vec3f(
            static_cast<float>(iter[0]) / 255.0f,
            static_cast<float>(iter[1]) / 255.0f,
            static_cast<float>(iter[2]) / 255.0f);
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param pr R component.
       * \param pg G component.
       * \param pb B component.
       */
      void setPixel(unsigned px, unsigned py, uint8_t pr, uint8_t pg, uint8_t pb)
      {
        uint8_t *iter = m_data + ((py * m_w + px) * 3);
        iter[0] = pr;
        iter[1] = pg;
        iter[2] = pb;
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param col Color value.
       */
      void setPixel(unsigned px, unsigned py, const Color &col)
      {
        this->setPixel(px, py,
            static_cast<uint8_t>(col.ri()),
            static_cast<uint8_t>(col.gi()),
            static_cast<uint8_t>(col.bi()));
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param col Color value.
       */
      void setPixel(unsigned px, unsigned py, const math::vec3f &col)
      {
        this->setPixel(px, py,
            static_cast<uint8_t>(col.x() * 255.0f),
            static_cast<uint8_t>(col.y() * 255.0f),
            static_cast<uint8_t>(col.z() * 255.0f));
      }
  };

  /** \brief RGBA32 image.
   */
  class ImageRGBA :
    public Image
  {
    public:
      /** \brief Empty constructor. */
      ImageRGBA() { }

      /** \brief Create a new image of certain dimensions.
       *
       * \param pw Width.
       * \param ph Height.
       */
      ImageRGBA(unsigned pw, unsigned ph) :
        Image(pw, ph, 32) { }

      /** \brief Constructor.
       *
       * Will throw an exception on failure.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      ImageRGBA(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Destructor. */
      virtual ~ImageRGBA() { }

    public:
      /** \brief Premultiply alpha channel of this image. */
      void premultiplyAlpha();

    public:
      /** \cond */
      virtual void load(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());
      /** \endcond */

    public:
      /** \brief Get a color value.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \return Color object.
       */
      Color getPixel(unsigned px, unsigned py) const
      {
        const uint8_t *iter = m_data + ((py * m_w + px) * 4);
        return Color(iter[0], iter[1], iter[2], iter[3]);
      }

      /** \brief Get a color value.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \return Color vector.
       */
      math::vec4f getPixelVec(unsigned px, unsigned py)
      {
        const uint8_t *iter = m_data + ((py * m_w + px) * 4);
        return math::vec4f(
            static_cast<float>(iter[0]) / 255.0f,
            static_cast<float>(iter[1]) / 255.0f,
            static_cast<float>(iter[2]) / 255.0f,
            static_cast<float>(iter[3]) / 255.0f);
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param pr R component.
       * \param pg G component.
       * \param pb B component.
       * \param pa A component.
       */
      void setPixel(unsigned px, unsigned py, uint8_t pr, uint8_t pg,
          uint8_t pb, uint8_t pa)
      {
        uint8_t *iter = m_data + ((py * m_w + px) * 4);
        iter[0] = pr;
        iter[1] = pg;
        iter[2] = pb;
        iter[3] = pa;
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param col Color value.
       */
      void setPixel(unsigned px, unsigned py, const Color &col)
      {
        this->setPixel(px, py,
            static_cast<uint8_t>(col.ri()),
            static_cast<uint8_t>(col.gi()),
            static_cast<uint8_t>(col.bi()),
            static_cast<uint8_t>(col.ai()));
      }

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param py Y coordinate.
       * \param col Color value.
       */
      void setPixel(unsigned px, unsigned py, const math::vec4f &col)
      {
        this->setPixel(px, py, static_cast<uint8_t>(col.x() * 255.0f),
            static_cast<uint8_t>(col.y() * 255.0f),
            static_cast<uint8_t>(col.z() * 255.0f),
            static_cast<uint8_t>(col.w() * 255.0f));
      }
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<ImageGray8> ImageGray8Sptr;

  /** Convenience typedef. */
  typedef boost::shared_ptr<ImageGray16> ImageGray16Sptr;

  /** Convenience typedef. */
  typedef boost::shared_ptr<ImageRGB> ImageRGBSptr;

  /** Convenience typedef. */
  typedef boost::shared_ptr<ImageRGBA> ImageRGBASptr;
}

#endif
