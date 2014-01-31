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

  MeshNatural<MeshNodeVal1d> mn;
  mn.for_each_node([](const MeshNodeVal1d& n) {
    std::cout << n.x;
  });
  DisplacementsFromSensors<int> disps1(1,2);


  MeshRegularSquare<MeshNodeVal1d> m;
  typedef InterpolatorLinearDelaunay<
    MeshNatural<MeshNodeVal1d>,
    MeshRegularSquare<MeshNodeVal1d>
  > myInterpolator;
  myInterpolator i;

  typedef MeshRegularSquare<MeshNodeVal3d> force_mesh_type;
  std::unique_ptr<force_mesh_type> force_mesh(new force_mesh_type);
  Forces<force_mesh_type> f(std::move(force_mesh));

  typedef MeshRegularSquare<MeshNodeVal3d> res_disp_mesh_type;
  std::unique_ptr<res_disp_mesh_type> res_disp_mesh(new res_disp_mesh_type);
  ResultantDisplacements<res_disp_mesh_type> res_disp(std::move(res_disp_mesh));

  DisplacementsFromSensors<int, MeshRegularSquare<MeshNodeVal1d>, myInterpolator> disps2(1, 2, m, i);
  return 0;
}


