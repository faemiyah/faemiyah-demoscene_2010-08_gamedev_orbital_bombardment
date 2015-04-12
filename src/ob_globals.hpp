#ifndef OB_GLOBALS_HPP
#define OB_GLOBALS_HPP

#include "gfx/font.hpp"
#include "gfx/mesh.hpp"
#include "gfx/texture_3d.hpp"
#include "snd/sample.hpp"
#include "ob_console.hpp"
#include "ob_fade.hpp"
#include "ob_height_map_planet.hpp"
#include "ob_menu.hpp"
#include "ob_particle.hpp"

#include <boost/array.hpp>

#include <functional>

namespace ob
{
  class Game;

  /** Number of meshes for siege cruiser. */
  static const unsigned OB_SIEGE_CRUISER_MESH_COUNT = 7;

  /** Global data container.
   *
   * This contains all the main data structures.
   *
   * Must be deleted prior to closing the OpenGL context and constructed
   * after creating the OpenGL context.
   *
   * Reinitializing this data structure (with new detail settings) will produce
   * a new clean game state. However, the construction of the globals class
   * only creates the essentials, not the game-dependant heavy precalc data,
   * that needs to be created by calling the precalculation method within the
   * class.
   */
  struct Globals
  {
    private:
      /** Flag telling whether game data generation is on. */
      static bool generate_enabled;

    private:
      /** Detail level. */
      std::string m_detail_level;

      /** Planet heightmap. */
      HeightMapPlanet m_height_map_planet;

      /** Font. */
      gfx::Font m_font;

      /** Console. */
      Console m_console;

      /** Blank SDL cursor. */
      SDL_Cursor *m_cursor_blank;

      /** Default SDL cursor for storage. */
      SDL_Cursor *m_cursor_default;

      /** Shader for atmosphere effect. */
      gfx::Shader *m_shader_atmosphere;

      /** Shader for distort effect. */
      gfx::Shader *m_shader_distort;

      /** Shader for static objects. */
      gfx::Shader *m_shader_object;

      /** Shader for overlay effects. */
      gfx::Shader *m_shader_overlay;

      /** Shader for overlay effects using point sprites. */
      gfx::Shader *m_shader_billboard;

      /** Shader for overlay effects using trace lines. */
      gfx::Shader *m_shader_overlay_line;

      /** Shader for the planet. */
      gfx::Shader *m_shader_planet;

      /** Shader for the planet. */
      gfx::Shader *m_shader_planet_schematic;

      /** Shader for atmosphere effect. */
      gfx::Shader *m_shader_sun;

      /** Flak bullet mesh. */
      gfx::Mesh *m_mesh_bullet_flak;

      /** Railgun bullet mesh. */
      gfx::Mesh *m_mesh_bullet_railgun;

      /** Missile mesh. */
      gfx::Mesh *m_mesh_missile_anti;

      /** Missile mesh. */
      gfx::Mesh *m_mesh_missile_nuke;

      /** Mesh for the silo. */
      gfx::Mesh *m_mesh_silo;

      /** Texture. */
      const gfx::Texture2D* m_texture_flak_ammo;

      /** Icon. */
      const gfx::Texture2D* m_texture_icon_bullet_flak;

      /** Icon. */
      const gfx::Texture2D* m_texture_icon_bullet_railgun;

      /** Icon. */
      const gfx::Texture2D* m_texture_icon_city;

      /** Icon. */
      const gfx::Texture2D* m_texture_icon_missile_anti_nuke;

      /** Icon. */
      const gfx::Texture2D* m_texture_icon_missile_anti_ship;

      /** Icon. */
      const gfx::Texture2D* m_texture_icon_silo;

      /** Texture. */
      const gfx::Texture2D* m_texture_menu;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_missile_anti_nuke;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_missile_anti_nuke_warning;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_missile_anti_ship;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_missile_anti_ship_warning;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_missile_nuke;

      /** Texture. */
      const gfx::Texture2D* m_texture_nuke_ammo;

      /** Texture for billboards. */
      boost::array<const gfx::Texture2D*, OB_PARTICLE_COUNT> m_texture_particle;

      /** Texture. */
      const gfx::Texture2D* m_texture_railgun_ammo;

      /** Texture. */
      const gfx::Texture2D* m_texture_reload;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_silo_anti_nuke;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_silo_anti_ship;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_silo_both;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_silo_anti_nuke_inactive;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_silo_anti_ship_inactive;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_silo_both_inactive;

      /** Texture. */
      const gfx::Texture2D* m_texture_skull;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_target;

      /** Texture for billboards. */
      const gfx::Texture2D* m_texture_trace;

      /** Sample. */
      snd::Sample *m_sample_alarm;

      /** Sample. */
      snd::Sample *m_sample_alarm_over;

      /** Sample. */
      snd::Sample *m_sample_contact;

      /** Sample. */
      snd::Sample *m_sample_flak;

      /** Sample. */
      snd::Sample *m_sample_illegal;

      /** Sample. */
      snd::Sample *m_sample_locked;

      /** Sample. */
      snd::Sample *m_sample_nuke;

      /** Sample. */
      snd::Sample *m_sample_nuke_explosion;

      /** Sample. */
      snd::Sample *m_sample_railgun;

      /** Sample. */
      snd::Sample *m_sample_railgun_lock;

      /** Sample. */
      snd::Sample *m_sample_route_change;

      /** Sample. */
      snd::Sample *m_sample_route_change_accepted;

      /** Sample. */
      snd::Sample *m_sample_target_destroyed;

      /** Game menu. */
      MenuSptr m_menu_game;

      /** Main menu. */
      MenuSptr m_menu_main;

      /** Precalculation trigger flag. */
      bool m_precalculated;

    public:
      /** \brief Constructor.
       *
       * \param screen Screen on which we are initializing.
       * \param detail Detail level to be used.
       */
      Globals(const gfx::SurfaceScreen &pscreen, const std::string &pdetail);

      /** \brief Destructor. */
      ~Globals();

    private:
      /** \brief Unreserve internal data.
       */
      void unreserve();

    public:
      /** \brief Precalculate.
       */
      void precalc();

      /** \brief Precalculate.
       *
       * \param subdivide Planet subdivision.
       * \param coalesce Planet coalesce.
       * \param texsize Planet texture and heightmap detail.
       * \param volsize Planet volumetric texture detail.
       * \param hmap Optional height map to use.
       */
      void precalc(unsigned subdivide, unsigned coalesce, unsigned texsize, unsigned volsize);

    private:
      /** \brief Create a blank cursor.
       *
       * \return Pointer to cursor.
       */
      static SDL_Cursor* create_cursor_blank();

      /** \brief Suspendable procedure.
       *
       * \param dst Target texture shared pointer.
       * \param cstr C-string of the filename
       */
      static void reserve_shader(gfx::ShaderSptr &dst, const char *cstr);

    public:
      /** \brief Accessor.
       *
       * \return Pointer to SDL cursor.
       */
      SDL_Cursor* getCursorBlank()
      {
        return m_cursor_blank;
      }

      /** \brief Accessor.
       *
       * \return Pointer to SDL cursor.
       */
      SDL_Cursor* getCursorDefault()
      {
        return m_cursor_default;
      }

      /** \brief Accessor.
       *
       * \return Console reference.
       */
      Console& getConsole()
      {
        return m_console;
      }

      /** \brief Accessor.
       *
       * \return Console reference.
       */
      const Console& getConsole() const
      {
        return m_console;
      }

      /** \brief Get the detail level.
       *
       * \return Detail level string.
       */
      const std::string& getDetailLevel() const
      {
        return m_detail_level;
      }

      /** \brief Accessor.
       *
       * \return Font reference.
       */
      const gfx::Font& getFont() const
      {
        return m_font;
      }

      /** \brief Accessor.
       *
       * \return The game menu.
       */
      Menu* getMenuGame()
      {
        return m_menu_game.get();
      }

      /** \brief Accessor.
       *
       * \return The main menu.
       */
      Menu* getMenuMain()
      {
        return m_menu_main.get();
      }

      /** \brief Accessor.
       *
       * \return Planet heightmap.
       */
      const HeightMapPlanet& getHeightMapPlanet() const
      {
        return m_height_map_planet;
      }

      /** \brief Accessor.
       *
       * \return Mesh pointer.
       */
      const gfx::Mesh* getMeshBulletFlak() const
      {
        return m_mesh_bullet_flak;
      }

      /** \brief Accessor.
       *
       * \return Mesh pointer.
       */
      const gfx::Mesh* getMeshBulletRailgun() const
      {
        return m_mesh_bullet_railgun;
      }

      /** \brief Accessor.
       *
       * \return Mesh pointer.
       */
      const gfx::Mesh* getMeshMissileAnti() const
      {
        return m_mesh_missile_anti;
      }

      /** \brief Accessor.
       *
       * \return Mesh pointer.
       */
      const gfx::Mesh* getMeshMissileNuke() const
      {
        return m_mesh_missile_nuke;
      }

      /** \brief Accessor.
       *
       * \return Mesh pointer.
       */
      const gfx::Mesh* getMeshSilo() const
      {
        return m_mesh_silo;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureFlakAmmo() const
      {
        return *m_texture_flak_ammo;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureIconBulletFlak() const
      {
        return *m_texture_icon_bullet_flak;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureIconBulletRailgun() const
      {
        return *m_texture_icon_bullet_railgun;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureIconCity() const
      {
        return *m_texture_icon_city;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureIconMissileAntiNuke() const
      {
        return *m_texture_icon_missile_anti_nuke;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureIconMissileAntiShip() const
      {
        return *m_texture_icon_missile_anti_ship;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureIconSilo() const
      {
        return *m_texture_icon_silo;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureMenu() const
      {
        return *m_texture_menu;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureMissileAntiNukeWarning() const
      {
        return *m_texture_missile_anti_nuke_warning;
      }

      /** \brief Get billboard texture for anti-nuke missiles.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureMissileAntiNuke() const
      {
        return *m_texture_missile_anti_nuke;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureMissileAntiShipWarning() const
      {
        return *m_texture_missile_anti_ship_warning;
      }

      /** \brief Get billboard texture for anti-ship missiles.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureMissileAntiShip() const
      {
        return *m_texture_missile_anti_ship;
      }

      /** \brief Get billboard texture for nukes.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureMissileNuke() const
      {
        return *m_texture_missile_nuke;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureNukeAmmo() const
      {
        return *m_texture_nuke_ammo;
      }

      /** \brief Accessor.
       *
       * \param op Particle enumeration.
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureParticle(ParticleTypeEnum op) const
      {
        return *m_texture_particle[static_cast<unsigned>(op)];
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureRailgunAmmo() const
      {
        return *m_texture_railgun_ammo;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureReload() const
      {
        return *m_texture_reload;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureSiloAntiNuke() const
      {
        return *m_texture_silo_anti_nuke;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureSiloAntiShip() const
      {
        return *m_texture_silo_anti_ship;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureSiloBoth() const
      {
        return *m_texture_silo_both;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureSiloAntiNukeInactive() const
      {
        return *m_texture_silo_anti_nuke_inactive;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureSiloAntiShipInactive() const
      {
        return *m_texture_silo_anti_ship_inactive;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureSiloBothInactive() const
      {
        return *m_texture_silo_both_inactive;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureSkull() const
      {
        return *m_texture_skull;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureTarget() const
      {
        return *m_texture_target;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture2D& getTextureTrace() const
      {
        return *m_texture_trace;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleAlarm() const
      {
        return *m_sample_alarm;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleAlarmOver() const
      {
        return *m_sample_alarm_over;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleContact() const
      {
        return *m_sample_contact;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleFlak() const
      {
        return *m_sample_flak;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleIllegal() const
      {
        return *m_sample_illegal;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleLocked() const
      {
        return *m_sample_locked;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleNuke() const
      {
        return *m_sample_nuke;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleNukeExplosion() const
      {
        return *m_sample_nuke_explosion;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleRailgun() const
      {
        return *m_sample_railgun;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleRailgunLock() const
      {
        return *m_sample_railgun_lock;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleRouteChange() const
      {
        return *m_sample_route_change;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleRouteChangeAccepted() const
      {
        return *m_sample_route_change_accepted;
      }

      /** \brief Accessor.
       *
       * \return Sample reference.
       */
      snd::Sample& getSampleTargetDestroyed() const
      {
        return *m_sample_target_destroyed;
      }

      /** \brief Accessor.
       *
       * \return Shader reference.
       */
      gfx::Shader& getShaderAtmosphere() const
      {
        return *m_shader_atmosphere;
      }

      /** \brief Accessor.
       *
       * \return Shader reference.
       */
      gfx::Shader& getShaderDistort() const
      {
        return *m_shader_distort;
      }

      /** \brief Accessor.
       *
       * \return Object shader reference.
       */
      gfx::Shader& getShaderObject() const
      {
        return *m_shader_object;
      }

      /** \brief Accessor.
       *
       * \return Overlay shader reference.
       */
      gfx::Shader& getShaderOverlay() const
      {
        return *m_shader_overlay;
      }

      /** \brief Accessor.
       *
       * \return Overlay billboard shader reference.
       */
      gfx::Shader& getShaderBillBoard() const
      {
        return *m_shader_billboard;
      }

      /** \brief Accessor.
       *
       * \return Overlay line shader reference.
       */
      gfx::Shader& getShaderOverlayLine() const
      {
        return *m_shader_overlay_line;
      }

      /** \brief Accessor.
       *
       * \return Planet shader reference.
       */
      gfx::Shader& getShaderPlanet() const
      {
        return *m_shader_planet;
      }

      /** \brief Accessor.
       *
       * \return Planet shader reference.
       */
      gfx::Shader& getShaderPlanetSchematic() const
      {
        return *m_shader_planet_schematic;
      }

      /** \brief Accessor.
       *
       * \return Shader reference.
       */
      gfx::Shader& getShaderSun() const
      {
        return *m_shader_sun;
      }

      /** \brief Tell if precalculation done.
       *
       * \return True if yes, false if no.
       */
      bool isDone() const
      {
        return m_precalculated;
      }

      /** \brief Set the detail level.
       *
       * This detail will be used on next precalculation phase.
       *
       * \param op New detail level.
       */
      void setDetailLevel(const std::string &op)
      {
        m_detail_level = op;
      }

    public:
      /** \brief Tell if generation is enabled.
       *
       * \return True if yes, false if no.
       */
      static bool is_generate_enabled()
      {
        return generate_enabled;
      }

      /** \brief Set generation on.
       */
      static void set_generate()
      {
        generate_enabled = true;
      }
  };

  /** \brief Initialize all globals.
   *
   * Will destroy previous globals.
   *
   * \param pscreen Screen to initialize on.
   * \param detail Detail level to be used.
   */
  extern void glob_init(const gfx::SurfaceScreen &pscreen, const std::string &pdetail);

  /** \brief Precalculation function.
   */
  extern void glob_precalc();

  /** \brief Destroy all resources reserved by globals. */
  extern void glob_quit();

  /** \brief Get the new game construct if it's ready.
   *
   * \return New game construct, ready to be pushed on top of UI stack or NULL.
   */
  extern Game* glob_get_game();

  /** \brief Queue new game for startup.
   *
   * Will do nothing if a game is already queued or a game is ready.
   */
  extern void glob_queue_game();

  /** \brief Set the game construct.
   *
   * \param op Game.
   */
  extern void glob_set_game(Game *op);

  /** Main screen fade. */
  extern Fade fade;

  /** Main game instance. */
  extern Game *game;

  /** Main global instance. */
  extern Globals *glob;
};

#endif
