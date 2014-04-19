#include "helpers/plot.hpp"

#include "MeshInterface.hpp"

namespace helpers {
  namespace plot {
    void 
    dumpForPlot(const MeshInterface& m, std::ostream& s)
    {
      s << m.dx() << " " << m.dy() << "\n";
    
      // I guess if somebody wants to make performance improvements, they can later rewrite the code
      // to go over xs and ys simultaneously, dump xs to s and ys to some temp sstream and then dump
      // that sstream to s. It *should* be faster.
      // But listen to Donald Knuth.
    
      // dump all xs
      s << m.node(0).x;
      for (size_t n = 1; n < m.no_nodes(); ++n) {
        s << " " << m.node(n).x;
      }
      s << "\n";
    
      // dump all ys
      s << m.node(0).y;
      for (size_t n = 1; n < m.no_nodes(); ++n) {
        s << " " << m.node(n).y;
      }
      s << "\n";
    
      for (size_t d = 0; d < m.D; ++d) {
        s << m.getValue(0, d);
        for (size_t n = 1; n < m.no_nodes(); ++n) {
          s << " " << m.getValue(n, d);
        }
        s << "\n";
      }
    }
    
    void
    dumpForPlot(const MeshInterface& m, std::string s)
    {
      std::ofstream fout(s, std::fstream::trunc);
      dumpForPlot(m, fout);
    }
  }
}
