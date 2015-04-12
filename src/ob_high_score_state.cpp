#include "ob_high_score_state.hpp"

#include "gfx/color.hpp"
#include "math/rect.hpp"
#include "thr/dispatch.hpp"
#include "ui/ui_stack.hpp"
#include "ob_constants.hpp"
#include "ob_high_scores.hpp"
#include "ob_globals.hpp"
#include "ob_settings.hpp"

using namespace ob;

/** Command speed. */
static const float COMMAND_SPEED = 0.02f;

/** Drifting speed. */
static const float DRIFT_SPEED = 0.0006f;

/** Movement trigger. */
static const int MOVEMENT_TRIGGER = 500;

HighScoreState::HighScoreState(float offset) :
  m_offset(offset),
  m_movement(1.0f),
  m_trigger(MOVEMENT_TRIGGER),
  m_command_down(false),
  m_command_up(false),
  m_newest_entry_ind(-1)
{
  const std::vector<HighScoreEntry> &hvec =
    conf->getHighScores().getEntries();
  for(unsigned ii = 0; (ii < hvec.size()); ++ii)
  {
    std::stringstream nstr, pstr;
    nstr << ii << ": " << ui::str_utf8(hvec[ii].getName());
    formatHighScore(pstr, hvec[ii].getScore());
    m_names.push_back(AppearingStringSptr(new AppearingString(ui::wstr_utf8(nstr.str()))));    
    m_scores.push_back(AppearingStringSptr(new AppearingString(ui::wstr_utf8(pstr.str()))));
    if(hvec[ii].isNewest())
    {
      m_newest_entry_ind = static_cast<int>(ii);
    }
  }

  m_full_height = static_cast<float>(m_names.size()) *
    (OB_FONT_SIZE + OB_FONT_GAP * 2.0f) + OB_FONT_GAP * 2.0f;
}

bool HighScoreState::handleKey(const ui::EventKey &ev, ui::UiStack &st)
{
  boost::ignore_unused_variable_warning(st);

  switch(ev.getCode())
  {
    case SDLK_BACKSPACE:
    case SDLK_ESCAPE:
    case SDLK_RETURN:
    case SDLK_WORLD_7:
      if(ev.isPress())
      {
        this->die();
      }
      break;

    case SDLK_UP:
      m_command_up = ev.isPress();
      m_trigger = MOVEMENT_TRIGGER;
      break;

    case SDLK_DOWN:
      m_command_down = ev.isPress();
      m_trigger = MOVEMENT_TRIGGER;
      break;

    case SDLK_HOME:
      if(ev.isPress())
      {
        m_offset = 0.0f;
      }
      break;

    case SDLK_END:
      if(ev.isPress())
      {
        m_offset = FLT_MAX;
      }
      break;

    default:
      break;
  }

  return true;
}

void HighScoreState::render(ui::UiStack &st, gfx::SurfaceScreen &screen)
{
  boost::ignore_unused_variable_warning(st);

  math::rect2f sarea = screen.getArea();

  screen.select2D();
  screen.clear(true, true);

  gfx::bind_shader_2d_texture();
  gfx::load_identity();

  gfx::draw_rect_textured_fill(sarea, gfx::Color(1.0f, 1.0f, 1.0f, 1.0f),
      glob->getTextureMenu());

  if(sarea.w() > sarea.h())
  {
    float diff = (sarea.w() - sarea.h()) * 0.5f;

    gfx::bind_shader_2d();
    gfx::load_identity();

    gfx::draw_rect(sarea.x1(), sarea.y1(), diff, sarea.h(),
        OB_COLOR_SHADE);
    gfx::draw_rect(sarea.w() - diff, sarea.y1(), diff, sarea.h());
    glLineWidth(OB_LINE_WIDTH);
    gfx::draw_line(sarea.x1() + diff, sarea.y1(), sarea.x1() + diff, sarea.y2(),
        OB_COLOR_MENU_BORDER);
    gfx::draw_line(sarea.x2() - diff, sarea.y1(), sarea.x2() - diff, sarea.y2(),
        OB_COLOR_MENU_BORDER);
    glLineWidth(1.0f);

    sarea = math::rect2f(sarea.x1() + diff, sarea.y1(),
        sarea.w() - diff * 2.0f, sarea.h());
  }


  gfx::bind_shader_2d_font();
  gfx::load_identity();

  float cy = sarea.h() - OB_FONT_SIZE + m_offset;
  for(unsigned ii = 0; (ii < m_names.size()); ++ii)
  {
    std::wstring name = m_names[ii]->get(),
      score = m_scores[ii]->get();
    if (static_cast<int>(ii) == m_newest_entry_ind)
    {
      gfx::draw_fill(0, OB_COLOR_NEW_HS_ENTRY);
      gfx::draw_fill(1, OB_COLOR_NEW_HS_ENTRY);
      gfx::draw_fill(2, OB_COLOR_NEW_HS_ENTRY);
      gfx::draw_fill(3, OB_COLOR_NEW_HS_ENTRY);
    } else
    {
      gfx::draw_fill(0, OB_COLOR_OLD_HS_ENTRY);
      gfx::draw_fill(1, OB_COLOR_OLD_HS_ENTRY);
      gfx::draw_fill(2, OB_COLOR_OLD_HS_ENTRY);
      gfx::draw_fill(3, OB_COLOR_OLD_HS_ENTRY);
    }

    gfx::draw_text(sarea.x1() + OB_FONT_GAP * 2.0f, cy, OB_FONT_SIZE, name, glob->getFont(), gfx::LEFT);
    gfx::draw_text(sarea.x2() - OB_FONT_GAP * 2.0f, cy, OB_FONT_SIZE, score, glob->getFont(), gfx::RIGHT);

    cy -= OB_FONT_SIZE + OB_FONT_GAP * 2.0f;
  }

  screen.update();
}

void HighScoreState::update(ui::UiStack &st)
{
  boost::ignore_unused_variable_warning(st);

  BOOST_FOREACH(AppearingStringSptr &vv, m_names)
  {
    vv->update();
  }
  BOOST_FOREACH(AppearingStringSptr &vv, m_scores)
  {
    vv->update();
  }

  math::rect2f sarea = st.getScreen().getArea();

  m_offset -= (m_command_up ? COMMAND_SPEED : 0.0f) -
    (m_command_down ? COMMAND_SPEED : 0.0f);

  if(--m_trigger < 0)
  {
    m_offset += m_movement * DRIFT_SPEED;
    m_trigger = 0;
  }
  if(m_full_height - m_offset < sarea.h())
  {
    m_offset = m_full_height - sarea.h();
    m_movement = -1.0f;
  }
  if(m_offset < 0.0f)
  {
    m_offset = 0.0f;
    m_movement = 1.0f;
  }
}

void HighScoreState::formatHighScore(std::stringstream &sstr, int score)
{
  int bils = 0;
  int mils = 0;
  int thos = 0;
  int ones = 0;
  int tmp = score;
  bool biggest = true;
  if (tmp >= 1000000000) {
    bils = tmp / 1000000000;
    tmp -= bils * 1000000000;
    sstr << bils << " ";
    biggest = false;
  }
  if (tmp >= 1000000) {
    mils = tmp / 1000000;
    tmp -= mils * 1000000;
    if (biggest) {
      sstr << mils << " ";
      biggest = false;
    } else {
      sstr << std::setfill('0') << std::setw(3) << mils << " ";
    }
  }
  if (tmp >= 1000) {
    thos = tmp / 1000;
    ones = tmp - (thos * 1000);
    if (biggest) {
      sstr << thos << " ";
      biggest = false;
    } else {
      sstr << std::setfill('0') << std::setw(3) << thos << " ";
    }
  }
  if (biggest) {
    sstr << ones;
  } else {
    sstr << std::setfill('0') << std::setw(3) << ones;
  }
}

