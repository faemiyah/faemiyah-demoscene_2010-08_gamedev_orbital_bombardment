#include "ob_settings.hpp"

#include "data/log.hpp"
#include "data/generic.hpp"
#include "snd/generic.hpp"
#include "ui/generic.hpp"
#include "ob_constants.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace ob;
namespace pt = boost::property_tree;

Settings *ob::conf;

#if defined(WIN32)
static const std::string DEFAULT_CONFIG_FILE(
    std::string(getenv("USERPROFILE")) + std::string("/orbital_bombardment.conf"));
#else
static const std::string DEFAULT_CONFIG_FILE(
    std::string(getenv("HOME")) + std::string("/.orbital_bombardment.conf"));
#endif

Settings::Settings()
{
  this->defaults();
  this->load(DEFAULT_CONFIG_FILE);
}

Settings::Settings(const std::string &pfname)
{
  this->defaults();
  this->load(pfname);
}

void Settings::defaults()
{
  m_camera_rot_speed_y.set(OB_CAMERA_ROT_SPEED_STEP * 4.0f, OB_CAMERA_ROT_SPEED_STEP, OB_CAMERA_ROT_SPEED_STEP * 10.0f);
  m_camera_rot_speed_x.set(-m_camera_rot_speed_y.get(), -OB_CAMERA_ROT_SPEED_STEP * 10.0f, OB_CAMERA_ROT_SPEED_STEP * 10.0f);
  m_detail.assign("desktop");
  m_fullscreen.set(0, 0, 1);
  m_resolution.assign("800x600@32");
  m_volume_music.set(0.5f, 0.0f, 1.0f);
  m_volume_samples.set(1.0f, 0.0f, 1.0f);

  m_detail_levels.clear();
  m_detail_levels.push_back("laptop");
  m_detail_levels.push_back("desktop");
  m_detail_levels.push_back("bleeding");

  m_resolutions.clear();
  m_resolutions.push_back("640x480@32");
  m_resolutions.push_back("800x600@32");
  m_resolutions.push_back("1024x600@32");
  m_resolutions.push_back("1024x768@32");
  m_resolutions.push_back("1280x720@32");
  m_resolutions.push_back("1280x800@32");
  m_resolutions.push_back("1280x960@32");
  m_resolutions.push_back("1280x1024@32");
  m_resolutions.push_back("1366x768@32");
  m_resolutions.push_back("1400x900@32");
  m_resolutions.push_back("1400x1050@32");
  m_resolutions.push_back("1600x1200@32");
  m_resolutions.push_back("1680x1050@32");
  m_resolutions.push_back("1920x1080@32");
  m_resolutions.push_back("1920x1200@32");

  this->setVolumeMusic(m_volume_music.get());
  this->setVolumeSamples(m_volume_samples.get());
}

void Settings::load(const std::string &pfname)
{
  data::shristr pfile;
  pt::ptree xtree;
  try
  {
    pfile = data::open_read(pfname);
    data::log(std::string("reading settings from ") + pfname);
    pt::read_xml(*pfile, xtree);
  }
  catch(...)
  {
    return;
  }

  pt::ptree::value_type root = xtree.front();
  if(root.first.compare("settings"))
  {
    std::stringstream err;
    err << "unknown root type: " << root.first;
    BOOST_THROW_EXCEPTION(std::invalid_argument(err.str()));
  }

  BOOST_FOREACH(pt::ptree::value_type &vv, root.second)
  {
    const std::string &type = vv.first;
    const pt::ptree &subtree = vv.second;

    if(!type.compare("camera_rot_speed_x"))
    {
      m_camera_rot_speed_x.set(subtree.get<float>(""));
    }
    else if(!type.compare("camera_rot_speed_y"))
    {
      m_camera_rot_speed_y.set(subtree.get<float>(""));
    }
    else if(!type.compare("detail"))
    {
      m_detail.assign(subtree.get<std::string>(""));
    }
    else if(!type.compare("fullscreen"))
    {
      m_fullscreen.set(subtree.get<int>(""));
    }
    else if(!type.compare(0, 4, "high"))
    {
      int score = subtree.get<int>("score");
      std::string name = subtree.get<std::string>("name");
      m_high_scores.add(score, ui::wstr_utf8(name), false);
    }
    else if(!type.compare("resolution"))
    {
      m_resolution.assign(subtree.get<std::string>(""));
    }
    else if(!type.compare("volume_music"))
    {
      this->setVolumeMusic(subtree.get<float>(""));
    }
    else if(!type.compare("volume_samples"))
    {
      this->setVolumeSamples(subtree.get<float>(""));
    }
    else if(type.compare("<xmlattr>"))
    {
      std::stringstream err;
      err << "unknown element: " << type;
      BOOST_THROW_EXCEPTION(std::invalid_argument(err.str()));
    }
  }
}

void Settings::save(const std::string &pfname)
{
  data::shrostr pfile;
  try
  {
    pfile = data::open_write(pfname);
    data::log(std::string("writing settings to ") + pfname);
  }
  catch(std::exception ee)
  {
    data::log(ee.what());
    return;
  }

  pt::ptree xtree;

  xtree.put("settings.camera_rot_speed_x", m_camera_rot_speed_x.get());
  xtree.put("settings.camera_rot_speed_y", m_camera_rot_speed_y.get());
  xtree.put("settings.detail", m_detail);
  xtree.put("settings.fullscreen", m_fullscreen.get());
  xtree.put("settings.resolution", m_resolution);
  xtree.put("settings.volume_music", m_volume_music.get());
  xtree.put("settings.volume_samples", m_volume_samples.get());

  const std::vector<HighScoreEntry> &hvec = m_high_scores.getEntries();
  for(unsigned ii = 0; (ii < hvec.size()); ++ii)
  {
    std::stringstream sstr;
    sstr << "settings.high" << ii << ".";
    xtree.put(sstr.str() + std::string("score"), hvec[ii].getScore());
    xtree.put(sstr.str() + std::string("name"),
        ui::str_utf8(hvec[ii].getName()));
  }

  pt::write_xml(*pfile, xtree);
}

void Settings::setVolumeMusic(float op)
{
  m_volume_music.set(op);
  snd::set_volume_music(m_volume_music.get());
}

void Settings::setVolumeSamples(float op)
{
  m_volume_samples.set(op);
  snd::set_volume_samples(m_volume_samples.get());
}

void ob::conf_init()
{
  if(NULL != conf)
  {
    std::stringstream sstr;
    sstr << "configuration already exists";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  conf = new Settings();
}

void ob::conf_quit()
{
  if(NULL == conf)
  {
    std::stringstream sstr;
    sstr << "no configuration exists";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  conf->save(DEFAULT_CONFIG_FILE);
  delete conf;
  conf = NULL;
}

