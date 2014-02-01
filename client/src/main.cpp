#include <iostream>
#include <memory>

#include "skin_object.h"
#include "skin_module.h"
#include "skin_sensor.h"

#include "main.hpp"
#include "Forces.hpp"
#include "MeshNodeVal1d.hpp"
#include "MeshNodeVal3d.hpp"
#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "DisplacementsFromSensors.hpp"
#include "ResultantDisplacements.hpp"


int main(int argc, char** argv) {


  typedef skin_module::skin_sensor_iterator skin_sensor_iterator;
  typedef MeshRegularSquare<MeshNodeVal1d> inter_source_mesh_type;
  typedef MeshNatural<MeshNodeVal1d, skin_sensor_iterator> raw_source_mesh_type;
  typedef InterpolatorLinearDelaunay<raw_source_mesh_type, inter_source_mesh_type> interpolator_type;
  typedef MeshRegularSquare<MeshNodeVal3d> force_mesh_type;
  typedef MeshRegularSquare<MeshNodeVal3d> res_disp_mesh_type;
  typedef DisplacementsFromSensors<skin_sensor_iterator, raw_source_mesh_type, inter_source_mesh_type, interpolator_type> disps_from_sensors_type;

  skin_module module;
  std::unique_ptr<inter_source_mesh_type> s_mesh(new inter_source_mesh_type);
  std::unique_ptr<interpolator_type> interpolator(new interpolator_type);
  std::unique_ptr<force_mesh_type> force_mesh(new force_mesh_type);
  std::unique_ptr<res_disp_mesh_type> res_disp_mesh(new res_disp_mesh_type);

  DisplacementsFromSensors<skin_sensor_iterator> disps1(
      module.sensors_iter_begin(),
      module.sensors_iter_end()
  );
  disps_from_sensors_type disps2(
      module.sensors_iter_begin(),
      module.sensors_iter_end(),
      std::move(s_mesh),
      std::move(interpolator)
  );
  Forces<force_mesh_type> f(std::move(force_mesh));

  ResultantDisplacements<res_disp_mesh_type> res_disp(std::move(res_disp_mesh));

  return 0;
}


