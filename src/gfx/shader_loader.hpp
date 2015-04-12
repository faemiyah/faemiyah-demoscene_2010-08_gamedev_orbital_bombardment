#ifndef GFX_SHADER_LOADER_HPP
#define GFX_SHADER_LOADER_HPP

#include "data/loader_settings.hpp"

namespace gfx
{
  /** \brief Image loader settings. */
  class ShaderLoader :
    public data::LoaderSettings
  {
    private:
      /** GLSL version. */
      unsigned m_version;

    public:
      /** \brief Default constructor. */
      ShaderLoader() :
        m_version(0) { }

    public:
      /** \brief What is the desired version?
       *
       * \return GLSL version code.
       */
      unsigned getVersion() const
      {
        return m_version;
      }

      /** \brief Set desired version.
       *
       * \param op New version.
       * \return A copy of this with given version set.
       */
      ShaderLoader& version(unsigned op)
      {
        m_version = op;
        return *this;
      }
  };
}

#endif
