#ifndef GFX_LIGHT_DIR_HPP
#define GFX_LIGHT_DIR_HPP

#include "math/vec.hpp"

namespace gfx
{
	/** \brief Directional light.
	 *
	 * The directional light also handles ambient lighting.
	 */
	class LightDirectional
	{
		private:
			/** Ambient light. */
			math::vec3f m_ambient;

			/** Diffuse light. */
			math::vec3f m_diffuse;

			/** Direction. */
			math::vec3f m_direction;

		public:
			/** \brief Get the ambient color vector.
			 *
			 * \return Color vector.
			 */
			inline const math::vec3f& getAmbient() const
			{
				return m_ambient;
			}

			/** \brief Get the diffuse color vector.
			 *
			 * \return Color vector.
			 */
			inline const math::vec3f& getDiffuse() const
			{
				return m_diffuse;
			}

			/** \brief Get the direction vector.
			 *
			 * \return Direction vector.
			 */
			inline const math::vec3f& getDir() const
			{
				return m_direction;
			}

			/** \brief Set the ambient color.
			 *
			 * \param pr Red.
			 * \param pg Green.
			 * \param pb Blue.
			 */
			inline void setAmbient(float pr, float pg, float pb)
			{
				m_ambient = math::vec3f(pr, pg, pb);
			}

			/** \brief Set the ambient color.
			 *
			 * \param op New color.
			 */
			inline void setAmbient(const math::vec3f &op)
			{
				m_ambient = op;
			}

			/** \brief Set the diffuse color.
			 *
			 * \param pr Red.
			 * \param pg Green.
			 * \param pb Blue.
			 */
			inline void setDiffuse(float pr, float pg, float pb)
			{
				m_diffuse = math::vec3f(pr, pg, pb);
			}

			/** \brief Set the diffuse color.
			 *
			 * \param op New color.
			 */
			inline void setDiffuse(const math::vec3f &op)
			{
				m_diffuse = op;
			}

			/** \brief Set the direction.
			 *
			 * \param px X coordinate.
			 * \param py Y coordinate.
			 * \param pz Z coordinate.
			 */
			inline void setDir(float px, float py, float pz)
			{
				m_direction = math::vec3f(px, py, pz);
			}

			/** \brief Set the direction.
			 *
			 * \param op New direction.
			 */
			inline void setDir(const math::vec3f &op)
			{
				m_direction = op;
			}

		public:
			/** \brief Empty constructor.
			 *
			 * Default values are somewhat remiscenent of OpenGL examples.
			 */
			inline LightDirectional()
			{
				this->setAmbient(0.10f, 0.10f, 0.125f);
				this->setDiffuse(1.0f, 0.95f, 0.9f);
				this->setDir(0.0f, 1.0f, 0.0f);
			}

			/** \brief Constructor.
			 *
			 * \param amb Ambient color vector.
			 * \param diff Diffuse color vector.
			 * \param dir Direction vector.
			 */
			inline LightDirectional(const math::vec3f &amb, const math::vec3f &diff,
					const math::vec3f &dir)
			{
				this->setAmbient(amb);
				this->setDiffuse(diff);
				this->setDir(dir);
			}

			/** \brief Constructor.
			 *
			 * \param ar Ambient R.
			 * \param ag Ambient G.
			 * \param ab Ambient B.
			 * \param dr Diffuse R.
			 * \param dg Diffuse G.
			 * \param db Diffuse B.
			 * \param dir Direction vector.
			 */
			inline LightDirectional(float ar, float ag, float ab, float dr, float dg, float db,
					const math::vec3f &dir)
			{
				this->setAmbient(ar, ag, ab);
				this->setDiffuse(dr, dg, db);
				this->setDir(dir);
			}

			/** \brief Constructor.
			 *
			 * \param ar Ambient R.
			 * \param ag Ambient G.
			 * \param ab Ambient B.
			 * \param dr Diffuse R.
			 * \param dg Diffuse G.
			 * \param db Diffuse B.
			 * \param px Direction X.
			 * \param py Direction Y.
			 * \param pz Direction Z.
			 */
			inline LightDirectional(float ar, float ag, float ab, float dr, float dg, float db,
					float px, float py, float pz)
			{
				this->setAmbient(ar, ag, ab);
				this->setDiffuse(dr, dg, db);
				this->setDir(px, py, pz);
			}
	};
}

#endif
