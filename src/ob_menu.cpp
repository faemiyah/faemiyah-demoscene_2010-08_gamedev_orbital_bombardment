#include "ob_menu.hpp"

#include "math/generic.hpp"
#include "snd/generic.hpp"
#include "ui/generic.hpp"
#include "ob_constants.hpp"
#include "ob_settings.hpp"

#include <sstream>

using namespace ob;

/** Maximum focus time. */
static const int FOCUS_TIME_MAX = 40;

/** Volume division number. */
static const int VOLUME_DIV = 100;

/** Volume division number (float). */
static const float VOLUME_DIV_F = static_cast<float>(VOLUME_DIV);

/** Volume step number. */
static const int VOLUME_STEP = 5;

/** Maximum focus time as float. */
static const float FOCUS_TIME_MAX_FLOAT = static_cast<float>(FOCUS_TIME_MAX);

/** \brief Find the index of a string in a vector, then modify it.
 *
 * \param op String to find.
 * \param add Number to add.
 * \param svec String vector.
 */
static std::string move_in_cstr_list(const std::string &op, int add,
    const std::vector<const char*> &svec)
{
  int ii = 0;
  for(; (ii < static_cast<int>(svec.size())); ++ii)
  {
    if(0 == op.compare(svec[static_cast<unsigned>(ii)]))
    {
      break;
    }
  }
  ii = math::min(math::max(ii + add, 0), static_cast<int>(svec.size()) - 1);
  return std::string(svec[static_cast<unsigned>(ii)]);
}

Menu::Menu(const char *name, MenuEnum func, Menu *parent) :
  m_parent(parent),
  m_focus_time(0),
  m_focus_time_float(0.0f),
  m_func(func)
{
  this->replaceText(name);
  this->updateText(0);
}

void Menu::add(Menu *op)
{
  m_recursive.push_back(MenuSptr(op));
}

float Menu::decFocusTime()
{
  m_focus_time = math::max(m_focus_time - 1, 0);
  m_focus_time_float = static_cast<float>(m_focus_time) / FOCUS_TIME_MAX_FLOAT;
  return m_focus_time_float;
}

float Menu::incFocusTime()
{
  m_focus_time = math::min(m_focus_time + 1, FOCUS_TIME_MAX);
  m_focus_time_float = static_cast<float>(m_focus_time) / FOCUS_TIME_MAX_FLOAT;
  return m_focus_time_float;
}

gfx::Color Menu::getFocusColor(float alpha)
{
  gfx::Color ret = OB_COLOR_ACTIVE;
  float mul = m_focus_time_float * 0.5f + 0.5f;
  ret.r() *= mul;
  ret.g() *= mul;
  ret.b() *= mul;
  ret.a() *= alpha;
  return ret;
}

void Menu::replaceText(const char *name)
{
  m_name_utf8.assign(name);
  m_name_wide = ui::wstr_utf8(name);
}

void Menu::updateText(int op)
{
  switch(m_func)
  {
    case DETAIL:
      {
        std::string detail = conf->getDetail(),
          new_detail = move_in_cstr_list(detail, op, conf->getDetailLevels());
        this->replaceText(std::string("Detail: ") + new_detail);
        conf->setDetail(new_detail);
      }
      break;

    case FULLSCREEN:
      {
        settingi &fs = conf->getFullscreen();
        if(op != 0)
        {
          fs.set(op);
        }
        this->replaceText((fs.get() > 0) ? "Fullscreen" : "Windowed");
      }
      break;

    case INVERT_MOUSE:
      {
        settingf &inv = conf->getCameraRotSpeedX();
        if(op != 0)
        {
          inv.set(static_cast<float>(-op) * conf->getCameraRotSpeedY().get());
        }
        this->replaceText((inv.get() >= 0.0f) ? "Invert mouse off" : "Invert mouse on");
      }
      break;

    case RESOLUTION:
      {
        std::string res = conf->getResolution();
        this->replaceText(move_in_cstr_list(res, op, conf->getResolutions()));
        conf->setResolution(m_name_utf8);
      }
      break;

    case SENSITIVITY:
      {
        conf->setSensitivity(conf->getSensitivity() +
            static_cast<float>(op) * OB_CAMERA_ROT_SPEED_STEP);
        float sens = conf->getSensitivity();
        std::stringstream sstr;
        sstr << "Sensitivity: " << math::lround(sens / OB_CAMERA_ROT_SPEED_STEP);
        this->replaceText(sstr.str());
      }
      break;

    case VOLUME_MUSIC:
      {
        settingf &vol = conf->getVolumeMusic();
        int currvol = math::lround(vol.get() * VOLUME_DIV);
        conf->setVolumeMusic(static_cast<float>(currvol + (op * VOLUME_STEP)) / VOLUME_DIV_F);
        std::stringstream sstr;
        sstr << "Music volume: " << math::lround(vol.get() * VOLUME_DIV);
        this->replaceText(sstr.str());
      }
      break;

    case VOLUME_SAMPLES:
      {
        settingf &vol = conf->getVolumeSamples();
        int currvol = math::lround(vol.get() * VOLUME_DIV);
        conf->setVolumeSamples(static_cast<float>(currvol + (op * VOLUME_STEP)) / VOLUME_DIV_F);
        std::stringstream sstr;
        sstr << "Sample volume: " << math::lround(vol.get() * VOLUME_DIV);
        this->replaceText(sstr.str());
      }
      break;

    default:
      break;
  }
}

