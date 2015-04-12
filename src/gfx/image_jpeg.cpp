#include "gfx/image_jpeg.hpp"

#include <stdio.h>

#include <sstream>

#include <boost/scoped_array.hpp>

#include "jpeglib.h"

/** Hold contents of JPEG reading structs safely. */
class JpegReader
{
  public:
    /** File handle. */
    FILE *m_fd;

    /** JPEG decompress struct. */
    struct jpeg_decompress_struct m_decompress;

    /** JPEG error manager. */
    struct jpeg_error_mgr m_err;

    /** Internal decoding data block. */
    uint8_t *m_block;

  public:
    /** \brief Constructor.
     *
     * \param filename File to open.
     */ 
    JpegReader(const std::string &filename) :
      m_block(NULL)
    {
      m_fd = fopen(filename.c_str(), "rb");
      if(NULL == m_fd)
      {
        std::stringstream sstr;
        sstr << "could not open '" << filename << "' for reading";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }

      m_decompress.err = jpeg_std_error(&m_err);
      jpeg_create_decompress(&m_decompress);
      jpeg_stdio_src(&m_decompress, m_fd);
    }

    /** \brief Destructor.
     */
    ~JpegReader()
    {
      if(NULL != m_fd)
      {
        fclose(m_fd);

        // decompress struct may only be reserved if file was properly opened
        jpeg_destroy_decompress(&m_decompress);
      }

      if(NULL != m_block)
      {
        delete[] m_block;
      }
    }

  public:
    /** \brief Accessor.
     *
     * \return Bit depth.
     */
    unsigned getBpp() const
    {
      switch(m_decompress.out_color_space)
      {
        case JCS_GRAYSCALE:
          if(1 != m_decompress.num_components)
          {
            std::ostringstream sstr;
            sstr << "invalid number of components for grayscale jpeg: " << m_decompress.num_components;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
          }
          return 8;

        case JCS_RGB:
          if(3 != m_decompress.num_components)
          {
            std::ostringstream sstr;
            sstr << "invalid number of components for RGB jpeg: " << m_decompress.num_components;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
          }
          return 24;

        default:
          return 0; // not supported
      }
    }

    /** \brief Read depth.
     *
     * The depth, if supplied, has been saved in a text comment field. Thus, this function is relatively
     * expensive.
     *
     * \return Depth as unsigned or 0.
     */
    unsigned getDepth() const
    {
      return 0;
    }

    /** \brief Read the data.
     *
     * \param row_pointers 
     */
    void read(uint8_t **row_pointers)
    {
      jpeg_start_decompress(&m_decompress);

      for(;;)
      {
        unsigned scanlines_left = m_decompress.image_height - m_decompress.output_scanline;

        if(0 >= scanlines_left)
        {
          break;
        }

        jpeg_read_scanlines(&m_decompress, &row_pointers[m_decompress.output_scanline], scanlines_left);
      }

      jpeg_finish_decompress(&m_decompress);
    }

    /** \brief Read header.
     */
    void readHeader()
    {
      jpeg_read_header(&m_decompress, TRUE);
    }

  public:
    /** \brief Accessor.
     *
     * \return Data block.
     */
    uint8_t* getBlock()
    {
      return m_block;
    }

    /** \brief Accessor.
     *
     * \return Height.
     */
    unsigned getHeight() const
    {
      return m_decompress.image_height;
    }

    /** \brief Accessor.
     *
     * \return Width.
     */
    unsigned getWidth() const
    {
      return m_decompress.image_width;
    }

    /** \brief Set the data block.
     *
     * \param op Data block.
     */
    void setBlock(uint8_t *op)
    {
      m_block = op;
    }
};

class JpegWriter
{
  private:
    /** File handle. */
    FILE *m_fd;

    /** JPEG compress struct. */
    struct jpeg_compress_struct m_compress;

    /** JPEG error manager. */
    struct jpeg_error_mgr m_err;

  public:
    /** \brief Constructor.
     *
     * \param filename File to open.
     */
    JpegWriter(const std::string &filename)
    {
      m_fd = fopen(filename.c_str(), "wb");
      if(NULL == m_fd)
      {
        std::stringstream sstr;
        sstr << "could not open '" << filename << "' for writing";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }

      m_compress.err = jpeg_std_error(&m_err);
      jpeg_create_compress(&m_compress);
      jpeg_stdio_dest(&m_compress, m_fd);
    }

    /** \brief Destructor. */
    ~JpegWriter()
    {
      if(NULL != m_fd)
      {
        fclose(m_fd);

        // compress struct may only be reserved if file was properly opened
        jpeg_destroy_compress(&m_compress);
      }
    }

  public:
    /** \brief Write actual file contents.
     *
     * \param pw Width.
     * \param ph Height.
     * \param pd Depth.
     * \param pb Bit depth.
     * \param pquality Quality to use.
     * \param row_pointers Row pointer input.
     */
    void write(unsigned pw, unsigned ph, unsigned pd, unsigned pb, int pquality, uint8_t **row_pointers)
    {
      if(8 == pb)
      {
        m_compress.input_components = 1;
        m_compress.in_color_space = JCS_GRAYSCALE;
      }
      else if(24 == pb)
      {
        m_compress.input_components = 3;
        m_compress.in_color_space = JCS_RGB;
      }
      else
      {
        std::ostringstream sstr;
        sstr << "invalid JPEG bit depth: " << pb;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }

      if((0 > pquality) || (100 < pquality))
      {
        std::ostringstream sstr;
        sstr << "invalid JPG quality value: " << pquality;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }

      m_compress.image_width = pw;	
      m_compress.image_height = ph;

      // TODO: depth
      boost::ignore_unused_variable_warning(pd);

      jpeg_set_defaults(&m_compress);
      jpeg_set_quality(&m_compress, pquality, TRUE);
      jpeg_start_compress(&m_compress, TRUE);

      for(;;)
      {
        unsigned scanlines_left = m_compress.image_height - m_compress.next_scanline;

        if(0 >= scanlines_left)
        {
          break;
        }

        jpeg_write_scanlines(&m_compress, &row_pointers[m_compress.next_scanline], scanlines_left);
      }

      jpeg_finish_compress(&m_compress);
    }
};

namespace gfx
{
  unsigned image_jpeg_probe(const std::string &filename, bool require_volume)
  {
    JpegReader reader(filename);

    reader.readHeader();

    if(require_volume)
    {
      if(reader.getDepth() <= 0)
      {
        std::stringstream sstr;
        sstr << '\'' << filename << "' is an image, excepted a volume";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
    }
    else if(reader.getDepth() > 0)
    {
      std::stringstream sstr;
      sstr << '\'' << filename << "' is a volume, excepted an image";
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }

    return reader.getBpp();
  }

  void image_jpeg_load_extended(unsigned &pw, unsigned &ph, unsigned &pd, unsigned &pb, uint8_t *&pdata,
      const std::string &filename, unsigned required_bpp)
  {
    JpegReader reader(filename);

    reader.readHeader();
    unsigned width = reader.getWidth(),
             height = reader.getHeight(),
             depth = reader.getDepth(),
             bpp = reader.getBpp();
    if(bpp != required_bpp)
    {
      std::stringstream sstr;
      sstr << '\'' << filename << "' has bit depth " << bpp << ", excepted " << required_bpp;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }

    unsigned data_size = width * height * bpp / 8;
    reader.setBlock(new uint8_t[data_size]);
    boost::scoped_array<uint8_t*> row_pointers(new uint8_t*[height]);
    {
      uint8_t *iter = reader.getBlock() + data_size;
      unsigned iter_sub = (width * bpp / 8);

      // JPEG and OpenGL scanlines are in different order
      for(unsigned ii = 0; (ii < height); ++ii)
      {
        iter -= iter_sub;
        row_pointers[ii] = iter;
      }
    }

    reader.read(row_pointers.get());

    if(depth > 0)
    {
      height /= depth;
    }

    pw = width;
    ph = height;
    pd = depth;
    pb = bpp;

    // yield ownership of block
    pdata = reader.getBlock();
    reader.setBlock(NULL);
  }

  void image_jpeg_load(unsigned &pw, unsigned &ph, unsigned &pb, uint8_t *&pdata, const std::string &filename,
      unsigned required_bpp)
  {
    unsigned depth = 0;

    image_jpeg_load_extended(pw, ph, depth, pb, pdata, filename, required_bpp);

    if(depth != 0)
    {
      std::ostringstream sstr;
      sstr << "JPEG file had depth axis information even if not assumed";
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }

  void image_jpeg_save_extended(const std::string &filename, unsigned pw, unsigned ph, unsigned pd,
      unsigned pb, uint8_t *pdata, int pquality)
  {
    if((0 >= pw) || (0 >= ph))
    {
      std::stringstream sstr;
      sstr << "invalid image dimensions: " << pw << "x" << ph;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
    if(pd > 0)
    {
      ph *= pd;
    }

    JpegWriter writer(filename);
    boost::scoped_array<uint8_t*> row_pointers(new uint8_t*[ph]);
    {
      uint8_t *iter = pdata + pw * ph * pb / 8;
      unsigned iter_sub = (pw * pb / 8);

      // PNG and OpenGL scanlines are in different order
      for(unsigned ii = 0; (ii < ph); ++ii)
      {
        iter -= iter_sub;
        row_pointers[ii] = iter;
      }
    }

    writer.write(pw, ph, pd, pb, pquality, row_pointers.get());
  }

  void image_jpeg_save(const std::string &filename, unsigned pw, unsigned ph,
      unsigned pb, uint8_t *pdata, int pquality)
  {
    image_jpeg_save_extended(filename, pw, ph, 0, pb, pdata, pquality);
  }
}

