#ifndef SKINPROVIDERLUCA_HPP
#define SKINPROVIDERLUCA_HPP

#include <string>

#include "cm/skin_provider/interface.hpp"

namespace cm {

/**
 * \brief   Skin provider which will read data from files recorded by Luca
 * Muscari
 *
 * I obtained a set of data Luca used for generating the results he included in
 * his Master thesis. To be able to re-use them easily, this class will take a
 * path to the dir with the data and read the files in there, throwing exception
 * if they're not valid results from Luca's experimets. If the files are valid
 * results from Luca's experiments, it will calculate the average of the 50
 * tries (this is what Luca did) and emit the values on a call to update().
 *
 * The structure of Luca's dataset is the following (dir
 * "./griglia40x40/Piattello/InfraTaxel4-6/0,5Newton"):
 *   
\verbatim
.
├── delta
│   ├── 0.txt
│   ├── 1.txt
│   ├── 2.txt
 (...)
│   ├── 48.txt
│   └── 49.txt
├── forze
│   ├── 0.txt
│   ├── 1.txt
│   ├── 2.txt
 (...)
│   ├── 48.txt
│   └── 49.txt
└── report.txt

2 directories, 101 files
\endverbatim
 * `report.txt` contains "metadata" about the experiment. Example contents:
\verbatim

Forma indenter: Sfera   Risoluzione griglia: 40x40 punti
Modulo Young: 177000 Pascal

PHI: 0.25

h: 0.002

z0: 0.00106527

h/z0: 1.87745

x_spacing: 0.00165641

y_spacing: 0.00143487
\endverbatim
 * `forze` contains reconstructed forces, as given by his code -- this is of not
 * too much meaning to us. `delta`, on the other hand, contains data
 * interpolated over a regular grid with base element -- a rectangle
 * `(x_spacing,y_spacing)`.
 *
 * \note  The data is stored (as per luca's source code) in a "column-major"
 * order, that is the first row corresponds to `y=0`, the second to
 * `y=y_spacing`, the third to `y=2*y_spacing` and so on...
 */
class SkinProviderLuca : public cm::SkinProviderInterface 
{
public:

  using typename cm::SkinProviderInterface::target_values_type;
  /**
   * \brief   Construct the provider, pointing it to dir_path.
   * \param   dir_path  a directory which contains dirs `forze`,`delta` and a
   *                    file `report.txt`
   */
  SkinProviderLuca(const std::string& dir_path);

private:
  target_values_type impl_update(void) const;
  Grid* impl_createGrid() const;
  SkinAttributes impl_getAttributes() const;

  struct SourceCell {
    std::array<double, 2>  relative_position;
  };

  std::vector<SourceCell> grid_source_;
  std::vector<double> avg_values_;
  double dx_;
  double dy_;
  double h_;
  double E_;
  double nu_;
  double taxelRadius_;
};

/**
 * \cond DEV
 */
namespace details {

/**
 * \brief   Take a directory (it's name) and check if it contains the right
 * structure for it to be Luca's data.
 *
 * Throws std::runtime_error on first error found.
 *
 * \note This check does not guarantee that the data *inside* the files will be
 * consistent. It merely checks the structure -- think of it like of an early
 * check.
 * \par
 * \note This does not require the presence of `forze` directory.
 */
void check_dir_validity_for_luca(const std::string& dir_path);

/**
 * \brief   Return structure for processReportTxt
 */
struct ReportTxtData {
  /**
   * \brief Thickenss of the skin
   */
  double h;
  /**
   * \brief Young's modulus for the skin's elastomer
   */
  double E;
  /**
   * \brief distance between two adjacent nodes, along x
   */
  double x_spacing;
  /**
   * \brief distance between two adjacent nodes, along y
   */
  double y_spacing;
  /**
   * \brief number of nodes in the x direction
   */
  size_t x_num;
  /**
   * \brief number of nodes in the y direction
   */
  size_t y_num;
};


/**
 * \brief   Extract data from report.txt (young's modulus, skin thickness,
 * x_spacing and y_spacing. Possibly also number of nodes in each direction
 * (x_num and y_num).
 */
void processReportTxt(const std::string& dir_path, ReportTxtData& output);
std::string extractToken(std::istream& stream, const char* token);
void extractResolution(std::istream& stream, ReportTxtData& save_to);
void extractElasticModulus(std::istream& stream, ReportTxtData& save_to);
void extractSkinThickness(std::istream& stream, ReportTxtData& save_to);
void extractx_spacing(std::istream& stream, ReportTxtData& save_to);
void extracty_spacing(std::istream& stream, ReportTxtData& save_to);

std::vector<double> getAverageData(const std::string& root_dir, const size_t expected_num);
std::vector<double> getDataFromOneFile(std::istream& datafile, const size_t expected_num);

} /* namespace details */

/**
 * \endcond
 */

} /* namespace cm */


#endif /* SKINPROVIDERLUCA_HPP */
