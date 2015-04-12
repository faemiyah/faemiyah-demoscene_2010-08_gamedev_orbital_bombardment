#include "gfx/volume.hpp"

#include "data/generic.hpp"
#include "gfx/color.hpp"
#include "gfx/image_jpeg.hpp"
#include "gfx/image_png.hpp"
#include "math/generic.hpp"
#include "math/random.hpp"

#include <sstream>

namespace fs = boost::filesystem;
using namespace gfx;

/** Color value scale int <-> float. */
static const float COLOR_SCALE = 1.0f / 255.0f;

void Perlin::fill(VolumeGray8 &op, unsigned level)
{
  boost::ignore_unused_variable_warning(level);
  
  op.fillGarble();
}

Color Perlin::noise(const std::list<VolumeGray8Sptr> &op, const math::vec3d &tt)
{
  float ns = 0.0f,
        wt = 1.0f;

  BOOST_FOREACH(const VolumeGray8Sptr &vv, op)
  {
    ns += wt * (vv->getAveragePixel(tt.x(), tt.y(), tt.z()) - 0.5f);
    wt *= 0.75f;
  }

  ns = ns + 0.5f;
  if(ns < 0.0f)
  {
    ns = -ns;
  }
  else if(ns > 1.0f)
  {
    ns = 2.0f - ns;
  }

  return Color(ns, ns, ns, ns);
}

/** Default Perlin noise parameters. */
Perlin perlin_default;

Volume::~Volume()
{
  this->unreserve();
}

void Volume::clear(uint8_t val)
{
  unsigned limit = this->getSizeBytes();
  for(unsigned ii = 0; (ii < limit); ++ii)
  {
    m_data[ii] = val;
  }
}

void Volume::loadBase(const fs::path &pfname, const ImageLoader &loader,
    unsigned reqbpp)
{
  boost::ignore_unused_variable_warning(loader);

  this->unreserve();

  boost::filesystem::path location = data::open_search(pfname);
  if(data::filename_is_png(pfname))
  {
    if(image_png_supports_bpp(reqbpp))
    {
      image_png_load_extended(m_w, m_h, m_d, m_b, m_data, location.generic_string(), reqbpp);
      m_filename = pfname;
    }
    else
    {
      std::ostringstream str;
      str << '\'' << pfname << "': bit depth " << reqbpp << " not supported in PNG";
      BOOST_THROW_EXCEPTION(std::runtime_error(str.str()));
    }
  }
  else if(data::filename_is_jpeg(pfname))
  {
    if(image_jpeg_supports_bpp(reqbpp))
    {
      image_jpeg_load_extended(m_w, m_h, m_d, m_b, m_data, location.generic_string(), reqbpp);
      m_filename = pfname;
    }
    else
    {
      std::ostringstream str;
      str << '\'' << pfname << "': bit depth " << reqbpp << " not supported in JPEG";
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

void Volume::perlinNoise(Perlin *op)
{
  if(NULL == op)
  {
    op = &perlin_default;
  }

  std::list<VolumeGray8Sptr> pn;
  for(unsigned ii = 2, level = 0; (ii <= m_d); ii*= 2, ++level)
  {
    VolumeGray8Sptr inst(new VolumeGray8(ii, ii, ii));
    pn.push_back(inst);
    op->fill(*inst, level);
  }

  for(unsigned ii = 0; (ii < m_d); ++ii)
  {
    double di = static_cast<double>(ii) / static_cast<double>(m_d - 1);

    for(unsigned jj = 0; (jj < m_h); ++jj)
    {
      double dj = static_cast<double>(jj) / static_cast<double>(m_h - 1);

      for(unsigned kk = 0; (kk < m_w); ++kk)
      {
        double dk = static_cast<double>(kk) / static_cast<double>(m_w - 1);

        this->setPixel(kk, jj, ii, op->noise(pn, math::vec3d(dk, dj, di)));
      }
    }
  }
}

void Volume::unreserve()
{
  delete[] m_data;
  m_data = NULL;
  m_w = 0;
  m_h = 0;
  m_w = 0;
  m_b = 0;
}

void Volume::write(const boost::filesystem::path &pfname)
{
  if(data::filename_is_png(pfname))
  {
    if(image_png_supports_bpp(m_b))
    {
      image_png_save_extended(pfname.generic_string(), m_w, m_h, m_d, m_b, m_data);
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
      image_jpeg_save_extended(pfname.generic_string(), m_w, m_h, m_d, m_b, m_data);
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

Volume* Volume::create(const boost::filesystem::path &pfname, const ImageLoader &loader)
{
  boost::filesystem::path location = data::open_search(pfname);

  if(data::filename_is_png(pfname))
  {
    unsigned bpp = image_png_probe(location.generic_string(), true);

    switch(bpp)
    {
      case 32:
        return new VolumeRGBA(pfname, loader);

      case 24:
        return new VolumeRGB(pfname, loader);

      case 16:
        return new VolumeGray16(pfname, loader);

      case 8:
        return new VolumeGray8(pfname, loader);

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
        return new VolumeRGB(pfname, loader);

      case 8:
        return new VolumeGray8(pfname, loader);

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

VolumeGray8::VolumeGray8(const fs::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

void VolumeGray8::fillGarble()
{
  for(unsigned ii = 0; (ii < (m_w * m_h * m_d)); ++ii)
  {
    m_data[ii] = static_cast<uint8_t>(math::mrand(0, 255));
  }
}

float VolumeGray8::getAveragePixel(float px, float py, float pz)
{
  float rx = -0.5f + px * static_cast<float>(m_w),
        ry = -0.5f + py * static_cast<float>(m_h),
        rz = -0.5f + pz * static_cast<float>(m_d),
        xpwt = 1.0f - (::ceilf(rx) - rx),
        xnwt = 1.0f - (rx - ::floorf(rx)),
        ypwt = 1.0f - (::ceilf(ry) - ry),
        ynwt = 1.0f - (ry - ::floorf(ry)),
        zpwt = 1.0f - (::ceilf(rz) - rz),
        znwt = 1.0f - (rz - ::floorf(rz));
  int ix1 = static_cast<int>(::floorf(rx)),
      ix2 = static_cast<int>(::ceilf(rx)),
      iy1 = static_cast<int>(::floorf(ry)),
      iy2 = static_cast<int>(::ceilf(ry)),
      iz1 = static_cast<int>(::floorf(rz)),
      iz2 = static_cast<int>(::ceilf(rz));
  float  x1y1z1 = this->getIntensityModulod(ix1, iy1, iz1),
        x2y1z1 = this->getIntensityModulod(ix2, iy1, iz1),
        x1y2z1 = this->getIntensityModulod(ix1, iy2, iz1),
        x1y1z2 = this->getIntensityModulod(ix1, iy1, iz2),
        x2y2z1 = this->getIntensityModulod(ix2, iy2, iz1),
        x1y2z2 = this->getIntensityModulod(ix1, iy2, iz2),
        x2y1z2 = this->getIntensityModulod(ix2, iy1, iz2),
        x2y2z2 = this->getIntensityModulod(ix2, iy2, iz2);
  // X weight first.
  float y1z1 = xnwt * x1y1z1 + xpwt * x2y1z1,
        y2z1 = xnwt * x1y2z1 + xpwt * x2y2z1,
        y1z2 = xnwt * x1y1z2 + xpwt * x2y1z2,
        y2z2 = xnwt * x1y2z2 + xpwt * x2y2z2;
  // Then Y.
  float z1 = ynwt * y1z1 + ypwt * y2z1,
        z2 = ynwt * y1z2 + ypwt * y2z2;
  // Then Z.
  return znwt * z1 + zpwt * z2;
}

float VolumeGray8::getIntensityModulod(int px, int py, int pz)
{
  unsigned ix = static_cast<unsigned>(math::congr(px, static_cast<int>(m_w)));
  unsigned iy = static_cast<unsigned>(math::congr(py, static_cast<int>(m_h)));
  unsigned iz = static_cast<unsigned>(math::congr(pz, static_cast<int>(m_d)));
  return static_cast<float>(m_data[iz * (m_w * m_h) + iy * m_w + ix]) * (1.0f / 255.0f);
}

void VolumeGray8::load(const fs::path &pfname,
    const ImageLoader &loader)
{
  this->loadBase(pfname, loader, 8);
}

void VolumeGray8::setPixel(unsigned px, unsigned py, unsigned pz,
    const Color &col)
{
  this->setPixel(px, py, pz,
      static_cast<uint8_t>(math::lround((col.r() + col.g() + col.b()) * (255.0f / 3.0f))));
}

VolumeGray16::VolumeGray16(const fs::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

void VolumeGray16::load(const fs::path &pfname,
    const ImageLoader &loader)
{
  this->loadBase(pfname, loader, 16);

  if(loader.hasPremultiplyAlpha())
  {
    this->premultiplyAlpha();
  }
}

void VolumeGray16::premultiplyAlpha()
{
  for(uint8_t *ii = m_data, *ee = m_data + (m_w * m_h * m_d * 2); (ii != ee); ii += 2)
  {
    float gray = static_cast<float>(ii[0]) * COLOR_SCALE,
          alpha = static_cast<float>(ii[1]) * COLOR_SCALE;

    ii[0] = static_cast<uint8_t>(math::lround(gray * alpha * 255.0f));
  }
}

void VolumeGray16::setPixel(unsigned px, unsigned py, unsigned pz,
    const Color &col)
{
  this->setPixel(px, py, pz,
      static_cast<uint8_t>(math::lround((col.r() + col.g() + col.b()) * (255.0f / 3.0f))),
      static_cast<uint8_t>(col.ai()));
}

VolumeRGB::VolumeRGB(const fs::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

void VolumeRGB::load(const fs::path &pfname,
    const ImageLoader &loader)
{
  this->loadBase(pfname, loader, 24);
}

void VolumeRGB::setPixel(unsigned px, unsigned py, unsigned pz,
    const Color &col)
{
  this->setPixel(px, py, pz,
      static_cast<uint8_t>(col.ri()),
      static_cast<uint8_t>(col.gi()),
      static_cast<uint8_t>(col.bi()));
}

VolumeRGBA::VolumeRGBA(const fs::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

void VolumeRGBA::combine(const VolumeGray8 &v1, const VolumeGray8 &v2,
    const VolumeGray8 &v3, const VolumeGray8 &v4)
{
  if(!this->hasMatchingDimensions(v1) ||
      !this->hasMatchingDimensions(v2) ||
      !this->hasMatchingDimensions(v3) ||
      !this->hasMatchingDimensions(v4))
  {
    std::stringstream sstr;
    sstr << "all combined volume dimensions do not match";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str().c_str()));
  }

  const uint8_t *data1 = v1.getData();
  const uint8_t *data2 = v2.getData();
  const uint8_t *data3 = v3.getData();
  const uint8_t *data4 = v4.getData();

  unsigned bsize = v1.getSizeBytes(); // All have same.
  for(unsigned ii = 0; (ii < bsize); ++ii)
  {
    unsigned idx = ii * 4;
    m_data[idx + 0] = data1[ii];
    m_data[idx + 1] = data2[ii];
    m_data[idx + 2] = data3[ii];
    m_data[idx + 3] = data4[ii];
  }
}

void VolumeRGBA::combine(const VolumeRGB &v1, const VolumeGray8 &v2)
{
  if(!this->hasMatchingDimensions(v1) ||
      !this->hasMatchingDimensions(v2))
  {
    std::stringstream sstr;
    sstr << "all combined volume dimensions do not match";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str().c_str()));
  }

  const uint8_t *data1 = v1.getData();
  const uint8_t *data2 = v2.getData();

  unsigned bsize = v2.getSizeBytes(); // Advance by smaller volume.
  for(unsigned ii = 0; (ii < bsize); ++ii)
  {
    unsigned idx = ii * 4;
    unsigned jj = ii * 3;
    m_data[idx + 0] = data1[jj + 0];
    m_data[idx + 1] = data1[jj + 1];
    m_data[idx + 2] = data1[jj + 2];
    m_data[idx + 3] = data2[ii];
  }
}

void VolumeRGBA::load(const fs::path &pfname,
    const ImageLoader &loader)
{
  this->loadBase(pfname, loader, 32);

  if(loader.hasPremultiplyAlpha())
  {
    this->premultiplyAlpha();
  }
}

void VolumeRGBA::premultiplyAlpha()
{
  for(uint8_t *ii = m_data, *ee = m_data + (m_w * m_h * m_d * 4); (ii != ee); ii += 4)
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
void VolumeRGBA::setPixel(unsigned px, unsigned py, unsigned pz,
    const Color &col)
{
  this->setPixel(px, py, pz,
      static_cast<uint8_t>(col.ri()),
      static_cast<uint8_t>(col.gi()),
      static_cast<uint8_t>(col.bi()),
      static_cast<uint8_t>(col.ai()));
}

