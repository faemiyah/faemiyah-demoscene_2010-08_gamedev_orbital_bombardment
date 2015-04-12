#include "gfx/mesh.hpp"

#include "data/log.hpp"
#include "data/xml_file.hpp"
#include "gfx/mesh_static.hpp"
#include "ui/generic.hpp"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace gfx;

namespace data
{
  template<> Mesh::store_type data::Storable<Mesh, MeshLoader>::g_store(0);
  template<> Mesh::registry_type *data::Registry<Mesh, MeshLoader>::g_registry = NULL;
}

void Mesh::addTexture(const std::string &id, const void *tex)
{
  if(id.compare(0, 7, "texture") == 0)
  {
    m_textures.push_back(static_cast<const Texture2D*>(tex));
    return;
  }
  if(id.compare(0, 6, "volume") == 0)
  {
    m_volumes.push_back(static_cast<const Texture3D*>(tex));
    return;
  }

  std::stringstream sstr;
  sstr << "unknown texture type: " << id;
  BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
}

void Mesh::addTextureFile(const std::string &type, const fs::path &pfname, const ImageLoader &loader)
{
  if(0 == type.compare("volume"))
  {
    if(Texture3D::storageContains(pfname))
    {
      this->addTexture(type, Texture3D::locate(pfname).get().get());
    }
    else
    {
      this->addTexture(type, Texture3D::create(pfname, loader).get().get());
    }
  }
  else
  {
    if(Texture2D::storageContains(pfname))
    {
      this->addTexture(type, Texture2D::locate(pfname).get().get());
    }
    else
    {
      this->addTexture(type, Texture2D::create(pfname, loader).get().get());
    }
  }
}

void Mesh::calcNormals()
{
  unsigned vsize = static_cast<unsigned>(m_vertex.size());
  m_normal.resize(0);
  m_normal.resize(vsize, math::vec3f(0.0f, 0.0f, 0.0f));
  std::vector<unsigned> refcnts;;
  refcnts.resize(vsize, 0);

  TriVec faces;
  m_lod.collect(faces);

  BOOST_FOREACH(const Triangle &tri, faces)
  {
    math::vec3f nor = this->calcNormal(tri);
    unsigned ia = tri.a(),
             ib = tri.b(),
             ic = tri.c();
    m_normal[ia] += nor;
    m_normal[ib] += nor;
    m_normal[ic] += nor;
    ++(refcnts[ia]);
    ++(refcnts[ib]);
    ++(refcnts[ic]);
  }

  for(unsigned ii = 0; (ii < vsize); ++ii)
  {
    math::vec3f &nn = m_normal[ii];

    unsigned rcnt = refcnts[ii];
    if(rcnt > 0)
    {
      nn = math::normalize(nn / static_cast<float>(rcnt));
    }
    else
    {
      // Fallback normal points up.
      nn = math::vec3f(0.0f, 1.0f, 0.0f);
    }
  }

  data::stl_trim(m_color);
  data::stl_trim(m_texcoord);
  data::stl_trim(m_vertex);
}

math::rect3f Mesh::getBoundary() const
{
  if(m_vertex.size() <= 0)
  {
    std::stringstream sstr;
    sstr << "boundary requested for mesh that has no vertices";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  math::rect3f ret(m_vertex[0]);
  for(unsigned ii = 1; (ii < m_vertex.size()); ++ii)
  {
    ret.expand(m_vertex[ii]);
  }
  return ret;
}

void Mesh::load(const fs::path &pfname, const pt::ptree &root, const MeshLoader &loader)
{
  math::vec3f obj_scale(1.0f, 1.0f, 1.0f);
  bool enable_center = false,
       enable_into = false,
       enable_scale = false;

  this->unreserve();

  BOOST_FOREACH(const pt::ptree::value_type &vv, root)
  {
    const std::string &type = vv.first;
    const pt::ptree &subtree = vv.second;

    if(0 == type.compare("offset"))
    {
      math::vec3f off(subtree.get<float>("x"),
          subtree.get<float>("y"),
          subtree.get<float>("z"));
      m_offset = math::vec3f(off.y(), off.z(), -off.x());
    }
    else if(0 == type.compare("scale"))
    {
      enable_scale = true;
      enable_center = subtree.get<bool>("center", 0);
      enable_into = subtree.get<bool>("into", 0);
      obj_scale.x() = subtree.get<float>("x", 1.0f);
      obj_scale.y() = subtree.get<float>("y", 1.0f);
      obj_scale.z() = subtree.get<float>("z", 1.0f);
    }
    else if(0 == type.compare("vertex"))
    {
      this->readVertex(subtree);
    }
    else if(0 == type.compare("face"))
    {
      // Must flip faces for correct orientation.
      m_lod.addFace(Triangle(subtree.get<unsigned>("a"),
            subtree.get<unsigned>("c"),
            subtree.get<unsigned>("b")));
    }
    else if((0 == type.compare(0, 7, "texture")) || (0 == type.compare(0, 9, "normalmap")))
    {
      fs::path subpath = pfname.parent_path().parent_path() / fs::path(subtree.get<std::string>(""));
      this->addTextureFile(type, subpath);
    }
    else if(0 == type.compare(0, 6, "volume"))
    {
      fs::path subpath = pfname.parent_path().parent_path() / fs::path(subtree.get<std::string>(""));
      this->addTextureFile(type, subpath);
    }
  }

  // perform transformations
  {
    math::rect3f area = this->getBoundary();
    
    if(enable_center || loader.hasTranslateCenter())
    {
      math::vec3f center = area.center();
      this->translate(-center);
      area = math::rect3f(area.getMin() - center, area.getMax() - center);
    }

    // scaling may be defined only in loader settings
    if(!enable_scale)
    {
      if(math::vec3f(1.0f, 1.0f, 1.0f) != loader.getScaleFlat())
      {
        enable_scale = true;
      }
    }

    if(enable_scale)
    {
      if(enable_into)
      {
        math::vec3f size = area.getMax() - area.getMin();
        math::vec3f object_fill_scale = obj_scale * loader.getScaleFlat() / size;
        float common_scale = math::min(object_fill_scale.x(), object_fill_scale.y(), object_fill_scale.z());

        obj_scale = math::vec3f(common_scale, common_scale, common_scale);
      }
      else
      {
        obj_scale *= loader.getScaleFlat();
      }

      this->scale(obj_scale);
    }
  }

  // settings
  m_sub_element = loader.isSubElement();

  this->compile();
}

void Mesh::readVertex(const pt::ptree &subtree)
{
  try {
    m_color.push_back(Color(subtree.get<float>("color.r"),
          subtree.get<float>("color.g"),
          subtree.get<float>("color.b"),
          subtree.get<float>("color.a")));
  } catch(...) { }
  try {
    m_normal.push_back(math::vec3f(subtree.get<float>("normal.x"),
          subtree.get<float>("normal.y"),
          subtree.get<float>("normal.z")));
  } catch(...) { }
  try {
    m_reference.push_back(math::vec4u(subtree.get<unsigned>("reference.a"),
          subtree.get<unsigned>("reference.b"),
          subtree.get<unsigned>("reference.c"),
          subtree.get<unsigned>("reference.d")));
  } catch(...) { }
  try {
    m_texcoord.push_back(math::vec2f(subtree.get<float>("texcoord.s"),
          subtree.get<float>("texcoord.t")));
  } catch(...) { }
  try {
    m_weight.push_back(math::vec4f(subtree.get<float>("weight.a"),
          subtree.get<float>("weight.b"),
          subtree.get<float>("weight.c"),
          subtree.get<float>("weight.d")));
  } catch(...) { }
  // Vertex MUST exist, others are voluntary.
  math::vec3f ver(subtree.get<float>("x"),
        subtree.get<float>("y"),
        subtree.get<float>("z"));
  // Must 'rotate' to correct orientation.
  m_vertex.push_back(math::vec3f(ver.y(), ver.z(), -ver.x()));
}

void Mesh::scale(const math::vec3f &svec)
{
  BOOST_FOREACH(math::vec3f &vv, m_vertex)
  {
    vv *= svec;
  }
  // Scale is also modified.
  m_offset *= svec;
}

void Mesh::translate(const math::vec3f &tvec)
{
  BOOST_FOREACH(math::vec3f &vv, m_vertex)
  {
    vv += tvec;
  }
}

void Mesh::unreserve()
{
  m_color.clear();
  m_normal.clear();
  m_reference.clear();
  m_texcoord.clear();
  m_vertex.clear();
  m_weight.clear();
  m_lod.unreserve();
  m_textures.clear();
  m_volumes.clear();
}

namespace data
{
  template<> Mesh* Registry<Mesh, MeshLoader>::registryCreate(const fs::path &pfname, const pt::ptree &root,
      const MeshLoader &loader)
  {
    std::string mesh_type("static"); // default type

    {
      pt::ptree::const_assoc_iterator iter = root.find("type");
      if(iter != root.not_found())
      {
        mesh_type.assign(iter->second.get_value<std::string>());
      }
    }

    {
      std::ostringstream sstr;
      sstr << "loading " << mesh_type << " mesh " << (loader.isSubElement() ? "as sub-element " : "") <<
        "from " << pfname;
      data::log(sstr.str());
    }

    registry_type::iterator iter = g_registry->find(mesh_type);
    if(iter == g_registry->end())
    {
      std::ostringstream sstr;
      sstr << "mesh id '" << mesh_type << "' not found in registry";
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
    return iter->second(pfname, root, loader);
  }
}

Mesh::container_type Mesh::createImplementation(const fs::path &pfname, const MeshLoader &loader)
{
  if(!data::filename_is_mesh(pfname))
  {
    // TODO: probe?
    std::ostringstream sstr;
    sstr << "unkown mesh file '" << pfname << '\'';
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  data::XmlFile xml_file(pfname);

  if(0 == xml_file.rootType().compare("mesh"))
  {
    Mesh *ret = Mesh::registryCreate(pfname, xml_file.tree(), loader);

    return Mesh::container_type(ret);
  }
  else if(0 == xml_file.rootType().compare("meta-mesh"))
  {
    container_type ret;
    math::vec3f obj_scale(1.0f, 1.0f, 1.0f);
    bool enable_center = false,
         enable_into = false,
         enable_scale = false;

    BOOST_FOREACH(const pt::ptree::value_type &vv, xml_file.tree())
    {
      const std::string &type = vv.first;
      const pt::ptree &subtree = vv.second;

      if(0 == type.compare("scale"))
      {
        enable_scale = true;
        enable_center = subtree.get<bool>("center", 0);
        enable_into = subtree.get<bool>("into", 0);
        obj_scale.x() = subtree.get<float>("x", 1.0f);
        obj_scale.y() = subtree.get<float>("y", 1.0f);
        obj_scale.z() = subtree.get<float>("z", 1.0f);
      }
      else
      {
        bool is_mesh = (0 == type.compare("mesh"));
        bool is_sub_mesh = (0 == type.compare("submesh"));

        if(is_mesh || is_sub_mesh)
        {
          MeshLoader sub_loader = is_sub_mesh ? loader.asSubMesh() : loader;

          pt::ptree::const_assoc_iterator iter = subtree.find("filename");
          if(iter != subtree.not_found())
          {
            fs::path subfname = pfname.parent_path() / fs::path(iter->second.get_value<std::string>());
            data::XmlFile sub_xml_file(subfname);

            if(0 != sub_xml_file.rootType().compare("mesh"))
            {
              std::ostringstream sstr;
              sstr << "unknown root type for additional mesh: " << subfname;
              BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
            }

            ret.add(Mesh::registryCreate(subfname, sub_xml_file.tree(), sub_loader));
          }
          else
          {
            ret.add(Mesh::registryCreate(pfname, subtree, sub_loader));
          }
        }
        else if((0 != type.compare("<xmlattr>")) && (0 != type.compare("<xmlcomment>")))
        {
          std::ostringstream sstr;
          sstr << "unknown element: '" << type << "' in meta-mesh file " << pfname;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
      }
    }

    if(ret.empty())
    {
      std::ostringstream sstr;
      sstr << "mesh collection " << pfname << " contains no meshes";
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }

    {
      math::rect3f area;

      {
        unsigned ii = 0;

        do {
          MeshSptr &vv = ret.get(ii);

          if(!vv->isSubElement())
          {
            area = vv->getBoundary();
            ++ii;
            break;
          }
        } while(ret.size() > ++ii);

        if(ret.size() <= ii)
        {
          std::ostringstream sstr;
          sstr << "mesh collection " << pfname << " contains only sub-elements";
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }

        for(; (ret.size() > ii); ++ii)
        {
          MeshSptr &vv = ret.get(ii);

          if(!vv->isSubElement())
          {
            area.expand(vv->getBoundary());
          }
        }
     }

      if(enable_center || loader.hasTranslateCenter())
      {
        math::vec3f center = area.center();

        BOOST_FOREACH(MeshSptr &vv, ret)
        {
          vv->translate(-center);
        }

        area = math::rect3f(area.getMin() - center, area.getMax() - center);
      }

      if(!enable_scale)
      {
        if(math::vec3f(1.0f, 1.0f, 1.0f) != loader.getScaleFlat())
        {
          enable_scale = true;
        }
      }

      if(enable_scale)
      {
        if(enable_into)
        {
          math::vec3f size = area.getMax() - area.getMin();
          math::vec3f object_fill_scale = obj_scale * loader.getScaleFlat() / size;
          float common_scale = math::min(object_fill_scale.x(), object_fill_scale.y(), object_fill_scale.z());

          obj_scale = math::vec3f(common_scale, common_scale, common_scale);
        }
        else
        {
          obj_scale *= loader.getScaleFlat();
        }

        BOOST_FOREACH(MeshSptr &vv, ret)
        {
          vv->scale(obj_scale);
          vv->compile(); // Meshes have been already compiled on load. if we don't scale, we don't need to recompile.
        }
      }
    }

    return ret;
  }

  {
    std::stringstream err;
    err << "unknown root type: " << xml_file.rootType();
    BOOST_THROW_EXCEPTION(std::invalid_argument(err.str()));
  }
}

