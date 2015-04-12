#ifndef GFX_UNIFORM_HPP
#define GFX_UNIFORM_HPP

#include "gfx/attribute.hpp"
#include "math/mat.hpp"

namespace gfx
{
  /** \brief Uniform shader variable.
   *
   * Uniforms are not type-checked due to the fract that OpenGL allows them to
   * be specified as types different from what they were specified in the
   * shader.
   */
  struct Uniform : public Attribute
  {
    public:
      /** \brief Empty constructor. */
      Uniform() { }

      /** \brief Constructor.
       *
       * \param src Previous incarnation.
       * \param pid New id.
       */
      Uniform(const Uniform &src, GLuint pid) :
        Attribute(src, pid) { }

      /** \brief Constructor.
       *
       * \param pname Name
       * \param ptype Type.
       */
      Uniform(const std::string &pname, const std::string &ptype) :
        Attribute(pname, ptype) { }

    public:
      /** \param Supply uniform data.
       *
       * \param p1 First parameter.
       */
      void update(int p1) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform1i(m_id, p1);
      }

      /** \param Supply uniform data.
       *
       * \param p1 First parameter.
       */
      void update(float p1) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform1f(m_id, p1);
      }

      /** \param Supply uniform data.
       *
       * \param p1 First parameter.
       * \param p2 Second parameter.
       */
      void update(int p1, int p2) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform2i(m_id, p1, p2);
      }

      /** \param Supply uniform data.
       *
       * \param p1 First parameter.
       * \param p2 Second parameter.
       */
      void update(float p1, float p2) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform2f(m_id, p1, p2);
      }

      /** \param Supply uniform data.
       *
       * \param p1 First parameter.
       * \param p2 Second parameter.
       * \param p3 Third parameter.
       */
      void update(int p1, int p2, int p3) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform3i(m_id, p1, p2, p3);
      }

      /** \param Supply uniform data.
       *
       * \param p1 First parameter.
       * \param p2 Second parameter.
       * \param p3 Third parameter.
       */
      void update(float p1, float p2, float p3) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform3f(m_id, p1, p2, p3);
      }

      /** \param Supply uniform data.
       *
       * \param p1 First parameter.
       * \param p2 Second parameter.
       * \param p3 Third parameter.
       * \param p4 Fourth parameter.
       */
      void update(int p1, int p2, int p3, int p4) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform4i(m_id, p1, p2, p3, p4);
      }

      /** \param Supply uniform data.
       *
       * \param p1 First parameter.
       * \param p2 Second parameter.
       * \param p3 Third parameter.
       * \param p4 Fourth parameter.
       */
      void update(float p1, float p2, float p3, float p4) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform4f(m_id, p1, p2, p3, p4);
      }

      /** \param Supply uniform data.
       *
       * \param p1 First two parameters.
       * \param p2 Latter two parameters.
       */
      void update(const math::vec2f &p1, const math::vec2f &p2) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform4f(m_id, p1.x(), p1.y(), p2.x(), p2.y());
      }

      /** \param Update one vector.
       *
       * \param op Vector to update.
       */
      void update(const math::vec2f &op) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform2fv(m_id, 1, op.m_array);
      }

      /** \param Update one vector.
       *
       * \param op Vector to update.
       */
      void update(const math::vec2i &op) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform2iv(m_id, 1, reinterpret_cast<const GLint*>(op.m_array));
      }

      /** \param Update one vector.
       *
       * \param op Vector to update.
       */
      void update(const math::vec3f &op) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform3fv(m_id, 1, op.m_array);
      }

      /** \param Update one vector.
       *
       * \param op Vector to update.
       */
      void update(const math::vec3i &op) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform3iv(m_id, 1, reinterpret_cast<const GLint*>(op.m_array));
      }

      /** \param Update one vector.
       *
       * \param op Vector to update.
       */
      void update(const math::vec4f &op) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform4fv(m_id, 1, op.m_array);
      }

      /** \param Update one vector.
       *
       * \param op Vector to update.
       */
      void update(const math::vec4i &op) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform4iv(m_id, 1, reinterpret_cast<const GLint*>(op.m_array));
      }

      /** \brief Update vector array.
       *
       * \param count Number of vectors to update.
       * \param array Array of vectors.
       */
      void update(unsigned count, const math::vec2f *array) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform2fv(m_id, count, array[0].m_array);
      }

      /** \brief Update vector array.
       *
       * \param count Number of vectors to update.
       * \param array Array of vectors.
       */
      void update(unsigned count, const math::vec2i *array) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform2iv(m_id, count, reinterpret_cast<const GLint*>(array[0].m_array));
      }

      /** \brief Update vector array.
       *
       * \param count Number of vectors to update.
       * \param array Array of vectors.
       */
      void update(unsigned count, const math::vec3f *array) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform3fv(m_id, count, array[0].m_array);
      }

      /** \brief Update vector array.
       *
       * \param count Number of vectors to update.
       * \param array Array of vectors.
       */
      void update(unsigned count, const math::vec3i *array) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform3iv(m_id, count, reinterpret_cast<const GLint*>(array[0].m_array));
      }

      /** \brief Update vector array.
       *
       * \param count Number of vectors to update.
       * \param array Array of vectors.
       */
      void update(unsigned count, const math::vec4f *array) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform4fv(m_id, count, array[0].m_array);
      }

      /** \brief Update vector array.
       *
       * \param count Number of vectors to update.
       * \param array Array of vectors.
       */
      void update(unsigned count, const math::vec4i *array) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniform4iv(m_id, count, reinterpret_cast<const GLint*>(array[0].m_array));
      }

      /** \brief Update one matrix.
       *
       * \param op A matrix.
       * \param transpose Set to true for transpose.
       */
      void update(const math::mat2f &op, bool transpose = false) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniformMatrix2fv(m_id, 1, transpose, op.m_array);
      }

      /** \brief Update one matrix.
       *
       * \param op A matrix.
       * \param transpose Set to true for transpose.
       */
      void update(const math::mat3f &op, bool transpose = false) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniformMatrix3fv(m_id, 1, transpose, op.m_array);
      }

      /** \brief Update one matrix.
       *
       * \param op A matrix.
       * \param transpose Set to true for transpose.
       */
      void update(const math::mat4f &op, bool transpose = false) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniformMatrix4fv(m_id, 1, transpose, op.m_array);
      }

      /** \brief Update matrix array.
       *
       * \param count Number of matrices to update.
       * \param array Array of matrices.
       * \param transpose Set to true for transpose.
       */
      void update(unsigned count, const math::mat2f *array, bool transpose = false) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniformMatrix2fv(m_id, count, transpose, array[0].m_array);
      }

      /** \brief Update matrix array.
       *
       * \param count Number of matrices to update.
       * \param array Array of matrices.
       * \param transpose Set to true for transpose.
       */
      void update(unsigned count, const math::mat3f *array, bool transpose = false) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniformMatrix3fv(m_id, count, transpose, array[0].m_array);
      }

      /** \brief Update matrix array.
       *
       * \param count Number of matrices to update.
       * \param array Array of matrices.
       * \param transpose Set to true for transpose.
       */
      void update(unsigned count, const math::mat4f *array, bool transpose = false) const
      {
        BOOST_ASSERT(INVALID_GLUINT_ID != m_id);
        glUniformMatrix4fv(m_id, count, transpose, array[0].m_array);
      }

    public:
      /** \brief Output a surface into a stream.
       *
       * \param lhs Left-hand-side operand.
       * \param rhs Right-hand-side operand.
       * \return Modified stream.
       */
      friend std::ostream& operator<<(std::ostream &lhs, const Uniform &rhs)
      {
        return lhs << "Uniform " << rhs.getType().c_str() << " \"" << rhs.getName().c_str() << "\": " <<
          rhs.id();
      }
  };
}

#endif
