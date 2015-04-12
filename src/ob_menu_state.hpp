#ifndef OB_MENU_STATE_HPP
#define OB_MENU_STATE_HPP

#include "gfx/texture_2d.hpp"
#include "ui/ui_state.hpp"
#include "ob_appearing_string.hpp"

#include <vector>

namespace ob
{
  class Game;
  class Menu;

  /** \brief Menu UI state.
   */
  class MenuState :
    public ui::UiState
  {
    private:
      /** Top-level menu. */
      Menu *m_menu;

      /** Current menu. */
      Menu *m_menu_curr;

      /** Menu getting removed. */
      Menu *m_menu_prev;

      /** Fade effect. */
      float m_fade;

      /** Fade effect. */
      float m_fade_delta;

      /** Selected menu index. */
      int m_index;

      /** Title appearing shitty. */
      AppearingString m_title;

      /** Version shitty. */
      AppearingString m_version;

      /** Info page displayed. */
      int m_page;

      /** Available help pages. */
      std::vector<const char*> m_help_pages;

    public:
      /** \brief Constructor.
       *
       * \param menu Menu to start from.
       * \param idx Index in that menu (or 0)
       */
      MenuState(Menu *menu, unsigned idx = 0);

      /** \brief Destructor. */
      virtual ~MenuState() { }

    public:
      /** \cond */
      virtual bool handleKey(const ui::EventKey &ev, ui::UiStack &st);
      virtual void reinstate(ui::UiStack &st);
      virtual void render(ui::UiStack &st, gfx::SurfaceScreen &screen);
      virtual void update(ui::UiStack &st);
      /** \endcond */

    private:
      /** \brief Activate a menu item.
       *
       * \param op Item to activate.
       * \param mode Activation mode, left, enter, right (neg, 0, pos).
       */
      void activate(Menu *op, int mode, ui::UiStack &st);

      /** \brief Ascend into upwards menu.
       */
      void ascend();

      /** \brief Return index to safe areas.
       */
      void checkIndex();

      /** \brief Descend into downwards menu.
       */
      void descend(Menu *op);

      /** \brief Check if the options screen sould be displayed.
       *
       * \return True if yes, false if no.
       */
      bool isOptionsMenu() const;

    public:
      /** \brief Overlay replacement task.
       *
       * \param pfname New overlay texture or NULL for delete.
       */
      void taskTexture(const char *pfname);

    public:
      /** \brief Create the game menu.
       *
       * Called once in globals constructor.
       *
       * \return Game menu.
       */
      static Menu* create_menu_game();

      /** \brief Create the main menu.
       *
       * Called once in globals constructor.
       *
       * \return Main menu.
       */
      static Menu* create_menu_main();
  };
}

#endif
