#ifndef GFX_MESH_HPP
#define GFX_MESH_HPP

#include "data/registry.hpp"
#include "data/store.hpp"
#include "gfx/lod.hpp"
#include "gfx/mesh_loader.hpp"
#include "gfx/texture_2d.hpp"
#include "gfx/texture_3d.hpp"
#include "math/mat.hpp"

namespace gfx
{
  class Shader;

  /** \brief A basic mesh class.
   *
   * Each mesh is either static or animated. The base class contains data
   * relevant to both of the variants.
   *
   * Note that if some of the data is not present, the mesh is just plain empty.
   */
  class Mesh :
    public data::Storable<Mesh, MeshLoader>,
    public data::Registry<Mesh, MeshLoader>
  {
    protected:
      /** Attribute data array. */
      std::vector<Color> m_color;

      /** Attribute data array. */
      std::vector<math::vec3f> m_normal;

      /** Attribute data array. */
      std::vector<math::vec4u> m_reference;

      /** Attribute data array. */
      std::vector<math::vec2f> m_texcoord;

      /** Attribute data array. */
      std::vector<math::vec3f> m_vertex;

      /** Attribute data array. */
      std::vector<math::vec4f> m_weight;

      /** Face data. */
      Lod m_lod;

      /** Element buffer. */
      BufferElem m_elem;

      /** Textures in this mesh. */
      std::vector<const Texture2D*> m_textures;

      /** Volumes in this mesh. */
      std::vector<const Texture3D*> m_volumes;

      /** \brief Offset of this mesh.
       *
       * Note that for most purposes, the offset will be zero. It is meant as
       * the mount point within parent objects, if applicable. The parent
       * objects should just plain read this offset as their position without
       * changing it.
       */
      math::vec3f m_offset;

      /** \brief Private name.
       *
       * This is different from the name associated in the storage. A mesh in storage may have a set of
       * elements and sub-meshes all with their distinct names.
       */
      std::string m_name;

      /** Is this a sub-element (as opposed to main element). */
      bool m_sub_element;

    public:
      /** \brief Accessor.
       *
       * \return Name.
       */
      const std::string& getName() const
      {
        return m_name;
      }

      /** \brief Accessor.
       *
       * \return Offset.
       */
      const math::vec3f& getOffset() const
      {
        return m_offset;
      }

      /** \brief Setter.
       *
       * \param op New offset.
       */
      void setOffset(const math::vec3f &op)
      {
        m_offset = op;
      }

      /** \brief Accessor.
       *
       * \return True if this is a sub-element.
       */
      bool isSubElement() const
      {
        return m_sub_element;
      }

    public:
      /** \brief Empty constructor. */
      Mesh() :
        m_offset(0.0f, 0.0f, 0.0f) { }

      /** \brief Destructor. */
      virtual ~Mesh() { }

    protected:
      /** \brief Calculate normals.
       *
       * Calculates normals from vertex and face data.
       */
      void calcNormals();

      /** \brief Load this from a file.
       *
       * Unknown elements will be passed into a specific interpretation
       * function that is implemented in inheriting classes.
       *
       * \param pfname Filename to load.
       * \param root Property tree root.
       * \param loader Loader settings.
       */
      void load(const boost::filesystem::path &pfname, const boost::property_tree::ptree &root,
          const MeshLoader &loader);

    public:
      /** \brief Add image from file.
       *
       * May load the texture in the process.
       *
       * Throws an exception on failure.
       *
       * \param type Texture type.
       * \param pfname Filename to load from.
       * \param loader Image loading parameters.
       */
      void addTextureFile(const std::string &type, const boost::filesystem::path &pfname,
          const ImageLoader &loader = ImageLoader());

      /** \brief Get the boundary of this mesh.
       *
       * \return Boundary calculated from this.
       */
      math::rect3f getBoundary() const;

      /** \brief Scale this object.
       *
       * Multiplies all vertices (but nothing else) with the given values.
       *
       * \param svec Scale vector.
       */
      void scale(const math::vec3f &svec);

      /** \brief Translate this object.
       *
       * Translates all vertices (but nothing else) with the given values.
       *
       * \param tvec Translation vector.
       */
      void translate(const math::vec3f &tvec);

    public:
      /** \brief Get the face normal for a given face.
       *
       * \param tri Triangle to use.
       * \return Resulting normal.
       */
      const math::vec3f calcNormal(const Triangle &tri)
      {
        const math::vec3f &va = m_vertex[tri.a()];

        return math::cross(m_vertex[tri.b()] - va, m_vertex[tri.c()] - va);
      }

      /** \brief Scale this object.
       *
       * Multiplies all vertices (but nothing else) with the given value.
       *
       * \param op Scale parameter.
       */
      void scale(float op)
      {
        this->scale(math::vec3f(op, op, op));
      }

    public:
      /** \brief Add one texture into this.
       *
       * Virtual function to handle adding a new texture.
       *
       * The id contains the type or detailed name of the texture that was either loaded from an XML file or
       * specified explicitly. Its processing may be omitted at will. The default loader accepts strings
       * starting with "texture" or "volume". Further implementations may also use such strings as "normalmap".
       *
       * The parameter is passed as void pointer due to type problems for the Texture inheritance. The default
       * implementation simply casts the pointer and pushes it into the texture or volume array of this.
       *
       * \param id Identifier.
       * \param tex Texture.
       */
      virtual void addTexture(const std::string &id, const void *tex);

      /** \brief Compile this to be ready for drawing.
       */
      virtual void compile() = 0;

      /** \brief Draw this.
       *
       * It is expected that the camera matrix be loaded into the OpenGL
       * stack prior to calling this method.
       *
       * \param psh Shader to use.
       * \param pmat Modelview matrix to use.
       */
      virtual void draw(const Shader &psh, const math::mat4f &op) const = 0;

      /** \brief Upload all elements in the mesh to the GPU.
       *
       * Despite being public, this function is not intended to be called by the user.
       */
      virtual void upload() = 0;

    protected:
      /** \brief Read one vertex from a property tree.
       *
       * Inheriting classes should call this base method.
       *
       * \param subtree Tree to read from.
       */
      virtual void readVertex(const boost::property_tree::ptree &subtree);

      /** \brief Free all resources required by the font.
       * 
       * Should be done before deinitializing OpenGL.
       *
       * Inheriting classes should call this base method.
       */
      virtual void unreserve();

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
          const MeshLoader &loader);
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Mesh> MeshSptr;
}

#endif
