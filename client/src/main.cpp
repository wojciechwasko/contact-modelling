#include <iostream>
#include <memory>

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

  DisplacementsFromSensors<int> disps1(1,2);

  typedef MeshRegularSquare<MeshNodeVal1d> inter_source_mesh_type;
  typedef MeshNatural<MeshNodeVal1d> raw_source_mesh_type;
  typedef InterpolatorLinearDelaunay<raw_source_mesh_type, inter_source_mesh_type> interpolator_type;
  typedef MeshRegularSquare<MeshNodeVal3d> force_mesh_type;
  typedef MeshRegularSquare<MeshNodeVal3d> res_disp_mesh_type;

  std::unique_ptr<inter_source_mesh_type> s_mesh(new inter_source_mesh_type);
  std::unique_ptr<interpolator_type> interpolator(new interpolator_type);
  std::unique_ptr<force_mesh_type> force_mesh(new force_mesh_type);
  std::unique_ptr<res_disp_mesh_type> res_disp_mesh(new res_disp_mesh_type);

  DisplacementsFromSensors<int, inter_source_mesh_type, interpolator_type>
    disps2(1, 2, std::move(s_mesh), std::move(interpolator));
  Forces<force_mesh_type> f(std::move(force_mesh));

  ResultantDisplacements<res_disp_mesh_type> res_disp(std::move(res_disp_mesh));

  return 0;
}


