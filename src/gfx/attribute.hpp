#ifndef GFX_ATTRIBUTE_HPP
#define GFX_ATTRIBUTE_HPP

#include "gfx/generic.hpp"

namespace gfx
{
  /** Turn vertex attribute array off.
   *
   * \param op ID to disable.
   */
  extern void vertex_attribute_array_disable(GLuint op);

  /** Turn vertex attribute array on.
   *
   * \param op ID to enable.
   */
  extern void vertex_attribute_array_enable(GLuint op);

  /** \brief Per-vertex attribute.
   *
   * Note that attributes do not know their names. They exist only as
   * referenced by shaders.
   *
   * Only the id of the attribute can be modified after it's creation, but
   * should not be touched unless the accompanying shader is recompiled.
   */
  struct Attribute
  {
    protected:
      /** Invalid value. */
      static const GLuint INVALID_GLUINT_ID = 0xFFFFFFFF;

    protected:
      /** Name. */
      std::string m_name;

      /** Type. */
      std::string m_type;

      /** Attribute id. */
      GLuint m_id;

    public:
      /** \brief Accessor.
       *
       * \return Type of this.
       */
      const std::string& getType() const
      {
        return m_type;
      }

      /** \brief Accessor.
       *
       * \return Name of this.
       */
      const std::string& getName() const
      {
        return m_name;
      }

      /** \brief Accessor.
       *
       * \return Id of this.
       */
      GLuint id() const
      {
        return m_id;
      }

      /** \brief Setter.
       *
       * \param op New id.
       */
      void setId(GLuint op)
      {
        m_id = op;
      }

      /** Assignment.
       *
       * \param rhs Right-hand-size operand.
       */
      const Attribute& operator=(const Attribute &rhs)
      {
        m_name = rhs.m_name;
        m_type = rhs.m_type;
        m_id = rhs.m_id;
        return *this;
      }

    public:
      /** \brief Empty constructor. */
      Attribute()
#if defined(DEBUG)
        : m_id(INVALID_GLUINT_ID)
#endif
        { }

      /** \brief Constructor.
       *
       * \param src Previous incarnation.
       * \param pid New id.
       */
      Attribute(const Attribute &src, GLuint pid) :
        m_name(src.m_name),
        m_type(src.m_type),
        m_id(pid) { }

      /** \brief Constructor.
       *
       * \param pname Name
       * \param ptype Type.
       */
      Attribute(const std::string &pname, const std::string &ptype) :
        m_name(pname),
        m_type(ptype),
        m_id(INVALID_GLUINT_ID) { }

      /** \brief Constructor.
       *
       * \param pname Name.
       * \param ptype Type.
       * \param pid Id.
       */
      Attribute(const std::string &pname, const std::string &ptype, GLuint pid) :
        m_name(pname),
        m_type(ptype),
        m_id(pid) { }

    public:
      /** \brief Enables this vertex attribute array.
       */
      void enable() const
      {
        vertex_attribute_array_enable(m_id);
      }

    public:
      /** \brief Output a surface into a stream.
       *
       * \param lhs Left-hand-side operand.
       * \param rhs Right-hand-side operand.
       * \return Modified stream.
       */
      friend std::ostream& operator<<(std::ostream &lhs, const Attribute &rhs)
      {
        return lhs << "Attribute " << rhs.getType().c_str() << " \"" << rhs.getName().c_str() << "\": " <<
          rhs.id();
      }
  };
}

#endif
