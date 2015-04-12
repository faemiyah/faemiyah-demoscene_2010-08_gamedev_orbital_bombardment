#ifndef GFX_ENTITY_OBJECT_HPP
#define GFX_ENTITY_OBJECT_HPP

#include "gfx/entity.hpp"
#include "gfx/mesh.hpp"

#include <vector>

namespace gfx
{
  class EntityObject;
  class Mesh;
  class Shader;

  /** Convenience typedef. */
  typedef boost::shared_ptr<EntityObject> EntityObjectSptr;

  /** \brief Normal object.
   *
   * Normal objects are essentially recursive trees of themselves. An object contains any number of meshes and
   * subsequently any number of recursive objects. The recursive object references are transformed downward
   * from the original orientation of the roof entity.
   */
  class EntityObject : public Entity
  {
    protected:
      /** Meshes.*/
      std::vector<const Mesh*> m_meshes;

      /** Recursive objects. */
      std::vector<EntityObjectSptr> m_objects;

      /** Do we want to draw this or not. */
      bool m_draw_disabled;

    public:
      /** \brief Construtor. */
      EntityObject();

      /** \brief Constructor.
       *
       * \param ppos Position.
       * \param prot Rotation.
       * \param pmesh Mesh to use.
       */
      EntityObject(const math::vec3d &ppos, const math::vec3d &prot) :
        Entity(ppos, prot),
        m_draw_disabled(false) { }

      /** \brief Constructor.
       *
       * \param ppos Position.
       * \param prot Rotation.
       * \param pmesh Mesh to use.
       */
      EntityObject(const math::vec3d &ppos, const math::vec3d &prot, const Mesh *pmesh);

      /** \brief Constructor.
       *
       * \param ppos Position.
       * \param prot Rotation.
       * \param op Meshes to use.
       */
      EntityObject(const math::vec3d &ppos, const math::vec3d &prot, const Mesh::container_type &op);

      /** \brief Constructor.
       *
       * \param pmesh mesh to use.
       */
      EntityObject(const Mesh *pmesh);

      /** \brief Constructor.
       *
       * \param op Meshes to use.
       */
      EntityObject(const Mesh::container_type &op);

      /** \brief Destructor.
       *
       * Although the meshes are pointers from other sources and are not destroyed when the object is
       * destroyed, all recursive objects are.
       */
      virtual ~EntityObject() { }

    public:
      /** \brief Add a mesh to this.
       *
       * \param op Mesh to add.
       */
      void addMesh(const Mesh *op);

      /** \brief Add a meshes to this.
       *
       * \param pmesh Meshes to add.
       */
      void addMesh(const Mesh::container_type &op);

      /** \brief Add a sub-object.
       *
       * \param pfname Canonized filename to search the object on.
       */
      void addSub(const std::string &pfname);

      /** \brief Add a sub-object.
       *
       * Will assume control and destruction of passed object.
       *
       * \param obj Object.
       */
      void addSub(EntityObject *obj);

      /** \brief Load a direction lookat for a sub-mesh.
       *
       * \param idx Index.
       * \param fw Forward.
       * \param up Up vector in object space.
       */
      void subDir(unsigned idx, const math::vec3f &fw, const math::vec3f &up);

      /** \brief Load an identity to a sub-mesh.
       *
       * \param idx Index.
       */
      void subLoadIdentity(unsigned idx);

      /** \brief Load a recursive lookat matrix for a sub-mesh.
       *
       * \param idx Index.
       * \param tgt Position to look at.
       * \param up Up vector in object space.
       */
      void subLookAt(unsigned idx, const math::vec3f &tgt, const math::vec3f &up);

    public:
      /** \brief Draw this object.
       *
       * Also draws all other objects recursively.
       *
       * \param psh Shader to use.
       * \param pmat Modelview matrix to use.
       */
      virtual void draw(const Shader &psh, const math::mat4f &pmat) const;

    protected:
      /** \brief Prepares the orientation for this object
       *
       * The default implementation just multiplies the object matrix into the
       * matrix stack.
       *
       * \param pmat Matrix currently on stack.
       */
      virtual math::mat4f orient(const math::mat4f &pmat) const;

    public:
      /** \brief Add a sub-object.
       *
       * \param pfname Canonized filename to search the object on.
       */
      void addSub(const char *pfname)
      {
        this->addSub(std::string(pfname));
      }

      /** \brief Adjust visibility of the object - show.
       */
      void enableDraw()
      {
        m_draw_disabled = false;
      }

      /** \brief Adjust visibility of the object - hide.
       */
      void disableDraw()
      {
        m_draw_disabled = true;
      }

      /** \brief Query visibility of the object.
       *
       * \return Status. true if drawing is disabled, false if not.
       */
      bool isDrawDisabled()
      {
        return m_draw_disabled;
      }

      /** \brief Load a direction lookat for a sub-mesh.
       *
       * Up is positive y in object space.
       *
       * \param idx Index.
       * \param fw Forward.
       */
      void subDir(unsigned idx, const math::vec3f &fw)
      {
        this->subDir(idx, fw, math::vec3f(0.0f, 1.0f, 0.0f));
      }

      /** \brief Load a recursive lookat matrix for a sub-mesh.
       *
       * Up is positive y in object space.
       *
       * \param idx Index.
       * \param tgt Position to look at.
       */
      void subLookAt(unsigned idx, const math::vec3f &tgt)
      {
        this->subLookAt(idx, tgt, math::vec3f(0.0f, 1.0f, 0.0f));
      }
  };
}

#endif
