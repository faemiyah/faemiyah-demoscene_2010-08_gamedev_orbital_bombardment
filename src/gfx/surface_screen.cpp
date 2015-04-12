#include "gfx/surface_screen.hpp"

#include "gfx/image.hpp"

#include "boost/lexical_cast.hpp"

#include <sstream>

namespace fs = boost::filesystem;
using namespace gfx;

SurfaceScreen::SurfaceScreen(unsigned pw, unsigned ph, unsigned pb, bool fs) :
  m_screen(NULL)
{
  this->setInternalState(pw, ph, pb);

  SDL_Init(SDL_INIT_VIDEO);
  setGlAttributes();
  m_screen = SDL_SetVideoMode(static_cast<int>(pw), static_cast<int>(ph), static_cast<int>(pb),
      createSdlFlags(fs));

  GLenum glew_err = glewInit();
  if(GLEW_OK != glew_err)
  {
    std::stringstream err;
    err << glewGetErrorString(glew_err);
    BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
  }

  // Reserve internal OpenGL variables.
  try
  {
    if(NULL == g_shader_2d)
    {
      Shader::container_type &cc = Shader::instanciate("shader/2d.xml");
      cc.setPersistent(true);
      g_shader_2d = cc.at();
    }
    if(NULL == g_shader_2d_font)
    {
      Shader::container_type &cc = Shader::instanciate("shader/2d_font.xml");
      cc.setPersistent(true);
      g_shader_2d_font = cc.at();
    }
    if(NULL == g_shader_2d_texture)
    {
      Shader::container_type &cc = Shader::instanciate("shader/2d_texture.xml");
      cc.setPersistent(true);
      g_shader_2d_texture = cc.at();
    }
  }
  catch(std::runtime_error err)
  {
    std::cerr << err.what() << std::endl;
  }
}

SurfaceScreen::~SurfaceScreen()
{
  if(m_screen)
  {
    SDL_FreeSurface(m_screen);
    SDL_Quit();
  }
}

void SurfaceScreen::clear(bool pc, bool pd)
{
  GLuint clear_flags = 0;
  
  if(pc)
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    clear_flags |= GL_COLOR_BUFFER_BIT;
  }
  if(pd)
  {
    glClearDepthf(1.0f);
    clear_flags |= GL_DEPTH_BUFFER_BIT;
  }

  clear_framebuffer(clear_flags);
}

void SurfaceScreen::save(const fs::path &pfname)
{
  ImageRGB image(m_w, m_h);

  glReadPixels(0, 0, static_cast<GLsizei>(m_w), static_cast<GLsizei>(m_h), GL_RGB, GL_UNSIGNED_BYTE,
      image.getData());

  image.write(pfname);
}

void SurfaceScreen::save(const std::string &pfname)
{
  this->save(fs::path(pfname));
}
  
void SurfaceScreen::setBoundary(int px, int py, unsigned pw, unsigned ph)
{
  glViewport(px, py, static_cast<GLsizei>(pw), static_cast<GLsizei>(ph));

  if((0 == px) && (0 == py) && (m_w == pw) && (m_h == ph))
  {
    mode_scissor(false);
  }
  else
  {
    mode_scissor(px, py, pw, ph);
  }
}

void SurfaceScreen::update()
{
  SDL_GL_SwapBuffers();
}

boost::tuple<unsigned, unsigned, unsigned> SurfaceScreen::parseResolution(const std::string &op)
{
  std::string before;
  size_t ca = op.find("@");
  unsigned width;
  unsigned height;
  unsigned bpp = 32;

  if(std::string::npos != ca)
  {
    std::string after = op.substr(ca + 1);

    bpp = boost::lexical_cast<unsigned>(after);

    if((bpp != 8) && (bpp != 16) && (bpp != 24) && (bpp != 32))
    {
      std::ostringstream sstr;
      sstr << "invalid bit depth in resolution string '" << op << '\'';
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }

    before = op.substr(0, ca);
  }
  else
  {
    before = op;
  }

  size_t cx = before.find("x");

  if(std::string::npos != cx)
  {
    std::string width_string = before.substr(0, cx);
    std::string height_string = before.substr(cx + 1);

    width = boost::lexical_cast<unsigned>(width_string);
    height = boost::lexical_cast<unsigned>(height_string);

    if((0 >= width) || (0 >= height))
    {
      std::ostringstream sstr;
      sstr << "invalid width x height in resolution string '" << op << '\'';
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }
  else
  {
    cx = op.find("p");
  
    if(std::string::npos == cx)
    {
      std::ostringstream sstr;
      sstr << "invalid resolution string '" << op << '\'';
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
    
    std::string progressive = op.substr(0, cx);
    height = boost::lexical_cast<unsigned>(op.substr(0, cx));

    switch(height)
    {
      case 1080:
        width = 1920;
        break;

      case 720:
        width = 1280;
        break;

      default:
        {
          std::ostringstream sstr;
          sstr << "invalid progressive mode identifier in resolution string '" << op << '\'';
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
        break;
    }
  }

  return boost::make_tuple(width, height, bpp);
}

