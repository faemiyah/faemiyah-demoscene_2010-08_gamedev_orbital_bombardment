#ifndef OB_MENU_HPP
#define OB_MENU_HPP

#include "defaults.hpp"

#include <string>
#include <vector>

namespace gfx
{
  struct Color;
}

namespace ob
{
  class Menu;

  /** Enumeration for potential menu effects to be used in the menu state. */
  enum MenuEnum
  {
    NONE = 0,
    DETAIL,
    FULLSCREEN,
    HELP,
    HIGH_SCORES,
    INVERT_MOUSE,
    QUIT,
    RESOLUTION,
    SENSITIVITY,
    START,
    VOLUME_MUSIC,
    VOLUME_SAMPLES
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Menu> MenuSptr;

  /** \brief Menu item
   *
   * Potentially contains other recursive menu items.
   */
  class Menu
  {
    private:
      /** Other menu items. */
      std::vector<MenuSptr> m_recursive;

      /** Parent item. */
      Menu *m_parent;

      /** Name of menu in UTF-8. */
      std::string m_name_utf8;

      /** Name of menu in wide characters. */
      std::wstring m_name_wide;

      /** Time spent in this menu item. */
      int m_focus_time;

      /** Relative time spent in this menu item. */
      float m_focus_time_float;

      /** Function of this menu. */
      MenuEnum m_func;

    public:
      /** \brief Constructor.
       *
       * \param name Menu name.
       * \param parent Parent menu (default: NULL).
       */
      Menu(const char *name, MenuEnum func = NONE, Menu *parent = NULL);

      /** \brief Destructor. */
      ~Menu() { }

    public:
      /** \brief Add a submenu.
       *
       * \param op Submenu.
       */
      void add(Menu *op);

      /** \brief Decrement focus time.
       *
       * \return The focus quotient, [0, 1] after operation.
       */
      float decFocusTime();

      /** \brief Increment focus time.
       *
       * \return The focus quotient, [0, 1] after operation.
       */
      float incFocusTime();

      /** \brief Get color depending on focus level.
       *
       * \param alpha Alpha component.
       * \return Color for this menu.
       */
      gfx::Color getFocusColor(float alpha);

      /** \brief Replace text of this menu.
       *
       * Replaces both UTF-8 and wide content.
       *
       * \param op New text.
       */
      void replaceText(const char *op);

      /** \brief Change text in this.
       *
       * Change parameters come from func in this.
       *
       * \param op Direction of change.
       */
      void updateText(int op);
      
    public:
      /** \brief Get a certain child.
       *
       * \param op Index.
       */
      Menu* getChild(unsigned op)
      {
        return m_recursive[op].get();
      }

      /** \brief Get a certain child.
       *
       * \param op Index.
       */
      const Menu* getChild(unsigned op) const
      {
        return m_recursive[op].get();
      }

      /** \brief Accessor.
       *
       * \return Current focus time.
       */
      int getFocusTime() const
      {
        return m_focus_time;
      }

      /** \brief Accessor.
       *
       * \return Current focus time.
       */
      float getFocusTimeFloat() const
      {
        return m_focus_time_float;
      }

      /** \brief Get the function.
       *
       * \return Function.
       */
      MenuEnum getFunc()
      {
        return m_func;
      }

      /** \brief Accessor.
       *
       * \return Parent menu item.
       */
      Menu* getParent()
      {
        return m_parent;
      }

      /** \brief Accessor.
       *
       * \return Parent menu item.
       */
      const Menu* getParent() const
      {
        return m_parent;
      }

      /** \brief Accessor.
       *
       * \return Recursive items.
       */
      std::vector<MenuSptr>& getRecursive()
      {
        return m_recursive;
      }

      /** \brief Accessor.
       *
       * \return Recursive items.
       */
      const std::vector<MenuSptr>& getRecursive() const
      {
        return m_recursive;
      }

      /** \brief Get number of recursive elements.
       *
       * \return Number of recursive elements.
       */
      unsigned getNumRecursive() const
      {
        return static_cast<unsigned>(m_recursive.size());
      }

      /** \brief Get the UTF-8 text content.
       *
       * \return Normal string text of this.
       */
      const std::string& getTextUtf8() const
      {
        return m_name_utf8;
      }

      /** \brief Get the text content.
       *
       * \return Wide string text of this.
       */
      const std::wstring& getText() const
      {
        return m_name_wide;
      }

      /** \brief Replace text of this menu.
       *
       * Replaces both UTF-8 and wide content.
       *
       * \param op New text.
       */
      void replaceText(const std::string &op)
      {
        this->replaceText(op.c_str());
      }

      /** \brief Set the focus time.
       *
       * \param op New focus time.
       */
      void setFocusTime(int op)
      {
        m_focus_time = op;
      }
  };
}

#endif
