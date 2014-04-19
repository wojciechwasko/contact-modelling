#ifndef PLOT_HPP
#define PLOT_HPP

#include <fstream>
#include <tuple>
#include <string>
#include <boost/filesystem.hpp>
#include "gnuplot-iostream.h"

class MeshInterface;

namespace helpers {
  namespace plot {

    /**
     * \brief   Dump in a format suitable for plotting with matplotlib using the provided
     *          tools/plot_mesh.py script
     * \param   m   mesh to be dumped
     * \param   s   stream to which to output
     *
     * Format of the file:
     * dx dy
     * x0 x1 x2 x3 x4 [...]
     * y0 y1 y2 y3 y4 [...]
     * val0 val1 val2 val3 val4 [...] # dimension 0
     * val0 val1 val2 val3 val4 [...] # dimension 1
     * ...
     * val0 val1 val2 val3 val4 [...] # dimension D-1
     */
    void dumpForPlot(const MeshInterface& m, std::ostream& s);

    /**
     * \brief   Dump in a format suitable for plotting with matplotlib using the provided
     *          tools/plot_mesh.py script
     * \param   m   mesh to be dumped
     * \param   s   name of the file to which to output
     * \note    The file will be *truncated*!
     *
     * Merely opens the file and forwards to dumpForPlot(std::ostream&)
     */
    void dumpForPlot(const MeshInterface& m, std::string s);

    template <class Mesh>
    void gnuplotPlotMesh(Mesh const& mesh, std::string const& title, bool use_pm3d = false)
    {
      boost::filesystem::path temp = boost::filesystem::unique_path("tmp-%%%%-%%%%-%%%%-%%%%");
      const std::string tempstr    = temp.native();  // optional
      std::ofstream out(tempstr);
      out.precision(10);

      double lastX = mesh.getValue(0,0);
      for (size_t in = 0; in < mesh.size(); ++in) {
        const typename Mesh::node_type& node = mesh.node(in);
        if (node.x != lastX) {
          out << std::endl;
          lastX = node.x;
        }
        out << node.x << " " << node.y << " " << mesh.getValue(in,0) << std::endl;
      }
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
