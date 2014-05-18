#include "cm/grid/plot.hpp"

#include "cm/grid/grid.hpp"

namespace cm {

void 
dumpForPlot(const Grid& m, std::ostream& s)
{
  s << m.getCellShape().dx() << " " << m.getCellShape().dy() << "\n";

  // I guess if somebody wants to make performance improvements, they can later rewrite the code
  // to go over xs and ys simultaneously, dump xs to s and ys to some temp sstream and then dump
  // that sstream to s. It *should* be faster.
  // But listen to Donald Knuth.

  // dump all xs
  s << m.cell(0).x;
  for (size_t n = 1; n < m.num_cells(); ++n) {
    s << " " << m.cell(n).x;
  }
  s << "\n";

  // dump all ys
  s << m.cell(0).y;
  for (size_t n = 1; n < m.num_cells(); ++n) {
    s << " " << m.cell(n).y;
  }
  s << "\n";

  for (size_t d = 0; d < m.dim(); ++d) {
    s << m.getValue(0, d);
    for (size_t n = 1; n < m.num_cells(); ++n) {
      s << " " << m.getValue(n, d);
    }
    s << "\n";
  }
}

void
dumpForPlot(const Grid& m, std::string s)
{
  std::ofstream fout(s, std::fstream::trunc);
  dumpForPlot(m, fout);
}

} /* namespace cm */
