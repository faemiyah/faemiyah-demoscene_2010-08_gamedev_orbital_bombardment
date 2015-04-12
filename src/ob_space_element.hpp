#ifndef OB_SPACE_ELEMENT_HPP
#define OB_SPACE_ELEMENT_HPP

#include "math/vec.hpp"

namespace ob
{
  /** \brief Space element.
   *
   * Non-class that contains all the necessary functionality for moving
   * 'space' objects.
   */
  class SpaceElement
  {
    public:
      /** \brief Missile aim predictor function
      *
      * \param taget_pos Target position.
      * \param taget_vel Target velocity (must be constant to work).
      * \param missile_pos Missile position.
      * \param missile_vel Missile velocity (must be constant to work).
      */
      static math::vec3d calculateMissileAimDirection(math::vec3d target_pos,
          math::vec3d target_vel, math::vec3d missile_pos,
          math::vec3d missile_vel);

      /** \brief Missile target predictor function
      *
      * \param taget_pos Target position.
      * \param taget_vel Target velocity (must be constant to work).
      * \param missile_pos Missile position.
      * \param missile_vel Missile velocity (must be constant to work).
      */
      static math::vec3d calculateMissileAimTarget(math::vec3d target_pos,
          math::vec3d target_vel, math::vec3d missile_pos,
          math::vec3d missile_vel);

      /** \brief Normal bullet aim predictor function
      *
      * \param taget_pos Target position.
      * \param taget_vel Target velocity (must be constant to work).
      * \param launcher_pos Bullet launcher position.
      * \param launcher_vel Bullet launcher velocity (must be constant to work).
      * \param bullet_vel Bullet added velocity after launch (scalar).
      * \return Direction to shoot to.
      */
      static math::vec3d calculateBulletAimDirection(math::vec3d target_pos,
          math::vec3d target_vel, math::vec3d launcher_pos,
          math::vec3d launcher_vel, double bullet_vel);

      /** \brief Normal bullet target predictor function
      *
      * \param taget_pos Target position.
      * \param taget_vel Target velocity (must be constant to work).
      * \param launcher_pos Bullet launcher position.
      * \param launcher_vel Bullet launcher velocity (must be constant to work).
      * \param bullet_vel Bullet added velocity after launch (scalar).
      * \return Position of impact.
      */
      static math::vec3d calculateBulletAimTarget(math::vec3d target_pos,
          math::vec3d target_vel, math::vec3d launcher_pos,
          math::vec3d launcher_vel, double bullet_vel);

      /** \brief Normal bullet flight time predictor function
      *
      * \param taget_pos Target position.
      * \param taget_vel Target velocity (must be constant to work).
      * \param launcher_pos Bullet launcher position.
      * \param launcher_vel Bullet launcher velocity (must be constant to work).
      * \param bullet_vel Bullet added velocity after launch (scalar).
      * \return Time to target (seconds).
      */
      static double calculateBulletTimeOfFlight(math::vec3d target_pos,
          math::vec3d target_vel, math::vec3d launcher_pos,
          math::vec3d launcher_vel, double bullet_vel);

      /** Movement function.
       *
       * \param dst_pos Destination (position).
       * \param dst_dir Destination (direction).
       * \param tgt_dir Target direction.
       * \param max_speed Maximum speed.
       * \param acc Acceleration.
       * \param lat_acc Lateral acceleration.
       * \param applyratio How much to change direction as opposed to merely accelerating forward (1=change direction with full power, 0=just accelerate forward).
       * \return Unit direction of object before fixes.
       */
      static math::vec3d updatePosDir(math::vec3d &dst_pos, math::vec3d &dst_dir,
          const math::vec3d &tgt_dir, float max_speed, float acc,
          float lat_acc, float applyratio);
  };
}

#endif
