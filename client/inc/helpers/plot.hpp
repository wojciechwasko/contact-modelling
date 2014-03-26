#ifndef PLOT_HPP
#define PLOT_HPP

#include <tuple>
#include <string>
#include "gnuplot-iostream.h"

namespace helpers {
  namespace plot {

    template <class Mesh>
    void plotMesh(Mesh const& mesh, std::string const& title)
    {
      std::vector<std::array<double,3>> plotdata;
      plotdata.reserve(mesh.size());
      mesh.for_each_node([&] (typename Mesh::const_reference n) {
        plotdata.push_back({n.x,n.y,*(n.vals[0])});
      });
      Gnuplot gp("gnuplot -persist");  
      gp << "splot '-' u 1:2:3 with lines title '" << title << "'\n";
      gp.send1d(plotdata);
    }
  }
}

#endif /* PLOT_HPP */
