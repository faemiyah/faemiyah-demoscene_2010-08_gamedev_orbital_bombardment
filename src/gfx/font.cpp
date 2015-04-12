#include "gfx/font.hpp"

#include "data/generic.hpp"
#include "gfx/glyph.hpp"
#include "gfx/texture_2d.hpp"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace gfx;

namespace data
{
  template<> Font::store_type data::Storable<Font, FontLoader>::g_store(0);
}

const float Font::GLYPH_SIDE_LENGTH = 1.0f / static_cast<float>(GLYPHS_PER_FONT_SIDE);

Font::Font()
{
  this->unreserve();
}

Font::Font(const fs::path &pfname, const FontLoader &loader)
{
  this->load(pfname, loader);
}

Font::~Font()
{
  this->unreserve();
}

void Font::addTexture(const std::string &pfname)
{
  m_textures.push_back(Texture2DSptr(new Texture2D(pfname)));
}

boost::tuple<unsigned, float> Font::calcLineWidth(float fs, const std::wstring &text, unsigned idx) const
{
  float retw = 0.0f;

  for(;; ++idx)
  {
    if(idx >= text.length())
    {
      return boost::tuple<unsigned, float>(0, retw);
    }

    wchar_t cc = text[idx];
    if(cc == '\n')
    {
      return boost::tuple<unsigned, float>(idx + 1, retw);
    }

    retw += this->getGlyph(cc).getAdvance().x() * fs;
  }
}

math::vec2f Font::calcTextDimensions(float fs, const std::wstring &text) const
{
  float rwid = 0.0f;
  int rows = 0;

  unsigned idx = 0;
  do {
    ++rows;

    float fwid;
    boost::tie(idx, fwid) = this->calcLineWidth(fs, text, idx);

    rwid = std::max(fwid, rwid);
  } while(idx);

  return math::vec2f(rwid, static_cast<float>(rows) * fs);
}

std::list<float>* Font::calcTextLengths(float fs, const std::wstring &text) const
{
  std::list<float> *ret = new std::list<float>();

  unsigned idx = 0;
  do {
    float fwid;
    boost::tie(idx, fwid) = this->calcLineWidth(fs, text, idx);
    ret->push_back(fwid);
  } while(idx);

  return ret;
}

void Font::compile()
{
  BOOST_FOREACH(GlyphMap::value_type &vv, m_glyphs)
  {
    vv.second.compile(*this);
  }

  m_space_width = this->getGlyph(' ').getAdvance().x();
}

void Font::load(const fs::path &pfname, const FontLoader &loader)
{
  boost::ignore_unused_variable_warning(loader);

  this->unreserve();

  data::shristr pfile(data::open_read(pfname));
  pt::ptree xtree;
  pt::read_xml(*pfile, xtree);

  pt::ptree::value_type root = xtree.front();
  if(root.first.compare("font"))
  {
    std::stringstream err;
    err << "unknown root type: " << root.first;
    BOOST_THROW_EXCEPTION(std::invalid_argument(err.str()));
  }

  BOOST_FOREACH(pt::ptree::value_type &vv, root.second)
  {
    const std::string &type = vv.first;
    const pt::ptree &subtree = vv.second;

    if(!type.compare("glyph"))
    {
      float x1 = subtree.get<float>("x1"),
            y1 = subtree.get<float>("y1"),
            s1 = subtree.get<float>("s1"),
            t1 = subtree.get<float>("t1"),
            x2 = subtree.get<float>("x2"),
            y2 = subtree.get<float>("y2"),
            s2 = subtree.get<float>("s2"),
            t2 = subtree.get<float>("t2");

      this->addGlyph(Glyph(static_cast<wchar_t>(subtree.get<int>("code")), subtree.get<float>("width"),
          subtree.get<float>("height"), subtree.get<float>("left"), subtree.get<float>("top"),
          subtree.get<float>("advance_x"), subtree.get<float>("advance_y"), math::vec4f(x1, y1, s1, t1),
          math::vec4f(x2, y2, s2, t2), subtree.get<unsigned>("page")));
    }
    else if(!type.compare("texture"))
    {
      fs::path subpath(fs::path(pfname).parent_path() / subtree.get<std::string>(""));
      this->addTexture(subpath.generic_string());
    }
  }

  this->compile();
}

void Font::unreserve()
{
  m_glyphs.clear();
  m_textures.clear();
  m_space_width = 0.0f;
}

Font::container_type Font::createImplementation(const fs::path &pfname, const FontLoader &loader)
{
  return container_type(new Font(pfname, loader));
}

std::ostream& gfx::operator<<(std::ostream &lhs, const Font &rhs)
{
  lhs << "Font:";
  BOOST_FOREACH(const GlyphMap::value_type &vv, rhs.m_glyphs)
  {
    lhs << "\n\t" << vv.second;
  }
  BOOST_FOREACH(const Texture2DSptr &vv, rhs.m_textures)
  {
    lhs << "\n\t" << *vv;
  }
  return lhs;
}

