#ifndef GFX_SURFACE_BASE_HPP
#define GFX_SURFACE_BASE_HPP

namespace gfx
{
  /** \brief Lowest common denominator between surfaces and textures.
   */
  class SurfaceBase
  {
    protected:
      /** Bit depth of this surface. */
      unsigned m_b;

      /** Height of this surface. */
      unsigned m_h;

      /** Width of this surface. */
      unsigned m_w;

    public:
      /** \brief Accessor.
       *
       * \return Bpp as unsigned.
       */
      unsigned getBpp() const
      {
        return m_b;
      }

      /** \brief Accessor.
       *
       * \return Height as unsigned.
       */
      unsigned getHeight() const
      {
        return m_h;
      }

      /** \brief Accessor.
       *
       * \return Width as unsigned.
       */
      unsigned getWidth() const
      {
        return m_w;
      }

      /** \brief Accessor.
       *
       * \return X size.
       */
      unsigned getSizeX() const
      {
        return m_w;
      }

      /** \brief Accessor.
       *
       * \return X size.
       */
      unsigned getSizeY() const
      {
        return m_h;
      }

    protected:
      /**  \brief Sets internal state.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pb Bit depth.
       */
      void setInternalState(unsigned pw, unsigned ph, unsigned pb);

    public:
      /** \brief Empty constructor. */
      SurfaceBase() { }

      /** \brief Constructor.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pb Bit depth.
       */
      SurfaceBase(unsigned pw, unsigned ph, unsigned pb);
  };
}

#endif
