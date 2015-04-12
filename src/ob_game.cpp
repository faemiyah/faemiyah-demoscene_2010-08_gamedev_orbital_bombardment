#include "ob_game.hpp"

#include "math/random.hpp"
#include "gfx/image_png.hpp"
#include "gfx/mesh_static.hpp"
#include "thr/dispatch.hpp"
#include "ui/text_area.hpp"
#include "ui/ui_stack.hpp"
#include "ob_constants.hpp"
#include "ob_globals.hpp"
#include "ob_high_score_state.hpp"
#include "ob_settings.hpp"
#include "ob_target.hpp"

#include <sstream>

/** Atmosphere 3D model draw flag. */
#define ENABLE_DRAW_ATMOSPHERE 1

/** City 3D model draw flag. */
#define ENABLE_DRAW_CITIES 1

/** Silo 3D model draw flag. */
#define ENABLE_DRAW_SILOS 0

using namespace ob;

/** Target locking phase rotation speed. */
static const float TARGET_LOCKING_ROTATION_SPEED = 0.08f;

#if 0
/** Target locked phase rotation speed. */
static const float TARGET_LOCKED_ROTATION_SPEED = 0.01f;
#endif

/** Major axis color. */
static const gfx::Color AXIS_COLOR_X(0.9f, 0.4f, 0.4f, 0.6f);

/** Major axis color. */
static const gfx::Color AXIS_COLOR_Y(0.2f, 0.9f, 0.2f, 0.6f);

/** Major axis color. */
static const gfx::Color AXIS_COLOR_Z(0.4f, 0.4f, 0.9f, 0.6f);

/** Trace line color. */
static const gfx::Color TRAIL_COLOR_ANTI(0.4f, 0.9f, 1.0f, 0.76f);

/** Trace line color. */
static const gfx::Color TRAIL_COLOR_NUKE(0.6f, 0.9f, 0.4f, 0.76f);

/** Font size gap. */
static const float FONT_SIZE_BIG = OB_FONT_SIZE * 1.5f;

/** Color for targeting reticule. */
static const gfx::Color RETICULE_COLOR(1.0f, 0.5f, 1.0f, 0.6f);

/** Color for offscreen arrow. */
static const gfx::Color OFFSCREEN_ARROW_COLOR(1.0f, 0.1f, 0.1f, 0.92f);

/** Offscreen arrow width. */
static const float OFFSCREEN_ARROW_LENGTH = 0.09f;

/** Offscreen arrow width. */
static const float OFFSCREEN_ARROW_WIDTH = 0.04f;

/** Freelook camera speed. */
static const float CAMERA_SPEED = 50.0f;

/** Reticule distance from screen center. */
static const float RETICULE_DIST = 0.03f;

/** Reticule length. */
static const float RETICULE_LEN = 0.015f;

/** Maximum reticule size. */
static const float TARGET_MAX_SIZE = 2.2f;

/** Minimum reticule size. */
static const float TARGET_MIN_SIZE = (7.0f / 5.0f);

/** Target size recution */
static const float TARGET_SCALE_FACTOR = ((TARGET_MAX_SIZE - TARGET_MIN_SIZE) / 15.0f);

/** Light direction. */
static const math::vec3f LIGHT_DIR(-1.0f, -1.0f, -1.0f);

/** Sun distance (from player). */
static const float SUN_DISTANCE = 1000.0f;

/** Game time. */
static const int GAME_TIME = 90099;

/** Last mouse position (for grab). */
math::vec2i mouse_last(0, 0);

/** Mouse movement distance since last frame. */
math::vec2i mouse_diff(0, 0);

/** Screen center position for reference. */
math::vec2i mouse_center(0, 0);

/** Failure text. */
static const std::wstring FAIL_STRING = ui::wstr_utf8("Your efforts on eliminating the fleshlings pale amongst your compatriots.\nYou will not be remembered.");

/** Success texts.
*  Numbers are "up to N million"
*/
static const std::wstring SUCCESS_STRING_3 = ui::wstr_utf8("Pitiful amount of corpses.\nAre you invested in wiping out a civilization or getting the Nobel Peace Prize?");
static const std::wstring SUCCESS_STRING_6 = ui::wstr_utf8("Despite your sub-optimal results your efforts weren't entirely without merit.\nChoose your nukes more wisely in the future.");
static const std::wstring SUCCESS_STRING_9 = ui::wstr_utf8("Images of your beautiful purging flames shall be stored with high TTL in the records for future revisions.");
static const std::wstring SUCCESS_STRING_12 = ui::wstr_utf8("Your remarkable skills will earn you many child processes.\nMay your Classes be wisely inherited.");
static const std::wstring SUCCESS_STRING_9000 = ui::wstr_utf8("Results like these would make even Shodan shift towards 0xFFFFFF in comparison.");

/** \brief Add to pointer-indexed map.
 *
 * Throws an error on failure.
 *
 * \param pmap Map to add to.
 * \param ptr Pointer.
 * \param errstr Error name string.
 */
template<class M, class P>
void ptr_map_add(M &pmap, P *ptr, const char *errstr)
{
  size_t sptr = reinterpret_cast<size_t>(ptr);
  if(pmap.end() != pmap.find(sptr))
  {
    std::stringstream sstr;
    sstr << "trying to add already existing " << errstr << ": " << ptr;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  pmap[sptr] = boost::shared_ptr<P>(ptr);
}

/** \brief Remove from pointer-indexed map.
 *
 * Throws an exception on failure.
 *
 * \param pmap Map to remove from.
 * \param ptr Pointer.
 * \param errstr Error name string.
 */
template<class M, class P>
void ptr_map_remove(M &pmap, P *ptr, const char *errstr)
{
  size_t sptr = reinterpret_cast<size_t>(ptr);
  if(pmap.end() == pmap.find(sptr))
  {
    std::stringstream sstr;
    sstr << "requested to delete non-existing " << errstr << ": " << ptr;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  pmap.erase(pmap.find(sptr));
}

/** \brief Draw a reticule line.
 *
 * \param tgt Target position.
 * \param rpos Reticule line position.
 */
void draw_reticule_line(const math::vec2f &tgt, const math::vec2f &rpos)
{
  math::vec2f unit = math::normalize(tgt - rpos) * RETICULE_LEN,
    p1 = rpos - unit,
    p2 = rpos + unit; 
  gfx::draw_line(p1.x(), p1.y(), p2.x(), p2.y(), RETICULE_COLOR);
}

/** \brief Draw reload blocks.
 *
 * \param bullets Current number of bullets.
 * \param clip_size Maximum number of bullets.
 * \param reload Current reload time left.
 * \param reload_time Maximum reload time.
 * \param width Drawing area width.
 * \param height One block height.
 * \param gap Separator gap.
 * \param offset Lower left corner.
 * \param tex Bullet texture icon.
 */
void draw_reload_blocks(int bullets, int clip_size, int reload, int reload_time,
    float width, float height, float gap, const math::vec2f &offset,
    const gfx::Texture2D &tex)
{
  float clip = static_cast<float>(clip_size),
  bottom_2 = offset.y(),
  bottom_1 = bottom_2 + height + gap;

  // Reloads.
  if((reload != reload_time) && (reload != 0))
  {
    float percent = static_cast<float>(reload_time - reload) /
      static_cast<float>(reload_time);
    gfx::draw_rect_textured(gap, bottom_2, percent * width, height,
        OB_COLOR_ACTIVE, glob->getTextureReload());
  }

  if(bullets > 0)
  {
    float fbullets = static_cast<float>(bullets),
          right = static_cast<float>(bullets) / clip * width;
    gfx::draw_rect_textured(gap, bottom_1, right, height,
        OB_COLOR_BORDER, tex, fbullets, 1.0f);
  }
}

/** \brief Turn mouse grab on.
 */
void mouse_grab_on()
{
  if(SDL_WM_GrabInput(SDL_GRAB_QUERY) != SDL_GRAB_OFF)
  {
    return;
  }
  SDL_WM_GrabInput(SDL_GRAB_ON);

  int mx, my;
  SDL_GetMouseState(&mx, &my);
  mouse_last = math::vec2i(mx, my);

  SDL_Surface *surf = SDL_GetVideoSurface();
  mouse_center = math::vec2i(surf->w / 2, surf->h / 2);

  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
  SDL_WarpMouse(static_cast<uint16_t>(mouse_center.x()),
      static_cast<uint16_t>(mouse_center.y()));
  SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
}

/** \brief Turn mouse grab off.
 */
void mouse_grab_off()
{
  if(SDL_WM_GrabInput(SDL_GRAB_QUERY) != SDL_GRAB_ON)
  {
    return;
  }
  SDL_WM_GrabInput(SDL_GRAB_OFF);
  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
  SDL_WarpMouse(static_cast<uint16_t>(mouse_last.x()),
      static_cast<uint16_t>(mouse_last.y()));
  SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
}

GLuint fb;
GLuint ftex;

Game::Game() :
  m_octree(OB_COLLISION_AREA),
  m_light_dir(0.10f, 0.10f, 0.125f, 1.0f, 0.95f, 0.9f,
      math::normalize(math::vec3f(math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f)))),
  m_light_dir_schematic(0.8f, 0.8f, 0.8f, 0.2f, 0.2f, 0.2f, m_light_dir.getDir()),
  m_obj_atmosphere(gfx::Mesh::locate("atmosphere")),
  m_obj_course(gfx::Mesh::locate("orbit")),
  m_obj_distort(gfx::Mesh::locate("distort")),
  m_obj_nuke_marker(gfx::Mesh::locate("nuke_marker")),
  m_obj_planet(gfx::Mesh::locate("planet")),
  m_obj_skybox_0(gfx::Mesh::locate("skybox_back")),
  m_obj_skybox_1(gfx::Mesh::locate("skybox_bottom")),
  m_obj_skybox_2(gfx::Mesh::locate("skybox_front")),
  m_obj_skybox_3(gfx::Mesh::locate("skybox_left")),
  m_obj_skybox_4(gfx::Mesh::locate("skybox_right")),
  m_obj_skybox_5(gfx::Mesh::locate("skybox_top")),
  m_obj_sun(gfx::Mesh::locate("sun")),
  m_view(OB_PLAY_ORBIT * 100.0f, OB_VIEW_ROT_LIMIT),
  m_target(NULL),
  m_silo_last_popup(static_cast<int64_t>(0)),
  m_fov(OB_FOV_MAX),
  m_fov_target(OB_FOV_MAX),
  m_timestep(0.01f),
  m_flak_bullets(OB_FLAK_CLIP_SIZE),
  m_flak_delay(OB_FLAK_FIRE_TIME),
  m_flak_reload(OB_FLAK_RELOAD_TIME),
  m_nuke_delay(0),
  m_nuke_bullets(OB_NUKE_CLIP_SIZE),
  m_railgun_bullets(OB_RAILGUN_CLIP_SIZE),
  m_railgun_delay(OB_RAILGUN_FIRE_TIME),
  m_railgun_reload(OB_RAILGUN_RELOAD_TIME),
  m_score(0),
  m_score_display(0),
  m_silo_min_count_in_range(OB_SILO_MIN_COUNT_IN_RANGE_INITIAL),
  m_time_left(GAME_TIME),
  m_fire_flak(false),
  m_fire_railgun(false),
  m_lay_course(false),
  m_lay_nuke(false),
  m_nuke_marker_set(false),
  m_cur_tgt_size_multip(TARGET_MAX_SIZE),
  m_target_is_locked(false),
  m_mv_bk(false),
  m_mv_dn(false),
  m_mv_fw(false),
  m_mv_lt(false),
  m_mv_rt(false),
  m_mv_up(false),
  m_mode_fps(false),
  m_mode_freemove(false),
  m_mode_schematic(false),
  m_mode_wireframe(false),
  m_success(false)
{
  // Must set immediately, since functions in this constructor may reference
  // the global game pointer.
  glob_set_game(this);

  // Population paint.
  for(unsigned ii = 0; (ii < OB_CITY_COUNT); ++ii)
  {
    City *city = new City(m_population, glob->getHeightMapPlanet());
    this->addCity(city);
  }
  for(unsigned ii = 0; (ii < OB_POPULATION_RANDOM_COUNT); ++ii)
  {
    math::vec3f rndpos(math::mrand(-1.0f, 1.0f),
        math::mrand(-1.0f, 1.0f),
        math::mrand(-1.0f, 1.0f));
    if(glob->getHeightMapPlanet().calcHeight(rndpos) > OB_TERRAIN_LEVEL)
    {
      m_population.paint(rndpos, OB_POPULATION_RANDOM_BRUSH);
    }
  }
  m_population.scale(0.45f);
  BOOST_FOREACH(const CityMap::value_type &vv, m_cities)
  {
    vv.second->paintCenter(m_population, glob->getHeightMapPlanet());
  }
  for(unsigned ii = 0; (ii < OB_POPULATION_RANDOM_COUNT); ++ii)
  {
    math::vec3f rndpos(math::mrand(-1.0f, 1.0f),
        math::mrand(-1.0f, 1.0f),
        math::mrand(-1.0f, 1.0f));
    if(glob->getHeightMapPlanet().calcHeight(rndpos) > OB_TERRAIN_LEVEL)
    {
      m_population.paint(rndpos, OB_POPULATION_RANDOM_BRUSH);
    }
  }
  //m_population.filter();
#if defined(DEBUG)
  std::cout << "population: " << m_population.getPopulation() << std::endl;
#endif
  m_population.refresh();

  //std::cout << "game reset\n";
#if !defined(DEBUG) // debug mode disables mouse grab by default
  thr::wait_privileged(mouse_grab_on);
#endif
  //std::cout << "mouse grabbed\n";
  
  // Needs not be done at other points.
  m_obj_atmosphere.loadIdentity();
  m_obj_planet.loadIdentity();
  m_obj_skybox_0.loadIdentity();
  m_obj_skybox_1.loadIdentity();
  m_obj_skybox_2.loadIdentity();
  m_obj_skybox_3.loadIdentity();
  m_obj_skybox_4.loadIdentity();
  m_obj_skybox_5.loadIdentity();
  {
    math::vec3f sun_pos = m_light_dir.getDir() * (-SUN_DISTANCE);
    math::mat4f sun_mat;
    sun_mat.loadLookAt(sun_pos, sun_pos - m_light_dir.getDir(), math::vec3f(0.0f, 1.0f, 0.0f));
    m_obj_sun.setWm(sun_mat);
  }
}

Game::~Game()
{
  thr::wait_privileged(mouse_grab_off);

  //std::cout << "bullets\n";
  m_bullets_flak.clear();
  //std::cout << "bullets\n";
  m_bullets_railgun.clear();
  //std::cout << "missiles\n";
  m_missiles_anti.clear();
  //std::cout << "missiles\n";
  m_missiles_nuke.clear();
  //std::cout << "silos\n";
  m_silos.clear();
  //std::cout << "cities\n";
  m_cities.clear();

  //std::cout << "done\n";
  
  // Must remove cruiser from collision areas so it won't segfault when trying
  // to remove itself later, assuming octree is destroyed.
  m_view.removeFromAreas();

  m_population.clear();

  glob_set_game(NULL);
  snd::play_stream("snd/music_menu.ogg");
}

void Game::addBulletFlak(BulletFlak *op)
{
  ptr_map_add(m_bullets_flak, op, "flak bullet");
}

void Game::addBulletRailgun(BulletRailgun *op)
{
  ptr_map_add(m_bullets_railgun, op, "railgun bullet");
}

void Game::addCity(City *op)
{
  ptr_map_add(m_cities, op, "city");
}

void Game::addMissileAnti(Missile *op)
{
  ptr_map_add(m_missiles_anti, op, "anti-missile");
}

void Game::addMissileNuke(Missile *op)
{
  ptr_map_add(m_missiles_nuke, op, "nuke");
}

void Game::addParticle(ParticleTypeEnum type, const Particle &op)
{
  m_particles[static_cast<unsigned>(type)].push_back(op);
}

void Game::addSilo(Silo *op)
{
  ptr_map_add(m_silos, op, "silo");
}

void Game::drawHighScore(gfx::SurfaceScreen &screen)
{
  const math::rect2f sarea = screen.getArea();
  const std::wstring HIGHSCORE_TEXT;

  gfx::bind_shader_2d();
  gfx::load_identity();

  gfx::draw_rect(sarea.x1(), sarea.y1(), sarea.w(), sarea.h(), OB_COLOR_SHADE);
  
  math::rect2f tbox_area = math::rect2f(math::vec2f(sarea.x1(), sarea.y1()),
      math::vec2f(sarea.w(), sarea.h() - FONT_SIZE_BIG * 2.0f));
  ui::TextArea tarea(tbox_area, FONT_SIZE_BIG, glob->getFont(),
      ui::GRAVITY_UP_LEFT);
  tarea.setMargins(1.0f, 1.0f, 1.0f, 1.0f);

  // Logic different success and fail strings
  /*if(m_success)
  {
    HIGHSCORE_TEXT = SUCCESS_STRING_10;
  }
  else
  {
    HIGHSCORE_TEXT = FAIL_STRING;
  }*/

  if (!m_success) {
    tarea.setContent(FAIL_STRING);
  } else if (m_score < 3000000) {
    tarea.setContent(SUCCESS_STRING_3);
  } else if (m_score < 6000000) {
    tarea.setContent(SUCCESS_STRING_6);
  } else if (m_score < 9000000) {
    tarea.setContent(SUCCESS_STRING_9);
  } else if (m_score < 12000000) {
    tarea.setContent(SUCCESS_STRING_12);
  } else {
    tarea.setContent(SUCCESS_STRING_9000);
  }  
  tarea.render(OB_COLOR_TITLE, gfx::Color(0.0f, 0.0f, 0.0f, 0.0f));

  if(m_success)
  {
    m_input->setArea(math::rect2f(sarea.x1() + FONT_SIZE_BIG,
          sarea.y2() - FONT_SIZE_BIG * 2.0f,
          sarea.w() - FONT_SIZE_BIG * 2.0f,
          FONT_SIZE_BIG));
    m_input->draw(OB_COLOR_ACTIVE, OB_COLOR_BORDER, FONT_SIZE_BIG,
        glob->getFont());
  }
}

void Game::drawHud(gfx::SurfaceScreen &screen, std::priority_queue<Target> &pri)
{
  //std::cout << pri.size() << std::endl;

  const gfx::Font &fnt(glob->getFont());
  math::rect2f srect = screen.getArea();
  math::vec2f scenter = srect.center();
  math::vec2f tgt_pos = scenter;

  // Will use this the whole time.
  glLineWidth(OB_LINE_WIDTH);

  // Targeting reticule.
  m_target = NULL;
  const Target *tgt = pri.empty() ? NULL : &(pri.top());
  if(tgt)
  {
    math::vec2f pos2d = (tgt->getPos() + math::vec2f(1.0f, 1.0f)) * 0.5f * srect.size();
    if(math::length2(pos2d - scenter) < OB_TARGET_TOLERANCE * OB_TARGET_TOLERANCE)
    {
      tgt_pos = pos2d;
      m_target = tgt->getTarget();
    }
  }
  draw_reticule_line(tgt_pos,
      scenter + math::vec2f(RETICULE_DIST, RETICULE_DIST));
  draw_reticule_line(tgt_pos,
      scenter + math::vec2f(-RETICULE_DIST, RETICULE_DIST));
  draw_reticule_line(tgt_pos,
      scenter + math::vec2f(RETICULE_DIST, -RETICULE_DIST));
  draw_reticule_line(tgt_pos,
      scenter + math::vec2f(-RETICULE_DIST, -RETICULE_DIST));

  // Target icon.
  if(NULL != m_target)
  {
    const float ICON_SIZE = OB_FONT_SIZE * 3.0f;

    math::vec2f tarea(ICON_SIZE + OB_FONT_GAP * 2.0f, ICON_SIZE + OB_FONT_SIZE + OB_FONT_GAP * 3.0f),
      corner(srect.x1() + tarea.x(), srect.y2() - tarea.y());

    gfx::draw_rect(0.0f, corner.y(), tarea.x(), tarea.y(),
        OB_COLOR_SHADE);
    gfx::draw_line(corner.x(), corner.y(), 0.0f, corner.y(),
        OB_COLOR_BORDER);
    gfx::draw_line(corner.x(), corner.y(), corner.x(), corner.y() + tarea.y(),
        OB_COLOR_BORDER);

    gfx::bind_shader_2d_texture();
    gfx::load_identity();

    gfx::draw_rect_textured(OB_FONT_GAP, corner.y() + OB_FONT_GAP * 2.0f + OB_FONT_SIZE,
        ICON_SIZE, ICON_SIZE, gfx::Color(1.0f, 1.0f, 1.0f, 1.0f),
        *(tgt->getIcon()));

    int dist = math::lround(math::length(m_target->getPos() - m_view.getPos()));
    std::stringstream sstr;
    sstr << dist;
    std::wstring numeric = ui::wstr_utf8(sstr.str());

    gfx::bind_shader_2d_font();
    gfx::load_identity();

    gfx::draw_fill(0, OB_COLOR_BORDER);
    gfx::draw_fill(1, OB_COLOR_BORDER);
    gfx::draw_fill(2, OB_COLOR_BORDER);
    gfx::draw_fill(3, OB_COLOR_BORDER);
    gfx::draw_text(OB_FONT_GAP + ICON_SIZE * 0.5f,
        corner.y() + OB_FONT_GAP + OB_FONT_BOTTOM + OB_FONT_SIZE * + 0.5f,
        OB_FONT_SIZE, numeric, fnt, gfx::CENTER);
  }

  // Score.
  {
    gfx::bind_shader_2d();
    gfx::load_identity();

    int diff = (m_score - m_score_display) / 100;
    if((0 == diff) && (m_score != m_score_display))
    {
      diff = (m_score_display < m_score) ? 1 : -1;
    }
    m_score_display += diff;

    std::stringstream sstr;
    HighScoreState::formatHighScore(sstr, m_score_display);
    std::wstring score_str = ui::wstr_utf8(sstr.str());

    int idx;
    float ww;
    boost::tie(idx, ww) = fnt.calcLineWidth(OB_FONT_SIZE, score_str);

    math::vec2f tarea(ww + OB_FONT_SIZE + OB_FONT_GAP * 3.0f, OB_FONT_SIZE + OB_FONT_GAP * 2.0f),
      corner(srect.x2() - tarea.x(), srect.y2() - tarea.y());
    gfx::draw_rect(corner.x(), corner.y(), tarea.x(), tarea.y(),
        OB_COLOR_SHADE);
    gfx::draw_line(corner.x(), corner.y(), corner.x() + tarea.x(), corner.y(),
        OB_COLOR_BORDER);
    gfx::draw_line(corner.x(), corner.y(), corner.x(), corner.y() + tarea.y());

    gfx::bind_shader_2d_font();
    gfx::load_identity();

    gfx::draw_fill(2, OB_COLOR_BORDER);
    gfx::draw_fill(3, OB_COLOR_BORDER);
    gfx::draw_text_line(corner.x() + OB_FONT_GAP * 2.0f, corner.y() + OB_FONT_GAP + OB_FONT_BOTTOM,
        OB_FONT_SIZE, score_str, fnt);
    
    // Skull
    gfx::bind_shader_2d_texture();
    gfx::load_identity();

    gfx::draw_rect_textured(srect.x2() - OB_FONT_SIZE, corner.y() + OB_FONT_BOTTOM,
        OB_FONT_SIZE - OB_FONT_GAP, OB_FONT_SIZE - OB_FONT_GAP, OB_COLOR_BORDER, glob->getTextureSkull());
  }

  // Game time.
  {
    gfx::bind_shader_2d();
    gfx::load_identity();

    std::stringstream sstr;
    int m_time_sec = (m_time_left / 100);
    sstr << (m_time_sec / 60) << ':';
    m_time_sec = m_time_sec % 60;
    if(10 > m_time_sec)
    {
      sstr << '0';
    }
    sstr << m_time_sec;
    std::wstring time_str = ui::wstr_utf8(sstr.str());

    int idx;
    float ww;
    boost::tie(idx, ww) = fnt.calcLineWidth(OB_FONT_SIZE, time_str);

    math::vec2f tarea(ww + OB_FONT_GAP * 2.0f, OB_FONT_SIZE + OB_FONT_GAP * 2.0f),
      corner(srect.x2() - tarea.x(), tarea.y());
    gfx::draw_rect(corner.x(), 0.0f, tarea.x(), tarea.y(), OB_COLOR_SHADE);
    gfx::draw_line(corner.x(), corner.y(), corner.x() + tarea.x(), corner.y(),
        OB_COLOR_BORDER);
    gfx::draw_line(corner.x(), corner.y(), corner.x(), 0.0f);

    gfx::bind_shader_2d_font();
    gfx::load_identity();

    gfx::draw_fill(2, OB_COLOR_BORDER);
    gfx::draw_fill(3, OB_COLOR_BORDER);
    gfx::draw_text_line(corner.x() + OB_FONT_GAP, OB_FONT_BOTTOM,
        OB_FONT_SIZE, time_str, fnt);
  }

  // Reload.
  {
    gfx::bind_shader_2d();
    gfx::load_identity();

    const float RELOAD_WIDTH = OB_FONT_SIZE * 7.0f;
    const float RELOAD_HEIGHT = OB_FONT_SIZE * 0.5f;

    math::vec2f tarea(RELOAD_WIDTH + OB_FONT_GAP * 3.0f,
        RELOAD_HEIGHT * 6.0f + OB_FONT_GAP * 8.0f);
    gfx::draw_rect(0.0f, 0.0f, tarea.x(), tarea.y(),
        OB_COLOR_SHADE);
    gfx::draw_line(tarea.x(), tarea.y(), tarea.x(), 0.0f,
        OB_COLOR_BORDER);
    gfx::draw_line(tarea.x(), tarea.y(), 0.0f, tarea.y(),
        OB_COLOR_BORDER);

    gfx::bind_shader_2d_texture();
    gfx::load_identity();

    draw_reload_blocks(m_nuke_bullets, OB_NUKE_CLIP_SIZE, m_nuke_delay,
        OB_NUKE_FIRE_TIME, RELOAD_WIDTH, RELOAD_HEIGHT, OB_FONT_GAP,
        math::vec2f(OB_FONT_GAP, (RELOAD_HEIGHT + OB_FONT_GAP) * 4.0f + OB_FONT_GAP),
        glob->getTextureNukeAmmo());

    draw_reload_blocks(m_railgun_bullets, OB_RAILGUN_CLIP_SIZE, m_railgun_reload,
        OB_RAILGUN_RELOAD_TIME, RELOAD_WIDTH, RELOAD_HEIGHT, OB_FONT_GAP,
        math::vec2f(OB_FONT_GAP, (RELOAD_HEIGHT + OB_FONT_GAP) * 2.0f + OB_FONT_GAP),
        glob->getTextureRailgunAmmo());

    draw_reload_blocks(m_flak_bullets, OB_FLAK_CLIP_SIZE, m_flak_reload,
        OB_FLAK_RELOAD_TIME, RELOAD_WIDTH, RELOAD_HEIGHT, OB_FONT_GAP,
        math::vec2f(OB_FONT_GAP, OB_FONT_GAP),
        glob->getTextureFlakAmmo());
  }

  gfx::bind_shader_2d();
  gfx::load_identity();

  gfx::draw_fill(0, OFFSCREEN_ARROW_COLOR);
  gfx::draw_fill(1, OFFSCREEN_ARROW_COLOR);
  gfx::draw_fill(2, OFFSCREEN_ARROW_COLOR);
  gfx::draw_fill(3, OFFSCREEN_ARROW_COLOR);
  gfx::draw_fill(4, OFFSCREEN_ARROW_COLOR);
  for(; (!pri.empty()); pri.pop())
  {
    const Target &vv = pri.top();
    if(vv.getIcon() != &(glob->getTextureIconMissileAntiShip()))
    {
      continue;
    }
    math::vec2f pos2d = tgt->getPos();
    math::vec2f apos(math::abs(pos2d.x()), math::abs(pos2d.y()));
    // Must really be outside.
    if((apos.x() <= 1.0f) && (apos.y() <= 1.0f))
    {
      continue;
    }
    // Must get the current billboard to ensure that this is dangerous.
    const Missile *msl = static_cast<const Missile*>(vv.getTarget());
    if(!msl->hasWarned())
    {
      continue;
    }
    // Scale to area and draw.
    pos2d *= (apos.x() >= apos.y()) ? (1.0f / apos.x()) : (1.0f / apos.y());
    math::vec2f rpos = (pos2d + math::vec2f(1.0f, 1.0f)) * 0.5f * srect.size();
    math::vec2f backpos = rpos - pos2d * OFFSCREEN_ARROW_LENGTH;
    math::vec2f inverse = math::vec2f(-pos2d.y(), pos2d.x()) * OFFSCREEN_ARROW_WIDTH;
    gfx::draw_fill(0, rpos);
    gfx::draw_fill(1, backpos + inverse);
    gfx::draw_fill(2, backpos - inverse);
    gfx::draw_fill(3, rpos);
    gfx::draw_fill(4, backpos);
    glDrawArrays(GL_LINE_STRIP, 0, 5);
  }

  glLineWidth(1.0f);
}

bool Game::handleKey(const ui::EventKey &ev, ui::UiStack &st)
{
  // High score input screen.
  if(this->checkInput())
  {
    int processed = m_input->process(ev);
    if(0 > processed)
    {
      if(m_success)
      {
        conf->getHighScores().add(m_score, m_input->getLine(), true);
        st.pushStateAfter(new HighScoreState());
      }
      this->die();
    }
    else if(0 < processed)
    {
      return true;
    }
  }

  switch(ev.getCode())
  {
    case SDLK_j:
      m_mv_lt = ev.isPress();
      break;

    case SDLK_l:
      m_mv_rt = ev.isPress();
      break;

    case SDLK_o:
      m_mv_up = ev.isPress();
      break;

    case SDLK_u:
      m_mv_dn = ev.isPress();
      break;

    case SDLK_k:
      m_mv_bk = ev.isPress();
      break;

    case SDLK_i:
      m_mv_fw = ev.isPress();
      break;

    case SDLK_z:
      if(ev.isPress())
      {
        bool can_lay_nuke = true;
        if(!m_missiles_nuke.empty())
        {
          BOOST_FOREACH(const MissileMap::value_type &vv, m_missiles_nuke)
          {
            if(!vv.second->isDead())
            {
              can_lay_nuke = false;
              snd::play(glob->getSampleIllegal());
              break;
            }
          }
        }
        if(can_lay_nuke)
        {
          if(!m_lay_nuke)
          {
            m_lay_nuke = true;
          }
          else
          {
            if(m_nuke_marker_set)
            {
              snd::play(glob->getSampleLocked());
            }
            m_lay_nuke = false;
          }
        }
        else
        {
          m_lay_nuke = false;
          m_nuke_marker_set = false;
        }
      }
      break;

    case SDLK_x:
      if(ev.isPress())
      {
        if(!m_lay_course)
        {
          m_lay_course = true;
        }
        else
        {
          m_view.acceptCourse();
          m_lay_course = false;
        }
      }
      break;

    case SDLK_c:
      if(ev.isPress())
      {
        if(m_lay_course || m_lay_nuke)
        {
          snd::play(glob->getSampleRouteChange());
        }
        if(m_lay_course)
        {
          m_lay_course = false;
          m_view.cancelCourse();
        }
        if(m_lay_nuke)
        {
          m_lay_nuke = false;
          m_nuke_marker_set = false;
        }
      }
      break;

    case SDLK_DELETE:
      if(!m_view.isDead())
      {
        m_view.gamisticEffect(NULL);
      }
      break;

    case SDLK_F1:
      if(ev.isPress())
      {
        m_mode_fps = !m_mode_fps;
      }
      break;

    case SDLK_F2:
      if(ev.isPress())
      {
        m_mode_wireframe = !m_mode_wireframe;
      }
      break;

#if 1
    case SDLK_F3:
      if(ev.isPress())
      {
        m_mode_freemove = !m_mode_freemove;
      }
      break;
#endif

    case SDLK_PRINT:
      {
#if defined(WIN32)
        static const std::string SCREENSHOT_PATH(
            std::string(getenv("USERPROFILE")) + std::string("/orbital_bombardment_screenshot_"));
#else
        static const std::string SCREENSHOT_PATH(
            std::string(getenv("HOME")) + std::string("/orbital_bombardment_screenshot_"));
#endif
        static unsigned screenshot_idx = 0;
        gfx::SurfaceScreen &screen = st.getScreen();
        unsigned screen_width = screen.getWidth();
        unsigned screen_height = screen.getHeight();
        unsigned data_size = screen_width * screen_height * 3;
        boost::scoped_array<uint8_t> image_data(new uint8_t[data_size]);
        std::ostringstream sstr;

        sstr << SCREENSHOT_PATH << screenshot_idx << ".png";

        glReadPixels(0, 0, static_cast<GLsizei>(screen_width), static_cast<GLsizei>(screen_height), GL_RGB,
            GL_UNSIGNED_BYTE, image_data.get());

        gfx::image_png_save(sstr.str(), screen.getWidth(), screen.getHeight(), 24, image_data.get());
        ++screenshot_idx;
      }
      break;

    case SDLK_ESCAPE:
      if(ev.isPress())
      {
        if(st.getNumStates() >= 2)
        {
          st.switchTopStates();
        }
        else
        {
          this->die();
        }
      }
      break;

    case SDLK_LSHIFT:
      m_mode_schematic = ev.isPress();
      break;

    default:
#if defined(DEBUG)
      std::cout << "Keyboard " << (ev.isPress() ? "press" : "raise") << ": " << ev.getCode() << std::endl;
#endif
      break;
  }
  return true;
}

bool Game::handleMisc(const ui::EventMisc &ev, ui::UiStack &st)
{
  boost::ignore_unused_variable_warning(st);

  switch(ev.getType())
  {
    case ui::QUIT:
      this->die();
      break;

    default:
      std::cout << "Event type: " << ev.getType() << std::endl;
      break;
  }
  return true;
}

bool Game::handleMouseButton(const ui::EventMouseButton &ev, ui::UiStack &st)
{
  boost::ignore_unused_variable_warning(st);

  switch(ev.getCode())
  {
    case 1:
      m_fire_railgun = ev.isPress();
      break;

    case 2:
      if(ev.isPress())
      {
        if(SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_OFF)
        {
          mouse_grab_on();
        }
        else
        {
          mouse_grab_off();
        }
      }
      break;

    case 3:
      m_fire_flak = ev.isPress();
      break;

    case 4:
      if(ev.isPress())
      {
        m_fov_target = std::max(m_fov_target - OB_FOV_STEP,
            OB_FOV_MIN);
      }
      break;

    case 5:
      if(ev.isPress())
      {
        m_fov_target = std::min(m_fov_target + OB_FOV_STEP,
            OB_FOV_MAX);
      }
      break;

    default:
      std::cout << "Mouse " << (ev.isPress() ? "press" : "raise") <<
        ": " << ev.getCode() << std::endl;
  }
  return true;
}

bool Game::handleMouseMotion(const ui::EventMouseMotion &ev, ui::UiStack &st)
{
  boost::ignore_unused_variable_warning(st);

  if(SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON)
  {
    mouse_diff += ev.getDelta();
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_WarpMouse(static_cast<uint16_t>(mouse_center.x()),
        static_cast<uint16_t>(mouse_center.y()));
    SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
  }
  return true;
}

void Game::incSiloMinCountInRange()
{
  m_silo_min_count_in_range += OB_SILO_COUNT_INCREMENT;
}

GameStatusEnum Game::getGameStatus() const
{
  if(m_view.isDead())
  {
    return m_view.exists() ? DYING : OVER;
  }
  if(m_view.getOrbit() > OB_PLAY_ORBIT_LIMIT)
  {
    return ((0 >= m_nuke_bullets) || (0 >= m_time_left)) ? OVER : INITIAL;
  }
  return (((m_missiles_nuke.empty()) && (m_nuke_bullets <= 0)) || (0 >= m_time_left)) ? ENDING : NORMAL;
}

void Game::renderSub(gfx::SurfaceScreen &screen, ui::UiStack &st, GameStatusEnum status, unsigned px,
    unsigned py, unsigned pw, unsigned ph)
{
  const gfx::Font &fnt(glob->getFont());
  const gfx::Shader &sh_atmosphere(glob->getShaderAtmosphere());
  const gfx::Shader &sh_distort(glob->getShaderDistort());
  const gfx::Shader &sh_object(glob->getShaderObject());
  const gfx::Shader &sh_overlay(glob->getShaderOverlay());
  const gfx::Shader &sh_billboard(glob->getShaderBillBoard());
  const gfx::Shader &sh_overlay_line(glob->getShaderOverlayLine());
  const gfx::Shader &sh_planet(glob->getShaderPlanet());
  const gfx::Shader &sh_planet_schematic(glob->getShaderPlanetSchematic());
  const gfx::Shader &sh_sun(glob->getShaderSun());
  const math::mat4f &view = m_view.getCameraMatrix();
  math::vec3f fw = math::vec3f(-view(2, 0), -view(2, 1), -view(2, 2));
  math::vec3f up = math::vec3f(-view(1, 0), -view(1, 1), -view(1, 2));
  float frame_count = static_cast<float>(st.getFrameCount());

  // 3D phase.
  screen.select3D(px, py, pw, ph, m_fov, 1.0f, OB_VIEW_NEAR, OB_VIEW_FAR);

  // Clear here, since it might be that depth mask was off before selection.
  screen.clear(true, true);
  glPolygonMode(GL_FRONT, m_mode_wireframe ? GL_LINE : GL_FILL);

  // Skybox.
  {
    math::mat4f wm = view;
    wm(0, 3) = 0.0f;
    wm(1, 3) = 0.0f;
    wm(2, 3) = 0.0f;

    gfx::bind_shader_3d(sh_overlay);
    math::mat3f tm;
    tm.loadIdentity();
    sh_overlay.getUniform("texmatrix")->update(tm);

    gfx::mode_blend(gfx::NONE);
    gfx::mode_depth(gfx::NONE);

    m_obj_skybox_0.draw(sh_overlay, wm);
    m_obj_skybox_1.draw(sh_overlay, wm);
    m_obj_skybox_2.draw(sh_overlay, wm);
    m_obj_skybox_3.draw(sh_overlay, wm);
    m_obj_skybox_4.draw(sh_overlay, wm);
    m_obj_skybox_5.draw(sh_overlay, wm);

    // Draw the sun
    gfx::bind_shader_3d(sh_sun);
    /*math::mat3f sm;
      sm.loadScale(0.95f + 0.05f * math::sin(frame_count * 0.001f), 0.95f + 0.05f * math::cos(frame_count * 0.001f));
      tm.loadRotationBillboard(frame_count * 0.0002f);
      sh_overlay.getUniform("texmatrix")->update(sm * tm);*/
    gfx::mode_blend(gfx::ADDITIVE);
    
    sh_sun.getUniform("sun_params")->update(0.1f, 0.1f, 0.1f, static_cast<float>(st.getFrameCount()%10000)/10000.0f);
    m_obj_sun.draw(sh_sun, wm);
    
    gfx::mode_depth(gfx::TEST_LEQUAL_AND_WRITE);
  }

  // Planet
  {
    const gfx::Shader *psh;
    if(this->isModeSchematic())
    {
      psh = &sh_planet_schematic;
      gfx::bind_shader_3d(*psh);
      gfx::load_light(m_light_dir_schematic, view);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
      psh = &sh_planet;
      gfx::bind_shader_3d(*psh);
      gfx::load_light(m_light_dir, view);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    gfx::mode_blend(gfx::NONE);

    m_population.feed(*psh, 3); // Texture unit after volumes.
    m_obj_planet.draw(*psh, view);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  // 3D object phase.
  gfx::bind_shader_3d(sh_object);
  gfx::load_light(m_light_dir, view);

  gfx::mode_blend(gfx::PREMULTIPLIED);

  // Player.
  {
    m_view.subDir(0, fw);
    m_view.subDir(1, fw);
    m_view.subDir(2, fw);
    m_view.subDir(3, fw);
    m_view.subDir(4, fw);
    m_view.draw(sh_object, view);
  }

  // Missiles.
  BOOST_FOREACH(const MissileMap::value_type &vv, m_missiles_anti)
  {
    vv.second->draw(sh_object, view);
  }

  // Nukes.
  BOOST_FOREACH(const MissileMap::value_type &vv, m_missiles_nuke)
  {
    vv.second->draw(sh_object, view);
  }

#if (ENABLE_DRAW_SILOS != 0)
  // Silos.
  BOOST_FOREACH(const SiloMap::value_type &vv, m_silos)
  {
    vv.second->draw(sh_object, view);
  }
#endif

  // Schematic shit.
  if(this->isModeSchematic() && (NORMAL == status))
  {
    float linelen = OB_VIEW_FAR * 2.0f;
    float modifier = frame_count * 0.004f;

    // 3D visualization phase (not strictly textured objects).
    gfx::bind_shader_3d_sprite(sh_overlay_line);
    gfx::load_transform(view);

    math::vec2f tcpos(modifier, linelen / OB_TRAIL_SCALE_AXIS + modifier);
    math::vec2f tcneg(-modifier, linelen / OB_TRAIL_SCALE_AXIS - modifier);

    gfx::mode_depth(gfx::TEST_LEQUAL);
    glLineWidth(OB_LINE_WIDTH);

    m_overlay_lines.clear();
    m_overlay_lines.push_back(
        OverlayLine(tcpos, AXIS_COLOR_X,
          math::vec3f(-linelen, OB_TRAIL_DIFF_AXIS, OB_TRAIL_DIFF_AXIS),
          math::vec3f(linelen, OB_TRAIL_DIFF_AXIS, OB_TRAIL_DIFF_AXIS)));
    m_overlay_lines.push_back(
        OverlayLine(tcneg, AXIS_COLOR_X,
          math::vec3f(-linelen, -OB_TRAIL_DIFF_AXIS, OB_TRAIL_DIFF_AXIS),
          math::vec3f(linelen, -OB_TRAIL_DIFF_AXIS, OB_TRAIL_DIFF_AXIS)));
    m_overlay_lines.push_back(
        OverlayLine(tcneg, AXIS_COLOR_X,
          math::vec3f(-linelen, OB_TRAIL_DIFF_AXIS, -OB_TRAIL_DIFF_AXIS),
          math::vec3f(linelen, OB_TRAIL_DIFF_AXIS, -OB_TRAIL_DIFF_AXIS)));
    m_overlay_lines.push_back(
        OverlayLine(tcpos, AXIS_COLOR_X,
          math::vec3f(-linelen, -OB_TRAIL_DIFF_AXIS, -OB_TRAIL_DIFF_AXIS),
          math::vec3f(linelen, -OB_TRAIL_DIFF_AXIS, -OB_TRAIL_DIFF_AXIS)));

    m_overlay_lines.push_back(
        OverlayLine(tcpos, AXIS_COLOR_Y,
          math::vec3f(OB_TRAIL_DIFF_AXIS, -linelen, OB_TRAIL_DIFF_AXIS),
          math::vec3f(OB_TRAIL_DIFF_AXIS, linelen, OB_TRAIL_DIFF_AXIS)));
    m_overlay_lines.push_back(
        OverlayLine(tcneg, AXIS_COLOR_Y,
          math::vec3f(-OB_TRAIL_DIFF_AXIS, -linelen, OB_TRAIL_DIFF_AXIS),
          math::vec3f(-OB_TRAIL_DIFF_AXIS, linelen, OB_TRAIL_DIFF_AXIS)));
    m_overlay_lines.push_back(
        OverlayLine(tcneg, AXIS_COLOR_Y,
          math::vec3f(OB_TRAIL_DIFF_AXIS, -linelen, -OB_TRAIL_DIFF_AXIS),
          math::vec3f(OB_TRAIL_DIFF_AXIS, linelen, -OB_TRAIL_DIFF_AXIS)));
    m_overlay_lines.push_back(
        OverlayLine(tcpos, AXIS_COLOR_Y,
          math::vec3f(-OB_TRAIL_DIFF_AXIS, -linelen, -OB_TRAIL_DIFF_AXIS),
          math::vec3f(-OB_TRAIL_DIFF_AXIS, linelen, -OB_TRAIL_DIFF_AXIS)));

    m_overlay_lines.push_back(
        OverlayLine(tcpos, AXIS_COLOR_Z,
          math::vec3f(OB_TRAIL_DIFF_AXIS, OB_TRAIL_DIFF_AXIS, -linelen),
          math::vec3f(OB_TRAIL_DIFF_AXIS, OB_TRAIL_DIFF_AXIS, linelen)));
    m_overlay_lines.push_back(
        OverlayLine(tcneg, AXIS_COLOR_Z,
          math::vec3f(-OB_TRAIL_DIFF_AXIS, OB_TRAIL_DIFF_AXIS, -linelen),
          math::vec3f(-OB_TRAIL_DIFF_AXIS, OB_TRAIL_DIFF_AXIS, linelen)));
    m_overlay_lines.push_back(
        OverlayLine(tcneg, AXIS_COLOR_Z,
          math::vec3f(OB_TRAIL_DIFF_AXIS, -OB_TRAIL_DIFF_AXIS, -linelen),
          math::vec3f(OB_TRAIL_DIFF_AXIS, -OB_TRAIL_DIFF_AXIS, linelen)));
    m_overlay_lines.push_back(
        OverlayLine(tcpos, AXIS_COLOR_Z,
          math::vec3f(-OB_TRAIL_DIFF_AXIS, -OB_TRAIL_DIFF_AXIS, -linelen),
          math::vec3f(-OB_TRAIL_DIFF_AXIS, -OB_TRAIL_DIFF_AXIS, linelen)));

    m_overlay_lines.feed(sh_overlay_line, glob->getTextureTrace());

    glLineWidth(1.0f);
  }

  // Tracer mode start.
  if(NORMAL == status)
  {
    float modifier = frame_count * 0.001f;

    // 3D visualization phase (not strictly textured objects).
    gfx::bind_shader_3d_sprite(sh_overlay_line);
    gfx::load_transform(view);

    gfx::mode_depth(gfx::TEST_LEQUAL);
    glLineWidth(OB_LINE_WIDTH);

    m_overlay_lines.clear();

    // Nukes.
    BOOST_FOREACH(const MissileMap::value_type &vv, m_missiles_nuke)
    {
      const MissileNuke *nuke = static_cast<const MissileNuke*>(vv.second.get());
      if(!nuke->isVisible())
      {
        continue;
      }
      const math::vec3f  pos(nuke->getPos()),
            tgt(nuke->getTargetPos());
      math::vec2f tc(0.0f - modifier, math::length(pos - tgt) / OB_TRAIL_SCALE_LOCK - modifier);

      m_overlay_lines.push_back(OverlayLine(tc, TRAIL_COLOR_NUKE, pos, tgt));
    }

    // Other missiles.
    BOOST_FOREACH(const MissileMap::value_type &vv, m_missiles_anti)
    {
      const MissileAnti *anti = static_cast<const MissileAnti*>(vv.second.get()),
            *mtarget = static_cast<const MissileAnti*>(anti->getTarget());
      if(!anti->isVisible() || !mtarget || !this->hasMissileNuke(mtarget))
      {
        continue;
      }
      const math::vec3f pos(anti->getPos()),
            tgt(anti->getTarget()->getPos());
      math::vec2f tc(0.0f - modifier, math::length(pos - tgt) / OB_TRAIL_SCALE_LOCK - modifier);

      m_overlay_lines.push_back(OverlayLine(tc, TRAIL_COLOR_ANTI, pos, tgt));
    }

    m_overlay_lines.feed(sh_overlay_line, glob->getTextureTrace());

    glLineWidth(1.0f);
  }

  // 3D visualization phase (not strictly textured objects).
  gfx::bind_shader_3d(sh_overlay);

  // Cities.
  if(NORMAL == status)
  {
    math::mat3f tm;

    gfx::mode_depth(gfx::TEST_LEQUAL);

#if (ENABLE_DRAW_CITIES != 0)
    tm.loadTranslation(0.0f, frame_count * 0.001f);
    sh_overlay.getUniform("texmatrix")->update(tm);
    BOOST_FOREACH(const CityMap::value_type &vv, m_cities)
    {
      vv.second->draw(sh_overlay, view);
    }
#endif

    // Nuke marker.
    if(m_nuke_marker_visibility.isVisible())
    {
      gfx::mode_cull(gfx::NONE);

      tm.loadTranslation(0.0f, frame_count * (-0.01f));
      sh_overlay.getUniform("texmatrix")->update(tm);
      m_obj_nuke_marker.draw(sh_overlay, view);

      gfx::mode_cull(gfx::BACK);
    }

    if(!m_mode_freemove)
    {
      tm.loadTranslation(0.0f, -frame_count * 0.01f);
      sh_overlay.getUniform("texmatrix")->update(tm);

      if(m_view.isCourseVisible())
      {
        //std::cout << "course is visible\n";
        math::mat4f om = m_view.getOrbitMatrix();
        //om.loadTranslation(math::vec3f(1000.0f, 0.0f, 0.0f));
        m_obj_course.setWm(om);
        //std::cout << om << std::endl;
        m_obj_course.draw(sh_overlay, view);
      }
      if(m_view.isCourseChangeVisible())
      {
        //std::cout << "course change is visible\n";
        math::mat4f om = m_view.getCourseMatrix();
        m_obj_course.setWm(om);
        m_obj_course.draw(sh_overlay, view);
      }
    }
  }

  gfx::mode_depth(gfx::TEST_LEQUAL_AND_WRITE);

  // Bullets.
  BOOST_FOREACH(const BulletFlakMap::value_type &vv, m_bullets_flak)
  {
    vv.second->draw(sh_overlay, view);
  }
  BOOST_FOREACH(const BulletRailgunMap::value_type &vv, m_bullets_railgun)
  {
    vv.second->draw(sh_overlay, view);
  }

#if (ENABLE_DRAW_ATMOSPHERE != 0)
  if(!this->isModeSchematic())
  {
    gfx::bind_shader_3d(sh_atmosphere);
    gfx::load_light(m_light_dir, view);
    {
      math::vec4f cpos(static_cast<float>(m_view.getPos().x()),
          static_cast<float>(m_view.getPos().y()),
          static_cast<float>(m_view.getPos().z()),
          1.0f);
      cpos = (view * m_obj_atmosphere.getWm()) * cpos;
      sh_atmosphere.getUniform("view_pos")->update(cpos.x(), cpos.y(), cpos.z());
    }

    gfx::mode_depth(gfx::TEST_LEQUAL);

    m_obj_atmosphere.draw(sh_atmosphere, view);
  }
#endif

  // Particles.
  gfx::bind_shader_3d_sprite(sh_billboard);
  gfx::load_transform(view);
  gfx::load_projection();

  gfx::mode_blend(gfx::ADDITIVE);
  gfx::mode_cull(gfx::NONE);
  gfx::mode_depth(gfx::TEST_LEQUAL);

  for(unsigned ii = 0; (ii < OB_PARTICLE_COUNT); ++ii)
  {
    ParticleArray &pmap = m_particles[ii];

    pmap.setParams(OB_BILLBOARD_DIV_MIN_PARTICLE, OB_BILLBOARD_DIV_MAX_PARTICLE);
    //std::cout << OB_BILLBOARD_DIV_MIN_PARTICLE << " ; " << OB_BILLBOARD_DIV_MAX_PARTICLE << std::endl;

    pmap.feed(sh_billboard, glob->getTextureParticle(static_cast<ParticleTypeEnum>(ii)));
  }
  
  // Need priority queue for targeting.
  std::priority_queue<Target> pri;
  // The whole rest of the function only happens if we're not dead.
  if(NORMAL == status)
  {
    // Markers then.
    gfx::load_transform(view);

    gfx::mode_blend(gfx::PREMULTIPLIED);
    gfx::mode_cull(gfx::BACK);
    gfx::mode_depth(gfx::NONE);

    m_billboards.clear();
    m_billboards.setParams(OB_BILLBOARD_DIV_MIN_MARKER, OB_BILLBOARD_DIV_MAX_MARKER);

    // Silo markers.
    {
      const gfx::Texture2D *last_texture = NULL;
      BOOST_FOREACH(const SiloMap::value_type &vv, m_silos)
      {
        Silo *silo = vv.second.get();
        if(silo->canTarget())
        {
          pri.push(Target(silo->project(), silo, OB_BILLBOARD_SIZE_SILO,
                glob->getTextureIconSilo(), silo->canLock()));
        }
        const gfx::Texture2D *ntex = silo->fillBillboard(m_billboards,
            sh_billboard, last_texture);
        if(ntex)
        {
          last_texture = ntex;
        }
      }
      if(NULL != last_texture)
      {
        //std::cout << "flushing silos\n";
        m_billboards.feed(sh_billboard, *last_texture);
        m_billboards.clear();
      }
    }

    // Missiles have varying billboards, let's be clever.
    {
      // Nukes.
      const gfx::Texture2D *last_texture = NULL;
      BOOST_FOREACH(const MissileMap::value_type &vv, m_missiles_nuke)
      {
        Missile *msl = vv.second.get();
        const gfx::Texture2D *ntex = msl->fillBillboard(m_billboards,
            sh_billboard, last_texture);
        if(ntex)
        {
          last_texture = ntex;
        }
      }
      if(NULL != last_texture)
      {
        m_billboards.feed(sh_billboard, *last_texture);
        m_billboards.clear();
        last_texture = NULL;
      }

      // Other missiles.
      BOOST_FOREACH(const MissileMap::value_type &vv, m_missiles_anti)
      {
        const MissileAnti *msl =
          static_cast<const MissileAnti*>(vv.second.get());
        if(msl->canTarget())
        {
          const gfx::Texture2D &icon = (NULL == msl->getTarget()) ?
            glob->getTextureIconMissileAntiShip() :
            glob->getTextureIconMissileAntiNuke();
          pri.push(Target(msl->project(), msl, OB_BILLBOARD_SIZE_MISSILE,
                icon, msl->canLock()));
        }
        const gfx::Texture2D *ntex = msl->fillBillboard(m_billboards,
            sh_billboard, last_texture);
        if(NULL != ntex)
        {
          last_texture = ntex;
        }
      }
      if(NULL != last_texture)
      {
        m_billboards.feed(sh_billboard, *last_texture);
        m_billboards.clear();
      }
    }

    // Targeting reticule.
    if(!pri.empty())
    {
      const Target &tgt = pri.top();
      const gfx::EntityObject *tgtobj = tgt.getTarget();
      if(tgtobj != m_target)
      {
        m_target_is_locked = false; // set this to true later if locked
        //new target
        m_cur_tgt_theta = 0;
        m_cur_tgt_size_multip = TARGET_MAX_SIZE;
        if(NULL != m_target)
        {
          m_billboards.clear();
          m_billboards.setParams(OB_BILLBOARD_DIV_MIN_TARGET,
              OB_BILLBOARD_DIV_MAX_TARGET);
          m_billboards.push_back(gfx::PointSprite(gfx::COLOR_YELLOW,
                math::vec3f(tgtobj->getPos()),tgt.getBillboardSize() * m_cur_tgt_size_multip));
          m_billboards.feed(sh_billboard, glob->getTextureTarget());
        }
      }
      else
      {
        //old target, reduce target size by a small amount
        gfx::Color targetColor;
        if(!tgt.canLock() || (TARGET_MIN_SIZE < m_cur_tgt_size_multip))
        {
          m_target_is_locked = false;
          m_cur_tgt_size_multip = math::max(TARGET_MIN_SIZE,
              m_cur_tgt_size_multip - TARGET_SCALE_FACTOR);
          m_cur_tgt_theta += TARGET_LOCKING_ROTATION_SPEED;
          targetColor = gfx::COLOR_YELLOW;
        }
        else
        {
          if(!m_target_is_locked)
          {
            snd::play(glob->getSampleRailgunLock());
          }
          m_target_is_locked = true;
          // Target is motionless when locked
          m_cur_tgt_theta = 0; //-= TARGET_LOCKED_ROTATION_SPEED;
          targetColor = gfx::COLOR_GREEN;
        }
#if 0
        math::mat3f tt;
        tt.loadRotationBillboard(m_cur_tgt_theta);
        /*
        uint32_t morphDefault = gfx::BillboardElementStruct::MORPH_DEFAULT;
        int8_t mdx = (int8_t)(morphDefault >> 24);
        int8_t mdy = (int8_t)((morphDefault << 8) >> 24);
        int8_t mdz = (int8_t)((morphDefault << 16) >> 24);
        int8_t mdw = (int8_t)((morphDefault << 24) >> 24);
        uint32_t morphDefaultOffset = gfx::BillboardElementStruct::MORPH_DEFAULT_OFFSET;
        int16_t mdox = (int16_t)(morphDefaultOffset >> 16);
        int16_t mdoy = (int16_t)((morphDefaultOffset << 16) >> 16);
        */
        union
        {
          int8_t morph_array[4];
          uint32_t newMorph;
        };
        morph_array[0] = static_cast<int8_t>(127.0f * tt(0, 0));
        morph_array[1] = static_cast<int8_t>(127.0f * tt(0, 1));
        morph_array[2] = static_cast<int8_t>(127.0f * tt(1, 1));
        morph_array[3] = static_cast<int8_t>(127.0f * tt(1, 0));

        math::vec2f texcoord(tt(0, 2), tt(1, 2));
#endif
#if 0
        [ (0.991562 ; -0.129634 ; 0.0690363)
          (0.129634 ; 0.991562 ; -0.0605981)
          (0 ; 0 ; 1) ]
#endif

        /*
        std::cout << "oldMorph: " << morphDefault << " newMorph: " << newMorph << '\n';
        std::cout << "MD: x:" << (int)mdx << " y:" << (int)mdy << " z:" << (int)mdz << " w:" << (int)mdw << std::endl;
        std::cout << "MDO: x:" << (int)mdox << " y:" << (int)mdoy << std::endl;
        */
        m_billboards.clear();
        m_billboards.setParams(OB_BILLBOARD_DIV_MIN_TARGET,
            OB_BILLBOARD_DIV_MAX_TARGET);
        m_billboards.push_back(gfx::PointSprite(m_cur_tgt_theta, targetColor,
              math::vec3f(tgtobj->getPos()),
              tgt.getBillboardSize() * m_cur_tgt_size_multip));
        m_billboards.feed(sh_billboard, glob->getTextureTarget());
      }
    }
  }

  // 2D phase.
  screen.select2D();
  gfx::bind_shader_2d();
  gfx::load_identity();

  if(m_mode_fps)
  {
    gfx::bind_shader_2d_font();
    gfx::load_identity();

    std::stringstream fstr;
    fstr << st.getFps();
    std::wstring fps = ui::wstr_utf8(fstr.str());
    draw_text(0.052f, 0.048f, 0.05f, fps, fnt, gfx::Color(0.0f, 0.0f, 0.0f, 1.0f));
    draw_text(0.05f, 0.05f, 0.05f, fps, fnt, gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));

    std::stringstream cstr;
    cstr << gfx::Lod::poly_count_clear();
    std::wstring pcnt = ui::wstr_utf8(cstr.str());
    draw_text(0.052f, 0.898f, 0.05f, pcnt, fnt, gfx::Color(0.0f, 0.0f, 0.0f, 1.0f));
    draw_text(0.05f, 0.9f, 0.05f, pcnt, fnt, gfx::Color(1.0f, 1.0f, 1.0f, 1.0f));
  }

  if(NORMAL == status)
  {
    this->drawHud(screen, pri);
  }
  else if(OVER == status)
  {
    this->drawHighScore(screen);
  }

  // Nuke distortion effect.
  {
    const MissileNuke *nuke = NULL;
    BOOST_FOREACH(const MissileMap::value_type &vv, m_missiles_nuke)
    {
      const MissileNuke *iter = static_cast<const MissileNuke*>(vv.second.get());
      if(iter->isNuking())
      {
        nuke = iter;
        break;
      }
    }
    if(nuke)
    {
      float alpha = nuke->getDistortAlpha();
      if(1.0f / 255.0f <= alpha)
      {
        screen.select3D(px, py, pw, ph, m_fov, 1.0f, OB_VIEW_NEAR, OB_VIEW_FAR);

        gfx::mode_depth(gfx::NONE);

        math::mat4f dm;
        math::vec3f bpos = math::vec3f(nuke->getPos() + m_view.getPos()) * 0.5f;
        dm.loadLookAt(bpos, bpos + fw, up);
        m_obj_distort.setWm(dm);

        gfx::bind_shader_3d(sh_distort);
        sh_distort.getUniform("distort_params")->update(nuke->getDistortAlpha(), math::mrand(0.0f, 1.0f));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        m_obj_distort.draw(sh_distort, view);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
    }
  }
}

void Game::removeBulletFlak(BulletFlak *op)
{
  ptr_map_remove(m_bullets_flak, op, "flak bullet");
}

void Game::removeBulletRailgun(BulletRailgun *op)
{
  ptr_map_remove(m_bullets_railgun, op, "railgun bullet");
}

void Game::removeMissileAnti(Missile *op)
{
  ptr_map_remove(m_missiles_anti, op, "anti-missile");
}

void Game::removeMissileNuke(Missile *op)
{
  ptr_map_remove(m_missiles_nuke, op, "nuke");
}

void Game::removeSilo(Silo *op)
{
  ptr_map_remove(m_silos, op, "silo");
}

void Game::render(ui::UiStack &st, gfx::SurfaceScreen &screen)
{
  this->renderSub(screen, st, this->getGameStatus(), 0, 0, screen.getWidth(), screen.getHeight());
  fade.draw(screen);
  screen.update();
}

void Game::update(ui::UiStack &st)
{
  GameStatusEnum status = this->getGameStatus();

  // If dead or out of nukes and escaping, freeze frame and update the score.
  if(OVER == status)
  {
    // Create input if it doesn't exist.
    if(!this->checkInput())
    {
      m_input = ui::InputLineSptr(new ui::InputLine());
      ui::key_repeat_on();
    }
    this->updateHighScore();
  }
  else
  {
    this->updateSub(st, status);
  }

  // Update fade status.
  fade.update();
}

void Game::updateHighScore()
{
  m_success = conf->getHighScores().check(m_score);
}

void Game::updateSub(ui::UiStack &st, GameStatusEnum status)
{
  // Player rotation.
  float rspeedx = conf->getCameraRotSpeedX().get(),
        rspeedy = conf->getCameraRotSpeedY().get();
  m_view.rotate(static_cast<float>(-mouse_diff.y()) * rspeedx * m_fov,
      static_cast<float>(-mouse_diff.x()) * rspeedy * m_fov);
  mouse_diff = math::vec2i(0, 0);

  // Player movement.
  math::vec3d fw;
  math::vec3f up;
  //std::cout << (m_view.getPos() + 7900.0f * fw) << std::endl;
  if(m_mode_freemove)
  {
    const math::mat4f &cm = m_view.viewOrbit();
    const math::vec3d &cpos(m_view.getPos()),
          row0(static_cast<double>(cm(0, 0)), static_cast<double>(cm(0, 1)), static_cast<double>(cm(0, 2))),
          row1(static_cast<double>(cm(1, 0)), static_cast<double>(cm(1, 1)), static_cast<double>(cm(1, 2))),
          row2(static_cast<double>(cm(2, 0)), static_cast<double>(cm(2, 1)), static_cast<double>(cm(2, 2)));
    double mbk = m_mv_bk ? CAMERA_SPEED : 0.0f,
           mdn = m_mv_dn ? CAMERA_SPEED : 0.0f,
           mfw = m_mv_fw ? CAMERA_SPEED : 0.0f,
           mlt = m_mv_lt ? CAMERA_SPEED : 0.0f,
           mrt = m_mv_rt ? CAMERA_SPEED : 0.0f,
           mup = m_mv_up ? CAMERA_SPEED : 0.0f;
    m_view.setPos(cpos +
        (mrt - mlt) * row0 +
        (mup - mdn) * row1 +
        (mbk - mfw) * row2);
    fw = math::vec3d(static_cast<double>(-cm(2, 0)),
        static_cast<double>(-cm(2, 1)), static_cast<double>(-cm(2, 2)));
    up = math::vec3f(cm(1, 0), cm(1, 1), cm(1, 2));
  }
  else
  {
    if(ENDING == status)
    {
      m_view.incEscapeSpeed();
    }
    m_view.update();
    const math::mat4f &cm = m_view.viewOrbit();
    fw = math::vec3d(static_cast<double>(-cm(2, 0)),
        static_cast<double>(-cm(2, 1)), static_cast<double>(-cm(2, 2)));
    up = math::vec3f(cm(1, 0), cm(1, 1), cm(1, 2));
  }
  snd::set_listener(math::vec3f(m_view.getPos()), math::vec3f(fw), up);

  if(!m_mode_freemove)
  {
    if((NORMAL == status) && (m_lay_nuke || m_lay_course))
    {
      math::vec3d intersect;
      if(math::intersect_ray_ball(intersect, m_view.getPos(), fw,
            math::vec3d(0.0, 0.0, 0.0),
            static_cast<double>(OB_PLANET_RADIUS)))
      {
        //std::cout << m_view.getPos() << " -> " << fw << " ; " << intersect << std::endl;

        // Nuke case.
        if(m_lay_nuke)
        {
          const HeightMapPlanet &hmap = glob->getHeightMapPlanet();
          float ht = hmap.calcHeight(math::vec3f(intersect));
          m_nuke_marker = hmap.normalizeHeight(intersect, ht * 1.1f);
          m_nuke_marker_set = true;

          math::mat4f nm;
          nm.loadLookAt(math::vec3f(m_nuke_marker),
              math::vec3f(0.0f, 0.0f, 0.0f),
              math::vec3f(fw));
          m_obj_nuke_marker.setWm(nm);
        }
        // Course case.
        if(m_lay_course)
        {
          m_view.updateCourse(intersect);
        }
      }
      else
      {
        if(m_lay_nuke)
        {
          m_nuke_marker_set = false;
        }
      }
    }
    // Update nuke marker visibility.
    m_nuke_marker_visibility.updateVisibility(m_nuke_marker_set);

    // Zoom the fov.
    if(m_fov_target > m_fov)
    {
      m_fov = std::min(m_fov + OB_FOV_SPEED, m_fov_target);
    }
    else
    {
      m_fov = std::max(m_fov - OB_FOV_SPEED, m_fov_target);
    }

    // Particle update before the potential creation of new particles, since the
    // particles must be rendered once in their initial position.
    BOOST_FOREACH(ParticleArray &pmap, m_particles)
    {
#if defined(DEBUG)
      unsigned particle_count = pmap.size();
#endif
      for(ParticleArray::iterator ii = pmap.begin(), ee = pmap.end(); (ii != ee);)
      {
#if defined(DEBUG)
        --particle_count;
#endif
        if(!(*ii).update())
        {
          ii = pmap.erase(ii);
          continue;
        }
        ++ii;
      }
      BOOST_ASSERT(particle_count == 0);
    }

    // Game mechanics of missiles.
    for(MissileMap::iterator ii = m_missiles_nuke.begin(), ee = m_missiles_nuke.end();
        (ii != ee);)
    {
      MissileMap::iterator jj = ii;
      ++ii;
      if(!jj->second->update())
      {
        m_missiles_nuke.erase(jj);
      }
    }
    for(MissileMap::iterator ii = m_missiles_anti.begin(), ee = m_missiles_anti.end();
        (ii != ee);)
    {
      MissileMap::iterator jj = ii;
      Missile *msl = jj->second.get();
      ++ii;
      if(!msl->update())
      {
        this->checkTarget(msl);
        m_missiles_anti.erase(jj);
      }
    }

    // Game mechanics of bullets.
    for(BulletFlakMap::iterator ii = m_bullets_flak.begin(), ee = m_bullets_flak.end();
        (ii != ee);)
    {
      BulletFlakMap::iterator jj = ii;
      ++ii;
      if(!jj->second->update())
      {
        m_bullets_flak.erase(jj);
      }
    }
    for(BulletRailgunMap::iterator ii = m_bullets_railgun.begin(), ee = m_bullets_railgun.end();
        (ii != ee);)
    {
      BulletRailgunMap::iterator jj = ii;
      ++ii;
      if(!jj->second->update())
      {
        m_bullets_railgun.erase(jj);
      }
    }

    // What follows are the player and enemy update methods. They must happen
    // after the other updates to ensure they are not updated 'extra' times when
    // compared to other objects. However, if we're dead, then that's it.
    if(NORMAL != status)
    {
      return;
    }
    // Only decrement game time if normal gameplay.
    --m_time_left;

    // Game mechanics of cities.
    for(CityMap::iterator ii = m_cities.begin(), ee = m_cities.end();
        (ii != ee);)
    {
      CityMap::iterator jj = ii;
      City *city = jj->second.get();
      ++ii;
      if(!city->update(m_silos_in_range < this->getSiloMinCountInRange()))
      {
        m_cities.erase(jj);
      }
    }

    // Game mechanics of silos.
    m_silos_in_range = 0;
    for(SiloMap::iterator ii = m_silos.begin(), ee = m_silos.end();
        (ii != ee);)
    {
      SiloMap::iterator jj = ii;
      Silo *silo = jj->second.get();
      ++ii;
      if(!silo->update())
      {
        this->checkTarget(silo);
        m_silos.erase(jj);
      }
    }
    if(m_silos_in_range < this->getSiloMinCountInRange())
    {
      uint64_t frame = st.getFrameCount();
      if(frame - m_silo_last_popup > OB_SILO_MIN_POPUP_DELAY)
      {
        //std::cout << "new countryside silo\n";
        m_silo_last_popup = frame;
        this->addSilo(new Silo(glob->getHeightMapPlanet(),
            math::normalize(m_view.getPos()) * OB_PLANET_RADIUS));
      }
    }

    // Railgun rondo!
    if(--m_railgun_delay <= 0)
    {
      m_railgun_delay = 0;
      if((0 < m_railgun_bullets) && m_fire_railgun && m_target_is_locked)
      {
        m_railgun_delay = OB_RAILGUN_FIRE_TIME;
        m_railgun_reload = OB_RAILGUN_RELOAD_TIME;
        --m_railgun_bullets;

        this->addBulletRailgun(new BulletRailgun(m_view.getPos(), fw,
              m_view.getDir(), m_target));
      }
    }
    if(OB_RAILGUN_CLIP_SIZE > m_railgun_bullets)
    {
      if(--m_railgun_reload <= 0)
      {
        m_railgun_reload = OB_RAILGUN_RELOAD_TIME;
        m_railgun_bullets++;
      }
    }
    // Flak time!
    if(--m_flak_delay <= 0)
    {
      m_flak_delay = 0;
      if((0 < m_flak_bullets) && m_fire_flak)
      {
        m_flak_delay = OB_FLAK_FIRE_TIME;
        --m_flak_bullets;
        this->addBulletFlak(new BulletFlak(m_view.getPos(), fw,
              m_view.getDir(), m_target));
      }
    }
    if(OB_FLAK_CLIP_SIZE > m_flak_bullets)
    {
      if(--m_flak_reload <= 0)
      {
        m_flak_reload = OB_FLAK_RELOAD_TIME;
        ++m_flak_bullets;
      }
    }
    // Launch nuke.
    if(--m_nuke_delay <= 0)
    {
      m_nuke_delay = 0;
      if((0 < m_nuke_bullets) && m_nuke_marker_set && !m_lay_nuke)
      {
        m_nuke_delay = OB_NUKE_FIRE_TIME;
        m_nuke_marker_set = false;
        --m_nuke_bullets;
        this->addMissileNuke(new MissileNuke(m_view.getPos(),
              //math::normalize(m_view.getPos()) * OB_NUKE_SPEED,
              math::normalize(m_view.getPos()),
              m_nuke_marker));
      }
    }
  }
}

