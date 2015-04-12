#ifndef GFX_FONT_LOADER_HPP
#define GFX_FONT_LOADER_HPP

#include "data/loader_settings.hpp"

namespace gfx
{
  /** \brief Image loader settings. */
  class FontLoader :
    public data::LoaderSettings
  {
    public:
      /** \brief Default constructor. */
      FontLoader() { }
  };
}

#endif
