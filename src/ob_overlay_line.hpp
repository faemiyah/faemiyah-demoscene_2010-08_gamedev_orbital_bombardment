#ifndef OB_OVERLAY_LINE_HPP
#define OB_OVERLAY_LINE_HPP

#include "gfx/color.hpp"
#include "math/vec.hpp"

/** \brief Line Overlay line class.
 *
 * Line sprite is fed as:
 *
 * 2 texcoord values, allowing mirroring of coordinates.
 *
 * 4 color values, start point color, RGBA.
 *
 * 3 vertex values, XYZ position.
 *
 * 3 vertex values, XYZ position.
 */
class OverlayLine
{
  protected:
    /** Feedable data. */
    float m_data[12];

  public:
    /** \brief Get first color part of this overlay line.
     *
     * \return Color part.
     */
    gfx::Color getColor() const
    {
      return gfx::Color(m_data[2], m_data[3], m_data[4], m_data[5]);
    }

    /** \brief Get the texcoord part of this overlay line.
     *
     * \return Texcoord part.
     */
    math::vec2f getTexcoord() const
    {
      return math::vec2f(m_data[0], m_data[1]);
    }

    /** \brief Get first vertex part of this overlay line.
     *
     * \return Vertex part.
     */
    math::vec3f getVertex1() const
    {
      return math::vec3f(m_data[6], m_data[7], m_data[8]);
    }

    /** \brief Get second vertex part of this overlay line.
     *
     * \return Vertex part.
     */
    math::vec3f getVertex2() const
    {
      return math::vec3f(m_data[9], m_data[10], m_data[11]);
    }

    /** \brief Set color of this overlay line.
     *
     * \param op Color.
     */
    void setColor(const gfx::Color &op)
    {
      m_data[2] = op.r();
      m_data[3] = op.g();
      m_data[4] = op.b();
      m_data[5] = op.a();
    }

    /** \brief Set texcoord of this overlay line.
     *
     * \param op Texcoord modifier.
     */
    void setTexcoord(const math::vec2f &op)
    {
      m_data[0] = op.x();
      m_data[1] = op.y();
    }

    /** \brief Set first vertex part of this overlay line.
     *
     * \param op Vertex.
     */
    void setVertex1(const math::vec3f &op)
    {
      m_data[6] = op.x();
      m_data[7] = op.y();
      m_data[8] = op.z();
    }

    /** \brief Set second vertex part of this overlay line.
     *
     * \param op Vertex.
     */
    void setVertex2(const math::vec3f &op)
    {
      m_data[9] = op.x();
      m_data[10] = op.y();
      m_data[11] = op.z();
    }

  public:
    /** \brief Empty constructor.
    */
    OverlayLine() { }

    /** \brief Construct a new overlay line.
     *
     * \param ptexcoord Texcoord.
     * \param pcolor1 Color.
     * \param pvertex1 First vertex.
     * \param pvertex2 Second vertex.
     */
    OverlayLine(const math::vec2f &ptexcoord, const gfx::Color &pcolor,
        const math::vec3f &pvertex1, const math::vec3f &pvertex2)
    {
      m_data[0] = ptexcoord.x();
      m_data[1] = ptexcoord.y();
      m_data[2] = pcolor.r();
      m_data[3] = pcolor.g();
      m_data[4] = pcolor.b();
      m_data[5] = pcolor.a();
      m_data[6] = pvertex1.x();
      m_data[7] = pvertex1.y();
      m_data[8] = pvertex1.z();
      m_data[9] = pvertex2.x();
      m_data[10] = pvertex2.y();
      m_data[11] = pvertex2.z();
    }

  public:
    /** \brief Get color pointer.
     *
     * \return Pointer for GL data.
     */
    float* colorPointer()
    {
      return m_data + 2;
    }

    /** \brief Get const color pointer.
     *
     * \return Pointer for GL data.
     */
    const float* colorPointer() const
    {
      return m_data + 2;
    }

    /** \brief Get texcoord pointer.
     *
     * \return Pointer for GL data.
     */
    float* texcoordPointer()
    {
      return m_data;
    }

    /** \brief Get const texcoord pointer.
     *
     * \return Pointer for GL data.
     */
    const float* texcoordPointer() const
    {
      return m_data;
    }

    /** \brief Get first vertex pointer.
     *
     * \return Pointer for GL data.
     */
    float* vertexPointer1()
    {
      return m_data + 6;
    }

    /** \brief Get const first vertex pointer.
     *
     * \return Pointer for GL data.
     */
    const float* vertexPointer1() const
    {
      return m_data + 6;
    }

    /** \brief Get second vertex pointer.
     *
     * \return Pointer for GL data.
     */
    float* vertexPointer2()
    {
      return m_data + 9;
    }

    /** \brief Get const second vertex pointer.
     *
     * \return Pointer for GL data.
     */
    const float* vertexPointer2() const
    {
      return m_data + 9;
    }
};

#endif
