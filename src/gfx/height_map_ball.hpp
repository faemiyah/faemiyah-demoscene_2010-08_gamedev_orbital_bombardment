#ifndef GFX_HEIGHT_MAP_BALL_HPP
#define GFX_HEIGHT_MAP_BALL_HPP

#include "gfx/image.hpp"

namespace gfx
{
  class HeightMapBall
  {
    private:
      /** Back image. */
      ImageGray8 m_bk;

      /** Down image. */
      ImageGray8 m_dn;

      /** Forward image. */
      ImageGray8 m_fw;

      /** Left image. */
      ImageGray8 m_lt;

      /** Right image. */
      ImageGray8 m_rt;

      /** Up image. */
      ImageGray8 m_up;

      /** Height field low point. */
      float m_min;

      /** Height field high point. */
      float m_max;

    public:
      /** \brief Ball heightmap constructor.
       *
       * \param pbk Heightmap back.
       * \param pdn Heightmap down.
       * \param pfw Heightmap forward.
       * \param plt Heightmap left.
       * \param prt Heightmap right.
       * \param pup Heightmap up.
       */
      HeightMapBall(const std::string &pbk, const std::string &pdn,
          const std::string &pfw, const std::string &plt,
          const std::string &prt, const std::string &pup,
          float pmin, float pmax);

      /** \brief Destructor. */
      virtual ~HeightMapBall() { }

    public:
      /** \brief Calculate height in a point.
       *
       * Assumes input already normalized to ball surface.
       * \param vv point vector.
       * \return Heightmap value at given point.
       */
      float calcHeightNormalized(const math::vec3f &pnt) const;

      /** \brief Calculate surface gradient in a point.
       *
       * \param pnt point vector.
       * \param gdist Gradient distance.
       * \return Normal value at given point.
       */
      math::vec3f calcNormal(const math::vec3f &pnt, float gdist) const;

      /** \brief Calculate surface gradient in a point.
       *
       * \param pnt point vector.
       * \param gdist Gradient distance.
       * \param ht Height value at given point.
       * \return Normal value at given point.
       */
      math::vec3f calcNormal(const math::vec3f &pnt, float gdist, float ht) const;

      /** \brief Normalize a height of given vertex.
       *
       * Will calculate the height as part of operation.
       *
       * \param vv Vertex to scale to corrct level.
       * \return Normalized vertex at correct height.
       */
      math::vec3f normalizeHeight(const math::vec3f &vv) const;

      /** \brief Normalize a height of given vertex.
       *
       * \param vv Vertex to scale to corrct level.
       * \param ht Previously calculated heightfield value.
       * \return Normalized vertex at correct height.
       */
      math::vec3f normalizeHeight(const math::vec3f &vv, float ht) const;

      /** \brief Normalize a height of given vertex (double precision).
       *
       * Will calculate the height as part of operation.
       *
       * \param vv Vertex to scale to corrct level.
       * \return Normalized vertex at correct height.
       */
      math::vec3d normalizeHeight(const math::vec3d &vv) const;

      /** \brief Normalize a height of given vertex (double precision).
       *
       * \param vv Vertex to scale to corrct level.
       * \param ht Previously calculated heightfield value.
       * \return Normalized vertex at correct height.
       */
      math::vec3d normalizeHeight(const math::vec3d &vv, float ht) const;

    public:
      /** \brief Calculate height in a point.
       *
       * \param pnt point vector.
       * \return Heightmap value at given point.
       */
      virtual float calcHeight(const math::vec3f &pnt) const;

    public:
      /** \brief Calculate teerrain level in a point.
       *
       * \param pnt point vector.
       * \return Heightmap value at given point.
       */
      virtual float calcTerrain(const math::vec3f &pnt) const = 0;
  };
}

#endif
