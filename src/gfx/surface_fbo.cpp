#include "gfx/surface_fbo.hpp"

#include "thr/dispatch.hpp"

using namespace gfx;

SurfaceFbo::SurfaceFbo(unsigned pw, unsigned ph, unsigned pb, bool pdepth) :
  Surface(pw, ph, pb),
  m_fbo_id(0),
  m_depth_id(0)
{
  if((24 != m_b) && (32 != m_b))
  {
    std::ostringstream sstr;
    sstr << "invalid FBO bit depth: " << m_b;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  thr::wait_privileged(&SurfaceFbo::reserve, this, pdepth);
}

SurfaceFbo::~SurfaceFbo()
{
  thr::wait_privileged(&SurfaceFbo::unreserve, this);
}

void SurfaceFbo::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
}

void SurfaceFbo::clear(bool pc, bool pd)
{
  GLuint clear_flags = 0;

  if(pc)
  {
    if(24 == m_b)
    {
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
    clear_flags |= GL_COLOR_BUFFER_BIT;
  }  
  if(pd && (0 < m_depth_id))
  {
    glClearDepthf(1.0f);
    clear_flags |= GL_DEPTH_BUFFER_BIT;
  }

  gfx::clear_framebuffer(clear_flags);
}

void SurfaceFbo::reserve(bool pdepth)
{
  // generate the texture
  m_tex.reserve();
  m_tex.bind();
  if(24 == m_b)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(m_w), static_cast<GLsizei>(m_h), 0, GL_RGB,
        GL_UNSIGNED_BYTE, 0);
  }
  else
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(m_w), static_cast<GLsizei>(m_h), 0, GL_RGBA,
        GL_UNSIGNED_BYTE, 0);
  }
  Texture2D::texParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR); // no mipmaps
  Texture2D::texParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  Texture2D::unbind();

  // generate depth texture
  if(pdepth)
  {
    glGenRenderbuffers(1, &m_depth_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_w, m_h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }

  // generate framebuffer
  glGenFramebuffers(1, &m_fbo_id);
  this->bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex.id(), 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_id);

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
  if(GL_FRAMEBUFFER_COMPLETE != status)
  {
    std::ostringstream sstr;
    sstr << "framebuffer error: " << status;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  this->unbind();
}

void SurfaceFbo::setBoundary(int px, int py, unsigned pw, unsigned ph)
{
  glViewport(px, py, static_cast<GLsizei>(pw), static_cast<GLsizei>(ph));

  if((0 == px) && (0 == py) && (m_w == pw) && (m_h == ph))
  {
    glDisable(GL_SCISSOR_TEST);
  }
  else
  {
    glEnable(GL_SCISSOR_TEST);
    glScissor(px, py, static_cast<GLsizei>(pw), static_cast<GLsizei>(ph));
  }

  this->bind();
}

void SurfaceFbo::unbind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SurfaceFbo::unreserve()
{
  glDeleteFramebuffers(1, &m_fbo_id);
  m_tex.unreserve();
  if(m_depth_id > 0)
  {
    glDeleteRenderbuffers(1, &m_depth_id);
  }
}

void SurfaceFbo::update()
{
  this->unbind();
}

