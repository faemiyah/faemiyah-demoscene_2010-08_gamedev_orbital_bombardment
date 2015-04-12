#ifndef OB_GAME_VIEW_HPP
#define OB_GAME_VIEW_HPP

#include "gfx/entity_object.hpp"
#include "ob_collision_element.hpp"
#include "ob_lifetime.hpp"
#include "ob_visualization.hpp"

namespace ob
{
  /** \brief Game view.
   *
   * The view and location of the player. Interpreted in relation to the world
   * sphere and a major circle path over it.
   */
  class GameView :
    public gfx::EntityObject,
    public CollisionElement,
    public Lifetime
  {
    private:
      /** Visualization toggle for course visibility. */
      Visualization m_course_visible;

      /** Visualization toggle for course visibility. */
      Visualization m_course_change_visible;

      /** Relative matrix in object space. */
      math::mat4f m_camera_matrix;

      /** Matrix for course marker. */
      math::mat4f m_course_matrix;

      /** Relative matrix in object space. */
      math::mat4f m_orbit_matrix;

      /** Direction of orbit. */
      math::vec3d m_dir;

      /** Direction of course. */
      math::vec3d m_course_dir;

      /** Direction of previous course. */
      math::vec3d m_old_dir;

      /** Escape speed. */
      float m_escape_speed;

      /** Offset to the camera. */
      float m_offset_angle;

      /** Target offset to the camera. */
      float m_offset_angle_target;

      /** Target offset to the camera. */
      float m_offset_angle_speed;

      /** Orbit distance. */
      float m_orbit;

      /** Rotation limit. */
      float m_rlimit;

      /** Counter to course change. */
      int m_time_to_course_change;

      /** Counter to course normalization. */
      int m_time_to_course_normalization;

      /** Whether to draw the course change? */
      bool m_flag_course_change;

    public:
      /** \brief constructor.
       *
       * Randomizes direction and position.
       *
       * \param orbit Orbit distance.
       * \param rlimit Rotation limit.
       */
      GameView(float orbit, float rlimit);

      /** \brief constructor.
       *
       * \param orbit Orbit distance.
       * \param rlimit Rotation limit.
       * \param dir Direction (forward in orbit).
       * \param pos Position.
       * \param rot Rotation.
       */
      GameView(float orbit, float rlimit, const math::vec3d &dir,
          const math::vec3d &pos, const math::vec3d &rot);

      /** \brief Destructor. */
      virtual ~GameView() { }

    public:
      /** \brief Accept course change. */
      void acceptCourse();

      /** \brief Cancel course change. */
      void cancelCourse();

      /** \brief Increment escape speed.
       *
       * To be done once per frame once game has been won.
       */
      void incEscapeSpeed();

      /** \brief Rotate the view.
       *
       * Shorthand for changing the rotation with limits.
       *
       * \param rx X rotation.
       * \param ry Y rotation.
       */
      void rotate(double rx, double ry);

      /** \brief Spawn some explosions.
       */
      void spawnExplosion();

      /** \brief Update course marker.
       *
       * \param tgt Target course destination.
       */
      void updateCourse(const math::vec3d &tgt);

      /** \brief Update the orbit.
       *
       * \return True if game continues, false if ended.
       */
      bool update();

      /** \brief Load orbiting view.
       *
       * Interprets camera position as an orbit and loads the view accordingly.
       */
      const math::mat4f &viewOrbit();

    public:
      /** \cond */
      virtual void gamisticEffect(void *args);
      /** \endcond */

    public:
      /** \brief Bring position back to orbit.
       *
       * Normalizes the position back to the orbit value given here.
       */
      void bringToOrbit()
      {
        m_pos = math::normalize(m_pos) * static_cast<double>(m_orbit);
      }

      /** \brief Accessor.
       *
       * \return Camera matrix.
       */
      const math::mat4f& getCameraMatrix() const
      {
        return m_camera_matrix;
      }

      /** \brief Accessor.
       *
       * \return Course matrix.
       */
      const math::mat4f& getCourseMatrix() const
      {
        return m_course_matrix;
      }

      /** \brief Accessor.
       *
       * \return Orbit level.
       */
      float getOrbit() const
      {
        return m_orbit;
      }

      /** \brief Accessor.
       *
       * \return Orbit matrix.
       */
      const math::mat4f& getOrbitMatrix() const
      {
        return m_orbit_matrix;
      }

      /** \brief Tell if the course should be visible.
       *
       * \return True if yes, false if no.
       */
      bool isCourseVisible() const
      {
        return m_course_visible.isVisible();
      }

      /** \brief Tell if the course change should be visible.
       *
       * \return True if yes, false if no.
       */
      bool isCourseChangeVisible() const
      {
        return m_course_change_visible.isVisible();
      }

      /** \brief Accessor.
       *
       * \return Direction of orbit movement.
       */
      const math::vec3d& getDir() const
      {
        return m_dir;
      }

      /** \brief Set movement direction of this.
       *
       * Set the movement direction.
       *
       * \param op New direction.
       */
      void setDir(const math::vec3d &op)
      {
        m_dir = op;
      }

      /** \brief Set view limits.
       *
       * Set the view area limit. This is interpreted as the largest rotation
       * allowed in the primary rotation axis. To make the clamping
       * non-distorted, when the secondary rotation axis is nonzero, the
       * primary axis is allowed more rotation.
       *
       * \param op Limit in radians.
       */
      void setRotationLimit(float op)
      {
        m_rlimit = op;
      }
  };
}

#endif
