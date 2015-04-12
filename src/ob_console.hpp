#ifndef OB_CONSOLE_HPP
#define OB_CONSOLE_HPP

#include "ui/console.hpp"

namespace ob
{
  /** \brief Console implementation.
   */
  class Console :
    public ui::Console
  {
    public:
      /** \brief Constructor.
       *
       * \param fnt Font to use.
       * \param fs Font size for this text box.
       * \param screen Screen to take size from.
       */
      Console(const gfx::Font &fnt, float fs,
          const gfx::SurfaceScreen &screen);

      /** \brief Destructor. */
      virtual ~Console() { }

    public:
      /** \cond */
      virtual void render(gfx::SurfaceScreen &screen);
      /** \endcond */
  };
}

#endif
