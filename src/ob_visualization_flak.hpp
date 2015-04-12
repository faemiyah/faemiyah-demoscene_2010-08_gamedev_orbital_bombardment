#ifndef OB_VISUALIZATION_FLAK_HPP
#define OB_VISUALIZATION_FLAK_HPP

#include "ob_visualization_mesh.hpp"

namespace ob
{
  /** \brief Visual representation of an orbit.
   *
   * This only involves the mesh representing the orbit.
   */
  class VisualizationFlak :
    public VisualizationMesh
  {
    public:
      /** \brief Constructor. */
      VisualizationFlak();

      /** \brief Destructor. */
      virtual ~VisualizationFlak() { }
  };
}

#endif
