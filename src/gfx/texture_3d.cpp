#include "gfx/texture_3d.hpp"

#include "data/log.hpp"
#include "gfx/volume.hpp"
#include "thr/dispatch.hpp"
#include "ui/generic.hpp"

namespace fs = boost::filesystem;
using namespace gfx;

namespace data
{
  template<> Texture3D::store_type data::Storable<Texture3D, ImageLoader>::g_store(0);
}

/** Logging message.
 *
 * \param type Type of logging.
 * \param filename Filename to potentially log.
 */
static void log_open(const char *type, const fs::path &pfname)
{
  if(!pfname.empty())
  {
    std::stringstream sstr;
    sstr << "loading " << type << " volume " << pfname;
    data::log(sstr.str());
  }
}

Texture3D::Texture3D(unsigned px, unsigned py, unsigned pz, unsigned pb, const void *pdata,
    const ImageLoader &loader) :
  VolumeBase(px, py, pz, pb)
{
  thr::wait_privileged(&Texture3D::upload, this, pdata, loader);
}

Texture3D::Texture3D(const fs::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

Texture3D::Texture3D(const VolumeGray8 &volume, const ImageLoader &loader)
{
  this->adapt(volume, loader);
}

Texture3D::Texture3D(const VolumeGray16 &volume, const ImageLoader &loader)
{
  this->adapt(volume, loader);
}

Texture3D::Texture3D(const VolumeRGB &volume, const ImageLoader &loader)
{
  this->adapt(volume, loader);
}

Texture3D::Texture3D(const VolumeRGBA &volume, const ImageLoader &loader)
{
  this->adapt(volume, loader);
}

void Texture3D::adapt(unsigned pw, unsigned ph, unsigned pd, unsigned pb, const void *data,
    const ImageLoader &loader)
{
  this->setInternalState(pw, ph, pd, pb);
  thr::wait_privileged(&Texture3D::upload, this, data, loader);
}

void Texture3D::adapt(const VolumeGray8 &volume, const ImageLoader &loader)
{
  this->setInternalState(volume.getWidth(), volume.getHeight(), volume.getDepth(), 8);
  log_open("gray8", volume.getFilename());
  thr::wait_privileged(&Texture3D::upload, this, volume.getData(), loader);
}

void Texture3D::adapt(const VolumeGray16 &volume, const ImageLoader &loader)
{
  this->setInternalState(volume.getWidth(), volume.getHeight(), volume.getDepth(), 16);
  log_open("gray16", volume.getFilename());
  thr::wait_privileged(&Texture3D::upload, this, volume.getData(), loader);
}

void Texture3D::adapt(const VolumeRGB &volume, const ImageLoader &loader)
{
  this->setInternalState(volume.getWidth(), volume.getHeight(), volume.getDepth(), 24);
  log_open("rgb", volume.getFilename());
  thr::wait_privileged(&Texture3D::upload, this, volume.getData(), loader);
}

void Texture3D::adapt(const VolumeRGBA &volume, const ImageLoader &loader)
{
  this->setInternalState(volume.getWidth(), volume.getHeight(), volume.getDepth(), 32);
  log_open("rgba", volume.getFilename());
  thr::wait_privileged(&Texture3D::upload, this, volume.getData(), loader);
}

void Texture3D::load(const fs::path &pfname, const ImageLoader &loader)
{
  boost::scoped_ptr<Volume> img(Volume::create(pfname, loader));

  switch(img->getBpp())
  {
    case 32:
      this->adapt(*static_cast<VolumeRGBA*>(img.get()), loader);
      break;

    case 24:
      this->adapt(*static_cast<VolumeRGB*>(img.get()), loader);
      break;

    case 16:
      this->adapt(*static_cast<VolumeGray16*>(img.get()), loader);
      break;

    case 8:
      this->adapt(*static_cast<VolumeGray8*>(img.get()), loader);
      break;

    default:
      {
        std::ostringstream sstr;
        sstr << "unknown image bit depth: " << img->getBpp();
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
  }
}

void Texture3D::upload(const void *pdata, const ImageLoader &loader)
{
  this->unreserve();

  glEnable(GL_TEXTURE_3D);
  glGenTextures(1, &m_id);
  this->bind();

  if(loader.hasNearest())
  {
    Texture3D::texParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    Texture3D::texParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else
  {
    Texture3D::texParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    Texture3D::texParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  if(loader.hasGenerateMipmaps())
  {
    Texture3D::texParameter(GL_GENERATE_MIPMAP, GL_TRUE);
  }

  GLenum wrap = loader.hasClamp() ? GL_CLAMP_TO_EDGE : GL_REPEAT;
  Texture3D::texParameter(GL_TEXTURE_WRAP_S, wrap);
  Texture3D::texParameter(GL_TEXTURE_WRAP_T, wrap);
  Texture3D::texParameter(GL_TEXTURE_WRAP_R, wrap);

  GLenum pformat = bpp_to_pformat(m_b);
  glTexImage3D(GL_TEXTURE_3D, 0, pformat, m_w, m_h, m_d, 0, pformat, GL_UNSIGNED_BYTE, pdata);

  if(!loader.hasGenerateMipmaps())
  {
    glGenerateMipmap(GL_TEXTURE_3D);
  }
}

Texture3D::container_type Texture3D::createImplementation(const fs::path &pfname, const ImageLoader &loader)
{
  return container_type(new Texture3D(pfname, loader));
}

std::ostream& gfx::operator<<(std::ostream &lhs, const Texture3D &rhs)
{
  lhs << "Volume: " << rhs.getWidth() << "x" << rhs.getHeight() << "x" << rhs.getDepth() << ' ';
  switch(rhs.getBpp())
  {
    case 8:
      return lhs << "GRAY";

    case 24:
      return lhs << "RGB";

    case 32:
      return lhs << "RGBA";

    default:
      return lhs << "UNKNOWN";
  }
}
