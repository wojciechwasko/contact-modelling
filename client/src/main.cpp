#include <iostream>
#include <memory>

#include "skin_sensor.h"

#include "main.hpp"
#include "Forces.hpp"
#include "MeshNode.hpp"
#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "DisplacementsFromSensorsNatural.hpp"
#include "DisplacementsFromSensorsInterpolated.hpp"
#include "ResultantDisplacements.hpp"

typedef skin_object::skin_sensor_iterator
  skin_sensor_iterator;
typedef MeshNatural<MeshNode<1>, skin_sensor_iterator>
  raw_mesh_type;
typedef MeshRegularSquare<MeshNode<1>>
  interpolated_mesh_type;
typedef InterpolatorLinearDelaunay<raw_mesh_type, interpolated_mesh_type>
  interpolator_type;
typedef MeshRegularSquare<MeshNode<3>>
  force_mesh_type;
typedef MeshRegularSquare<MeshNode<3>>
  resulting_disp_mesh_type;
typedef DisplacementsFromSensorsNatural<skin_sensor_iterator>
  natural_disps_from_sensors_type;
typedef DisplacementsFromSensorsInterpolated<skin_sensor_iterator, interpolated_mesh_type, interpolator_type>
  interpolated_disps_from_sensors_type;

int main(int argc, char** argv)
{
  std::cout << "Starting!\n";

  skin_object s_object;
  skin_reader* s_reader = s_object.reader(); 
  init_skinware(&s_object, s_reader);

  std::unique_ptr<interpolated_mesh_type> s_mesh(new interpolated_mesh_type);
  std::unique_ptr<interpolator_type> interpolator(new interpolator_type);
  std::unique_ptr<force_mesh_type> force_mesh(new force_mesh_type);
  std::unique_ptr<resulting_disp_mesh_type> resulting_disp_mesh(new resulting_disp_mesh_type);

  natural_disps_from_sensors_type disps_natural(
      s_object.sensors_iter_begin(),
      s_object.sensors_iter_end()
  );
  interpolated_disps_from_sensors_type disps_interpolated(
      s_object.sensors_iter_begin(),
      s_object.sensors_iter_end(),
      std::move(s_mesh),
      std::move(interpolator)
  );

  Forces<force_mesh_type> f(std::move(force_mesh));

  ResultantDisplacements<resulting_disp_mesh_type> res_disp(std::move(resulting_disp_mesh));

  std::for_each(
    disps_natural.nodes_cbegin(),
    disps_natural.nodes_cend(),
    [&](natural_disps_from_sensors_type::node_type n) {
      std::cout << "x: " << n.x << ", y: " << n.y << std::endl;    
    });


  std::cout << "Finishing!\n";
  return 0;
}


bool init_skinware(skin_object* so, skin_reader* sr)
{
  const int obj_load_res = so->load();
  if (obj_load_res == SKIN_SUCCESS) {
    std::cout << "Skin object loaded successfully\n";
  } else if (obj_load_res == SKIN_FAIL) {
    std::cerr << "skin_object.load() : SKIN_FAIL returned\n" << std::flush;
    return true;
  } else if (obj_load_res == SKIN_NO_MEM) {
    std::cerr << "skin_object.load() : SKIN_NO_MEM returned\n" << std::flush;
    return true;
  }

  const int reader_start_res = sr->start(SKIN_ALL_SENSOR_TYPES, SKIN_ACQUISITION_SPORADIC);
  if (reader_start_res == SKIN_SUCCESS) {
    std::cout << "Reader initialised properly!\n";
  } else if (reader_start_res == SKIN_BAD_ID) {
    std::cerr << "reader.start() : SKIN_BAD_ID returned\n" << std::flush;
    return true;
  } else if (reader_start_res == SKIN_PARTIAL) {
    std::cerr << "reader.start() : SKIN_PARTIAL returned\n" << std::flush;
    return true;
  } else if (reader_start_res == SKIN_FAIL) {
    std::cerr << "reader.start() : SKIN_FAIL returned\n" << std::flush;
    return true;
  }

  std::cout << std::flush;
  return false;
}
