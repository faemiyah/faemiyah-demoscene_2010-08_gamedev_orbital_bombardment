#include "data/xml_file.hpp"

#include <boost/property_tree/xml_parser.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace data;

XmlFile::XmlFile(const char *pfname)
{
  this->load(fs::path(pfname));
}

XmlFile::XmlFile(const std::string &pfname)
{
  this->load(fs::path(pfname));
}

XmlFile::XmlFile(const fs::path &pfname)
{
  this->load(pfname);
}

void XmlFile::load(const fs::path &pfname)
{
  data::shristr istr = data::open_read(pfname);

  pt::read_xml(*istr, m_xtree);
}
