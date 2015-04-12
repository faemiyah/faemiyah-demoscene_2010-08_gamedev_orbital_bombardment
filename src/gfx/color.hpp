#ifndef GFX_COLOR_HPP
#define GFX_COLOR_HPP

#include "gfx/generic.hpp"
#include "math/generic.hpp"

namespace gfx
{
  /** \brief Representation of one color.
   *
   * The colors are saved as single-precision floating point. While they can
   * be modified as integers, this happens in a floating-point level and will
   * sacrifice precision.
   *
   * The operations done to the color values are not bounded. The user is
   * responsible for keeping the values legal.
   */
  struct Color
  {
    /** Internal array. */
    float m_array[4];

    /** \brief Accessor.
     *
     * \return Component.
     */
    float& r()
    {
      return m_array[0];
    }

    /** \brief Accessor.
     *
     * \return Component.
     */
    const float& r() const
    {
      return m_array[0];
    }

    /** \brief Accessor.
     *
     * \return Component.
     */
    float& g()
    {
      return m_array[1];
    }

    /** \brief Accessor.
     *
     * \return Component.
     */
    const float& g() const
    {
      return m_array[1];
    }

    /** \brief Accessor.
     *
     * \return Component.
     */
    float& b()
    {
      return m_array[2];
    }

    /** \brief Accessor.
     *
     * \return Component.
     */
    const float& b() const
    {
      return m_array[2];
    }

    /** \brief Accessor.
     *
     * \return Component.
     */
    float& a()
    {
      return m_array[3];
    }

    /** \brief Accessor.
     *
     * \return Component.
     */
    const float& a() const
    {
      return m_array[3];
    }

    /** \brief Integer accessor.
     *
     * \return Component.
     */
    int ri() const
    {
      return math::lround(m_array[0] * 255.0f);
    }

    /** \brief Integer accessor.
     *
     * \return Component.
     */
    int gi() const
    {
      return math::lround(m_array[1] * 255.0f);
    }

    /** \brief Integer accessor.
     *
     * \return Component.
     */
    int bi() const
    {
      return math::lround(m_array[2] * 255.0f);
    }

    /** \brief Integer accessor.
     *
     * \return Component.
     */
    int ai() const
    {
      return math::lround(m_array[3] * 255.0f);
    }

    /** \brief Unsigned byte representation.
     *
     * Takes some time, so for most purposes it's better to precalculate this.
     *
     * \return Color representation fitting into one doubleword.
     */
    uint32_t ub4()
    {
      uint32_t ret;
      uint8_t *ret8 = reinterpret_cast<uint8_t*>(&ret);
      ret8[0] = static_cast<uint8_t>(this->ri());
      ret8[1] = static_cast<uint8_t>(this->gi());
      ret8[2] = static_cast<uint8_t>(this->bi());
      ret8[3] = static_cast<uint8_t>(this->ai());
      return ret;
    }

    /** \brief Integer set.
     *
     * \param op New component.
     */
    void setR(int op)
    {
      m_array[0] = static_cast<float>(op) / 255.0f;
    }

    /** \brief Integer set.
     *
     * \param op New component.
     */
    void setG(int op)
    {
      m_array[1] = static_cast<float>(op) / 255.0f;
    }

    /** \brief Integer set.
     *
     * \param op New component.
     */
    void setB(int op)
    {
      m_array[2] = static_cast<float>(op) / 255.0f;
    }

    /** \brief Integer set.
     *
     * \param op New component.
     */
    void setA(int op)
    {
      m_array[3] = static_cast<float>(op) / 255.0f;
    }

    /** \brief Set all values.
     *
     * Alpha can be omitted in which case it is 255.
     *
     * \param pr Red component.
     * \param pg Green component.
     * \param pb Blue component.
     * \param pa Alpha component.
     */
    void set(int pr, int pg, int pb, int pa = 255)
    {
      this->setR(pr);
      this->setG(pg);
      this->setB(pb);
      this->setA(pa);
    }

    /** \brief Set all values.
     *
     * Alpha can be omitted in which case it is 1.
     *
     * \param pr Red component.
     * \param pg Green component.
     * \param pb Blue component.
     * \param pa Alpha component.
     */
    void set(float pr, float pg, float pb, float pa = 1.0f)
    {
      m_array[0] = pr;
      m_array[1] = pg;
      m_array[2] = pb;
      m_array[3] = pa;
    }

    /** \brief Empty constructor.
     *
     * Only so that STL vector resize() would work.
     */
    Color() { }

    /** \brief Constructor for HTML notation.
     *
     * \param noration.
     */
    Color(const char *notation);

    /** \brief Constructor.
     *
     * Alpha can be omitted in which case it is 255.
     *
     * \param pr Red component.
     * \param pg Green component.
     * \param pb Blue component.
     * \param pa Alpha component.
     */
    Color(int pr, int pg, int pb, int pa = 255)
    {
      this->setR(pr);
      this->setG(pg);
      this->setB(pb);
      this->setA(pa);
    }

    /** \brief Constructor.
     *
     * Alpha can be omitted in which case it is 1.
     *
     * \param pr Red component.
     * \param pg Green component.
     * \param pb Blue component.
     * \param pa Alpha component.
     */
    Color(float pr, float pg, float pb, float pa = 1.0f)
    {
      m_array[0] = pr;
      m_array[1] = pg;
      m_array[2] = pb;
      m_array[3] = pa;
    }

    /** Create from CMYK values.
     *
     * Note: This is NOT a true CMYK conversion algorithm.
     *
     * \param pc Cyan component.
     * \param pm Magenta component.
     * \param py Yellow component.
     * \param pk Black component.
     * \param pa Alpha component.
     * \return New color.
     */
    static Color fromCMYK(float pc, float pm, float py, float pk, float pa);

    /** \brief Addition operator.
     *
     * \param rhs Right-hand-side operand.
     * \return New color.
     */
    Color operator+(const Color &rhs) const
    {
      return Color(m_array[0] + rhs.r(),
          m_array[1] + rhs.g(),
          m_array[2] + rhs.b(),
          m_array[3] + rhs.a());
    }

    /** \brief Multiplication operator.
     *
     * \param rhs Right-hand-side operand.
     * \return New color.
     */
    Color operator*(float rhs) const
    {
      return Color(m_array[0] * rhs,
          m_array[1] * rhs,
          m_array[2] * rhs,
          m_array[3] * rhs);
    }
  };

  /** Common color. */
  static const gfx::Color COLOR_GREEN(0.0f, 1.0f, 0.0f, 1.0f);
  /** Common color. */
  static const gfx::Color COLOR_WHITE(1.0f, 1.0f, 1.0f, 1.0f);
  /** Common color. */
  static const gfx::Color COLOR_YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
}

#endif
