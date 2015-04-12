#ifndef OB_VISUALIZATION_RAILGUN_HPP
#define OB_VISUALIZATION_RAILGUN_HPP

#include "ob_visualization_mesh.hpp"

namespace ob
{
  /** \brief Visual representation of an orbit.
   *
   * This only involves the mesh representing the orbit.
   */
  class VisualizationRailgun :
    public VisualizationMesh
  {
    public:
      /** \brief Constructor. */
      VisualizationRailgun();

      /** \brief Destructor. */
      virtual ~VisualizationRailgun() { }
  };
}

#endif
