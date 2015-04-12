#ifndef GFX_SURFACE_HPP
#define GFX_SURFACE_HPP

#include "gfx/array.hpp"
#include "gfx/color.hpp"
#include "gfx/light_directional.hpp"
#include "gfx/shader.hpp"
#include "gfx/surface_base.hpp"
#include "math/rect.hpp"

namespace gfx
{
  class Font;
  class Glyph;
  class Texture2D;

  /** \brief Drawing surface.
   *
   * This top-class surface contains both the basic elements of surfaces and
   * the general drawing primitives.
   *
   * The static variables assigned for the surface are in the class namespace
   * simply and only so that they should not pollute the gfx:: namespace, as
   * they are related to drawing and surfaces anyway. The primitive operations
   * are still on the global gfx namespace for code brevity.
   *
   * The class itself is used as an abstraction of any drawable surface,
   * i.e. screen or texture.
   */
  class Surface : public SurfaceBase
  {
    public:
      /** Default font distance field value above which full intensity is reached. */
      static const float DEFAULT_FONT_CEIL;

      /** Default font distance field value below which fragment is discarded. */
      static const float DEFAULT_FONT_FLOOR;

    protected:
      /** Point sprite buffer length. */
      static const int BILLBOARD_ARRAY_SIZE = 16384;

      /** Space in the primitive drawing buffers. */
      static const int PRIMITIVE_ARRAY_SIZE = 10;

      /** Color buffer for primitive operations. */
      static ArrayA4f g_array_color;

      /** Vertex buffer for primitive operations. */
      static ArrayA4f g_array_vertex;

      /** 2D shader. */
      static Shader *g_shader_2d;

      /** Font shader. */
      static Shader *g_shader_2d_font;

      /** Font shader. */
      static Shader *g_shader_2d_texture;

      /** Projection matrix for 3D shaders. */
      static math::mat4f g_matrix_projection;

      /** Projection matrix for 3D shaders. */
      static math::mat4f g_matrix_stack;

      /** Multiplier to be applied when inputting integer coordinates for
       * primitives. */
      static float g_shader_2d_mul;

    public:
      /** \brief Empty constructor. */
      Surface() { }

      /** \brief Constructor.
       *
       * \param pw Current width.
       * \param ph Current height.
       * \param pb Current bit depth.
       */
      Surface(unsigned pw, unsigned ph, unsigned pb) :
        SurfaceBase(pw, ph, pb) { }

      /** \brief Destructor. */
      virtual ~Surface() { }

    public:
      /** \brief Get the 2D area.
       *
       * Returns the floating-point area of this surface.
       *
       * \return Rectangle representing the area.
       */
      math::rect2f getArea() const;

      /** \brief Select this surface for 2D drawing.
       *
       * 2D drawing coordinates are mapped thus, that the screen maps from
       * lower left corner (0, 0) to the largest exact square that fits the
       * screen fully. If the screen is not a perfect square, the coordinates
       * extend further into that direction.
       * 
       * \param px Left border of clip rectangle.
       * \param py Bottom border of clip rectangle.
       * \param pw Width of the clip rectangle.
       * \param ph Height of the clip rectangle.
       */
      void select2D(unsigned px, unsigned py, unsigned pw, unsigned ph);

      /** \brief Select this surface for 3D drawing.
       *
       * The pixel aspect ratio is intended for displaying correct images on
       * monitors such as 1280x1024 with 4:3 aspect ratio, wherein the ratio
       * would be (4/3)/(5/4) = 1.0667
       *
       * The view angle is calculated in such a fashion, it is what would be
       * in effect if the screen was exactly square. non-square aspect ratio
       * will cause increased field of view to that direction and decreased to
       * another direction. This is opposed to most systems wherein the angle
       * is represented along the Y axis.
       *
       * \param px Left border of clip rectangle.
       * \param py Bottom border of clip rectangle.
       * \param pw Width of the clip rectangle.
       * \param ph Height of the clip rectangle.
       * \param pview View angle in radians assuming the screen was square.
       * \param paspect Pixel aspect ratio (defaults to 1).
       * \param pnear Near clip
       */
      void select3D(unsigned px, unsigned py, unsigned pw, unsigned ph, float pview, float paspect = 1.0f,
          float pnear = 1.0f, float pfar = 1024.0f);

      /** \brief Convert a floating-point area into a pixel area.
       *
       * \param parea Input area.
       * \return Same area in pixel coordinates.
       */
      math::rect2i toPixelArea(const math::rect2f &parea) const;

    public:
      /** \brief Clear the screen.
       *
       * \param pc Color buffer clear flag.
       * \param pd Depth buffer clear flag.
       */
      virtual void clear(bool pc, bool pd) = 0;

      /** \brief Set the clip boundary for this surface.
       * 
       * \param px Left border of clip rectangle.
       * \param py Bottom border of clip rectangle.
       * \param pw Width of the clip rectangle.
       * \param ph Height of the clip rectangle.
       */
      virtual void setBoundary(int px, int py, unsigned pw, unsigned ph) = 0;

      /** \brief Update the contents of the screen. */
      virtual void update() = 0;

    public:
      /** \brief Clear wrapper.
       *
       * Clears both depth and color buffer.
       */
      void clear()
      {
        this->clear(true, true);
      }

      /** \brief Clear wrapper.
       *
       * Depth buffer flag is assumed true.
       *
       * \param pc Color buffer clear flag.
       */
      void clear(bool pc)
      {
        this->clear(pc, true);
      }

      /** \brief Convert a 3D eye coordinate into 2D coordinate.
       *
       * \param epos Normalized eye coordinate position.
       * \return 2D pixel position.
       */
      math::vec2i convertTo2D(const math::vec4f &rpos) const
      {
        math::vec2f rr(rpos.x() / rpos.z(), rpos.y() / rpos.z());
        return math::vec2i(math::lround((rr.x() * 0.5f + 0.5f) * static_cast<float>(m_w)),
            math::lround((rr.y() * 0.5f + 0.5f) * static_cast<float>(m_h)));
      }

      /** \brief Project a real-world coordinate.
       *
       * \param rpos Real world coordinate position.
       * \return 2D pixel position.
       */
      math::vec2i project(const math::vec3f &rpos) const
      {
        math::vec4f tpos(rpos.x(), rpos.y(), rpos.z(), 1.0f);
        std::cout << "Projected position: " << get_matrix_stack() * tpos << std::endl;
        return this->convertTo2D(get_matrix_stack() * tpos);
      }

      /** \brief Select this surface for 2D drawing.
       *
       * A wrapper for 2D selection, uses surface size as the view area.
       */
      void select2D()
      {
        this->select2D(0, 0, m_w, m_h);
      }

      /** \brief Select this surface for 3D drawing.
       *
       * A wrapper for 3D selection, uses surface size as the view area.
       *
       * \param pview View angle along the shorter axis.
       * \param paspect Pixel aspect ratio (defaults to 1).
       */
      void select3D(float pview, float paspect = 1.0f, float pnear = 1.0f, float pfar = 1024.0f)
      {
        this->select3D(0, 0, m_w, m_h, pview, paspect, pnear, pfar);
      }

    public:
      /** \brief Accessor.
       *
       * \return The matrix stack.
       */
      static const math::mat4f& get_matrix_stack()
      {
        return g_matrix_stack;
      }

      /** \brief Accessor.
       *
       * \return 2d multiplier.
       */
      static float get_mul_2d()
      {
        return g_shader_2d_mul;
      }

      /** \brief Get the projection matrix.
       *
       * \return Projection matrix.
       */
      static const math::mat4f& get_projection()
      {
        return g_matrix_projection;
      }
     
    public:
      /** \cond */
      friend std::ostream& operator<<(std::ostream &lhs, const Surface &rhs);
      friend void bind_shader_2d();
      friend void bind_shader_2d_font();
      friend void bind_shader_2d_font(float, float);
      friend void bind_shader_2d_texture();
      friend void bind_shader_3d(const Shader&);
      friend void bind_shader_3d_sprite(const Shader&);
      friend inline void draw_fill(unsigned, const Color&);
      friend inline void draw_fill(unsigned, int, int);
      friend inline void draw_fill(unsigned, float, float);
      friend inline void draw_fill(unsigned, int, int, float, float);
      friend inline void draw_fill(unsigned, float, float, float, float);
      friend inline void draw_fill(float, float, float, const Color&, const math::vec3f&);
      friend inline void load_light(const LightDirectional&, const math::mat4f&);
      friend inline void load_identity();
      friend inline void load_projection();
      friend inline void load_transform(const math::mat4f&);
      /** \endcond */
  };

  /** \brief Output Surface to a stream.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Stream after input.
   */
  std::ostream& operator<<(std::ostream &lhs, const Surface &rhs);

  /** \brief Bind the default 2D shader.
   *
   * Shader bindings must be done after selecting a surface for drawing.
   */
  extern void bind_shader_2d();

  /** \brief Bind the default 2D font shader.
   *
   * Shader bindings must be done after selecting a surface for drawing.
   *
   * Also set non-default text parameters.
   * 
   * \param fnt_bd Font boldness.
   * \param fnt_aa Font antialiaising factor.
   */
  extern void bind_shader_2d_font(float fnt_bd, float fnt_aa);

  /** Specialization of bind_shader_2d_font.
   *
   * Supplies default values.
   */
  inline void bind_shader_2d_font()
  {
    bind_shader_2d_font(Surface::DEFAULT_FONT_FLOOR, Surface::DEFAULT_FONT_CEIL);
  }

  /** \brief Bind the default 2D texture shader.
   *
   * Shader bindings must be done after selecting a surface for drawing.
   */
  extern void bind_shader_2d_texture();

  /** \brief Bind the given 3D shader.
   *
   * Shader bindings must be done after selecting a surface for drawing.
   *
   * Binds light uniforms, modelview matrix and full matrix stack.
   *
   * \param sh Shader to bind.
   */
  extern void bind_shader_3d(const Shader &sh);

  /** \brief Bind the given point/line sprite 3D shader.
   *
   * Shader bindings must be done after selecting a surface for drawing.
   *
   * Billboard shaders will not bind light uniforms, but they will bind
   * projection matrix uniforms.
   *
   * \param sh Shader to bind.
   */
  extern void bind_shader_3d_sprite(const Shader &sh);

  /** \brief Draw one pixel.
   *
   * \param px X coordinate.
   * \param py Y coordinate.
   * \param pc Color.
   */
  extern void draw_pixel(int px, int py, const Color &pc);

  /** \brief Draw one pixel.
   *
   * \param px X coordinate.
   * \param py Y coordinate.
   * \param pc Color.
   */
  extern void draw_pixel(float px, float py, const Color &pc);

  /** \brief Draw one pixel.
   *
   * Uses the colors specified previously.
   *
   * \param px X coordinate.
   * \param py Y coordinate.
   */
  extern void draw_pixel(int x, int y);

  /** \brief Draw one pixel.
   *
   * Uses the colors specified previously.
   *
   * \param px X coordinate.
   * \param py Y coordinate.
   */
  extern void draw_pixel(float x, float y);

  /** \brief Draw a line.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   * \param pc1 First coordinate color.
   * \param pc2 Second coordinate color.
   */
  extern void draw_line(int x1, int y1, int x2, int y2, const Color &pc1, const Color &pc2);

  /** \brief Draw a line.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   * \param pc1 First coordinate color.
   * \param pc2 Second coordinate color.
   */
  extern void draw_line(float x1, float y1, float x2, float y2, const Color &pc1, const Color &pc2);

  /** \brief Draw a line.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   * \param pc Color to draw the line with.
   */
  extern void draw_line(int x1, int y1, int x2, int y2, const Color &pc);

  /** \brief Draw a line.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   * \param pc Color to draw the line with.
   */
  extern void draw_line(float x1, float y1, float x2, float y2, const Color &pc);

  /** \brief Draw a line.
   *
   * Uses the colors specified previously.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   */
  extern void draw_line(int x1, int y1, int x2, int y2);

  /** \brief Draw a line.
   *
   * Uses the colors specified previously.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   */
  extern void draw_line(float x1, float y1, float x2, float y2);

  /** \brief Draw a line loop.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   * \param x3 Third X coordinate.
   * \param y3 Third Y coordinate.
   * \param x4 Fourth X coordinate.
   * \param y4 Fourth Y coordinate.
   * \param pc Color to use.
   */
  extern void draw_line_loop(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, const Color &pc);

  /** \brief Draw a line loop.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   * \param x3 Third X coordinate.
   * \param y3 Third Y coordinate.
   * \param x4 Fourth X coordinate.
   * \param y4 Fourth Y coordinate.
   * \param pc Color to use.
   */
  extern void draw_line_loop(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,
      const Color &pc);

  /** \brief Draw a line loop.
   *
   * Use colors specified previously.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   * \param x3 Third X coordinate.
   * \param y3 Third Y coordinate.
   * \param x4 Fourth X coordinate.
   * \param y4 Fourth Y coordinate.
   * \param pc Color to use.
   */
  extern void draw_line_loop(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

  /** \brief Draw a line loop.
   *
   * Use colors specified previously.
   *
   * \param x1 First X coordinate.
   * \param y1 First Y coordinate.
   * \param x2 Second X coordinate.
   * \param y2 Second Y coordinate.
   * \param x3 Third X coordinate.
   * \param y3 Third Y coordinate.
   * \param x4 Fourth X coordinate.
   * \param y4 Fourth Y coordinate.
   * \param pc Color to use.
   */
  extern void draw_line_loop(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

  /** \brief Draw a rectangle.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pc Color.
   */
  extern void draw_rect(int px, int py, int pw, int ph, const Color &pc);

  /** \brief Draw a rectangle.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pc Color.
   */
  extern void draw_rect(float px, float py, float pw, float ph, const Color &pc);

  /** \brief Draw a rectangle.
   *
   * Uses the colors specified previously.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   */
  extern void draw_rect(int px, int py, int pw, int ph);

  /** \brief Draw a rectangle.
   *
   * Uses the colors specified previously.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   */
  extern void draw_rect(float px, float py, float pw, float ph);

  /** \brief Draw a contour rectangle.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pc Color.
   */
  extern void draw_rect_contour(int px, int py, int pw, int ph, const Color &pc);

  /** \brief Draw a contour rectangle.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pc Color.
   */
  extern void draw_rect_contour(float px, float py, float pw, float ph, const Color &pc);

  /** \brief Draw a contour rectangle.
   *
   * Uses the colors specified previously.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   */
  extern void draw_rect_contour(int px, int py, int pw, int ph);

  /** \brief Draw a contour rectangle.
   *
   * Uses the colors specified previously.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   */
  extern void draw_rect_contour(float px, float py, float pw, float ph);

  /** \brief Draw a contour rectangle.
   *
   * This version has a width inward.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pb Border width.
   * \param pc Color.
   */
  extern void draw_rect_contour(int px, int py, int pw, int ph, int pb, const Color &pc);

  /** \brief Draw a contour rectangle.
   *
   * This version has a width inward.
   *
   * Uses the colors specified previously.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pb Border width.
   */
  extern void draw_rect_contour(int px, int py, int pw, int ph, int pb);

  /** \brief Draw a contour rectangle.
   *
   * This version has a width inward.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pb Border width.
   * \param pc Color.
   */
  extern void draw_rect_contour(float px, float py, float pw, float ph, float pb, const Color &pc);

  /** \brief Draw a contour rectangle.
   *
   * This version has a width inward.
   *
   * Uses the colors specified previously.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pb Border width.
   */
  extern void draw_rect_contour(float px, float py, float pw, float ph, float pb);

  /** \brief Draw a textured rectangle.
   * 
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pc Color.
   * \param pt Texture.
   */
  extern void draw_rect_textured(int px, int py, int pw, int ph, const Color &pc, const Texture2D &t);

  /** \brief Draw a textured rectangle.
   * 
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pc Color.
   * \param pt Texture.
   */
  extern void draw_rect_textured(float px, float py, float pw, float ph, const Color &pc, const Texture2D &t);

  /** \brief Draw a textured rectangle.
   * 
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pc Color.
   * \param pt Texture.
   * \param srepeat S repeat.
   * \param trepeat T repeat.
   */
  extern void draw_rect_textured(float px, float py, float pw, float ph, const Color &pc, const Texture2D &pt,
      float srepeat, float trepeat);

  /** \brief Draw a textured rectangle.
   * 
   * Uses the colors specified previously.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pt Texture.
   */
  extern void draw_rect_textured(int px, int py, int pw, int ph, const Texture2D &pt);

  /** \brief Draw a textured rectangle.
   * 
   * Uses the colors specified previously.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param pw Width.
   * \param ph Height.
   * \param pt Texture.
   */
  extern void draw_rect_textured(float px, float py, float pw, float ph, const Texture2D &pt);

  /** \brief Fill an area but keep texture aspect ratio.
   *
   * Clips texture as needed.
   *
   * \param area Area to fill.
   * \param col Color to use.
   * \param tex Texture to use.
   */
  extern void draw_rect_textured_fill(const math::rect2f &area, const Color &col, const Texture2D &tex);

  /** \brief Fill a texture into an area, keeping the aspect ratio.
   *
   * \param area Area to fill.
   * \param col Color to use.
   * \param tex Texture to use.
   */
  extern void draw_rect_textured_fit(const math::rect2f &area, const Color &col, const Texture2D &tex);

  /** \brief Draw a glyph.
   *
   * Color must have been already specified.
   *
   * \param px Left x coordinate.
   * \param py Bottom y coordinate.
   * \param fs Font size.
   * \param gly Glyph to draw.
   * \return X distance to advance.
   */
  extern float draw_glyph(float px, float py, float fs, const Glyph &gly);

  /** \brief Draw one line of text.
   * 
   * The text is drawn from the lower left coordinate of the first glyph.
   * 
   * Color must have been already specified.
   * 
   * \param px Left x coordinate of the first character in the first row.
   * \param py Bottom y coordinate of the first character of the first row.
   * \param fs Font size.
   * \param text Text to render.
   * \param fnt Font to use.
   * \param idx Starting index (default: zero).
   * \return The index of the start of the next row or zero if at end.
   */
  extern unsigned draw_text_line(float px, float py, float fs, const std::wstring &text, const Font &fnt,
      unsigned idx = 0);

  /** \brief Draw some text.
   *
   * Color must have been already specified.
   *
   * \param px X coordinate of text center.
   * \param py Y coordinate of text center.
   * \param fs Font size.
   * \param text Text to render.
   * \param fnt Font to use.
   * \param pc Color.
   * \param justify Justification mode.
   */
  extern void draw_text(float x, float y, float fs, const std::wstring &text, const Font &fnt,
      TextJustify justify = LEFT);

  /** \brief Draw some text.
   * 
   * \param px X coordinate of text center.
   * \param py Y coordinate of text center.
   * \param fs Font size.
   * \param text Text to render.
   * \param fnt Font to use.
   * \param pc Color.
   * \param justify Justification mode.
   */
  extern void draw_text(float x, float y, float fs, const std::wstring &text, const Font &fnt, const Color &pc,
      TextJustify justify = LEFT);

  /** \brief Wrapper for draw_text.
   *
   * \param px X coordinate of text center.
   * \param py Y coordinate of text center.
   * \param fs Font size.
   * \param text Text to render.
   * \param fnt Font to use.
   * \param justify Text justify.
   */
  inline void draw_text(int px, int py, int fs,   const std::wstring &text, const Font &fnt,
      TextJustify justify = LEFT)
  {
    float mul2d = Surface::get_mul_2d();
    draw_text(static_cast<float>(px) * mul2d, static_cast<float>(py) * mul2d,
        static_cast<float>(fs) * mul2d, text, fnt, justify);
  }

  /** \brief Wrapper for draw_text.
   *
   * \param px X coordinate of text center.
   * \param py Y coordinate of text center.
   * \param fs Font size.
   * \param text Text to render.
   * \param pc Color.
   * \param justify Text justify.
   */
  inline void draw_text(int px, int py, int fs, const std::wstring &text, const Font &fnt, const Color &pc,
      TextJustify justify = LEFT)
  {
    float mul2d = Surface::get_mul_2d();
    draw_text(static_cast<float>(px) * mul2d, static_cast<float>(py) * mul2d,
        static_cast<float>(fs) * mul2d, text, fnt, pc, justify);
  }

  /** \brief Fill nth element of the color buffer.
   *
   * \param idx Color index.
   * \param pc Color to fill with.
   */
  inline void draw_fill(unsigned idx, const Color &pc)
  {
    idx *= 4;
    Surface::g_array_color[idx + 0] = pc.r();
    Surface::g_array_color[idx + 1] = pc.g();
    Surface::g_array_color[idx + 2] = pc.b();
    Surface::g_array_color[idx + 3] = pc.a();
  }

  /** \brief Fill nth element of the vertex buffer.
   *
   * \param idx Vertex index.
   * \param px X coordinate.
   * \param py Y coordinate.
   */
  inline void draw_fill(unsigned idx, int px, int py)
  {
    idx *= 4;
    Surface::g_array_vertex[idx + 0] = static_cast<float>(px) * Surface::get_mul_2d();
    Surface::g_array_vertex[idx + 1] = static_cast<float>(py) * Surface::get_mul_2d();
  }

  /** \brief Fill nth element of the vertex buffer.
   *
   * \param idx Vertex index.
   * \param px X coordinate.
   * \param py Y coordinate.
   */
  inline void draw_fill(unsigned idx, float px, float py)
  {
    idx *= 4;
    Surface::g_array_vertex[idx + 0] = px;
    Surface::g_array_vertex[idx + 1] = py;
  }

  /** \brief Fill nth element of the vertex buffer.
   *
   * \param idx Vertex index.
   * \param pp Coordinates.
   */
  inline void draw_fill(unsigned idx, const math::vec2f &pp)
  {
    draw_fill(idx, pp.x(), pp.y());
  }

  /** \brief Fill vertex buffer and the texture coordinates.
   *
   * \param idx Vertex index.
   * \param px X coordinate.
   * \param py Y coordinate.
   * \param ps S coordinate.
   * \param pt T coordinate.
   */
  inline void draw_fill(unsigned idx, int px, int py, float ps, float pt)
  {
    idx *= 4;
    Surface::g_array_vertex[idx + 0] = static_cast<float>(px) * Surface::get_mul_2d();
    Surface::g_array_vertex[idx + 1] = static_cast<float>(py) * Surface::get_mul_2d();
    Surface::g_array_vertex[idx + 2] = ps;
    Surface::g_array_vertex[idx + 3] = pt;
  }

  /** \brief Fill vertex buffer and the texture coordinates.
   *
   * \param idx Vertex index.
   * \param px X coordinate.
   * \param py Y coordinate.
   * \param ps S coordinate.
   * \param pt T coordinate.
   */
  inline void draw_fill(unsigned idx, float px, float py, float ps, float pt)
  {
    idx *= 4;
    Surface::g_array_vertex[idx + 0] = px;
    Surface::g_array_vertex[idx + 1] = py;
    Surface::g_array_vertex[idx + 2] = ps;
    Surface::g_array_vertex[idx + 3] = pt;
  }

  /** \brief Load a new directional light.
   *
   * \param op New directional light.
   * \param wm World matrix (camera orientation).
   */
  inline void load_light(const LightDirectional &op, const math::mat4f &wm)
  {
    const Shader *sh = Shader::getCurrent();

    sh->getUniformLightAmbient().update(op.getAmbient());
    sh->getUniformLightDiffuse().update(op.getDiffuse());
    sh->getUniformLightDir().update(math::normalize(-(wm * op.getDir())));
  }

  /** \brief Update matrix stack with identity matrix.
   */
  inline void load_identity()
  {
    Surface::g_matrix_stack = Surface::g_matrix_projection;
    Shader::getCurrent()->getUniformTransform().update(Surface::get_matrix_stack());
  }

  /** \brief Update projection matrix to the graphics card.
   *
   * Should only be called if necessary, normally only modelview and the full
   * matrix stack are needed.
   *
   * \param sh Shader to update to.
   */
  inline void load_projection()
  {
    Shader::getCurrent()->getUniformProjection().update(Surface::get_projection());
  }

  /** \brief Update matrix stack to the graphics card
   *
   * Sets the full transformation matrix prior to updating.
   *
   * \param op New modelview matrix.
   */
  inline void load_transform(const math::mat4f &op)
  {
    const Shader *sh = Shader::getCurrent();
    const Uniform *uniform_modelview = sh->getUniform("modelview"),
                  *uniform_transform = sh->getUniform("transform");

    if(NULL != uniform_modelview)
    {
      uniform_modelview->update(op);
    }
    if(NULL != uniform_transform)
    {
      Surface::g_matrix_stack = Surface::get_projection() * op;
      uniform_transform->update(Surface::get_matrix_stack());
    }
  }
}

#endif
