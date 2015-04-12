#ifndef DATA_LOG_HPP
#define DATA_LOG_HPP

#include "defaults.hpp"

#include <boost/signals2.hpp>

namespace data
{
  /** Log signaller. */
  extern boost::signals2::signal<void (const std::string&)> log;

  /** \brief Default log output signal.
   *
   * \param str
   */
  extern void log_default(const std::string &op);
}

#endif
