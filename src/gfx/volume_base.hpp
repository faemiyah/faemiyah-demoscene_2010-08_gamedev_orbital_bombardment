#ifndef GFX_VOLUME_BASE
#define GFX_VOLUME_BASE

#include "gfx/surface_base.hpp"

namespace gfx
{
  /** \brief Base class for volumes. */
  class VolumeBase :
    public SurfaceBase
  {
    protected:
      /** Z dimension (depth). */
      unsigned m_d;

    public:
      /** \brief Accessor.
       *
       * \return Depth.
       */
      unsigned getDepth() const
      {
        return m_d;
      }

      /** \brief Accessor.
       *
       * \return X size.
       */
      unsigned getSizeZ() const
      {
        return m_d;
      }

    public:
      /** \brief Empty constructor. */
      VolumeBase() { }

      /** \brief Constructor. 
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       * \param pb Bit depth.
       */
      VolumeBase(unsigned pw, unsigned ph, unsigned pd, unsigned pb);

      /** \brief Destructor. */
      ~VolumeBase() { }

    protected:
      /**  \brief Sets internal state.
       *
       * \param pw Width.
       * \param ph Height.
       * \param pd Depth.
       * \param pb Bit depth.
       */
      void setInternalState(unsigned pw, unsigned ph, unsigned pd,
          unsigned pb);
  };
}

#endif
