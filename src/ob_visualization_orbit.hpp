#ifndef OB_VISUALIZATION_ORBIT_HPP
#define OB_VISUALIZATION_ORBIT_HPP

#include "ob_visualization_mesh.hpp"

namespace ob
{
  /** \brief Visual representation of an orbit.
   *
   * This only involves the mesh representing the orbit.
   */
  class VisualizationOrbit :
    public VisualizationMesh
  {
    public:
      /** \brief Constructor. */
      VisualizationOrbit();

      /** \brief Destructor. */
      virtual ~VisualizationOrbit() { }
  };
}

#endif
