#include "gfx/image.hpp"

#include "data/generic.hpp"
#include "gfx/image_jpeg.hpp"
#include "gfx/image_png.hpp"

#include <sstream>

namespace fs = boost::filesystem;
using namespace gfx;

/** Color value scale int <-> float. */
static const float COLOR_SCALE = 1.0f / 255.0f;

Image::~Image()
{
  this->unreserve();
}

void Image::loadBase(const boost::filesystem::path &pfname, const ImageLoader &loader, unsigned reqbpp)
{
  boost::ignore_unused_variable_warning(loader);

  this->unreserve();

  boost::filesystem::path location = data::open_search(pfname);
  if(data::filename_is_png(pfname))
  {
    if(image_png_supports_bpp(reqbpp))
    {
      image_png_load(m_w, m_h, m_b, m_data, location.generic_string(), reqbpp);
      m_filename = pfname;
    }
    else
    {
      std::ostringstream str;
      str << pfname << ": bit depth " << reqbpp << " not supported in PNG";
      BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
    }
  }
  else if(data::filename_is_jpeg(pfname))
  {
    if(image_jpeg_supports_bpp(reqbpp))
    {
      image_jpeg_load(m_w, m_h, m_b, m_data, location.generic_string(), reqbpp);
      m_filename = pfname;
    }
    else
    {
      std::ostringstream str;
      str << pfname << ": bit depth " << reqbpp << " not supported in JPEG";
      BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
    }
  }
  else
  {
    std::ostringstream str;
    str << "unknown image type: " << pfname;
    BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
  }
}

void Image::unreserve()
{
  delete[] m_data;
  m_data = NULL;
  m_w = 0;
  m_h = 0;
  m_b = 0;
}

void Image::write(const boost::filesystem::path &pfname)
{
  if(data::filename_is_png(pfname))
  {
    if(image_png_supports_bpp(m_b))
    {
      image_png_save(pfname.generic_string(), m_w, m_h, m_b, m_data);
    }
    else
    {
      std::ostringstream str;
      str << '\'' << pfname << "': bit depth " << m_b << " not supported in PNG";
      BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
    }
  }
  else if(data::filename_is_jpeg(pfname))
  {
    if(image_jpeg_supports_bpp(m_b))
    {
      image_jpeg_save(pfname.generic_string(), m_w, m_h, m_b, m_data);
    }
    else
    {
      std::ostringstream str;
      str << '\'' << pfname << "': bit depth " << m_b << " not supported in JPEG";
      BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
    }
  }
  else
  {
    std::ostringstream str;
    str << "unknown image type: " << pfname;
    BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
  }
}

Image* Image::create(const boost::filesystem::path &pfname, const ImageLoader &loader)
{
  boost::filesystem::path location = data::open_search(pfname);

  if(data::filename_is_png(pfname))
  {
    unsigned bpp = image_png_probe(location.generic_string(), false);

    switch(bpp)
    {
      case 32:
        return new ImageRGBA(pfname, loader);

      case 24:
        return new ImageRGB(pfname, loader);

      case 16:
        return new ImageGray16(pfname, loader);

      case 8:
        return new ImageGray8(pfname, loader);

      default:
        break;
    }

    {
      std::ostringstream sstr;
      sstr << '\'' << pfname << "': unknown PNG bit depth: " << bpp;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }
  else if(data::filename_is_jpeg(pfname))
  {
    unsigned bpp = image_jpeg_probe(location.generic_string(), false);

    switch(bpp)
    {
      case 24:
        return new ImageRGB(pfname, loader);

      case 8:
        return new ImageGray8(pfname, loader);

      default:
        break;
    }

    {
      std::ostringstream sstr;
      sstr << '\'' << pfname << "': unknown JPEG bit depth: " << bpp;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }

  {
    std::ostringstream sstr;
    sstr << "unrecognized image file name: '" << pfname << '\'';
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
}

ImageGray8::ImageGray8(const fs::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

void ImageGray8::load(const fs::path &pfname, const ImageLoader &loader)
{
  this->loadBase(pfname, loader, 8);
}

float ImageGray8::getHeightValue(float px, float py) const
{
  int rx = math::lround(px * static_cast<float>(m_w - 1)),
      ry = math::lround(py * static_cast<float>(m_h - 1));
  rx = std::max(std::min(rx, static_cast<int>(m_w)), 0);
  ry = std::max(std::min(ry, static_cast<int>(m_h)), 0);
  return this->getLuminance(static_cast<unsigned>(rx), static_cast<unsigned>(ry));
}

ImageGray16::ImageGray16(const fs::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

void ImageGray16::load(const boost::filesystem::path &pfname, const ImageLoader &loader)
{
  this->loadBase(pfname, loader, 16);

  if(loader.hasPremultiplyAlpha())
  {
    this->premultiplyAlpha();
  }
}

void ImageGray16::premultiplyAlpha()
{
  for(uint8_t *ii = m_data, *ee = m_data + (m_w * m_h * 2); (ii != ee); ii += 2)
  {
    float gray = static_cast<float>(ii[0]) * COLOR_SCALE,
          alpha = static_cast<float>(ii[1]) * COLOR_SCALE;

    ii[0] = static_cast<uint8_t>(math::lround(gray * alpha * 255.0f));
  }
}

ImageRGB::ImageRGB(const fs::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

void ImageRGB::load(const fs::path &pfname, const ImageLoader &loader)
{
  this->loadBase(pfname, loader, 24);
}

ImageRGBA::ImageRGBA(const fs::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

void ImageRGBA::load(const boost::filesystem::path &pfname, const ImageLoader &loader)
{
  this->loadBase(pfname, loader, 32);

  if(loader.hasPremultiplyAlpha())
  {
    this->premultiplyAlpha();
  }
}

void ImageRGBA::premultiplyAlpha()
{
  for(uint8_t *ii = m_data, *ee = m_data + (m_w * m_h * 4); (ii != ee); ii += 4)
  {
    float red = static_cast<float>(ii[0]) * COLOR_SCALE,
          green = static_cast<float>(ii[1]) * COLOR_SCALE,
          blue = static_cast<float>(ii[2]) * COLOR_SCALE,
          alpha = static_cast<float>(ii[3]) * COLOR_SCALE;

    ii[0] = static_cast<uint8_t>(math::lround(red * alpha * 255.0f));
    ii[1] = static_cast<uint8_t>(math::lround(green * alpha * 255.0f));
    ii[2] = static_cast<uint8_t>(math::lround(blue * alpha * 255.0f));
  }
}

