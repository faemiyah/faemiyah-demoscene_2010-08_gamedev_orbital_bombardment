#ifndef OB_VISUALIZATION_NUKE_HPP
#define OB_VISUALIZATION_NUKE_HPP

#include "ob_visualization_mesh.hpp"

namespace ob
{
  /** \brief Visual representation of a nuke launch marker
   */
  class VisualizationNuke :
    public VisualizationMesh
  {
    public:
      /** \brief Constructor. */
      VisualizationNuke();

      /** \brief Destructor. */
      virtual ~VisualizationNuke() { }
  };
}

#endif
