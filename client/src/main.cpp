#include <iostream>
#include <memory>

#include "skin_object.h"
#include "skin_sensor.h"
#include "skin_reader.h"

#include "main.hpp"
#include "Forces.hpp"
#include "MeshNode.hpp"
#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "DisplacementsFromSensors.hpp"
#include "ResultantDisplacements.hpp"


int main(int argc, char** argv)
{
  std::cout << "Starting!\n";

  typedef skin_object::skin_sensor_iterator skin_sensor_iterator;
  typedef MeshRegularSquare<MeshNode<1>> inter_source_mesh_type;
  typedef MeshNatural<MeshNode<1>, skin_sensor_iterator> raw_source_mesh_type;
  typedef InterpolatorLinearDelaunay<raw_source_mesh_type, inter_source_mesh_type> interpolator_type;
  typedef MeshRegularSquare<MeshNode<3>> force_mesh_type;
  typedef MeshRegularSquare<MeshNode<3>> res_disp_mesh_type;
  typedef DisplacementsFromSensors<skin_sensor_iterator, raw_source_mesh_type, inter_source_mesh_type, interpolator_type> disps_from_sensors_type;

  skin_object s_object;
  skin_reader* s_reader = s_object.reader(); 

  const int obj_load_res = s_object.load();
  if (obj_load_res == SKIN_SUCCESS) {
    std::cout << "Skin object loaded successfully\n";
  } else if (obj_load_res == SKIN_FAIL) {
    std::cerr << "skin_object.load() : SKIN_FAIL returned\n";
  } else if (obj_load_res == SKIN_NO_MEM) {
    std::cerr << "skin_object.load() : SKIN_NO_MEM returned\n";
  }
  std::unique_ptr<inter_source_mesh_type> s_mesh(new inter_source_mesh_type);
  std::unique_ptr<interpolator_type> interpolator(new interpolator_type);
  std::unique_ptr<force_mesh_type> force_mesh(new force_mesh_type);
  std::unique_ptr<res_disp_mesh_type> res_disp_mesh(new res_disp_mesh_type);

  DisplacementsFromSensors<skin_sensor_iterator> disps1(
      s_object.sensors_iter_begin(),
      s_object.sensors_iter_end()
  );
  disps_from_sensors_type disps2(
      s_object.sensors_iter_begin(),
      s_object.sensors_iter_end(),
      std::move(s_mesh),
      std::move(interpolator)
  );
  Forces<force_mesh_type> f(std::move(force_mesh));

  ResultantDisplacements<res_disp_mesh_type> res_disp(std::move(res_disp_mesh));

  const int reader_start_res = s_reader->start(SKIN_ALL_SENSOR_TYPES, SKIN_ACQUISITION_SPORADIC);
  if (reader_start_res == SKIN_SUCCESS) {
    std::cout << "Reader initialised properly!\n";
  } else if (reader_start_res == SKIN_BAD_ID) {
    std::cerr << "reader.start() : SKIN_BAD_ID returned\n";
  } else if (reader_start_res == SKIN_PARTIAL) {
    std::cerr << "reader.start() : SKIN_PARTIAL returned\n";
  } else if (reader_start_res == SKIN_FAIL) {
    std::cerr << "reader.start() : SKIN_FAIL returned\n";
  }

  std::cout << "Finishing!\n";
  std::cout << std::flush;
  std::cerr << std::flush;

  return 0;
}


