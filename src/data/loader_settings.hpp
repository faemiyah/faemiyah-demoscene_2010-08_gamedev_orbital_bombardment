#ifndef DATA_LOADER_HPP
#define DATA_LOADER_HPP

#include "defaults.hpp"

namespace data
{
  /** \brief Generic loader setting base.
   *
   * Derieved implementations may supply more settings than just flags. */
  class LoaderSettings
  {
    private:
      /** Flags. */
      uint32_t m_flags;

    protected:
      /** \brief Default constructor. */
      LoaderSettings() :
        m_flags(0) { }

    protected:
      /** \brief Tell if a flag(s) are set.
       *
       * If multiple flags are OR'd into given flags, they all must be set.
       *
       * \return True if yes, false if no.
       */
      bool hasFlag(uint32_t op) const
      {
        return (m_flags & op) == op;
      }

      /** \brief Remove flag(s).
       *
       * \param Flag(s) to remove.
       */
      void removeFlag(uint32_t op)
      {
        m_flags &= ~op;
      }

      /** \brief Set flag(s).
       *
       * \param op Flag(s) to set. */
      void setFlag(uint32_t op)
      {
        m_flags |= op;
      }
  };
}

#endif
