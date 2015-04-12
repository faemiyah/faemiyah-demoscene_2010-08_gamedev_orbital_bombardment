#ifndef GFX_VOLUME_HPP
#define GFX_VOLUME_HPP

#include "gfx/image_loader.hpp"
#include "gfx/volume_base.hpp"
#include "math/vec.hpp"

#include <boost/filesystem.hpp>

#include <list>

namespace gfx
{
  struct Color;
  class Perlin;

  /** \brief Three-dimensional volume.
   *
   * Used as a basis of 3D textures.
   */
  class Volume :
    public VolumeBase
  {
    protected:
      /** Internal data. */
      uint8_t *m_data;

      /** \brief Filename of this.
       *
       * Not set by default, only if loaded from somewhere.
       */
      boost::filesystem::path m_filename;

    public:
      /** \brief Get the data.
       *
       * \return Data pointer.
       */
      uint8_t* getData()
      {
        return m_data;
      }

      /** \brief Get the data.
       *
       * \return Data pointer.
       */
      const uint8_t* getData() const
      {
        return m_data;
      }

      /** \brief Accessor.
       *
       * \return Get the filename.
       */
      const boost::filesystem::path& getFilename() const
      {
        return m_filename;
      }

      /** \brief Get projected data size in bytes.
       *
       * \return Size of data in this.
       */
      unsigned getSizeBytes() const
      {
        return m_w * m_h * m_d * (m_b / 8);
      }

      /** \brief Tell if other volume has matching dimensions.
       *
       * \param other Other volume.
       */
      bool hasMatchingDimensions(const Volume &other) const
      {
        return ((m_w == other.m_w) && (m_h == other.m_h) && (m_d == other.m_d));
      }

    protected:
      /** \brief Empty constructor. */
      Volume() :
        m_data(NULL) { }

      /** \brief Constructor. 
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       * \param pb Bit depth.
       */
      Volume(unsigned pw, unsigned ph, unsigned pd, unsigned pb) :
        VolumeBase(pw, ph, pd, pb),
        m_data(new uint8_t[pw * ph * pd * pb / 8]) { }

    public:
      /** \brief Destructor. */
      virtual ~Volume();

    protected:
      /** \brief Loader base.
       *
       * \param pfname Filename to load.
       * \param loader Loader settings.
       * \param reqbpp Requested bit depth.
       */
      void loadBase(const boost::filesystem::path &pfname, const ImageLoader &loader, unsigned reqbpp);

      /** \brief Free all data associated with this.
       */
      void unreserve();

    public:
      /** \brief Clear the contents of the volume into a value.
       *
       * \param val Value to clear to.
       */
      void clear(uint8_t val);

      /** \brief Fill volume with Perlin Noise
       *
       * Borrows from the concept of Noise by Ken Perlin:
       * http://mrl.nyu.edu/~perlin/
       *
       * \param op Perlin parameters.
       */
      void perlinNoise(Perlin *op = NULL);

      /** \brief Write this into a file.
       *
       * \param pfname Filename to write.
       */
      void write(const boost::filesystem::path &pfname);

    public:
      /** \brief Clear the contents of the volume into a value.
       *
       * \param val Value to clear to.
       */
      void clear(float val)
      {
        this->clear(static_cast<uint8_t>(math::lround(val * 255.0f)));
      }

    public:
      /** \brief Load an image.
       *
       * Replaces volume data with data from given source.
       *
       * \param pfname Filename to load.
       * \param loader Loader settings.
       */
      virtual void load(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader()) = 0;

      /** \brief Set a pixel.
       *
       * \param px X coordinate.
       * \param px y coordinate.
       * \param px z coordinate.
       * \param col Color.
       */
      virtual void setPixel(unsigned px, unsigned py, unsigned pz, const gfx::Color &col) = 0;

    public:
      /** \brief Create an image.
       *
       * Returns an image object of the correct type. Casting to specific type is left to the user.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       * \return Newly created image object.
       */
      static Volume* create(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());
  };

  /** \brief Grayscale volume. */
  class VolumeGray8 :
    public Volume
  {
    public:
      /** \brief Constructor.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       */
      VolumeGray8(unsigned pw, unsigned ph, unsigned pd) :
        Volume(pw, ph, pd, 8) { }

      /** \brief Constructor.
       *
       * Will throw an exception on failure.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      VolumeGray8(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Destructor. */
      virtual ~VolumeGray8() { }

    public:
      /** \brief Fill with garble.
       */
      void fillGarble();

      /** \brief Get the interpolated value of a pixel in a given position.
       *
       * \param px X coordinate.
       * \param px y coordinate.
       * \param px z coordinate.
       * \return Intensity value.
       */
      float getAveragePixel(float px, float py, float pz);

      /** \brief Get intensity value.
       *
       * Coordinates are modulo'd.
       *
       * \param px X coordinate.
       * \param px y coordinate.
       * \param px z coordinate.
       */
      float getIntensityModulod(int px, int py, int pz);

    public:
      /** \cond */
      virtual void load(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());
      virtual void setPixel(unsigned px, unsigned py, unsigned pz, const Color &col);
      /** \endcond */

    public:
      /** \brief Get the interpolated value of a pixel in a given position.
       *
       * \param px X coordinate.
       * \param px y coordinate.
       * \param px z coordinate.
       * \return Intensity value.
       */
      float getAveragePixel(double px, double py, double pz)
      {
        return this->getAveragePixel(static_cast<float>(px), static_cast<float>(py), static_cast<float>(pz));
      }

      /** Set a pixel.
       *
       * \param px X coordinate.
       * \param px y coordinate.
       * \param px z coordinate.
       * \param pg Gray color.
       */
      void setPixel(unsigned px, unsigned py, unsigned pz, uint8_t pg)
      {
        unsigned idx = (pz * (m_w * m_h) + py * m_w + px);
        m_data[idx] = pg;
      }
  };

  /** \brief Luminance-alpha volume. */
  class VolumeGray16 :
    public Volume
  {
    public:
      /** \brief Constructor.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       */
      VolumeGray16(unsigned pw, unsigned ph, unsigned pd) :
        Volume(pw, ph, pd, 16) { }

      /** \brief Constructor.
       *
       * Will throw an exception on failure.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      VolumeGray16(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Destructor. */
      virtual ~VolumeGray16() { }

    public:
      /** \brief Premultiply alpha channel of this volume. */
      void premultiplyAlpha();

    public:
      /** \cond */
      virtual void load(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());
      virtual void setPixel(unsigned px, unsigned py, unsigned pz, const Color &col);
      /** \endcond */

    public:
      /** Set a pixel.
       *
       * \param px X coordinate.
       * \param px y coordinate.
       * \param px z coordinate.
       * \param pg Gray color.
       * \param pa Alpha color.
       */
      void setPixel(unsigned px, unsigned py, unsigned pz, uint8_t pg, uint8_t pa)
      {
        unsigned idx = (pz * (m_w * m_h) + py * m_w + px) * 2;
        m_data[idx + 0] = pg;
        m_data[idx + 1] = pa;
      }
  };

  /** \brief RGB volume. */
  class VolumeRGB :
    public Volume
  {
    public:
      /** \brief Constructor.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       */
      VolumeRGB(unsigned pw, unsigned ph, unsigned pd) :
        Volume(pw, ph, pd, 24) { }

      /** \brief Constructor.
       *
       * Will throw an exception on failure.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      VolumeRGB(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Destructor. */
      virtual ~VolumeRGB() { }

    public:
      /** \cond */
      virtual void load(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());
      virtual void setPixel(unsigned px, unsigned py, unsigned pz, const Color &col);
      /** \endcond */

    public:
      /** Set a pixel.
       *
       * \param px X coordinate.
       * \param px y coordinate.
       * \param px z coordinate.
       * \param pr R color.
       * \param pg G color.
       * \param pb B color.
       */
      void setPixel(unsigned px, unsigned py, unsigned pz, uint8_t pr,
          uint8_t pg, uint8_t pb)
      {
        unsigned idx = (pz * (m_w * m_h) + py * m_w + px) * 3;
        m_data[idx + 0] = pr;
        m_data[idx + 1] = pg;
        m_data[idx + 2] = pb;
      }
  };

  /** \brief RGBA volume. */
  class VolumeRGBA :
    public Volume
  {
    public:
      /** \brief Constructor.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       */
      VolumeRGBA(unsigned pw, unsigned ph, unsigned pd) :
        Volume(pw, ph, pd, 32) { }

      /** \brief Constructor.
       *
       * Will throw an exception on failure.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      VolumeRGBA(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());

      /** \brief Destructor. */
      virtual ~VolumeRGBA() { }

    public:
      /** \brief Combine other volumes to make up the content in this.
       *
       * \param v1 Volume 1.
       * \param v2 Volume 2.
       * \param v2 Volume 3.
       * \param v3 Volume 4.
       */
      void combine(const VolumeGray8 &v1, const VolumeGray8 &v2, const VolumeGray8 &v3, const VolumeGray8 &v4);

      /** \brief Combine other volumes to make up the content in this.
       *
       * \param v1 Volume 1.
       * \param v2 Volume 2.
       */
      void combine(const VolumeRGB &v1, const VolumeGray8 &v2);

      /** \brief Premultiply alpha channel of this volume. */
      void premultiplyAlpha();

    public:
      /** \cond */
      virtual void load(const boost::filesystem::path &pfname, const ImageLoader &loader = ImageLoader());
      virtual void setPixel(unsigned px, unsigned py, unsigned pz, const Color &col);
      /** \endcond */

    public:
      /** Set a pixel.
       *
       * \param px X coordinate.
       * \param px y coordinate.
       * \param px z coordinate.
       * \param pr R color.
       * \param pg G color.
       * \param pb B color.
       */
      void setPixel(unsigned px, unsigned py, unsigned pz, uint8_t pr, uint8_t pg, uint8_t pb,
          uint8_t pa)
      {
        unsigned idx = (pz * (m_w * m_h) + py * m_w + px) * 4;
        m_data[idx + 0] = pr;
        m_data[idx + 1] = pg;
        m_data[idx + 2] = pb;
        m_data[idx + 3] = pa;
      }
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Volume> VolumeSptr;

  /** Convenience typedef. */
  typedef boost::shared_ptr<VolumeGray8> VolumeGray8Sptr;

  /** \brief Perlin Noise parameters class.
   *
   * Default implementation produces 'cloud' noise.
   */
  class Perlin
  {
    public:
      /** \brief Constructor. */
      Perlin() { }

      /** \brief Destructor. */
      virtual ~Perlin() { }

    public:
      /** \brief Initializes the noise bank.
       *
       * \param op Target volume.
       */
      virtual void fill(VolumeGray8 &op, unsigned level);

      /** \brief Generates noise from noise texture list.
       *
       * \param op Target volume.
       * \param tt Texture indexing.
       */
      virtual Color noise(const std::list<VolumeGray8Sptr> &op, const math::vec3d &tt);
  };
}

#endif
