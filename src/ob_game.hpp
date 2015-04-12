#ifndef OB_GAME_HPP
#define OB_GAME_HPP

#include "gfx/entity_object_overlay.hpp"
#include "gfx/light_directional.hpp"
#include "ui/input_line.hpp"
#include "ui/ui_state.hpp"
#include "ob_city.hpp"
#include "ob_bullet_flak.hpp"
#include "ob_bullet_railgun.hpp"
#include "ob_game_view.hpp"
#include "ob_missile_anti.hpp"
#include "ob_missile_nuke.hpp"
#include "ob_octree.hpp"
#include "ob_overlay_line.hpp"
#include "ob_overlay_line_array.hpp"
#include "ob_particle.hpp"
#include "ob_population_map.hpp"
#include "ob_silo.hpp"

#include <queue>

#ifdef WIN32
#include <boost/unordered_map.hpp>
#else
#include <tr1/unordered_map>
#endif

namespace ob
{
  class Target;

  /** Convenience typedef. */
  typedef std::map<size_t, BulletFlakSptr> BulletFlakMap;

  /** Convenience typedef. */
  typedef std::map<size_t, BulletRailgunSptr> BulletRailgunMap;

  /** Convenience typedef. */
  typedef std::map<size_t, CitySptr> CityMap;

  /** Convenience typedef. */
  typedef std::map<size_t, MissileSptr> MissileMap;

  /** Convenience typedef. */
  typedef gfx::PointSpriteArray<ob::Particle> ParticleArray;

  /** Convenience typedef. */
  typedef std::map<size_t, SiloSptr> SiloMap;

  /** Game status enumeration.
   *
   * Represents the actions that should be taken.
   */
  enum GameStatusEnum
  {
    INITIAL = 0, // not yet entered orbit
    NORMAL, // gameplay
    DYING, // fail
    ENDING, // escaping
    OVER // dead or escaped
  };

  /** \brief Orbital Bombardment main gameplay state.
   *
   * Does not contain any graphics related data, only data that
   * references such stuff.
   */
  class Game :
    public ui::UiState
  {
    private:
      /** \brief Collision octree.
       *
       * This MUST be the first item in the constructor, since other entities
       * may require it.
       */
      Octree m_octree;

      /** Directional light. */
      gfx::LightDirectional m_light_dir;

      /** Directional light. */
      gfx::LightDirectional m_light_dir_schematic;

      /** Entity for the planet object. */
      gfx::EntityObject m_obj_atmosphere;

      /** Entity for the course object. */
      gfx::EntityObjectOverlay m_obj_course;

      /** Entity for the distortion effect of nuke. */
      gfx::EntityObjectOverlay m_obj_distort;

      /** Entity for the nuke marker object. */
      gfx::EntityObjectOverlay m_obj_nuke_marker;

      /** Entity for the planet object. */
      gfx::EntityObject m_obj_planet;

      /** Entity for skybox. */
      gfx::EntityObjectOverlay m_obj_skybox_0;

      /** Entity for skybox. */
      gfx::EntityObjectOverlay m_obj_skybox_1;

      /** Entity for skybox. */
      gfx::EntityObjectOverlay m_obj_skybox_2;

      /** Entity for skybox. */
      gfx::EntityObjectOverlay m_obj_skybox_3;

      /** Entity for skybox. */
      gfx::EntityObjectOverlay m_obj_skybox_4;

      /** Entity for skybox. */
      gfx::EntityObjectOverlay m_obj_skybox_5;

      /** Entity for sun. */
      gfx::EntityObjectOverlay m_obj_sun;

      /** Game view (camera). */
      GameView m_view;

      /** Input line for UI text. */
      ui::InputLineSptr m_input;

      /** Cities. */
      CityMap m_cities;

      /** Map of railgun bullets. */
      BulletFlakMap m_bullets_flak;

      /** Map of railgun bullets. */
      BulletRailgunMap m_bullets_railgun;

      /** Map of missiles. */
      MissileMap m_missiles_anti;

      /** Map of nukes. */
      MissileMap m_missiles_nuke;

      /** Map of particles. */
      boost::array<ParticleArray, OB_PARTICLE_COUNT> m_particles;

      /** Billboard array. */
      BillboardArray m_billboards;

      /** Line array. */
      OverlayLineArray<OverlayLine> m_overlay_lines;

      /** Map of silos. */
      SiloMap m_silos;

      /** Population. */
      PopulationMap m_population;

      /** Entity object reference (as target). */
      const gfx::EntityObject *m_target;

      /** Nuke marker visibility. */
      Visualization m_nuke_marker_visibility;

      /** Nuke marker. */
      math::vec3d m_nuke_marker;

      /** Last frame a silo was created. */
      uint64_t m_silo_last_popup;

      /** Current field of view. */
      float m_fov;

      /** Field of view we're converging into. */
      float m_fov_target;

      /** Current timestep. */
      float m_timestep;

      /** Flak bullets left. */
      int m_flak_bullets;

      /** Flak fire delay left. */
      int m_flak_delay;

      /** Flak fire delay left. */
      int m_flak_reload;

      /** Nuke fire delay left. */
      int m_nuke_delay;

      /** Nuke fire delay left. */
      int m_nuke_bullets;

      /** Railgun bullets left. */
      int m_railgun_bullets;

      /** Railgun fire delay left. */
      int m_railgun_delay;

      /** Railgun reload delay left. */
      int m_railgun_reload;

      /** \brief Current score.
       *
       * NOTE: score is never displayed to the player as "score", only kills.
       * It's just easier to handle it as 'score' here.
       */
      int m_score;

      /** Displayed score. */
      int m_score_display;

      /** Silos in range. */
      int m_silo_min_count_in_range;

      /** Silos in range. */
      int m_silos_in_range;

      /** Time left for player. */
      int m_time_left;

      /** Fire flak flag. */
      bool m_fire_flak;

      /** Fire railgun flag. */
      bool m_fire_railgun;

      /** Laying course. */
      bool m_lay_course;

      /** Laying nuke. */
      bool m_lay_nuke;

      /** Nuke marker is set? */
      bool m_nuke_marker_set;

      /** Current target size multiplier*/
      float m_cur_tgt_size_multip;

      /** Current target rotation angle*/
      float m_cur_tgt_theta;

      /** Target locked*/
      bool m_target_is_locked;

      /** Free move back. */
      bool m_mv_bk;

      /** Free move down. */
      bool m_mv_dn;

      /** Free move forward. */
      bool m_mv_fw;

      /** Free move left. */
      bool m_mv_lt;

      /** Free move right. */
      bool m_mv_rt;

      /** Free move up. */
      bool m_mv_up;

      /** FPS counter on. */
      bool m_mode_fps;

      /** Free move on. */
      bool m_mode_freemove;

      /** Schematic mode. */
      bool m_mode_schematic;

      /** Wireframe mode on. */
      bool m_mode_wireframe;

      /** Flag that's set to true is high score would get onto the list. */
      bool m_success;

    public:
      /** \brief Check if game has been won (and we should start escaping).
       *
       * \return True if yes, false if no.
       */
      bool isGameWon()
      {
        return (m_nuke_bullets <= 0) && m_missiles_nuke.empty();
      }

      /** \brief Check input requirement.
       *
       * This equates to the need to draw the input area.
       *
       * \return True if input mode on, false if not.
       */
      bool checkInput() const
      {
        return (NULL != m_input.get());
      }

      /** \brief Check target equality.
       *
       * Unsets the target if it's this.
       *
       * \param op Target.
       */
      void checkTarget(gfx::EntityObject *op)
      {
        if(m_target == op)
        {
          m_target = NULL;
        }
      }

      /** \brief Accessor.
       *
       * \return Flak bullet map.
       */
      BulletFlakMap& getBulletsFlak()
      {
        return m_bullets_flak;
      }

      /** \brief Accessor.
       *
       * \return Railgun bullet map.
       */
      BulletRailgunMap& getBulletsRailgun()
      {
        return m_bullets_railgun;
      }

      /** \brief Get the cities.
       *
       * \return City vector.
       */
      CityMap& getCities()
      {
        return m_cities;
      }

      /** \brief Accessor.
       *
       * \return Missile map.
       */
      MissileMap& getMissilesAnti()
      {
        return m_missiles_anti;
      }

      /** \brief Accessor.
       *
       * \return Nuke map.
       */
      MissileMap& getMissilesNuke()
      {
        return m_missiles_nuke;
      }

      /** \brif Accessor.
       *
       * \return Light direction.
       */
      const math::vec3f& getLightDiffuse() const
      {
        return m_light_dir.getDiffuse();
      }

      /** \brif Accessor.
       *
       * \return Light direction.
       */
      const math::vec3f& getLightDir() const
      {
        return m_light_dir.getDir();
      }

      /** \brief Accessor.
       *
       * \return Score.
       */
      int getScore() const
      {
        return m_score;
      }

      /** \brief Accessor.
       *
       * \return Silo count limit in range.
       */
      int getSiloMinCountInRange() const
      {
        return m_silo_min_count_in_range;
      }

      /** \brief Accessor.
       *
       * \return Octree reference.
       */
      Octree& getOctree()
      {
        return m_octree;
      }

      /** \brief Accessor.
       *
       * \return Octree reference.
       */
      const Octree& getOctree() const
      {
        return m_octree;
      }

      /** \brief Accessor.
       *
       * \param op Particle type.
       * \return Particle array.
       */
      ParticleArray& getParticles(ParticleTypeEnum op)
      {
        return m_particles[static_cast<unsigned>(op)];
      }

      /** \brief Accessor.
       *
       * \param op Particle type.
       * \return Particle array.
       */
      const ParticleArray& getParticles(ParticleTypeEnum op) const
      {
        return m_particles[static_cast<unsigned>(op)];
      }

      /** \brief Accessor.
       *
       * \return Population map.
       */
      PopulationMap& getPopulation()
      {
        return m_population;
      }

      /** \brief Accessor.
       *
       * \return Population map.
       */
      const PopulationMap& getPopulation() const
      {
        return m_population;
      }

      /** \brief Get the silos.
       *
       * \return Silo map.
       */
      SiloMap& getSilos()
      {
        return m_silos;
      }

      /** \brief Accessor.
       *
       * \return Silo minimum count in range.
       */
      int getSiloMinCountInRange()
      {
        return m_silo_min_count_in_range;
      }

      /** \brief Accessor.
       *
       * \return Texture reference.
       */
      const gfx::Texture3D& getTexturePopulation() const
      {
        return m_population.getTexture();
      }

      /** \brief Accessor.
       *
       * \return Current timestep.
       */
      float getTimestep()
      {
        return m_timestep;
      }

      /** \brief Accessor.
       *
       * \return Game view.
       */
      GameView& getView()
      {
        return m_view;
      }

      /** \brief Accessor.
       *
       * \return Game view.
       */
      const GameView& getView() const
      {
        return m_view;
      }

      /** \brief Checker method.
       *
       * \param op Wanted missile.
       * \return True if still in map, false otherwise.
       */
      bool hasMissileNuke(const Missile *op) const
      {
        size_t sptr = reinterpret_cast<size_t>(op);
        return (m_missiles_nuke.find(sptr) != m_missiles_nuke.end());
      }

      /** \brief Increment the score.
       *
       * \param op Amount to increment.
       */
      void incrementScore(int op)
      {
        m_score += op;
      }

      /** \brief Increment number of silos in range.
      */
      void incSilosInRange()
      {
        ++m_silos_in_range;
      }

      /** \brief Flag checker.
       *
       * \return True if yes, false if no.
       */
      bool isModeSchematic() const
      {
        return m_lay_nuke ^ m_mode_schematic;
      }

    public:
      /** \brief Constructor. */
      Game();

      /** \brief Destructor. */
      virtual ~Game();

    private:
      /** \brief Draw the high score input display.
       *
       * \param screen Screen to use.
       */
      void drawHighScore(gfx::SurfaceScreen &screen);

      /** \brief Draw targeting reticule and HUD elements.
       *
       * \param screen Screen to use.
       * \param pri Priority queue of targets.
       */
      void drawHud(gfx::SurfaceScreen &screen, std::priority_queue<Target> &pri);

      /** \brief Update the game state.
       *
       * \param screen Screen construct.
       * \param st UI stack.
       */
      void updateHighScore();

      /** \brief Update the game state.
       *
       * \param st UI stack.
       * \param status Game status.
       */
      void updateSub(ui::UiStack &st, GameStatusEnum status);

    public:
      /** \brief Add a flak bullet to the global map.
       *
       * \param op Flak bullet to add.
       */
      void addBulletFlak(BulletFlak *op);

      /** \brief Add a railgun bullet to the global map.
       *
       * \param op Railgun bullet to add.
       */
      void addBulletRailgun(BulletRailgun *op);

      /** \brief Add a city to the global map.
       *
       * \param op City to add.
       */
      void addCity(City *op);

      /** \brief Add a missile to the global map.
       *
       * \param op Missile to add.
       */
      void addMissileAnti(Missile *op);

      /** \brief Add a missile to the global map.
       *
       * \param op Missile to add.
       */
      void addMissileNuke(Missile *op);

      /** \brief Add a particle to the global map.
       *
       * \param type Particle type.
       * \param op Particle to add.
       */
      void addParticle(ParticleTypeEnum type, const Particle &op);

      /** \brief Add a silo to the global map.
       *
       * \param silo Silo to add.
       */
      void addSilo(Silo *silo);

      /** \brief Flag checker.
       *
       * Game is running when player orbit is low enough and player is not
       * dead.
       *
       * \return Game status enumeration.
       */
      GameStatusEnum getGameStatus() const;

      /** \brief Increment minimum count of silos in range.
       *
       * To be called for each nuke hit.
       */
      void incSiloMinCountInRange();

      /** \brief Sub-render.
       *
       * Takes a rendering box as an additional input.
       *
       * \param screen Screen to render to.
       * \param st UI stack.
       * \param status Game status.
       * \param px Render area x1.
       * \param px Render area y1.
       * \param px Render area width.
       * \param px Render area height.
       */
      void renderSub(gfx::SurfaceScreen &screen, ui::UiStack &st, GameStatusEnum status,
          unsigned px, unsigned py, unsigned pw, unsigned ph);

      /** \brief Remove a flak bullet from the global map.
       *
       * \param op Pointer to remove.
       */
      void removeBulletFlak(BulletFlak *op);

      /** \brief Remove a railgun bullet from the global map.
       *
       * \param op Pointer to remove.
       */
      void removeBulletRailgun(BulletRailgun *op);

      /** \brief Remove a missile from the global map.
       *
       * \param op Missile to remove.
       */
      void removeMissileAnti(Missile *op);

      /** \brief Remove a missile from the global map.
       *
       * \param op Missile to remove.
       */
      void removeMissileNuke(Missile *op);

      /** \brief Remove a silo from the global map.
       *
       * \param silo Silo to remove.
       */
      void removeSilo(Silo *silo);

    public:
      /** \cond */
      virtual bool handleKey(const ui::EventKey &ev, ui::UiStack &st);
      virtual bool handleMisc(const ui::EventMisc &ev, ui::UiStack &st);
      virtual bool handleMouseButton(const ui::EventMouseButton &ev, ui::UiStack &st);
      virtual bool handleMouseMotion(const ui::EventMouseMotion &ev, ui::UiStack &st);
      virtual void render(ui::UiStack &st, gfx::SurfaceScreen &screen);
      virtual void update(ui::UiStack &st);
      /** \endcond */
  };
}

#endif
