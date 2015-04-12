#include "ob_globals.hpp"

#include "data/log.hpp"
#include "gfx/mesh_static.hpp"
#include "thr/dispatch.hpp"
#include "ob_atmosphere.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_menu_state.hpp"
#include "ob_planet.hpp"
#include "ob_visualization_city.hpp"
#include "ob_visualization_distort.hpp"
#include "ob_visualization_flak.hpp"
#include "ob_visualization_nuke.hpp"
#include "ob_visualization_orbit.hpp"
#include "ob_visualization_railgun.hpp"
#include "ob_visualization_sun.hpp"

#include <sstream>

namespace fs = boost::filesystem;
using namespace ob;

bool Globals::generate_enabled = false;

Fade ob::fade;
Game *ob::game = NULL;
Globals *ob::glob = NULL;

/** The game thread has stopped processing. */
static bool game_is_ready = false;

/** Thread used for game startups. */
static thr::ThreadSptr game_thread;

/** \brief Task for the creation of the next actual game state.
 *
 * This means, cities and population map.
 */
static void glob_task_game()
{
  if(game != NULL)
  {
    std::stringstream sstr;
    sstr << "can't create a game task when previous game task exists";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  new Game(); // Will set the global value by itself.
  game_is_ready = true;
  fade.setDelta(-OB_FADE_DELTA);
}

/** \brirf Log to console.
 *
 * \param op String to log.
 */
static void log_console(const std::string &op)
{
  glob->getConsole().addRow(ui::wstr_utf8(op));
}

Globals::Globals(const gfx::SurfaceScreen &pscreen, const std::string &pdetail) :
  m_detail_level(pdetail),
  m_font("fnt/default.xml"),
  m_console(m_font, OB_CONSOLE_FONT_SIZE, pscreen),
  m_cursor_blank(create_cursor_blank()),
  m_cursor_default(SDL_GetCursor()),
  m_menu_game(MenuState::create_menu_game()),
  m_menu_main(MenuState::create_menu_main()),
  m_precalculated(false) { }

Globals::~Globals()
{
  this->unreserve();
  SDL_SetCursor(m_cursor_default);
  SDL_FreeCursor(m_cursor_blank);
}

void Globals::precalc()
{
  if(m_detail_level.compare("laptop") == 0)
  {
    this->precalc(6, 3, 512, 64);
    return;
  }
  
  if(m_detail_level.compare("desktop") == 0)
  {
    this->precalc(7, 3, 1024, 128);
    return;
  }

  if(m_detail_level.compare("bleeding") == 0)
  {
    this->precalc(8, 3, 2048, 128);
    //this->precalc(8, 3, 2048, 256); // 256^3 is just plain ridiculous.
    return;
  }

  if(m_detail_level.compare("custom") != 0)
  {
    std::stringstream sstr;
    sstr << "unknown detail level: " << m_detail_level;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  // TODO: implement.
  BOOST_THROW_EXCEPTION(std::runtime_error("unimplemented"));
}

void Globals::precalc(unsigned subdivide, unsigned coalesce, unsigned texsize, unsigned volsize)
{
  thr::wait_privileged(&Globals::unreserve, this);

  gfx::Texture2D::createParaller("gfx/console_background.png");
  snd::play_stream("snd/music_menu.ogg");

  thr::wait();

  m_console.setBackground(gfx::Texture2D::locate("console_background").get().get());
  
  gfx::Shader::createParaller("shader/3d_static.xml");
  gfx::Shader::createParaller("shader/3d_overlay.xml");
  gfx::Shader::createParaller("shader/3d_point_sprite.xml");
  gfx::Shader::createParaller("shader/3d_overlay_line.xml");
  gfx::Shader::createParaller("shader/ob_atmosphere.xml");
  gfx::Shader::createParaller("shader/ob_distort.xml");
  gfx::Shader::createParaller("shader/ob_sun.xml");
  gfx::Shader::createParaller("shader/ob_world.xml");
  gfx::Shader::createParaller("shader/ob_world_schematic.xml");

  gfx::Texture2D::createParaller("gfx/textures/texture_flak_ammo.png");
  gfx::Texture2D::createParaller("gfx/textures/icon_bullet_flak.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/textures/icon_bullet_railgun.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/textures/icon_city.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/textures/icon_missile_anti_nuke.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/textures/icon_missile_anti_ship.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/textures/icon_silo.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/mainmenu_background.png");
  gfx::Texture2D::createParaller("gfx/billboards/billboard_missile_anti_nuke.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_warning_yellow_frame_1.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_missile_anti_ship.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_warning_red_frame_1.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_missile_nuke.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/textures/texture_nuke_ammo.png");
  gfx::Texture2D::createParaller("gfx/textures/texture_railgun_ammo.png");
  gfx::Texture2D::createParaller("gfx/textures/texture_reload.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_silo_anti_nuke.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_silo_anti_nuke_new.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_silo_anti_ship.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_silo_anti_ship_new.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_silo_both.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/billboards/billboard_silo_both_new.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/textures/texture_skull.png");
  gfx::Texture2D::createParaller("gfx/billboards/billboard_target.png", gfx::ImageLoader().clamp());
  gfx::Texture2D::createParaller("gfx/textures/texture_trace.png");

  const char *fname_particle[OB_PARTICLE_COUNT] =
  {
    "gfx/billboards/particle_crackle_1.png",
    "gfx/billboards/particle_crackle_2.png",
    "gfx/billboards/particle_crackle_3.png",
    "gfx/billboards/particle_crackle_4.png",
    "gfx/billboards/particle_glow_medium.png",
    "gfx/billboards/particle_glow_sharp.png",
    "gfx/billboards/particle_glow_soft.png",
    "gfx/billboards/particle_shockwave_narrow_1.png",
    "gfx/billboards/particle_shockwave_narrow_2.png",
    "gfx/billboards/particle_shockwave_narrow_3.png",
    "gfx/billboards/particle_shockwave_narrow_4.png",
    "gfx/billboards/particle_shockwave_wide_1.png",
    "gfx/billboards/particle_shockwave_wide_2.png",
    "gfx/billboards/particle_shockwave_wide_3.png",
    "gfx/billboards/particle_shockwave_wide_4.png",
    "gfx/billboards/particle_smoke_hard_1.png",
    "gfx/billboards/particle_smoke_hard_2.png",
    "gfx/billboards/particle_smoke_hard_3.png",
    "gfx/billboards/particle_smoke_hard_4.png",
    "gfx/billboards/particle_smoke_soft_1.png",
    "gfx/billboards/particle_smoke_soft_2.png",
    "gfx/billboards/particle_smoke_soft_3.png",
    "gfx/billboards/particle_smoke_soft_4.png",
    "gfx/billboards/particle_sparkle_1.png",
    "gfx/billboards/particle_sparkle_2.png",
    "gfx/billboards/particle_sparkle_3.png",
    "gfx/billboards/particle_sparkle_4.png",
    "gfx/billboards/particle_sparkle_5.png",
    "gfx/billboards/particle_sparkle_6.png"
  };
  for(unsigned ii = 0; (ii < OB_PARTICLE_COUNT); ++ii)
  {
    gfx::Texture2D::createParaller(fname_particle[ii], gfx::ImageLoader().clamp());
  }

  // Speed up planet creation by potentially loading planet maps, slight code duplication.
  for(unsigned ii = 0; (ii < 10); ++ii)
  {
    std::ostringstream sstream_fname;
    sstream_fname << PLANET_FILENAME << "_map_" << texsize << "_" << ii << ".png";
    std::string fname = sstream_fname.str();

    if(data::file_exists(fname))
    {
      gfx::Texture2D::createParaller(fname, gfx::ImageLoader().clamp().noPremultiplyAlpha());
    }
  }

  // Speed up planet creation by potentially loading planet volumes, slight code duplication.
  for(unsigned ii = 0; (ii < 2); ++ii)
  {
    std::ostringstream sstream_fname;
    sstream_fname << PLANET_FILENAME << "_volume_" << volsize << "_" << ii << ".png";
    std::string fname = sstream_fname.str();

    if(data::file_exists(fname))
    {
      gfx::Texture3D::createParaller(fname, gfx::ImageLoader().noPremultiplyAlpha());
    }
  }

  // Speed up skybox creation by loading environment maps.
  std::string texstr;
  {
    std::stringstream sstr;
    sstr << '_' << texsize << ".png";
    texstr = sstr.str();
  }
  std::string environ_back = std::string("gfx/maps/enviroment_map_back") + texstr;
  std::string environ_bottom = std::string("gfx/maps/enviroment_map_bottom") + texstr;
  std::string environ_front = std::string("gfx/maps/enviroment_map_front") + texstr;
  std::string environ_left = std::string("gfx/maps/enviroment_map_left") + texstr;
  std::string environ_right = std::string("gfx/maps/enviroment_map_right") + texstr;
  std::string environ_top = std::string("gfx/maps/enviroment_map_top") + texstr;

  gfx::Texture2D::createParaller(environ_back);
  gfx::Texture2D::createParaller(environ_bottom);
  gfx::Texture2D::createParaller(environ_front);
  gfx::Texture2D::createParaller(environ_left);
  gfx::Texture2D::createParaller(environ_right);
  gfx::Texture2D::createParaller(environ_top);

  snd::Sample::createParaller("snd/ob_alarm.sample");
  snd::Sample::createParaller("snd/ob_alarm_over.sample");
  snd::Sample::createParaller("snd/ob_contact.sample");
  snd::Sample::createParaller("snd/ob_flak_short.sample");
  snd::Sample::createParaller("snd/ob_illegal_action.sample");
  snd::Sample::createParaller("snd/ob_impact_in.sample");
  snd::Sample::createParaller("snd/ob_locked.sample");
  snd::Sample::createParaller("snd/ob_nuke.sample");
  snd::Sample::createParaller("snd/ob_nuke_explosion.sample");
  snd::Sample::createParaller("snd/ob_railgun.sample");
  snd::Sample::createParaller("snd/ob_railgun_lock_long.sample");
  snd::Sample::createParaller("snd/ob_route_change.sample");
  snd::Sample::createParaller("snd/ob_route_change_accepted.sample");
  snd::Sample::createParaller("snd/ob_target_destroyed.sample");

  gfx::Mesh::createParaller("mdl/siegecruiser.mesh");

  m_mesh_missile_anti = gfx::Mesh::create("mdl/missile_anti.mesh").get().get();
  m_mesh_missile_nuke = gfx::Mesh::create("mdl/missile_icbm.mesh").get().get();
  m_mesh_silo = gfx::Mesh::create("mdl/silo.mesh").get().get();
  gfx::Mesh::store("atmosphere", new Atmosphere(subdivide - 2, coalesce));
  gfx::Mesh::store("city", new VisualizationCity());
  gfx::Mesh::store("distort", new VisualizationDistort());
  gfx::Mesh::store("orbit", new VisualizationOrbit());
  gfx::Mesh::store("nuke_marker", new VisualizationNuke());
  gfx::Mesh::store("sun", new VisualizationSun());
  m_mesh_bullet_flak = gfx::Mesh::store("bullet_flak", new VisualizationFlak()).get().get();
  m_mesh_bullet_railgun = gfx::Mesh::store("bullet_railgun", new VisualizationRailgun()).get().get();

  // Ensure all paraller tasks are done before performing tasks that depend on them. */
  thr::wait(); 

  gfx::Mesh::store("planet", new Planet(subdivide, coalesce, texsize, volsize, &m_height_map_planet, generate_enabled));

  std::string textype("texture");
  gfx::Mesh::create("mdl/skybox_back.mesh").get()->addTextureFile(textype, environ_back);
  gfx::Mesh::create("mdl/skybox_bottom.mesh").get()->addTextureFile(textype, environ_bottom);
  gfx::Mesh::create("mdl/skybox_front.mesh").get()->addTextureFile(textype, environ_front);
  gfx::Mesh::create("mdl/skybox_left.mesh").get()->addTextureFile(textype, environ_left);
  gfx::Mesh::create("mdl/skybox_right.mesh").get()->addTextureFile(textype, environ_right);
  gfx::Mesh::create("mdl/skybox_top.mesh").get()->addTextureFile(textype, environ_top);

  m_shader_object = gfx::Shader::locate("3d_static").get().get();
  m_shader_overlay = gfx::Shader::locate("3d_overlay").get().get();
  m_shader_billboard = gfx::Shader::locate("3d_point_sprite").get().get();
  m_shader_overlay_line = gfx::Shader::locate("3d_overlay_line").get().get();
  m_shader_atmosphere = gfx::Shader::locate("ob_atmosphere").get().get();
  m_shader_distort = gfx::Shader::locate("ob_distort").get().get();
  m_shader_sun = gfx::Shader::locate("ob_sun").get().get();
  m_shader_planet = gfx::Shader::locate("ob_world").get().get();
  m_shader_planet_schematic = gfx::Shader::locate("ob_world_schematic").get().get();

  m_texture_flak_ammo = gfx::Texture2D::locate("texture_flak_ammo").get().get();
  m_texture_icon_bullet_flak = gfx::Texture2D::locate("icon_bullet_flak").get().get();
  m_texture_icon_bullet_railgun = gfx::Texture2D::locate("icon_bullet_railgun").get().get();
  m_texture_icon_city = gfx::Texture2D::locate("icon_city").get().get();
  m_texture_icon_missile_anti_nuke = gfx::Texture2D::locate("icon_missile_anti_nuke").get().get();
  m_texture_icon_missile_anti_ship = gfx::Texture2D::locate("icon_missile_anti_ship").get().get();
  m_texture_icon_silo = gfx::Texture2D::locate("icon_silo").get().get();
  m_texture_menu = gfx::Texture2D::locate("mainmenu_background").get().get();
  m_texture_missile_anti_nuke = gfx::Texture2D::locate("billboard_missile_anti_nuke").get().get();
  m_texture_missile_anti_nuke_warning = gfx::Texture2D::locate("billboard_warning_yellow_frame_1").get().get();
  m_texture_missile_anti_ship = gfx::Texture2D::locate("billboard_missile_anti_ship").get().get();
  m_texture_missile_anti_ship_warning = gfx::Texture2D::locate("billboard_warning_red_frame_1").get().get();
  m_texture_missile_nuke = gfx::Texture2D::locate("billboard_missile_nuke").get().get();
  m_texture_nuke_ammo = gfx::Texture2D::locate("texture_nuke_ammo").get().get();
  m_texture_railgun_ammo = gfx::Texture2D::locate("texture_railgun_ammo").get().get();
  m_texture_reload = gfx::Texture2D::locate("texture_reload").get().get();
  m_texture_silo_anti_nuke = gfx::Texture2D::locate("billboard_silo_anti_nuke").get().get();
  m_texture_silo_anti_nuke_inactive = gfx::Texture2D::locate("billboard_silo_anti_nuke_new").get().get();
  m_texture_silo_anti_ship = gfx::Texture2D::locate("billboard_silo_anti_ship").get().get();
  m_texture_silo_anti_ship_inactive = gfx::Texture2D::locate("billboard_silo_anti_ship_new").get().get();
  m_texture_silo_both = gfx::Texture2D::locate("billboard_silo_both").get().get();
  m_texture_silo_both_inactive = gfx::Texture2D::locate("billboard_silo_both_new").get().get();
  m_texture_skull = gfx::Texture2D::locate("texture_skull").get().get();
  m_texture_target = gfx::Texture2D::locate("billboard_target").get().get();
  m_texture_trace = gfx::Texture2D::locate("texture_trace").get().get();

  for(unsigned ii = 0; (ii < OB_PARTICLE_COUNT); ++ii)
  {
    m_texture_particle[ii] = gfx::Texture2D::locate(fname_particle[ii]).get().get();
  }

  m_sample_alarm = snd::Sample::locate("ob_alarm").get().get();
  m_sample_alarm_over = snd::Sample::locate("ob_alarm_over").get().get();
  m_sample_contact = snd::Sample::locate("ob_contact").get().get();
  m_sample_flak = snd::Sample::locate("ob_flak_short").get().get();
  m_sample_illegal = snd::Sample::locate("ob_illegal_action").get().get();
  m_sample_locked = snd::Sample::locate("ob_locked").get().get();
  m_sample_nuke = snd::Sample::locate("ob_nuke").get().get();
  m_sample_railgun = snd::Sample::locate("ob_railgun").get().get();
  m_sample_railgun_lock = snd::Sample::locate("ob_railgun_lock_long").get().get();
  m_sample_route_change = snd::Sample::locate("ob_route_change").get().get();
  m_sample_route_change_accepted = snd::Sample::locate("ob_route_change_accepted").get().get();
  m_sample_target_destroyed = snd::Sample::locate("ob_target_destroyed").get().get();

  m_precalculated = true;
}

void Globals::reserve_shader(gfx::ShaderSptr &dst, const char *cstr)
{
  dst = gfx::ShaderSptr(new gfx::Shader(cstr));
}

void Globals::unreserve()
{
  m_precalculated = false;
  m_console.setBackground(NULL);
  gfx::Shader::storageClear();
  m_shader_atmosphere = NULL;
  m_shader_distort = NULL;
  m_shader_object = NULL;
  m_shader_overlay = NULL;
  m_shader_billboard = NULL;
  m_shader_overlay_line = NULL;
  m_shader_planet = NULL;
  m_shader_sun = NULL;
  snd::Sample::storageClear();
  m_sample_alarm = NULL;
  m_sample_alarm_over = NULL;
  m_sample_contact = NULL;
  m_sample_flak = NULL;
  m_sample_illegal = NULL;
  m_sample_locked = NULL;
  m_sample_nuke = NULL;
  m_sample_railgun = NULL;
  m_sample_route_change = NULL;
  m_sample_route_change_accepted = NULL;
  m_sample_target_destroyed = NULL;
  gfx::Mesh::storageClear();
  m_mesh_bullet_flak = NULL;
  m_mesh_bullet_railgun = NULL;
  m_mesh_missile_anti = NULL;
  m_mesh_missile_nuke = NULL;
  m_mesh_silo = NULL;
  gfx::Texture2D::storageClear();
  m_texture_flak_ammo = NULL;
  m_texture_icon_bullet_flak = NULL;
  m_texture_icon_bullet_railgun = NULL;
  m_texture_icon_city = NULL;
  m_texture_icon_missile_anti_nuke = NULL;
  m_texture_icon_missile_anti_ship = NULL;
  m_texture_icon_silo = NULL;
  m_texture_missile_anti_nuke = NULL;
  m_texture_missile_anti_ship = NULL;
  m_texture_missile_nuke = NULL;
  m_texture_railgun_ammo = NULL;
  m_texture_silo_anti_nuke = NULL;
  m_texture_silo_anti_ship = NULL;
  m_texture_silo_both = NULL;
  m_texture_silo_anti_nuke_inactive = NULL;
  m_texture_silo_anti_ship_inactive = NULL;
  m_texture_silo_both_inactive = NULL;
  m_texture_target = NULL;
  m_texture_trace = NULL;
  for(unsigned ii = 0; (ii < m_texture_particle.size()); ++ii)
  {
    m_texture_particle[ii] = NULL;
  }
  gfx::Texture3D::storageClear();
}

SDL_Cursor* Globals::create_cursor_blank()
{
  uint8_t cdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  return SDL_CreateCursor(cdata, cdata, 8, 1, 0, 0);
}

void ob::glob_init(const gfx::SurfaceScreen &pscreen, const std::string &pdetail)
{
  glob_quit();
  data::log.connect(data::log_default);
  glob = new Globals(pscreen, pdetail);
  data::log.connect(log_console);
}

void ob::glob_precalc()
{
  if(!glob)
  {
    std::stringstream sstr;
    sstr << "globals not initialized";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  glob->precalc();
}

void ob::glob_quit()
{
  delete glob;
  glob = NULL;
  // If globals is NULL, set_game will not start a new thread.
  delete glob_get_game();
  game = NULL;
  data::log.disconnect_all_slots();
}

Game* ob::glob_get_game()
{
  if(game_is_ready)
  {
    if(NULL != game_thread.get())
    {
      game_thread->join();
      game_thread = thr::ThreadSptr();
    }
    return game;
  }
  return NULL;
}

void ob::glob_queue_game()
{
  if(game_is_ready)
  {
    BOOST_THROW_EXCEPTION(std::runtime_error(
          "can't queue a game when a game is ready"));
  }
  // Do not start a game calculator thread if one already running.
  if(NULL == game_thread.get())
  {
    game_thread = thr::ThreadSptr(new boost::thread(glob_task_game));
    fade.setDelta(OB_FADE_DELTA);
  }
}

void ob::glob_set_game(Game *op)
{
  if(op)
  {
    if(game)
    {
      std::stringstream sstr;
      sstr << "trying to set game when previous exists";
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
    game = op;
  }
  else
  {
    game = NULL;
    game_is_ready = false;
  }
}

