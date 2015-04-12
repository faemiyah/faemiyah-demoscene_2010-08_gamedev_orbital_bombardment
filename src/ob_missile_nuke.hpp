#ifndef OB_MISSILE_NUKE_HPP
#define OB_MISSILE_NUKE_HPP

#include "ob_missile.hpp"
#include "snd/source.hpp"

namespace ob
{
  /** \brief Nuke (missile).
   *
   * Nukes are launched by the player against cities.
   */
  class MissileNuke :
    public Missile
  {
    private:
      /** Target position in surface to strike. */
      math::vec3d m_target;

      /** Nuking flag. */
      bool m_nuking;

      /** Nuke nearing sound reference. */
      snd::Source *m_beepingsound;

    public:
      /** \brief Constructor.
       *
       * \param pos Starting position.
       * \param dir Starting direction.
       * \param target Here to strike.
       */
      MissileNuke(const math::vec3d &pos, const math::vec3d &dir, const math::vec3d &target);

      /** \brief Destructor. */
      virtual ~MissileNuke() { }

    public:
      /** \brief Get the alpha for distort effect.
       *
       * \return Alpha param for distort.
       */
      float getDistortAlpha() const;

    public:
      /** \cond */
      virtual math::vec3d getTargetPos() const;
      virtual bool update();
      /** \endcond */

    private:
      /** Update the nuking.
       *
       * \return True if this is still alive.
       */
      bool updateNuking();

    public:
      /** \brief Tell if this is nuking now.
       *
       * \return True if yes, false if no.
       */
      bool isNuking() const
      {
        return m_nuking;
      }
  };
}

#endif
