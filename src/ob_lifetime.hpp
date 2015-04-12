#ifndef OB_LIFETIME_HPP
#define OB_LIFETIME_HPP

namespace ob
{
  /** \brief Simple lifetime counter.
   *
   * Designed to be added as a property to objects.
   */
  class Lifetime
  {
    protected:
      /** Lifetime existed. */
      int m_age;

      /** Lifetime remaining. */
      int m_lifetime;

      /** Death flag. */
      bool m_dead;

    public:
      /** \brief Empty constructor. */
      Lifetime();

      /** \brief Constructor.
       *
       * \param lt Lifetime to start with.
       */
      Lifetime(int lt);

      /** \brief Destructor. */
      ~Lifetime() { }

    public:
      /** \brief Decrement age.
       *
       * Done after death. Informs if this should be removed.
       *
       * \return True if still alive.
       */
      bool decrementAge();

      /** \brief Die.
       *
       * Will set current age to given value and turn on death flag.
       *
       * After this, decrementAge should be called instead of increment.
       *
       * \param dtime Death time left.
       * \return True if not already dead, otherwise false.
       */
      bool die(int dtime);

      /** \brief Reset time of death.
       *
       * Will set lifetime to given parameter. Should only be called early in the life of a bullet.
       *
       * Does not return anything.
       *
       * \param timetodie New lifetime.
       */
      void resetLife(int timetodie);

      /** \brief Age one step.
       *
       * Informs if object still has right to live.
       *
       * \param prob Probability of death after lifetime expended (1 per).
       * \return True if still alive.
       */
      bool incrementAge(int prob = 100);

    public:
      /** \brief Tell if still exists.
       *
       * Things may exist even if they're dead.
       *
       * \return True if yes, false if no.
       */
      bool exists() const
      {
        return (m_age >= 0);
      }

      /** \brief Tell if this is dead.
       *
       * \return True if yes, false if no.
       */
      bool isDead() const
      {
        return m_dead;
      }
  };
}

#endif
