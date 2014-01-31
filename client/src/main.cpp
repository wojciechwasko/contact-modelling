#include <iostream>

#include "main.hpp"
#include "Forces.hpp"
#include "MeshNodeVal1d.hpp"
#include "MeshNodeVal3d.hpp"
#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "DisplacementsFromSensors.hpp"


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

  MeshRegularSquare<MeshNodeVal3d> force_mesh;
  Forces<MeshRegularSquare<MeshNodeVal3d>> f(&force_mesh);

  DisplacementsFromSensors<int, MeshRegularSquare<MeshNodeVal1d>, myInterpolator> disps2(1, 2, m, i);
  return 0;
}

