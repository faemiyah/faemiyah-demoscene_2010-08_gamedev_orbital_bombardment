#ifndef OB_VISUALIZATION_DISTORT_HPP
#define OB_VISUALIZATION_DISTORT_HPP

#include "ob_visualization_mesh.hpp"

namespace ob
{
  /** \brief Visual representation of a sun
   *
   * This is actualla kind-of fake flat mesh.
   */
  class VisualizationDistort :
    public VisualizationMesh
  {
    public:
      /** \brief Constructor. */
      VisualizationDistort();

      /** \brief Destructor. */
      virtual ~VisualizationDistort() { }

    public:
      /** \cond */
      //virtual void draw(const gfx::Shader &pshader, const math::mat4f &pmat) const;
      /** \endcond */
  };
}

#endif
