#ifndef OB_VISUALIZATION_CITY_HPP
#define OB_VISUALIZATION_CITY_HPP

#include "ob_visualization_mesh.hpp"

namespace ob
{
  /** \brief Visual representation of a city.
   *
   * This only involves the mesh representing the city marker.
   */
  class VisualizationCity :
    public VisualizationMesh
  {
    public:
      /** \brief Constructor. */
      VisualizationCity();

      /** \brief Destructor. */
      virtual ~VisualizationCity() { }
  };
}

#endif
