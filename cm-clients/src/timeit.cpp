#include <iostream>
#include <ctime>
#include <string>

#include "cm/cm.hpp"
#include "cm/details/string.hpp"
#include "cm/details/elastic_model_boussinesq.hpp"
#include "cm/details/elastic_model_love.hpp"

using cm::details::sb;

constexpr double d = 1e-3;

constexpr size_t grid_sizes[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

constexpr cm::SkinAttributes skin_attr = {0.002, 300000, 0.5, 0.004};

/**
 * \brief Measures own CPU lifetime. Outputs it on destruction
 */
class Timer
{
  std::string str_;
  std::clock_t start_;

public:
  Timer(std::string str)
    : str_(str), start_(std::clock())
  {};

  ~Timer()
  {
    double duration_clocks = std::clock() - start_;
    std::cerr
      << str_ << ": "
      << (1000.0*duration_clocks/CLOCKS_PER_SEC) << " [ms]."
      << std::endl;
  }
};

cm::Grid* genGrid(const size_t sqrt_no_nodes);

int main() {
  for (auto sqrt_no_nodes : grid_sizes) {
    cm::Grid* grid = genGrid(sqrt_no_nodes);
    if (!grid) {
      throw std::runtime_error(sb() 
        << "Empty ptr for cm::Grid @ "
        << sqrt_no_nodes << " nodes."
      );
    }
    // --- time forward Boussinesq-Cerruti with exact psi calculation
    {
      Timer t{sb() << "B-C, psi_exact=T, @" <<sqrt_no_nodes};
      auto mat = cm::details::forces_to_displacements_matrix(
          *grid,*grid,
          skin_attr, true
      );
    }

    // --- time forward Boussinesq-Cerruti with psi set to 0.25
    {
      Timer t{sb() << "B-C, psi_exact=F, @" <<sqrt_no_nodes};
      auto mat = cm::details::forces_to_displacements_matrix(
          *grid,*grid,
          skin_attr, false
      );
    }

    // --- time forward Love's solution
    {
      Timer t{sb() << "Love, @" <<sqrt_no_nodes};
      auto mat = cm::details::pressures_to_displacements_matrix(
          *grid,*grid,
          skin_attr
      );
    }

    delete grid;
  }
  return 0;
}

cm::Grid* genGrid(const size_t sqrt_no_nodes)
{
  return cm::Grid::fromFill(1, cm::Square(d), 0, 0, sqrt_no_nodes*d, sqrt_no_nodes*d);
}
