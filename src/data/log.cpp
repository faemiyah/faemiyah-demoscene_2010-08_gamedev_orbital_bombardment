#include "data/log.hpp"

boost::signals2::signal<void (const std::string&)> data::log;

void data::log_default(const std::string &op)
{
  std::cout << op << std::endl;
}

