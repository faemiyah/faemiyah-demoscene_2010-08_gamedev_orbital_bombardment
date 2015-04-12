#ifndef GFX_POINT_SPRITE_HPP
#define GFX_POINT_SPRITE_HPP

#include "gfx/color.hpp"
#include "math/vec.hpp"

namespace gfx
{
  /** \brief Point sprite class.
   *
   * Point sprite data is fed as:
   *
   * 4 morph values, this is the top left 2x2 part of a 3x3 matrix in
   * column-major order. The shader uses this for 2D transformations.
   *
   * 4 color values, RGBA.
   *
   * 4 vertex values, XYZ position and size of point sprite in world coordinates.
   *
   * For convenience, texcoord attribute is used for morph.
   */
  class PointSprite
  {
    protected:
      /** Feedable data. */
      float m_data[12];

    public:
      /** \brief Get color part of this point sprite.
       *
       * \return Color part.
       */
      gfx::Color getColor() const
      {
        return gfx::Color(m_data[4], m_data[5], m_data[6], m_data[7]);
      }

      /** \brief Get alpha part of this point sprite.
       *
       * \return Alpha part.
       */
      float getColorAlpha() const
      {
        return m_data[7];
      }

      /** \brief Get size of this point sprite.
       *
       * \return Size.
       */
      float getSize() const
      {
        return m_data[11];
      }

      /** \brief Get the morph part of this point sprite.
       *
       * \return Morph.
       */
      math::vec4f getMorph() const
      {
        return math::vec4f(m_data[0], m_data[1], m_data[2], m_data[3]);
      }

      /** \brief Get vertex part of this point sprite.
       *
       * \return Vertex part.
       */
      math::vec3f getVertex() const
      {
        return math::vec3f(m_data[8], m_data[9], m_data[10]);
      }

      /** \brief Set color of this point sprite.
       *
       * \param op Color.
       */
      void setColor(const gfx::Color &op)
      {
        m_data[4] = op.r();
        m_data[5] = op.g();
        m_data[6] = op.b();
        m_data[7] = op.a();
      }

      /** \brief Set alpha component of this point sprite.
       *
       * \param op Alpha.
       */
      void setColorAlpha(float op)
      {
        m_data[7] = op;
      }

      /** \brief Set color of this point sprite, ignore alpha.
       *
       * \param op Color.
       */
      void setColorRGB(const gfx::Color &op)
      {
        m_data[4] = op.r();
        m_data[5] = op.g();
        m_data[6] = op.b();
      }

      /** \brief Set the morph component of this point sprite.
       *
       * \param op Morph.
       */
      void setMorph(const math::vec4f& op)
      {
        m_data[0] = op.x();
        m_data[1] = op.y();
        m_data[2] = op.z();
        m_data[3] = op.w();
      }

      /** \brief Set morph to be rotation-only for this point sprite.
       *
       * \param op Rotation.
       */
      void setRotation(float op)
      {
        float cr = cosf(op);
        float sr = sinf(op);

        m_data[0] = cr;
        m_data[1] = -sr;
        m_data[2] = sr;
        m_data[3] = cr;
      }

      /** \brief Set morph to be rotation and scale for this point sprite.
       *
       * \param rot Rotation.
       * \param scalex X scale.
       * \param scaley Y scale.
       *
       */
      void setRotationScale(float rot, float scalex, float scaley)
      {
        float cr = cosf(rot);
        float sr = sinf(rot);

        m_data[0] = scalex * cr;
        m_data[1] = -scalex * sr;
        m_data[2] = scaley * sr;
        m_data[3] = scaley * cr;
      }

      /** \brief Set morph to be scale-only for this point sprite.
       *
       * \param scalex X scale.
       * \param scaley Y scale.
       */
      void setScale(float scalex, float scaley)
      {
        m_data[0] = scalex;
        m_data[1] = 0.0f;
        m_data[2] = 0.0f;
        m_data[3] = scaley;
      }

      /** \brief Set size of this point sprite.
       *
       * \param op Size.
       */
      void setSize(float op)
      {
        m_data[11] = op;
      }

      /** \brief Set vertex part of this point sprite.
       *
       * \param op Vertex.
       */
      void setVertex(const math::vec3f &op)
      {
        m_data[8] = op.x();
        m_data[9] = op.y();
        m_data[10] = op.z();
      }

    public:
      /** \brief Empty constructor.
       */
      PointSprite() { }

      /** \brief Construct a new point sprite.
       *
       * \param pmorph Morph.
       * \param pcolor Color.
       * \param pvertex Vertex.
       * \param psize Point size.
       */
      PointSprite(const math::vec4f &pmorph, const Color &pcolor,
          const math::vec3f &pvertex, float psize)
      {
        m_data[0] = pmorph.x();
        m_data[1] = pmorph.y();
        m_data[2] = pmorph.z();
        m_data[3] = pmorph.w();
        m_data[4] = pcolor.r();
        m_data[5] = pcolor.g();
        m_data[6] = pcolor.b();
        m_data[7] = pcolor.a();
        m_data[8] = pvertex.x();
        m_data[9] = pvertex.y();
        m_data[10] = pvertex.z();
        m_data[11] = psize;
      }

      /** \brief Construct a new point sprite.
       *
       * Generate morph parameter from x, y and scale.
       *
       * \param prot Rotation.
       * \param pscalex X scale.
       * \param pscaley Y scale.
       * \param pcolor Color.
       * \param pvertex Vertex.
       * \param psize Point size.
       */
      PointSprite(float prot, float pscalex, float pscaley, const Color &pcolor,
          const math::vec3f &pvertex, float psize)
      {
        float cr = cosf(prot);
        float sr = sinf(prot);
        m_data[0] = pscalex * cr;
        m_data[1] = -pscalex * sr;
        m_data[2] = pscaley * sr;
        m_data[3] = pscaley * cr;
        m_data[4] = pcolor.r();
        m_data[5] = pcolor.g();
        m_data[6] = pcolor.b();
        m_data[7] = pcolor.a();
        m_data[8] = pvertex.x();
        m_data[9] = pvertex.y();
        m_data[10] = pvertex.z();
        m_data[11] = psize;
      }

      /** \brief Construct a new point sprite.
       *
       * Omit scale, the point sprite is only rotated.
       *
       * \param prot Rotation (in radians).
       * \param pcolor Color.
       * \param pvertex Vertex.
       * \param psize Point size.
       */
      PointSprite(float prot, const Color &pcolor, const math::vec3f &pvertex,
          float psize)
      {
        float cr = cosf(prot);
        float sr = sinf(prot);
        m_data[0] = cr;
        m_data[1] = -sr;
        m_data[2] = sr;
        m_data[3] = cr;
        m_data[4] = pcolor.r();
        m_data[5] = pcolor.g();
        m_data[6] = pcolor.b();
        m_data[7] = pcolor.a();
        m_data[8] = pvertex.x();
        m_data[9] = pvertex.y();
        m_data[10] = pvertex.z();
        m_data[11] = psize;
      }

      /** \brief Construct a new point sprite.
       *
       * Omit morph, the point sprite is generated centered.
       *
       * \param pcolor Color.
       * \param pvertex Vertex.
       * \param psize Point size.
       */
      PointSprite(const Color &pcolor, const math::vec3f &pvertex, float psize)
      {
        m_data[0] = 1.0f;
        m_data[1] = 0.0f;
        m_data[2] = 0.0f;
        m_data[3] = 1.0f;
        m_data[4] = pcolor.r();
        m_data[5] = pcolor.g();
        m_data[6] = pcolor.b();
        m_data[7] = pcolor.a();
        m_data[8] = pvertex.x();
        m_data[9] = pvertex.y();
        m_data[10] = pvertex.z();
        m_data[11] = psize;
      }

    public:
      /** \brief Get color pointer.
       *
       * \return Pointer for GL data.
       */
      float* colorPointer()
      {
        return m_data + 4;
      }

      /** \brief Get const color pointer.
       *
       * \return Pointer for GL data.
       */
      const float* colorPointer() const
      {
        return m_data + 4;
      }

      /** \brief Get texcoord pointer.
       *
       * \return Pointer for GL data.
       */
      float* texCoordPointer()
      {
        return m_data;
      }

      /** \brief Get texcoord pointer.
       *
       * \return Pointer for GL data.
       */
      const float* texCoordPointer() const
      {
        return m_data;
      }

      /** \brief Get vertex pointer.
       *
       * \return Pointer for GL data.
       */
      float* vertexPointer()
      {
        return m_data + 8;
      }

      /** \brief Get vertex pointer.
       *
       * \return Pointer for GL data.
       */
      const float* vertexPointer() const
      {
        return m_data + 8;
      }
  };
}

#endif
