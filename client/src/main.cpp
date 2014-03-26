#include <iostream>
#include <memory>
#include <stdexcept>


#include "main.hpp"
#include "SkinConnector.hpp"
#include "SensorValuesConverter.hpp"
#include "Forces.hpp"
#include "MeshNode.hpp"
#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "DisplacementsFromSensorsNatural.hpp"
#include "DisplacementsFromSensorsInterpolated.hpp"
#include "ResultantDisplacements.hpp"

#include "helpers/plot.hpp"

typedef SkinConnector<skin_object>
  skin_connector_type;
typedef MeshNatural<MeshNode<1>, skin_connector_type>
  raw_mesh_type;
typedef MeshRegularSquare<MeshNode<1>>
  interpolated_mesh_type;
typedef InterpolatorLinearDelaunay<raw_mesh_type, interpolated_mesh_type, PointsOutsideSourceMeshPolicy::InterpolateToZero>
  interpolator_type;
typedef MeshRegularSquare<MeshNode<3>>
  force_mesh_type;
typedef MeshRegularSquare<MeshNode<3>>
  resulting_disp_mesh_type;
typedef DisplacementsFromSensorsNatural<skin_connector_type>
  natural_disps_from_sensors_type;
typedef DisplacementsFromSensorsInterpolated<skin_connector_type, interpolated_mesh_type, interpolator_type>
  interpolated_disps_from_sensors_type;

int main(int argc, char** argv)
{
  std::cout << "Starting!\n";

  std::unique_ptr<skin_connector_type> skin_conn;
  try {
    skin_conn.reset(new skin_connector_type(SensorValuesConverter));
  } catch (const std::exception& e) {
    std::cerr << "Something went wrong with the inialisation:\n"
      << e.what() << "\nExiting...\n";
    return 1;
  }

  natural_disps_from_sensors_type disps_natural(*skin_conn);

  // "interpolated" mesh
  std::unique_ptr<interpolated_mesh_type>
    s_mesh(new interpolated_mesh_type(disps_natural.getMesh(), 0.001));
  // interpolator used to go from a natural mesh to the interpolated.
  // "interpolated" sensor readings
  interpolated_disps_from_sensors_type disps_interpolated(
      *skin_conn,
      std::move(s_mesh)
  );

  // forces mesh
  std::unique_ptr<force_mesh_type>
    force_mesh(new force_mesh_type(disps_natural.getMesh(), 0.01));
  Forces<force_mesh_type> f(std::move(force_mesh));

  // displacemetns obtained from (displacements) -> (forces) -> (displacements)
  std::unique_ptr<resulting_disp_mesh_type>
    resulting_disp_mesh(new resulting_disp_mesh_type(disps_natural.getMesh(), 0.01));
  ResultantDisplacements<resulting_disp_mesh_type> res_disp(std::move(resulting_disp_mesh));

  std::cout << "Requesting new data.\n";
  disps_interpolated.update();
  disps_natural.update();
  std::cout << "New data available.\n";

  std::for_each(
    disps_interpolated.nodes_cbegin(),
    disps_interpolated.nodes_cend(),
    [&](const interpolated_disps_from_sensors_type::node_type & n) {
      //std::cout <<  "x: " << n.x << ",\ty: " << n.y << ",\tvalue: " << *n.vals[0] << "\n";
    });

  using helpers::plot::plotMesh;
  plotMesh(disps_interpolated.getSourceMesh(), "natural");
  plotMesh(disps_interpolated.getTargetMesh(), "interpolated");

  std::cout << "Finishing!\n";
  return 0;
}
