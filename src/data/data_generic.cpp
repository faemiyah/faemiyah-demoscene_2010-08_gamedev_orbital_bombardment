#include "data/generic.hpp"

#include <boost/filesystem/fstream.hpp>

#include <sstream>

using namespace data;
namespace fs = boost::filesystem;

bool data::file_exists(const fs::path &pfname)
{
  if(!fs::exists(pfname))
  {
#if defined(DATADIR)
    fs::path fpath = fs::path(DATADIR) / pfname;
    return fs::exists(fpath);
#else
    return false;
#endif
  }
  return true;
}

shristr data::open_read(const fs::path &pfname)
{
  // TODO: ZIPfile.
  
  // TODO: Datadir ZIPfile.
  
  return shristr(new fs::ifstream(open_search(pfname), std::ifstream::binary));
}

shrostr data::open_write(const fs::path &pfname)
{
  shrostr ret(new fs::ofstream(pfname));
  if(ret->fail())
  {
    std::stringstream sstr;
    sstr << "could not open \"" << pfname << "\" for writing";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }  
  return ret;
}

fs::path data::open_search(const fs::path &pfname)
{
  if(!fs::exists(pfname))
  {
#if defined(DATADIR)
    fs::path fpath = fs::path(DATADIR) / pfname;
    if(!fs::exists(fpath))
    {
#endif
      std::stringstream err;
      err << "could not find file: " << pfname;
      BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
#if defined(DATADIR)
    }
    return fpath;
#endif
  }

  return pfname;
}

