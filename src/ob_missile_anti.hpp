#ifndef OB_MISSILE_ANTI
#define OB_MISSILE_ANTI

#include "ob_missile.hpp"

/** If 1, anti-missile tracks it's previous direction. */
#define MISSILE_ANTI_NEED_PREV_DIR 1

namespace ob
{
  /** \brief Anti-missile.
   *
   * Anti-ship OR anti-nuke.
   */
  class MissileAnti :
    public Missile
  {
    private:
      /** Target to home on. */
      const Missile *m_target;

#if (0 != MISSILE_ANTI_NEED_PREV_DIR)
      /** Rot during previous frame. */
      math::vec3d m_prev_dir;
#endif

      /** Should we still gain height? */
      bool gainingheight;

      /** How long to raise */
      int raisephaseend;

    public:
      /** \brief Accessor.
       * 
       * \return Target.
       */
      const Missile* getTarget() const
      {
        return m_target;
      }

    public:
      /** \brief Constructor.
       *
       * Target may be NULL to fire player instead.
       *
       * \param pos Position.
       * \param dir Direction.
       * \param tgt Target missile.
       */
      MissileAnti(const math::vec3d &pos, const math::vec3d &dir,
          const Missile *tgt);

      /** \brief Destructor. */
      virtual ~MissileAnti() { }

    public:
      /** \cond */
      virtual math::vec3d getTargetPos() const;
      virtual bool update();
      /** \endcond */
  };
}

#endif
