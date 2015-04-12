#ifndef OB_VISUALIZATION_SUN_HPP
#define OB_VISUALIZATION_SUN_HPP

#include "ob_visualization_mesh.hpp"

namespace ob
{
  /** \brief Visual representation of a sun
   *
   * This is actualla kind-of fake flat mesh.
   */
  class VisualizationSun :
    public VisualizationMesh
  {
    public:
      /** \brief Constructor. */
      VisualizationSun();

      /** \brief Destructor. */
      virtual ~VisualizationSun() { }

    public:
      /** \cond */
      //virtual void draw(const gfx::Shader &pshader, const math::mat4f &pmat) const;
      /** \endcond */
  };
}

#endif
