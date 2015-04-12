#ifndef GFX_SHADER_HPP
#define GFX_SHADER_HPP

#include "data/store.hpp"
#include "gfx/attribute.hpp"
#include "gfx/shader_loader.hpp"
#include "gfx/uniform.hpp"

#include <boost/assert.hpp>
#include <boost/filesystem.hpp>

#include <map>
#include <string>

namespace gfx
{
  /** \brief Representation of a shader.
   *
   * Shaders contain both a vertex program and a fragment program. If you want to reuse programs, please use a
   * bit of copypasta.
   *
   * Several well-known vertex attributes are stored in the shader should they exist, the rest are retrievable
   * via the attribute map. Same for uniforms. Requesting a well-known attribute or uniform that is not
   * specified within the shader results in unspecified behavior.
   */
  class Shader :
    public data::Storable<Shader, ShaderLoader>
  {
    public:
      /** Enum for distinct common attribute and uniform names for faster access. */
      enum ShaderVariableName
      {
        /** Reserved shader name. */
        SHADER_VARIABLE_NAME_NONE,

        /** Well-known attribute name. */
        ATTRIBUTE_COLOR,

        /** Well-known attribute name. */
        ATTRIBUTE_NORMAL,

        /** Well-known attribute name. */
        ATTRIBUTE_REFERENCE,

        /** Well-known attribute name. */
        ATTRIBUTE_TEXCOORD,

        /** Well-known attribute name. */
        ATTRIBUTE_VERTEX,

        /** Well-known attribute name. */
        ATTRIBUTE_WEIGHT,

        /** Well-known uniform name. */
        UNIFORM_LIGHT_AMBIENT,

        /** Well-known uniform name. */
        UNIFORM_LIGHT_DIFFUSE,

        /** Well-known uniform name. */
        UNIFORM_LIGHT_DIR,

        /** Well-known uniform name. */
        UNIFORM_LIGHT_POS,

        /** Well-known uniform name. */
        UNIFORM_MODELVIEW,

        /** Well-known uniform name. */
        UNIFORM_NORMALMAP,

        /** Well-known uniform name. */
        UNIFORM_PROJECTION,

        /** Well-known uniform name. */
        UNIFORM_TEX0,

        /** Well-known uniform name. */
        UNIFORM_TEX1,

        /** Well-known uniform name. */
        UNIFORM_TEX2,

        /** Well-known uniform name. */
        UNIFORM_TEX3,

        /** Well-known uniform name. */
        UNIFORM_TEX4,

        /** Well-known uniform name. */
        UNIFORM_TEX5,

        /** Well-known uniform name. */
        UNIFORM_TEX6,

        /** Well-known uniform name. */
        UNIFORM_TEX7,

        /** Well-known uniform name. */
        UNIFORM_TEXPARAMS,

        /** Well-known uniform name. */
        UNIFORM_TRANSFORM,
      };

      /** Convenience typeded. */
      typedef std::map<std::string, Attribute> AttributeMap;

      /** Convenience typeded. */
      typedef std::map<std::string, Uniform> UniformMap;

      /** Convenience typedef. */
      typedef std::map<ShaderVariableName, Attribute> AttributeMapFast;

      /** Convenience typedef. */
      typedef std::map<ShaderVariableName, Uniform> UniformMapFast;

    private:
      /** Currently active shader. */
      static const Shader *g_current_shader;

    private:
      /** Version. */
      unsigned m_version;

      /** Vertex program. */
      std::string m_vshader;

      /** Geometry program. */
      std::string m_gshader;

      /** Fragment program. */
      std::string m_fshader;

      /** Map of attributes. */
      AttributeMap m_attributes;

      /** Map of uniforms. */
      UniformMap m_uniforms;

      /** Common attributes (for fast access). */
      AttributeMapFast m_attributes_common;

      /** Common uniforms (for fast access). */
      UniformMapFast m_uniforms_common;

      /** Vertex shader id. */
      unsigned m_vsid;

      /** Geometry shader id. */
      unsigned m_gsid;

      /** Fragment shader id. */
      unsigned m_fsid;

      /** Shader program id. */
      unsigned m_id;

    public:
      /** \brief Get common attribute.
       *
       * Asserts if attribute does not exist. Results undefined on release build.
       *
       * \param op Common attribute id.
       */
      const Attribute& getAttrCommon(ShaderVariableName op) const
      {
        BOOST_ASSERT(this->hasAttribute(op));
        return m_attributes_common.find(op)->second;
      }

      /** \brief Get common uniform.
       *
       * Asserts if uniform does not exist. Results undefined on release build.
       *
       * \param op Common uniform id.
       */
      const Uniform& getUniformCommon(ShaderVariableName op) const
      {
        BOOST_ASSERT(this->hasUniform(op));
        return m_uniforms_common.find(op)->second;
      }

    public:
      /** \brief Get internal attribute.
       *
       * \return Vertex attribute reference.
       */
      const Attribute& getAttrColor() const
      {
        return this->getAttrCommon(ATTRIBUTE_COLOR);
      }

      /** \brief Get internal attribute.
       *
       * \return Vertex attribute reference.
       */
      const Attribute& getAttrNormal() const
      {
        return this->getAttrCommon(ATTRIBUTE_NORMAL);
      }

      /** \brief Get internal attribute.
       *
       * \return Vertex attribute reference.
       */
      const Attribute& getAttrReference() const
      {
        return this->getAttrCommon(ATTRIBUTE_REFERENCE);
      }

      /** \brief Get internal attribute.
       *
       * \return Vertex attribute reference.
       */
      const Attribute& getAttrTexCoord() const
      {
        return this->getAttrCommon(ATTRIBUTE_TEXCOORD);
      }

      /** \brief Get internal attribute.
       *
       * \return Vertex attribute reference.
       */
      const Attribute& getAttrVertex() const
      {
        return this->getAttrCommon(ATTRIBUTE_VERTEX);
      }

      /** \brief Get internal attribute.
       *
       * \return Vertex attribute reference.
       */
      const Attribute& getAttrWeight() const
      {
        return this->getAttrCommon(ATTRIBUTE_WEIGHT);
      }

      /** \brief Get internal attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformLightAmbient() const
      {
        return this->getUniformCommon(UNIFORM_LIGHT_AMBIENT);
      }

      /** \brief Get internal attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformLightDiffuse() const
      {
        return this->getUniformCommon(UNIFORM_LIGHT_DIFFUSE);
      }

      /** \brief Get internal attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformLightDir() const
      {
        return this->getUniformCommon(UNIFORM_LIGHT_DIR);
      }

      /** \brief Get internal attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformLightPos() const
      {
        return this->getUniformCommon(UNIFORM_LIGHT_POS);
      }

      /** \brief Get internal attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformModelview() const
      {
        return this->getUniformCommon(UNIFORM_MODELVIEW);
      }

      /** \brief Get internal attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformNormalmap() const
      {
        return this->getUniformCommon(UNIFORM_NORMALMAP);
      }

      /** \brief Get internal attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformProjection() const
      {
        return this->getUniformCommon(UNIFORM_PROJECTION);
      }

      /** \brief Get internal attribute.
       *
       * Returns the first texture attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformTex() const
      {
        return this->getUniformCommon(UNIFORM_TEX0);
      }

      /** \brief Get internal attribute.
       *
       * \param idx Texture attribute index.
       * \return Uniform reference.
       */
      const Uniform& getUniformTex(unsigned idx) const
      {
        unsigned name = static_cast<ShaderVariableName>(static_cast<unsigned>(UNIFORM_TEX0) + idx);

        // 8 texture units supported
        BOOST_ASSERT(static_cast<unsigned>(UNIFORM_TEX0) <= name);
        BOOST_ASSERT(static_cast<unsigned>(UNIFORM_TEX7) >= name);

        return this->getUniformCommon(static_cast<ShaderVariableName>(name));
      }

      /** \brief Get internal attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformTexParams() const
      {
        return this->getUniformCommon(UNIFORM_TEXPARAMS);
      }

      /** \brief Get internal attribute.
       *
       * \return Uniform reference.
       */
      const Uniform& getUniformTransform() const
      {
        return this->getUniformCommon(UNIFORM_TRANSFORM);
      }

    public:
      /** \brief Constructor.
       *
       * \param pfname Filename to open.
       * \param loader Loader settings.
       */
      Shader(const boost::filesystem::path &pfname, const ShaderLoader &loader = ShaderLoader());

      /** \brief Destructor. */
      ~Shader();

    public:
      /** \brief Bind this shader for use.
       *
       * \return True if bound, false if already bound.
       */
      bool bind() const;

      /** \brief Compile this shader.
       *
       * Despite being public, no-one else but the shader should ever call'
       * this method.
       */
      void compile();

      /** \brief Get a named attribute.
       *
       * \param name Name of the attribute.
       * \return Pointer to the named attribute or NULL.
       */
      const Attribute* getAttribute(const std::string &name) const;

      /** \brief Get a named uniform.
       *
       * \param name Name of the uniform.
       * \return Pointer to the named uniform or NULL.
       */
      const Uniform* getUniform(const std::string &name) const;

      /** \brief Load shader from disk.
       *
       * Will also compile the shader. Due to this, shaders should not be
       * reserved prior to initializing the OpenGL context.
       *
       * \param pfname Filename to load from.
       * \param loader Loader settings.
       */
      void load(const boost::filesystem::path &pfname, const ShaderLoader &loader);

    private:
      /** \brief Sanity check for having an attribute.
       *
       * Should only be used to check whether or not common attributes exist.
       *
       * \param op Attribute enum.
       */
      bool hasAttribute(ShaderVariableName op) const;

      /** \brief Sanity check for having an attribute.
       *
       * Should only be used to check whether or not common attributes exist.
       *
       * \param op Attribute enum.
       */
      bool hasUniform(ShaderVariableName op) const;

      /** \brief Unreserve. */
      void unreserve();

      /** \brief Replace an attribute id within the shader.
       *
       * \param op Attribute to replace.
       */
      void updateAttribute(Attribute &op);

      /** \brief Replace an uniform id within the shader.
       *
       * \param op Uniform to replace.
       */
      void updateUniform(Uniform &op);

    public:
      /** \brief Create implementation.
       *
       * Despite being public, not to be called by user.
       *
       * \param pfname File to load.
       * \param loader Loader settings.
       * \return Container for all created instances.
       */
      static container_type createImplementation(const boost::filesystem::path &pfname,
          const ShaderLoader &loader = ShaderLoader());

    public:
      /** \brief Get the currently active shader.
       *
       * \return Currently active shader.
       */
      static const Shader* getCurrent()
      {
        return g_current_shader;
      }

    public:
      /** \cond */
      friend std::ostream& operator<<(std::ostream &lhs, const Shader &rhs);
      /** \endcond */
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Shader> ShaderSptr;

  /** \brief Output shader to a stream.
   *
   * \param lhs Left-hand-side operand.
   * \param rhs Right-hand-side operand.
   * \return Stream after input.
   */
  std::ostream& operator<<(std::ostream &lhs, const Shader &rhs);
}

#endif
