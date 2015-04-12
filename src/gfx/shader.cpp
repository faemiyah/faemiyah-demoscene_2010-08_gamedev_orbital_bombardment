#include "gfx/shader.hpp"

#include "data/log.hpp"
#include "data/xml_file.hpp"
#include "thr/dispatch.hpp"
#include "ui/generic.hpp"

#include "boost/lexical_cast.hpp"

static const unsigned GL_LOG_INITIAL_SIZE = 4096;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace gfx;

namespace data
{
  template<> Shader::store_type data::Storable<Shader, ShaderLoader>::g_store(0);
}

Shader const *Shader::g_current_shader = NULL;

/** \brief Well-known shader variable name database.
  */
class ShaderVariableNameDatabase
{
  private:
    /** Convenience typedef. */
    typedef std::map<Shader::ShaderVariableName, const char*> ShaderVariableNameMap;

  private:
    /** Id to name -mapping. */
    ShaderVariableNameMap m_names;

  public:
    /** \brief Constructor. */
    ShaderVariableNameDatabase();

    /** \brief Destructor. */
    ~ShaderVariableNameDatabase() { }

  public:
    /** Map name to id.
     *
     * \param op Name.
     */
    Shader::ShaderVariableName getId(const char *op) const;

    /** Map id to name.
     *
     * \param op Id.
     */
    const char* getName(Shader::ShaderVariableName op) const;
};

ShaderVariableNameDatabase::ShaderVariableNameDatabase()
{
  m_names[Shader::ATTRIBUTE_COLOR] = "color";
  m_names[Shader::ATTRIBUTE_NORMAL] = "normal";
  m_names[Shader::ATTRIBUTE_REFERENCE] = "reference";
  m_names[Shader::ATTRIBUTE_TEXCOORD] = "texcoord";
  m_names[Shader::ATTRIBUTE_VERTEX] = "vertex";
  m_names[Shader::ATTRIBUTE_WEIGHT] = "weight";
  m_names[Shader::UNIFORM_LIGHT_AMBIENT] = "light_ambient";
  m_names[Shader::UNIFORM_LIGHT_DIFFUSE] = "light_diffuse";
  m_names[Shader::UNIFORM_LIGHT_DIR] = "light_dir";
  m_names[Shader::UNIFORM_LIGHT_POS] = "light_pos";
  m_names[Shader::UNIFORM_MODELVIEW] = "modelview";
  m_names[Shader::UNIFORM_NORMALMAP] = "normalmap";
  m_names[Shader::UNIFORM_PROJECTION] = "projection";
  m_names[Shader::UNIFORM_TEX0] = "tex0";
  m_names[Shader::UNIFORM_TEX1] = "tex1";
  m_names[Shader::UNIFORM_TEX2] = "tex2";
  m_names[Shader::UNIFORM_TEX3] = "tex3";
  m_names[Shader::UNIFORM_TEX4] = "tex4";
  m_names[Shader::UNIFORM_TEX5] = "tex5";
  m_names[Shader::UNIFORM_TEX6] = "tex6";
  m_names[Shader::UNIFORM_TEX7] = "tex7";
  m_names[Shader::UNIFORM_TEXPARAMS] = "texparams";
  m_names[Shader::UNIFORM_TRANSFORM] = "transform";
}

Shader::ShaderVariableName ShaderVariableNameDatabase::getId(const char *op) const
{
  BOOST_FOREACH(const ShaderVariableNameMap::value_type &vv, m_names)
  {
    if(0 == strcmp(vv.second, op))
    {
      return vv.first;
    }
  }

  return Shader::SHADER_VARIABLE_NAME_NONE;
}

/** Map id to name.
 *
 * \param op Id.
 */
const char* ShaderVariableNameDatabase::getName(Shader::ShaderVariableName op) const
{
  ShaderVariableNameMap::const_iterator ii = m_names.find(op);

  BOOST_ASSERT(m_names.end() != ii);

  return ii->second;
}

static ShaderVariableNameDatabase g_name_database;

/** \brief Delete a program.
 *
 * \param program Program.
 * \param vshader Vertex shader id.
 * \param gshader Geometry shader id.
 * \param fshader Fragment shader id.
 */
void delete_program(unsigned program, unsigned vshader, unsigned gshader, unsigned fshader)
{
  if(0 < program)
  {
    if(0 < vshader)
    {
      glDetachShader(program, vshader);
    }
    if(0 < gshader)
    {
      glDetachShader(program, gshader);
    }
    if(0 < fshader)
    {
      glDetachShader(program, fshader);
    }

    glDeleteProgram(program);
  }
  if(vshader)
  {
    glDeleteShader(vshader);
  }
  if(gshader)
  {
    glDeleteShader(fshader);
  }
  if(fshader)
  {
    glDeleteShader(fshader);
  }
}

/** \brief Get a program log.
 *
 * \param op Program id.
 * \return Program info log as a string.
 */
std::string get_program_log(unsigned op)
{
  GLsizei logsize = GL_LOG_INITIAL_SIZE,
          reqsize;
  char *log = NULL;

  do {
    delete[] log;
    log = new char[logsize];
    glGetProgramInfoLog(op, logsize, &reqsize, log);
  } while(reqsize >= logsize);

  std::string ret(log);
  delete[] log;
  return ret;
}

/** \brief Get a shader log.
 *
 * \param op Shader id.
 * \return Shader info log as a string.
 */
std::string get_shader_log(unsigned op)
{
  GLsizei logsize = GL_LOG_INITIAL_SIZE,
          reqsize;
  char *log = NULL;

  do {
    delete[] log;
    log = new char[logsize];
    glGetShaderInfoLog(op, logsize, &reqsize, log);
  } while(reqsize >= logsize);

  std::string ret(log);
  delete[] log;
  return ret;
}

/** \brief Create a program out of two shaders.
 *
 * \param vprog Vertex shader.
 * \param fprog Fragmen shader.
 * \return Program that was successfully created or zero.
 */
unsigned create_program(unsigned vprog, unsigned gprog, unsigned fprog)
{
  unsigned ret = glCreateProgram();

  glAttachShader(ret, vprog);
  if(0 < gprog)
  {
    glAttachShader(ret, gprog);
  }
  glAttachShader(ret, fprog);
  glLinkProgram(ret);

  GLint compile_info;
  glGetProgramiv(ret, GL_LINK_STATUS, &compile_info);
  if(GL_TRUE != compile_info)
  {
    std::string log = get_program_log(ret);
    glDeleteProgram(ret);

    std::stringstream err;
    err << "shader link failed: " << log;
    BOOST_THROW_EXCEPTION(std::runtime_error(err.str().c_str()));
    return 0;
  }
#if defined(DEBUG)
  std::cout << get_program_log(ret);
#endif

  return ret;
}

/** \brief Create a shader of given type and data.
 *
 * \param type Shader type.
 * \param hdr Shader header.
 * \param code Shader code.
 * \return Shader that was successfully created or zero.
 */
unsigned create_shader(unsigned type, const std::string &hdr, const std::string &code)
{
  unsigned ret = glCreateShader(type);
  const char *data[] = { hdr.c_str(), code.c_str() };

  glShaderSource(ret, 2, data, NULL);
  glCompileShader(ret);

  GLint compile_info;
  glGetShaderiv(ret, GL_COMPILE_STATUS, &compile_info);
  if(GL_TRUE != compile_info)
  {
    std::string log = get_shader_log(ret);
    glDeleteShader(ret);

    std::stringstream err;
    err << "shader compile failed: \"" << hdr << '\n' << code << "\": " << log;
    BOOST_THROW_EXCEPTION(std::runtime_error(err.str().c_str()));
    return 0;
  }
#if defined(DEBUG)
  std::cout << get_shader_log(ret);
#endif

  return ret;
}

Shader::Shader(const fs::path &filename, const ShaderLoader &loader) :
  m_version(0),
  m_vsid(0),
  m_gsid(0),
  m_fsid(0),
  m_id(0)
{
  this->load(filename, loader);
}

Shader::~Shader()
{
  this->unreserve();
}

bool Shader::bind() const
{
  if(g_current_shader != this)
  {
    glUseProgram(m_id);
    g_current_shader = this;
    return true;
  }
  return false;
}

void Shader::compile()
{
  std::stringstream hdr_version, hdr_attribute, hdr_uniform, hdr_fragment;

  if(0 < m_version)
  {
    hdr_version << "#version " << m_version << std::endl;
    hdr_fragment << "out vec4 c_out;\n";
  }

  BOOST_FOREACH(const AttributeMap::value_type &vv, m_attributes)
  {
    hdr_attribute << ((150 <= m_version) ? "in" : "attribute") << ' ' << vv.second.getType() << ' ' <<
      vv.first << ";\n";
  }

  BOOST_FOREACH(const UniformMap::value_type &vv, m_uniforms)
  {
    hdr_uniform << "uniform " << vv.second.getType() << ' ' << vv.first << ";\n";
  }

  m_vsid = create_shader(GL_VERTEX_SHADER, hdr_version.str() + hdr_uniform.str() + hdr_attribute.str(),
      m_vshader);
  BOOST_ASSERT(0 < m_vsid);

  if(0 < m_gshader.length())
  {
    m_gsid = create_shader(GL_GEOMETRY_SHADER, hdr_version.str() + hdr_uniform.str(), m_gshader);
    BOOST_ASSERT(0 < m_gsid);
  }

  m_fsid = create_shader(GL_FRAGMENT_SHADER, hdr_version.str() + hdr_uniform.str() + hdr_fragment.str(),
      m_fshader);
  BOOST_ASSERT(0 < m_fsid);

  m_id = create_program(m_vsid, m_gsid, m_fsid);
  BOOST_ASSERT(0 < m_id);

#if defined(DEBUG)
  std::cout << *this << std::endl;
#endif

  // Compiling succeeded, get all attribute and uniform locations.
  BOOST_FOREACH(AttributeMap::value_type &vv, m_attributes)
  {
    this->updateAttribute(vv.second);
  }
  BOOST_FOREACH(UniformMap::value_type &vv, m_uniforms)
  {
    this->updateUniform(vv.second);
  }
}

const Attribute* Shader::getAttribute(const std::string &name) const
{
  AttributeMap::const_iterator iter = m_attributes.find(name);
  if(m_attributes.end() == iter)
  {
    return NULL;
  }
  return &((*iter).second);
}

const Uniform* Shader::getUniform(const std::string &name) const
{
  UniformMap::const_iterator iter = m_uniforms.find(name);
  if(m_uniforms.end() == iter)
  {
    return NULL;
  }
  return &((*iter).second);
}

bool Shader::hasAttribute(ShaderVariableName op) const
{
  if(m_attributes_common.end() == m_attributes_common.find(op))
  {
    return false;
  }

  return (NULL != this->getAttribute(g_name_database.getName(op)));
}

bool Shader::hasUniform(ShaderVariableName op) const
{
  if(m_uniforms_common.end() == m_uniforms_common.find(op))
  {
    return false;
  }
  
  return (NULL != this->getUniform(g_name_database.getName(op)));
}

void Shader::load(const fs::path &pfname, const ShaderLoader &loader)
{
  this->unreserve();

  {
    std::ostringstream sstr;
    sstr << "loading shader " << pfname;
    data::log(sstr.str());
  }

  data::XmlFile xml_file(pfname);

  if(0 != xml_file.rootType().compare("shader"))
  {
    std::stringstream err;
    err << "unknown root type: " << xml_file.rootType();
    BOOST_THROW_EXCEPTION(std::invalid_argument(err.str()));
  }

  BOOST_FOREACH(const pt::ptree::value_type &vv, xml_file.tree())
  {
    const std::string &type = vv.first;
    const pt::ptree &subtree = vv.second;

    if(0 == type.compare("version"))
    {
      m_version = subtree.get<unsigned>("");
    }
    else if(0 == type.compare("attribute"))
    {
      std::string pname = subtree.get<std::string>("name"),
        ptype = subtree.get<std::string>("type");
      m_attributes[pname] = Attribute(pname, ptype);
    }
    else if(0 == type.compare("uniform"))
    {
      std::string pname = subtree.get<std::string>("name"),
        ptype = subtree.get<std::string>("type");
      m_uniforms[pname] = Uniform(pname, ptype);
    }
    else if(0 == type.compare("vertexprogram"))
    {
      m_vshader = subtree.get<std::string>("");
    }
    else if(0 == type.compare("geometryprogram"))
    {
      m_gshader = subtree.get<std::string>("");
    }
    else if(0 == type.compare("fragmentprogram"))
    {
      m_fshader = subtree.get<std::string>("");
    }
    else if(0 != type.compare("<xmlattr>"))
    {
      std::stringstream err;
      err << "unknown element: " << type;
      BOOST_THROW_EXCEPTION(std::invalid_argument(err.str()));
    }
  }

  // Version provided by loader may override the one from the file.
  m_version = math::max(loader.getVersion(), m_version);

  thr::wait_privileged(&Shader::compile, this);
}

void Shader::updateAttribute(Attribute &op)
{
  const std::string &pname = op.getName();
  int pid = glGetAttribLocation(m_id, pname.c_str());
  if(0 > pid)
  {
    std::ostringstream err;
    err << "no attribute \"" << pname << "\" in shader " << std::endl << *this;
    BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
    return;
  }
  op.setId(static_cast<unsigned>(pid));

#if defined(DEBUG)
  std::cout << op << std::endl;
#endif

  // Common access.
  {
    Shader::ShaderVariableName common_name = g_name_database.getId(pname.c_str());

    if(Shader::SHADER_VARIABLE_NAME_NONE != common_name)
    {
      m_attributes_common[common_name] = op;
    }
  }
}

void Shader::updateUniform(Uniform &op)
{
  const std::string &pname = op.getName();
  int pid = glGetUniformLocation(m_id, pname.c_str());
  if(0 > pid)
  {
    std::ostringstream err;
    err << "no uniform \"" << pname << "\" in shader " << std::endl << *this;
    BOOST_THROW_EXCEPTION(std::runtime_error(err.str()));
    return;
  }
  op.setId(static_cast<unsigned>(pid));

#if defined(DEBUG)
  std::cout << op << std::endl;
#endif

  // Common access.
  {
    Shader::ShaderVariableName common_name = g_name_database.getId(pname.c_str());

    if(Shader::SHADER_VARIABLE_NAME_NONE != common_name)
    {
      m_uniforms_common[common_name] = op;
    }
  }
}

void Shader::unreserve()
{
  thr::wait_privileged(delete_program, m_id, m_vsid, m_gsid, m_fsid);

  m_fshader.assign("");
  m_vshader.assign("");
  m_attributes.clear();
  m_attributes_common.clear();
  m_uniforms.clear();
  m_uniforms_common.clear();
  m_version = m_id = m_vsid = m_gsid = m_fsid = 0;
}

Shader::container_type Shader::createImplementation(const fs::path &pfname, const ShaderLoader &loader)
{
  return container_type(new Shader(pfname, loader));
}

std::ostream& gfx::operator<<(std::ostream &lhs, const Shader &rhs)
{
  lhs << "== Vertex ==\n" << rhs.m_vshader;
  
  if(0 < rhs.m_gshader.length())
  {
    lhs << "\n== Geometry ==\n" << rhs.m_gshader;
  }

  return lhs << "\n== Fragment ==\n" << rhs.m_fshader << "\n== id: " << rhs.m_id << " ==";
}
