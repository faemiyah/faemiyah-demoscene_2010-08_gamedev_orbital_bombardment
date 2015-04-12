#include "gfx/surface.hpp"

#include "gfx/buffer.hpp"
#include "gfx/font.hpp"

#include <sstream>

using namespace gfx;

const float Surface::DEFAULT_FONT_CEIL = 0.51f;
const float Surface::DEFAULT_FONT_FLOOR = 0.41f;
ArrayA4f Surface::g_array_color(PRIMITIVE_ARRAY_SIZE);
ArrayA4f Surface::g_array_vertex(PRIMITIVE_ARRAY_SIZE);
Shader *Surface::g_shader_2d = NULL;
Shader *Surface::g_shader_2d_font = NULL;
Shader *Surface::g_shader_2d_texture = NULL;
math::mat4f Surface::g_matrix_projection;
math::mat4f Surface::g_matrix_stack;
float Surface::g_shader_2d_mul;

math::rect2f Surface::getArea() const
{
  float ww = static_cast<float>(m_w),
        hh = static_cast<float>(m_h);

  if(m_w > m_h)
  {
    return math::rect2f(0.0f, 0.0f, ww / hh, 1.0f);
  }
  return math::rect2f(0.0f, 0.0f, 1.0f, hh / ww);
}

void Surface::select2D(unsigned px, unsigned py, unsigned pw, unsigned ph)
{
  float h_1 = static_cast<float>(m_h - 1),
        w_1 = static_cast<float>(m_w - 1),
        divw = 1.0f / static_cast<float>(m_w),
        divh = 1.0f / static_cast<float>(m_h),
        divwh = w_1 / h_1,
        divhw = h_1 / w_1;

  if(m_w > m_h)
  {
    g_matrix_projection.loadOrthoProjection(-1.0f + divw, -1.0f + divh, (2.0f - 2.0f * divw) * divhw,
        2.0f - 2.0f * divh);
    g_shader_2d_mul = 1.0f / h_1;
  }
  else
  {
    g_matrix_projection.loadOrthoProjection(-1.0f + divw,  -1.0f + divh, 2.0f - 2.0f * divw,
        (2.0f - 2.0f * divh) * divwh);
    g_shader_2d_mul = 1.0f / w_1;
  }

  mode_blend(PREMULTIPLIED);
  mode_cull(NONE);
  mode_depth(NONE);

  this->setBoundary(static_cast<int>(px), static_cast<int>(py), pw, ph);
}

void Surface::select3D(unsigned px, unsigned py, unsigned pw, unsigned ph, float pview, float paspect,
    float pnear, float pfar)
{
  float ratiox = static_cast<float>(pw) * paspect,
        ratioy = static_cast<float>(ph);

  if(ratiox > ratioy)
  {
    ratiox = static_cast<float>(ratiox) / static_cast<float>(ratioy);
    ratioy = 1.0f;
  }
  else
  {
    ratioy = static_cast<float>(ratioy) / static_cast<float>(ratiox);
    ratiox = 1.0f;
  }

  pview = atanf((ratioy / ratiox) * tanf(pview * 0.5f)) * 2.0f;
  //std::cout << "ratios: " << ratiox << " / " << ratioy << " -> " << pview << std::endl;
  g_matrix_projection.loadPerspective(pview, ratiox / ratioy, pnear, pfar);

  mode_cull(BACK);
  mode_depth(gfx::TEST_LEQUAL_AND_WRITE);

  this->setBoundary(static_cast<int>(px), static_cast<int>(py), pw, ph);
}

math::rect2i Surface::toPixelArea(const math::rect2f &parea) const
{
  float ww = static_cast<float>(m_w),
        hh = static_cast<float>(m_h);

  if(ww > hh)
  {
    return math::rect2i(math::lround(parea.x1() * ww / (ww / hh)),
        math::lround(parea.y1() * hh),
        math::lround(parea.w() * ww / (ww / hh)),
        math::lround(parea.h() * hh));
  }
  return math::rect2i(math::lround(parea.x1() * ww),
      math::lround(parea.y1() * hh / (hh / ww)),
      math::lround(parea.w() * ww),
      math::lround(parea.h() * hh / (hh / ww)));
}

void gfx::bind_shader_2d()
{
  Shader *sh = Surface::g_shader_2d;

  if(sh->bind())
  {
    const Attribute &cc = sh->getAttrColor();
    const Attribute &vv = sh->getAttrVertex();

    sh->bind();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    enable_buffers(cc, vv);

    Surface::g_array_color.feed(cc);
    Surface::g_array_vertex.feed(vv);
  }
}

void gfx::bind_shader_2d_font(float fnt_bd, float fnt_aa)
{
  Shader *sh = Surface::g_shader_2d_font;

  if(sh->bind())
  {
    const Attribute &cc = sh->getAttrColor();
    const Attribute &vv = sh->getAttrVertex();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    enable_buffers(cc, vv);

    Surface::g_array_color.feed(cc);
    Surface::g_array_vertex.feed(vv);

    sh->getUniformTexParams().update(fnt_bd, fnt_aa);
    sh->getUniformTex().update(0);
    glActiveTexture(GL_TEXTURE0);
  }
}

void gfx::bind_shader_2d_texture()
{
  Shader *sh = Surface::g_shader_2d_texture;

  if(sh->bind())
  {
    const Attribute &cc = sh->getAttrColor();
    const Attribute &vv = sh->getAttrVertex();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    enable_buffers(cc, vv);

    Surface::g_array_color.feed(cc);
    Surface::g_array_vertex.feed(vv);

    sh->getUniformTex().update(0);
    glActiveTexture(GL_TEXTURE0);
  }
}

void gfx::bind_shader_3d(const Shader &sh)
{
  sh.bind();
}

void gfx::bind_shader_3d_sprite(const Shader &sh)
{
  if(sh.bind())
  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    sh.getUniformTex().update(0);
    glActiveTexture(GL_TEXTURE0);
  }
}

void gfx::draw_pixel(int px, int py, const Color &pc)
{
  draw_fill(0, px, py);
  draw_fill(0, pc);
  glDrawArrays(GL_POINTS, 0, 1);
}

void gfx::draw_pixel(float px, float py, const Color &pc)
{
  draw_fill(0, px, py);
  draw_fill(0, pc);
  glDrawArrays(GL_POINTS, 0, 1);
}

void gfx::draw_pixel(int px, int py)
{
  draw_fill(0, px, py);
  glDrawArrays(GL_POINTS, 0, 1);
}

void gfx::draw_pixel(float px, float py)
{
  draw_fill(0, px, py);
  glDrawArrays(GL_POINTS, 0, 1);
}

void gfx::draw_line(int x1, int y1, int x2, int y2, const Color &pc1, const Color &pc2)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  draw_fill(0, pc1);
  draw_fill(1, pc2);
  glDrawArrays(GL_LINES, 0, 2);
}

void gfx::draw_line(float x1, float y1, float x2, float y2, const Color &pc1, const Color &pc2)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  draw_fill(0, pc1);
  draw_fill(1, pc2);
  glDrawArrays(GL_LINES, 0, 2);
}

void gfx::draw_line(int x1, int y1, int x2, int y2, const Color &pc)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  draw_fill(0, pc);
  draw_fill(1, pc);
  glDrawArrays(GL_LINES, 0, 2);
}

void gfx::draw_line(float x1, float y1, float x2, float y2, const Color &pc)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  draw_fill(0, pc);
  draw_fill(1, pc);
  glDrawArrays(GL_LINES, 0, 2);
}

void gfx::draw_line(int x1, int y1, int x2, int y2)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  glDrawArrays(GL_LINES, 0, 2);
}

void gfx::draw_line(float x1, float y1, float x2, float y2)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  glDrawArrays(GL_LINES, 0, 2);
}

void gfx::draw_line_loop(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, const Color &pc)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  draw_fill(2, x3, y3);
  draw_fill(3, x4, y4);
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void gfx::draw_line_loop(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,
    const Color &pc)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  draw_fill(2, x3, y3);
  draw_fill(3, x4, y4);
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void gfx::draw_line_loop(int x1, int y1, int x2, int y2, int x3, int y3,
    int x4, int y4)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  draw_fill(2, x3, y3);
  draw_fill(3, x4, y4);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void gfx::draw_line_loop(float x1, float y1, float x2, float y2, float x3,
    float y3, float x4, float y4)
{
  draw_fill(0, x1, y1);
  draw_fill(1, x2, y2);
  draw_fill(2, x3, y3);
  draw_fill(3, x4, y4);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void gfx::draw_rect(int px, int py, int pw, int ph, const Color &pc)
{
  int x2 = px + pw - 1,
      y2 = py + ph - 1;
  draw_fill(0, px, py);
  draw_fill(1, x2, py);
  draw_fill(2, px, y2);
  draw_fill(3, x2, y2);
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gfx::draw_rect(float px, float py, float pw, float ph, const Color &pc)
{
  float x2 = px + pw,
        y2 = py + ph;
  draw_fill(0, px, py);
  draw_fill(1, x2, py);
  draw_fill(2, px, y2);
  draw_fill(3, x2, y2);
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gfx::draw_rect(int px, int py, int pw, int ph)
{
  int x2 = px + pw - 1,
      y2 = py + ph - 1;
  draw_fill(0, px, py);
  draw_fill(1, x2, py);
  draw_fill(2, px, y2);
  draw_fill(3, x2, y2);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gfx::draw_rect(float px, float py, float pw, float ph)
{
  float x2 = px + pw,
        y2 = py + ph;
  draw_fill(0, px, py);
  draw_fill(1, x2, py);
  draw_fill(2, px, y2);
  draw_fill(3, x2, y2);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gfx::draw_rect_contour(int px, int py, int pw, int ph,
    const Color &pc)
{
  int x2 = px + pw - 1,
      y2 = py + ph - 1;
  draw_line_loop(px, py, x2, py, x2, y2, px, y2, pc);
}

void gfx::draw_rect_contour(float px, float py, float pw, float ph, const Color &pc)
{
  float x2 = px + pw,
        y2 = py + ph;
  draw_line_loop(px, py, x2, py, x2, y2, px, y2, pc);
}

void gfx::draw_rect_contour(int px, int py, int pw, int ph)
{
  int x2 = px + pw - 1,
      y2 = py + ph - 1;
  draw_line_loop(px, py, x2, py, x2, y2, px, y2);
}

void gfx::draw_rect_contour(float px, float py, float pw, float ph)
{
  float x2 = px + pw,
        y2 = py + ph;
  draw_line_loop(px, py, x2, py, x2, y2, px, y2);
}

void gfx::draw_rect_contour(int px, int py, int pw, int ph, int pb, const Color &pc)
{
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  draw_fill(4, pc);
  draw_fill(5, pc);
  draw_fill(6, pc);
  draw_fill(7, pc);
  draw_fill(8, pc);
  draw_fill(9, pc);
  draw_rect_contour(px, py, pw, ph, pb);
}

void gfx::draw_rect_contour(int px, int py, int pw, int ph, int pb)
{
  int x2 = px + pw - 1,
      y2 = py + ph - 1;

  draw_fill(0, px, py);
  draw_fill(1, px + pb, py + pb);
  draw_fill(2, x2, py);
  draw_fill(3, x2 - pb, py + pb);
  draw_fill(4, x2, y2);
  draw_fill(5, x2 - pb, y2 - pb);
  draw_fill(6, px, y2);
  draw_fill(7, px + pb, y2 -pb);
  draw_fill(8, px, py);
  draw_fill(9, px + pb, py + pb);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
}

void gfx::draw_rect_contour(float px, float py, float pw, float ph, float pb, const Color &pc)
{
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  draw_fill(4, pc);
  draw_fill(5, pc);
  draw_fill(6, pc);
  draw_fill(7, pc);
  draw_fill(8, pc);
  draw_fill(9, pc);
  draw_rect_contour(px, py, pw, ph, pb);
}

void gfx::draw_rect_contour(float px, float py, float pw, float ph, float pb)
{
  float x2 = px + pw,
        y2 = py + ph;

  draw_fill(0, px, py);
  draw_fill(1, px + pb, py + pb);
  draw_fill(2, x2, py);
  draw_fill(3, x2 - pb, py + pb);
  draw_fill(4, x2, y2);
  draw_fill(5, x2 - pb, y2 - pb);
  draw_fill(6, px, y2);
  draw_fill(7, px + pb, y2 -pb);
  draw_fill(8, px, py);
  draw_fill(9, px + pb, py + pb);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
}

void gfx::draw_rect_textured(int px, int py, int pw, int ph, const Color &pc, const Texture2D &pt)
{
  int x2 = px + pw - 1,
      y2 = py + ph - 1;
  pt.bind();
  draw_fill(0, px, py, 0.0f, 0.0f);
  draw_fill(1, x2, py, 1.0f, 0.0f);
  draw_fill(2, px, y2, 0.0f, 1.0f);
  draw_fill(3, x2, y2, 1.0f, 1.0f);
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gfx::draw_rect_textured(float px, float py, float pw, float ph, const Color &pc, const Texture2D &pt)
{
  float x2 = px + pw,
        y2 = py + ph;
  pt.bind();
  draw_fill(0, px, py, 0.0f, 0.0f);
  draw_fill(1, x2, py, 1.0f, 0.0f);
  draw_fill(2, px, y2, 0.0f, 1.0f);
  draw_fill(3, x2, y2, 1.0f, 1.0f);
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gfx::draw_rect_textured(float px, float py, float pw, float ph, const Color &pc, const Texture2D &pt,
    float srepeat, float trepeat)
{
  float x2 = px + pw,
        y2 = py + ph;
  pt.bind();
  draw_fill(0, px, py, 0.0f, 0.0f);
  draw_fill(1, x2, py, srepeat, 0.0f);
  draw_fill(2, px, y2, 0.0f, trepeat);
  draw_fill(3, x2, y2, srepeat, trepeat);
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gfx::draw_rect_textured(int px, int py, int pw, int ph, const Texture2D &pt)
{
  int x2 = px + pw - 1,
      y2 = py + ph - 1;
  pt.bind();
  draw_fill(0, px, py, 0.0f, 0.0f);
  draw_fill(1, x2, py, 1.0f, 0.0f);
  draw_fill(2, px, y2, 0.0f, 1.0f);
  draw_fill(3, x2, y2, 1.0f, 1.0f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gfx::draw_rect_textured(float px, float py, float pw, float ph, const Texture2D &pt)
{
  float x2 = px + pw,
        y2 = py + ph;
  pt.bind();
  draw_fill(0, px, py, 0.0f, 0.0f);
  draw_fill(1, x2, py, 1.0f, 0.0f);
  draw_fill(2, px, y2, 0.0f, 1.0f);
  draw_fill(3, x2, y2, 1.0f, 1.0f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gfx::draw_rect_textured_fill(const math::rect2f &area, const Color &col, const Texture2D &tex)
{
  float ratiot = static_cast<float>(tex.getWidth()) / static_cast<float>(tex.getHeight());
  float ratioa = area.w() / area.h();
  if(ratiot > ratioa)
  {
    gfx::draw_rect_textured(area.x1(), area.y1(),
        ratiot / ratioa * area.w(), area.h(),
        col, tex);
  }
  else
  {
    gfx::draw_rect_textured(area.x1(), area.y1(),
        area.w(), ratioa / ratiot * area.h(),
        col, tex);
  }
}

void gfx::draw_rect_textured_fit(const math::rect2f &area, const Color &col, const Texture2D &tex)
{
  float ratiot = static_cast<float>(tex.getWidth()) / static_cast<float>(tex.getHeight());
  float ratioa = area.w() / area.h();
  if(ratiot > ratioa)
  {
    gfx::draw_rect_textured(area.x1(), area.y1(),
        area.w(), ratioa / ratiot * area.h(),
        col, tex);
  }
  else
  {
    gfx::draw_rect_textured(area.x1(), area.y1(),
        ratiot / ratioa * area.w(), area.h(),
        col, tex);
  }
}

float gfx::draw_glyph(float px, float py, float fs, const Glyph &gly)
{
  const math::vec4f &corner1 = gly.getCorner1(),
                    &corner2 = gly.getCorner2();
  float rendx1 = px + (corner1.x() * fs),
        rendy1 = py + (corner1.y() * fs),
        rendx2 = px + (corner2.x() * fs),
        rendy2 = py + (corner2.y() * fs);

  //if(gly.getCode() == 'T')
  //{
  //  std::cout << corner1 << " : " << corner2 << std::endl;
  //}

  gly.bind();
  draw_fill(0, rendx1, rendy1, corner1.z(), corner1.w());
  draw_fill(1, rendx2, rendy1, corner2.z(), corner1.w());
  draw_fill(2, rendx1, rendy2, corner1.z(), corner2.w());
  draw_fill(3, rendx2, rendy2, corner2.z(), corner2.w());
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  return gly.getAdvance().x() * fs;
}

unsigned gfx::draw_text_line(float px, float py, float fs, const std::wstring &text, const Font &fnt,
    unsigned idx)
{
  for(;; ++idx)
  {
    if(idx >= text.length())
    {
      return 0;
    }
    wchar_t cc = text[idx];
    if(cc == '\n')
    {
      return idx + 1;
    }

    px += draw_glyph(px, py, fs, fnt.getGlyph(cc));
  }
}

void gfx::draw_text(float px, float py, float fs, const std::wstring &text, const Font &fnt,
    TextJustify justify)
{
  switch(justify)
  {
    case LEFT:
      {
        unsigned idx = 0;
        do {
          idx = draw_text_line(px, py, fs, text, fnt, idx);
          py -= fs;
        } while(idx);
      }
      return;

    case RIGHT:
      {
        unsigned idx = 0;
        do {
          unsigned tmp;
          float fwid;
          boost::tie(tmp, fwid) = fnt.calcLineWidth(fs, text, idx);
          idx = draw_text_line(px - fwid, py, fs, text, fnt, idx);
          py -= fs;
        } while(idx);
      }
      return;

    case CENTER:
    default:
      {
        std::list<float> *rlen = fnt.calcTextLengths(fs, text);
        py += static_cast<float>(static_cast<int>(rlen->size()) - 2) * 0.5f * fs;
        unsigned idx = 0;
        BOOST_FOREACH(float fwid, *rlen)
        {
          idx = draw_text_line(px - fwid * 0.5f, py, fs, text, fnt, idx);
          py -= fs;
        }
        delete rlen;
      }
      return;
  }
}

void gfx::draw_text(float px, float py, float fs, const std::wstring &text, const Font &fnt, const Color &pc,
    TextJustify justify)
{
  draw_fill(0, pc);
  draw_fill(1, pc);
  draw_fill(2, pc);
  draw_fill(3, pc);
  draw_text(px, py, fs, text, fnt, justify);
}

std::ostream& gfx::operator<<(std::ostream &lhs, const Surface &rhs)
{
  return lhs << rhs.m_w << "x" << rhs.m_h << "@" << rhs.m_b << "bpp";
}

