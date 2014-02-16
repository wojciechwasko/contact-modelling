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

typedef SkinConnector<skin_object>
  skin_connector_type;
typedef MeshNatural<MeshNode<1>, skin_connector_type>
  raw_mesh_type;
typedef MeshRegularSquare<MeshNode<1>>
  interpolated_mesh_type;
typedef InterpolatorLinearDelaunay<raw_mesh_type, interpolated_mesh_type>
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

  std::unique_ptr<interpolated_mesh_type> s_mesh(new interpolated_mesh_type);
  std::unique_ptr<interpolator_type> interpolator(new interpolator_type);
  std::unique_ptr<force_mesh_type> force_mesh(new force_mesh_type);
  std::unique_ptr<resulting_disp_mesh_type> resulting_disp_mesh(new resulting_disp_mesh_type);

  std::unique_ptr<skin_connector_type> skin_conn;
  try {
    skin_conn.reset(new skin_connector_type(SensorValuesConverter));
  } catch (const std::exception& e) {
    std::cerr << "Something went wrong with the inialisation:\n"
      << e.what() << "\nExiting...\n";
    return 1;
  }

  natural_disps_from_sensors_type disps_natural(*skin_conn);
  interpolated_disps_from_sensors_type disps_interpolated(
      *skin_conn,
      std::move(s_mesh),
      std::move(interpolator)
  );

  Forces<force_mesh_type> f(std::move(force_mesh));

  ResultantDisplacements<resulting_disp_mesh_type> res_disp(std::move(resulting_disp_mesh));

  std::for_each(
    disps_natural.nodes_cbegin(),
    disps_natural.nodes_cend(),
    [&](const natural_disps_from_sensors_type::node_type& n) {
      std::cout << "x: " << n.x << ", y: " << n.y << std::endl;    
    });

  std::cout << "Requesting new data.\n";
  disps_natural.update();
  std::cout << "New data available.\n";

  std::for_each(
    disps_natural.nodes_cbegin(),
    disps_natural.nodes_cend(),
    [&](const natural_disps_from_sensors_type::node_type & s) {
      std::cout << *s.vals[0] << "\n";
    });

  std::cout << "Finishing!\n";
  return 0;
}
