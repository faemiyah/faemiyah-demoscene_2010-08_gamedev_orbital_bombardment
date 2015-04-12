#include "ob_menu_state.hpp"

#include "snd/generic.hpp"
#include "thr/dispatch.hpp"
#include "ui/ui_stack.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"
#include "ob_high_score_state.hpp"
#include "ob_menu.hpp"

using namespace ob;

/** Focused menu item. */
static const float FONT_SIZE_MAX = 1.25f * OB_FONT_SIZE;

/** Title font size. */
static const float FONT_SIZE_TITLE = OB_FONT_SIZE * 1.67f;

/** Version font size. */
static const float FONT_SIZE_VERSION = OB_FONT_SIZE * 0.67f;

/** Gap value. */
static const float TITLE_BOTTOM = FONT_SIZE_TITLE * 0.2f;

/** Gap value. */
static const float VERSION_BOTTOM = FONT_SIZE_VERSION * 0.2f;

/** \brief Collect text lengths.
 *
 * \param tgt Target vector.
 * \param src Source menus.
 * \param global Global multiplier.
 */
void collect_widths(std::vector<float> &tgt, const std::vector<MenuSptr> &src, float global)
{
  BOOST_FOREACH(const MenuSptr &vv, src)
  {
    const Menu *cmenu = vv.get();
    float fsize = math::mix(OB_FONT_SIZE, FONT_SIZE_MAX, cmenu->getFocusTimeFloat());
    int idx;
    float width;
    boost::tie(idx, width) = glob->getFont().calcLineWidth(fsize, cmenu->getText());
    tgt.push_back(width * global);
  }
}

/** \brief Draw a menu.
 *
 * \param wid Width vector.
 * \param mvec Menu vector.
 * \param global Global multiplier.
 * \param maxwid Maximum width encountered.
 */
void draw_menu_list(std::vector<float> &wid, const std::vector<MenuSptr> &mvec, float global, float maxwid,
    const math::rect2f &sarea)
{
  float FONT_TOP = static_cast<float>(mvec.size()) * (FONT_SIZE_MAX + OB_FONT_GAP) + OB_FONT_GAP;

  for(unsigned ii = 0; (ii < mvec.size()); ++ii)
  {
    gfx::Color mcolor = mvec[ii]->getFocusColor(global);
    gfx::draw_fill(0, mcolor);
    gfx::draw_fill(1, mcolor);
    gfx::draw_fill(2, mcolor);
    gfx::draw_fill(3, mcolor);

    gfx::draw_text_line(sarea.w() - OB_FONT_GAP - maxwid * global + (maxwid - wid[ii]),
        FONT_TOP - static_cast<float>(ii + 1) * (FONT_SIZE_MAX + OB_FONT_GAP) + OB_FONT_BOTTOM,
        math::mix(OB_FONT_SIZE, FONT_SIZE_MAX, mvec[ii]->getFocusTimeFloat()),
        mvec[ii]->getText(), glob->getFont());
  }
}

MenuState::MenuState(Menu *menu, unsigned idx) :
  m_menu(menu),
  m_menu_curr(menu),
  m_menu_prev(NULL),
  m_index(static_cast<int>(idx)),
  m_page(-1)
{
  m_help_pages.push_back("gfx/instructions_1.png");
  m_help_pages.push_back("gfx/instructions_2.png");
  m_help_pages.push_back("gfx/instructions_3.png");
  m_help_pages.push_back("gfx/instructions_4.png");
  m_help_pages.push_back("gfx/instructions_5.png");

  m_title.set(m_menu->getText());
  m_version.set(ui::wstr_utf8(VERSION));
}

void MenuState::activate(Menu *op, int mode, ui::UiStack &st)
{
  if((0 == mode) && (op->getNumRecursive() > 0))
  {
    this->descend(op);
  }

  if(op->getFunc() == HELP)
  {
    m_page = math::max(math::min(m_page + mode, static_cast<int>(m_help_pages.size()) - 1), 0);
    gfx::Texture2D::replaceTempTexture(m_help_pages[static_cast<unsigned>(m_page)]);
    return;
  }
  m_page = -1;
  if(!isOptionsMenu())
  {
    gfx::Texture2D::replaceTempTexture(NULL);
  }

  switch(op->getFunc())
  {
    case HIGH_SCORES:
      if(0 == mode)
      {
        st.pushState(new HighScoreState());
      }
      break;

    case QUIT:
      if(0 == mode)
      {
        if(st.getNumStates() >= 2)
        {
          st.switchTopStates();
          st.killTopState();
        }
        else
        {
          this->die();
        }
      }
      break;

    case START:
      if(st.getNumStates() >= 2)
      {
        st.switchTopStates();
      }
      else
      {
        glob_queue_game();
      }
      break;

    default:
      op->updateText(mode);
      break;
  }
}

void MenuState::ascend()
{
  this->descend(m_menu_curr->getParent());
}

void MenuState::checkIndex()
{
  if(m_index >= static_cast<int>(m_menu_curr->getNumRecursive()))
  {
    --m_index;
  }
  if(0 > m_index)
  {
    m_index = 0;
  }
  if((m_menu_curr->getChild(static_cast<unsigned>(m_index))->getFunc() != HELP) &&
      !isOptionsMenu())
  {
    gfx::Texture2D::replaceTempTexture(NULL);
  }
}

void MenuState::descend(Menu *op)
{
  m_menu_prev = m_menu_curr;
  m_menu_curr = op;
  // Sanity check.
  if(NULL == m_menu_curr)
  {
    m_menu_curr = m_menu;
  }
  // Check if no move.
  if(m_menu_prev == m_menu_curr)
  {
    m_menu_prev = NULL;
  }
  else
  {  
    m_menu_curr->setFocusTime(0);
    m_title.set(m_menu_curr->getText());
    m_version.set((NULL == m_menu_curr->getParent()) ? ui::wstr_utf8(VERSION) : std::wstring());

    // Options screen has a background.
    if(isOptionsMenu())
    {
      gfx::Texture2D::replaceTempTexture("gfx/options_background.png");
    }
    else
    {
      gfx::Texture2D::replaceTempTexture(NULL);
    }
  }
  this->checkIndex();
}

bool MenuState::handleKey(const ui::EventKey &ev, ui::UiStack &st)
{
  if(ev.isPress())
  {
    switch(ev.getCode())
    {
      case SDLK_RETURN:
        this->activate(m_menu_curr->getChild(static_cast<unsigned>(m_index)), 0, st);
        break;

      case SDLK_BACKSPACE:
      case SDLK_WORLD_7:
        this->ascend();
        break;

      case SDLK_ESCAPE:
        if((HELP == m_menu_curr->getChild(static_cast<unsigned>(m_index))->getFunc()) &&
            (NULL != gfx::Texture2D::getTempTexture()))
        {
          gfx::Texture2D::replaceTempTexture(NULL);
          m_page = -1;
        }
        else if(NULL == m_menu_curr->getParent())
        {
          if(st.getNumStates() >= 2)
          {
            st.switchTopStates();
          }
          else
          {
            m_index = static_cast<int>(m_menu_curr->getNumRecursive()) - 1;
            this->checkIndex();
          }
        }
        else
        {
          this->ascend();
        }
        break;

      case SDLK_LEFT:
        this->activate(m_menu_curr->getChild(static_cast<unsigned>(m_index)), -1, st);
        break;

      case SDLK_RIGHT:
        this->activate(m_menu_curr->getChild(static_cast<unsigned>(m_index)), 1, st);
        break;

      case SDLK_UP:
        --m_index;
        this->checkIndex();
        break;
        
      case SDLK_DOWN:
        ++m_index;
        this->checkIndex();
        break;

      case SDLK_HOME:
      case SDLK_PAGEUP:
        m_index = 0;
        break;

      case SDLK_END:
      case SDLK_PAGEDOWN:
        m_index = static_cast<int>(m_menu->getNumRecursive());
        if(0 < m_index)
        {
          --m_index;
        }
        break;

      default:
        break;
    }
  }

  return true;
}

bool MenuState::isOptionsMenu() const
{
  return (0 == m_menu_curr->getTextUtf8().compare("Options"));
}

void MenuState::reinstate(ui::UiStack &st)
{
  ui::key_repeat_on();

  BOOST_FOREACH(MenuSptr &vv, m_menu_curr->getRecursive())
  {
    if(vv->getFunc() == START)
    {
      vv->replaceText((st.getNumStates() > 1) ? "Continue" : "Start");
    }
    if(vv->getFunc() == QUIT)
    {
      vv->replaceText((st.getNumStates() > 1) ? "End game" : "Exit");
    }
  }
}

void MenuState::render(ui::UiStack &st, gfx::SurfaceScreen &screen)
{
  Game *cg = glob_get_game();
  math::rect2f sarea = screen.getArea();

  screen.clear(true, true);
  screen.select2D();

  if(!cg)
  {    
    gfx::bind_shader_2d_texture();
    gfx::load_identity();

    gfx::draw_rect_textured_fill(sarea, gfx::Color(1.0f, 1.0f, 1.0f, 1.0f), glob->getTextureMenu());
  }

  std::vector<float> currlen, prevlen;
  float maxwidth_prev = FLT_MIN;
  if(m_menu_prev)
  {
    collect_widths(prevlen, m_menu_prev->getRecursive(), m_menu_prev->getFocusTimeFloat());
    BOOST_FOREACH(float vv, prevlen)
    {
      maxwidth_prev = math::max(maxwidth_prev, vv);
    }
  }
  float maxwidth_curr = FLT_MIN;
  collect_widths(currlen, m_menu_curr->getRecursive(), m_menu_curr->getFocusTimeFloat());
  BOOST_FOREACH(float vv, currlen)
  {
    maxwidth_curr = math::max(maxwidth_curr, vv);
  }
  float maxwidth = math::max(maxwidth_curr, maxwidth_prev);
  math::vec2f corner(sarea.w() - maxwidth - OB_FONT_GAP * 2.0f,
      sarea.h() - FONT_SIZE_TITLE - OB_FONT_GAP * 2.0f);

  // If game is on, draw the game state here.
  if(NULL != cg)
  {
    cg->renderSub(screen, st, INITIAL,
        static_cast<unsigned>(0), static_cast<unsigned>(0),
        static_cast<unsigned>(corner.x() / gfx::Surface::get_mul_2d()),
        static_cast<unsigned>(corner.y() / gfx::Surface::get_mul_2d()));
    screen.select2D();
  }

  gfx::bind_shader_2d();
  gfx::load_identity();

  gfx::draw_rect(0.0f, corner.y(), sarea.w(), sarea.h() - corner.y(), OB_COLOR_SHADE);
  gfx::draw_rect(corner.x(), 0.0f, sarea.w() - corner.x(), corner.y());
  gfx::draw_fill(0, 0.0f, corner.y());
  gfx::draw_fill(1, corner.x(), corner.y());
  gfx::draw_fill(2, corner.x(), 0.0f);
  gfx::draw_fill(0, OB_COLOR_MENU_BORDER);
  gfx::draw_fill(1, OB_COLOR_MENU_BORDER);
  gfx::draw_fill(2, OB_COLOR_MENU_BORDER);
  glLineWidth(OB_LINE_WIDTH);
  glDrawArrays(GL_LINE_STRIP, 0, 3);
  glLineWidth(1.0f);
  //gfx::draw_line(corner.x(), corner.y(), 0.0f, corner.y(), OB_COLOR_BORDER);
  //gfx::draw_line(corner.x(), corner.y(), corner.x(), 0.0f);

  gfx::bind_shader_2d_font();
  gfx::load_identity();

  if(m_menu_prev)
  {
    draw_menu_list(prevlen, m_menu_prev->getRecursive(), m_menu_prev->getFocusTimeFloat(), maxwidth_prev,
        sarea);
  }

  draw_menu_list(currlen, m_menu_curr->getRecursive(), m_menu_curr->getFocusTimeFloat(), maxwidth_curr,
      sarea);

  gfx::draw_fill(0, OB_COLOR_TITLE);
  gfx::draw_fill(1, OB_COLOR_TITLE);
  gfx::draw_fill(2, OB_COLOR_TITLE);
  gfx::draw_fill(3, OB_COLOR_TITLE);
  gfx::draw_text_line(OB_FONT_GAP, sarea.h() - FONT_SIZE_TITLE - OB_FONT_GAP + TITLE_BOTTOM,
      FONT_SIZE_TITLE, m_title.get(), glob->getFont());

  {
    unsigned idx;
    float width;
    
    boost::tie(idx, width) = glob->getFont().calcLineWidth(FONT_SIZE_VERSION, m_version.get() );

    gfx::draw_text_line(sarea.w() - OB_FONT_GAP - width,
        sarea.h() - FONT_SIZE_VERSION - OB_FONT_GAP + VERSION_BOTTOM, FONT_SIZE_VERSION, m_version.get(),
        glob->getFont());
  }

  {
    const gfx::Texture2D *ttex = gfx::Texture2D::getTempTexture();
    if(NULL != ttex)
    {
      gfx::bind_shader_2d_texture();
      gfx::load_identity();

      math::rect2f harea(0.0f, 0.0f, corner.x(), corner.y());
      gfx::draw_rect_textured_fit(harea, gfx::Color(1.0f, 1.0f, 1.0f, 1.0f), *ttex);
    }
  }

  fade.draw(screen);
  screen.update();
}

void MenuState::update(ui::UiStack &st)
{
  m_menu_curr->incFocusTime();
  if(NULL != m_menu_prev)
  {
    float focus_prev = m_menu_prev->decFocusTime();
    if(focus_prev <= 0.0f)
    {
      m_menu_prev = NULL;
    }
  }

  std::vector<MenuSptr> &mvec = m_menu_curr->getRecursive();
  for(unsigned ii = 0; (ii < mvec.size()); ++ii)
  {
    Menu *cmenu = mvec[ii].get();
    // Hack: do not increment or modify previous focus menu ever.
    if(cmenu != m_menu_prev)
    {
      if(static_cast<int>(ii) == m_index)
      {
        cmenu->incFocusTime();
      }
      else
      {
        cmenu->decFocusTime();
      }
    }
  }

  m_title.update();
  m_version.update();

  Game *curr_game = glob_get_game();
  if(curr_game && (1 >= st.getNumStates()))
  {
    st.pushState(curr_game);
    snd::play_stream("snd/music_game.ogg");
  }

  fade.update();
}

Menu* MenuState::create_menu_game()
{
  return NULL;
}

Menu* MenuState::create_menu_main()
{
  Menu *ret = new Menu("Orbital Bombardment"),
       *options = new Menu("Options", NONE, ret);

  options->add(new Menu("Detail level", DETAIL, options));
  options->add(new Menu("Fullscreen", FULLSCREEN, options));
  options->add(new Menu("Resolution", RESOLUTION, options));
  options->add(new Menu("Sensitivity", SENSITIVITY, options));
  options->add(new Menu("Invert mouse", INVERT_MOUSE, options));
  options->add(new Menu("Music volume", VOLUME_MUSIC, options));
  options->add(new Menu("Samples volume", VOLUME_SAMPLES, options));

  ret->add(new Menu("Start", START, ret));
  ret->add(options);
  ret->add(new Menu("Instructions", HELP, ret));
  ret->add(new Menu("Records", HIGH_SCORES, ret));
  ret->add(new Menu("Exit", QUIT, ret));
  return ret;
}

