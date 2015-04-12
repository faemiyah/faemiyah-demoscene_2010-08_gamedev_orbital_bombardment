#ifndef OB_CONSTANTS_HPP
#define OB_CONSTANTS_HPP

#include "gfx/color.hpp"

namespace ob
{
  /** Billboard limit. */
  static const float OB_BILLBOARD_DIV_MAX_MARKER = 0.3f;

  /** Billboard limit. */
  static const float OB_BILLBOARD_DIV_MIN_MARKER = 0.03f;

  /** Billboard limit. */
  static const float OB_BILLBOARD_DIV_MAX_PARTICLE = 0.5f;

  /** Billboard limit. */
  static const float OB_BILLBOARD_DIV_MIN_PARTICLE = 0.005f;

  /** Billboard size. */
  static const float OB_BILLBOARD_SIZE_CITY = 82.0f;

  /** Billboard size. */
  static const float OB_BILLBOARD_SIZE_MISSILE = 24.0f;

  /** Billboard size. */
  static const float OB_BILLBOARD_SIZE_SILO = 60.0f;

  /** Billboard size. */
  static const float OB_BILLBOARD_SIZE_SMOKE = 7.0f;

  /** Billboard limit. */
  static const float OB_BILLBOARD_DIV_MAX_TARGET = 0.4f;

  /** Billboard limit. */
  static const float OB_BILLBOARD_DIV_MIN_TARGET = 0.04f;

  /** Camera rotation sensitivity step. */
  static const float OB_CAMERA_ROT_SPEED_STEP = 0.0005f;

  /** City marker / outskirts size (km). */
  static const float OB_CITY_MARKER_HEIGHT = 40.0;

  /** Console font size. */
  static const float OB_CONSOLE_FONT_SIZE = 0.035f;

  /** Global collision area. */
  static const float OB_COLLISION_AREA = 8192.0f;

  /** Font size base. */
  static const float OB_FONT_SIZE = 0.05f;

  /** Font size base. */
  static const float OB_FONT_BOTTOM = OB_FONT_SIZE * 0.2f;

  /** Font size base. */
  static const float OB_FONT_GAP = OB_FONT_SIZE * 0.1f;

  /** Field of view high cap. */
  static const float OB_FOV_MAX = static_cast<float>(M_PI) * 0.41f;

  /** Field of view low cap. */
  static const float OB_FOV_MIN = 0.125f * OB_FOV_MAX;

  /** Field of view change speed. */
  static const float OB_FOV_SPEED = 0.025f;

  /** One FOV step. */
  static const float OB_FOV_STEP = (OB_FOV_MAX - OB_FOV_MIN) * 0.125f;

  /** Width used for decorative lines. */
  static const float OB_LINE_WIDTH = 2.8f;

  /** Nuke marker height. */
  static const float OB_NUKE_MARKER_HEIGHT = 630.0f;

  /** Nuke marker radius. */
  static const float OB_NUKE_MARKER_RADIUS = 155.0f;

  /** Planet radius (km). */
  static const float OB_PLANET_RADIUS = 5000.0;

  /** Planet radius difference. */
  static const float OB_PLANET_RADIUS_DIFF = 0.02f;

  /** Play orbit (km). */
  static const float OB_PLAY_ORBIT = 7900.0f; // 2900m from surface

  /** Play orbit limit (km). */
  static const float OB_PLAY_ORBIT_LIMIT = 8190.0f; // Start game when 290km from orbit.

  /** Proximity at which a nuke is considered a legal target (km). */
  static const float OB_PROXIMITY_NUKE = 2850.0f;

  /** Proximity at which the player ship is considered a legal target (km). */
  static const float OB_PROXIMITY_SHIP = 3500.0f;

  /** Population scale factor (for transforming stuff in volumes). */
  static const float OB_POPULATION_OUTSKIRTS_PROXIMITY = 380.0f;

  /** Population scale factor (for transforming stuff in volumes). */
  static const float OB_POPULATION_RANDOM_BRUSH = 0.75f;

  /** Orbit converge speed. */
  static const float OB_ORBIT_CONVERGE = 512.0f;

  /** Orbit movement speed. */
  static const float OB_ORBIT_SPEED = 50.0f;

  /** \brief Target tolerance.
   * 
   * Distance in screen estate inside which a target is considered valid. */
  static const float OB_TARGET_TOLERANCE = 0.08f;

  /** \brief Terrain level.
   * 
   * Height map value larger than this is considered terrain. */
  static const float OB_TERRAIN_LEVEL = 0.4703f;

  /** Trail scale. */
  static const float OB_TRAIL_DIFF_AXIS = 96.0f;

  /** Trail scale. */
  static const float OB_TRAIL_SCALE_AXIS = 100.0f;

  /** Trail scale. */
  static const float OB_TRAIL_SCALE_LOCK = 50.0f;

  /** View farplane. */
  //static const float OB_VIEW_FAR = 8192.0f;
  static const float OB_VIEW_FAR = 65535.0f;

  /** View nearplane. */
  static const float OB_VIEW_NEAR = 1.0f;

  /** View rotation limit. */
  static const float OB_VIEW_ROT_LIMIT = 1.0f;

  /** Bullet death probability (1 per). */
  static const int OB_BULLET_DEATH_PROBABILITY = 90;

  /** Time taken from course alter command to accept alteration. */
  static const int OB_COURSE_ACCEPT_TIME = 1400;

  /** Time taken since alteration accept to actual course change. */
  static const int OB_COURSE_CHANGE_TIME = 800;

  /** Time taken since alteration accept to actual course change. */
  static const int OB_COURSE_NORMALIZATION_TIME = 800;

  /** Flak clip size. */
  static const int OB_FLAK_CLIP_SIZE = 30;

  /** Flak fire delay. */
  static const int OB_FLAK_FIRE_TIME = 10;

  /** Flak reload delay. */
  static const int OB_FLAK_RELOAD_TIME = 30;

  /** Nuke clip size. */
  static const int OB_NUKE_CLIP_SIZE = 10;

  /** Nuke reload time. */
  static const int OB_NUKE_FIRE_TIME = 3200;
  
  /** Particle lifetime. */
  static const int OB_PARTICLE_TIME_MUZZLE_EFFECT = 70;
  
  /** Particle lifetime. */
  static const int OB_PARTICLE_TIME_SMOKE = 300;

  /** Population map detail level. */
  static const int OB_POPULATION_DETAIL = 256;

  /** After this, population may be capped. */
  static const int OB_POPULATION_RANDOM_LIMIT = 144;

  /** Railgun clip size. */
  static const int OB_RAILGUN_CLIP_SIZE = 6;

  /** Railgun fire delay. */
  static const int OB_RAILGUN_FIRE_TIME = 60;

  /** Railgun reload delay. */
  static const int OB_RAILGUN_RELOAD_TIME = 150;

  /** Silo reload time (frames). */
  static const int OB_SILO_MIN_POPUP_DELAY = 6;

  /** Minimum count of silos in range */
  static const int OB_SILO_MIN_COUNT_IN_RANGE_INITIAL = 10;

  /** How many more silos should be added after each nuke hit*/
  static const int OB_SILO_COUNT_INCREMENT = 4;

  /** Probability (1 per) of a silo appearing per frame. */
  static const int OB_SILO_POPUP_PROBABILITY = 10;

  /** Silo reload time (frames). */
  static const int OB_SILO_RELOAD = 1800;

  /** Number of cities in-game. */
  static const unsigned OB_CITY_COUNT = 96;

  /** Random population instance count. */
  static const unsigned OB_POPULATION_RANDOM_COUNT = 16384;

  /** Numeber of silos spawned per city. */
  static const unsigned OB_SILOS_PER_CITY = 3;

  /** Multiplier for scores to make the deathtoll more sensible. */
  static const int SCORE_MULTIPLIER = 127;

  /** Player faction. */
  static const uint32_t OB_FACTION_PLAYER = 1 + 2 + 4;

  /** Player faction. */
  static const uint32_t OB_FACTION_PLAYER_BULLET = 2 + 4;

  /** Player faction. */
  static const uint32_t OB_FACTION_PLAYER_MISSILE = 4;

  /** Enemy faction. */
  static const uint32_t OB_FACTION_ENEMY = 8 + 16;

  /** Enemy faction. */
  static const uint32_t OB_FACTION_ENEMY_MISSILE = 16;

  /** \brief City faction.
   *
   * This simple, cities can only be hurt by nukes, nothing else.
   */
  static const uint32_t OB_FACTION_CITY = 1 + 2 + 8 + 16;

  /** Color for active stuff. */
  static const gfx::Color OB_COLOR_ACTIVE(1.0f, 0.404f, 0.594f, 0.8f);

  /** Color for border lines. */
  static const gfx::Color OB_COLOR_BORDER(0.1f, 0.9f, 0.3f, 0.8f);

  /** Menu border lines. */
  //static const gfx::Color OB_COLOR_MENU_BORDER(0.698f, 0.216f, 0.722f, 0.8f);
  static const gfx::Color OB_COLOR_MENU_BORDER(0.702f, 0.235f, 0.298f, 0.8f);

  /** Shade color.*/
  static const gfx::Color OB_COLOR_SHADE(0.2f, 0.0f, 0.0f, 0.4f);

  /** Title menu item. */
  static const gfx::Color OB_COLOR_TITLE(0.8f, 0.8f, 0.8f, 0.85f);

  /** Color of old highscore entry. */
  static const gfx::Color OB_COLOR_OLD_HS_ENTRY(0.8f, 0.8f, 0.8f, 0.85f);

  /** Color of new highscore entry. */
  static const gfx::Color OB_COLOR_NEW_HS_ENTRY(0.7f, 0.7f, 1.0f, 0.85f);
}

#endif
