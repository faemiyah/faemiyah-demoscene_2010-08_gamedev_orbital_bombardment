#include "gfx/texture_2d.hpp"

#include "data/log.hpp"
#include "gfx/image.hpp"
#include "gfx/image_png.hpp"
#include "thr/dispatch.hpp"
#include "ui/generic.hpp"

namespace fs = boost::filesystem;
using namespace gfx;

namespace data
{
  template<> Texture2D::store_type data::Storable<Texture2D, ImageLoader>::g_store(0);
}
Texture2D* Texture2D::g_temp_texture;
boost::filesystem::path Texture2D::g_temp_texture_name;

/** Maximum anisotropy. */
static float g_max_anisotropy = 0.0f;

/** Logging message.
 *
 * \param type Type of logging.
 * \param filename Filename to potentially log.
 */
static void log_open(const char *type, const fs::path &pfname)
{
  if(!pfname.empty())
  {
    std::ostringstream sstr;
    sstr << "loading " << type << " texture " << pfname;
    data::log(sstr.str());
  }
}

Texture2D::Texture2D(const boost::filesystem::path &pfname, const ImageLoader &loader)
{
  this->load(pfname, loader);
}

Texture2D::Texture2D(const ImageGray8 &pimg, const ImageLoader &loader)
{
  this->adapt(pimg, loader);
}

Texture2D::Texture2D(const ImageGray16 &pimg, const ImageLoader &loader)
{
  this->adapt(pimg, loader);
}

Texture2D::Texture2D(const ImageRGB &pimg, const ImageLoader &loader)
{
  this->adapt(pimg, loader);
}

Texture2D::Texture2D(const ImageRGBA &pimg, const ImageLoader &loader)
{
  this->adapt(pimg, loader);
}

void Texture2D::adapt(const ImageGray8 &pimg, const ImageLoader &loader)
{
  this->setInternalState(pimg.getWidth(), pimg.getHeight(), 8);
  log_open("gray8", pimg.getFilename());
  thr::wait_privileged(&Texture2D::upload, this, pimg.getData(), loader);
}

void Texture2D::adapt(const ImageGray16 &pimg, const ImageLoader &loader)
{
  this->setInternalState(pimg.getWidth(), pimg.getHeight(), 16);
  log_open("gray16", pimg.getFilename());
  thr::wait_privileged(&Texture2D::upload, this, pimg.getData(), loader);
}

void Texture2D::adapt(const ImageRGB &pimg, const ImageLoader &loader)
{
  this->setInternalState(pimg.getWidth(), pimg.getHeight(), 24);
  log_open("rgb", pimg.getFilename());
  thr::wait_privileged(&Texture2D::upload, this, pimg.getData(), loader);
}

void Texture2D::adapt(const ImageRGBA &pimg, const ImageLoader &loader)
{
  this->setInternalState(pimg.getWidth(), pimg.getHeight(), 32);
  log_open("rgba", pimg.getFilename());
  thr::wait_privileged(&Texture2D::upload, this, pimg.getData(), loader);
}

void Texture2D::load(const fs::path &pfname, const ImageLoader &loader)
{
  boost::scoped_ptr<Image> img(Image::create(pfname, loader));

  switch(img->getBpp())
  {
    case 32:
      this->adapt(*static_cast<ImageRGBA*>(img.get()), loader);
      break;

    case 24:
      this->adapt(*static_cast<ImageRGB*>(img.get()), loader);
      break;

    case 16:
      this->adapt(*static_cast<ImageGray16*>(img.get()), loader);
      break;

    case 8:
      this->adapt(*static_cast<ImageGray8*>(img.get()), loader);
      break;

    default:
      {
        std::ostringstream sstr;
        sstr << "unknown image bit depth: " << img->getBpp();
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
  }
}

void Texture2D::upload(const uint8_t *pdata, const ImageLoader &loader)
{
  this->unreserve();

  // Sanity check for row length.
  if(0 != (m_w * m_b / 8 % 4))
  {
    std::ostringstream sstr;
    sstr << "pixel unpack lenght for image of size " << m_w << 'x' << m_h << '@' << m_b <<
      ") is not divisible by 4";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  glEnable(GL_TEXTURE_2D);
  this->reserve();
  this->bind();

  if(0.0f >= g_max_anisotropy)
  {
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &g_max_anisotropy);
  }

  if(loader.getAnisotropy() > g_max_anisotropy)
  {
    std::ostringstream sstr;
    sstr << "image requested anisotropy " << loader.getAnisotropy() << " which is greater than maximum (" <<
      g_max_anisotropy << ')';
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  if(loader.hasNearest())
  {
    Texture2D::texParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    Texture2D::texParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    Texture2D::texParameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, loader.getAnisotropy());
  }
  else // Default to trilinear.
  {
    Texture2D::texParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    Texture2D::texParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    Texture2D::texParameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, loader.getAnisotropy());
  }

  if(loader.hasGenerateMipmaps())
  {
    Texture2D::texParameter(GL_GENERATE_MIPMAP, GL_TRUE);
  }

  GLenum wrap = loader.hasClamp() ? GL_CLAMP_TO_EDGE : GL_REPEAT;
  Texture2D::texParameter(GL_TEXTURE_WRAP_S, wrap);
  Texture2D::texParameter(GL_TEXTURE_WRAP_T, wrap);

  GLenum pformat = bpp_to_pformat(m_b);
  glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(pformat), static_cast<GLsizei>(m_w),
      static_cast<GLsizei>(m_h), 0, pformat, GL_UNSIGNED_BYTE, pdata);

  if(!loader.hasGenerateMipmaps())
  {
    glGenerateMipmap(GL_TEXTURE_2D);
  }
}

Texture2D::container_type Texture2D::createImplementation(const fs::path &pfname, const ImageLoader &loader)
{
  return container_type(new Texture2D(pfname, loader));
}

const Texture2D* Texture2D::replaceTempTexture(const char *pfname, const ImageLoader &loader)
{
  // If clear.
  if(NULL == pfname)
  {
    if(NULL != g_temp_texture)
    {
      storageRemove(g_temp_texture_name);
      g_temp_texture = NULL;
      g_temp_texture_name.clear();
    }
    return NULL;
  }

  // Need canonname if not empty.
  fs::path canon = canonize(fs::path(pfname));

  // Might already match.
  if(NULL != g_temp_texture)
  {
    if(g_temp_texture_name == canon)
    {
      return g_temp_texture;
    }

    // Remove previous.
    storageRemove(g_temp_texture_name);
    g_temp_texture = NULL;
    g_temp_texture_name.clear();
  }

  // Get new.
  container_type container = create(pfname, loader);
  g_temp_texture = container.get().get();
  g_temp_texture_name = canon;
  return g_temp_texture;
}

std::ostream& gfx::operator<<(std::ostream &lhs, const Texture2D &rhs)
{
  lhs << "Texture: " << rhs.getWidth() << "x" << rhs.getHeight() << ' ';
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
