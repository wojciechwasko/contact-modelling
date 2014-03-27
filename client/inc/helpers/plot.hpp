#ifndef PLOT_HPP
#define PLOT_HPP

#include <fstream>
#include <tuple>
#include <string>
#include <boost/filesystem.hpp>
#include "gnuplot-iostream.h"

namespace helpers {
  namespace plot {

    template <class Mesh>
    void plotMesh(Mesh const& mesh, std::string const& title, bool use_pm3d = false)
    {
      boost::filesystem::path temp = boost::filesystem::unique_path("tmp-%%%%-%%%%-%%%%-%%%%");
      const std::string tempstr    = temp.native();  // optional
      std::ofstream out(tempstr);
      out.precision(10);

      std::vector<std::array<double,3>> plotdata;
      plotdata.reserve(mesh.size());
      double lastX = *(mesh.cbegin()->vals[0]);
      mesh.for_each_node([&] (typename Mesh::const_reference n) {
        plotdata.push_back({n.x,n.y,*(n.vals[0])});

        if (n.x != lastX) {
          out << std::endl;
          lastX = n.x;
        }
        out << n.x << " " << n.y << " " << *(n.vals[0]) << std::endl;
      });
      Gnuplot gp("gnuplot -persist");  
      gp << "set isosamples 60\n";
      gp << "set xlabel 'x'\n";
      gp << "set ylabel 'y'\n";
      gp << "splot '" << tempstr << "' u 1:2:3 with ";
      if (use_pm3d)
        gp << " pm3d ";
      else
        gp << " points ";
      gp << " title '" << title << "'\n";

      std::cout << "Using temporary file " << tempstr << std::endl;
    }
  }
}

#endif /* PLOT_HPP */
