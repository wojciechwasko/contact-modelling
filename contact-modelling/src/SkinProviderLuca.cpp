#include "cm/skin_provider/luca.hpp"

#include <stdexcept>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>

#include "cm/grid/grid.hpp"
#include "cm/details/string.hpp"
#include "cm/details/exception.hpp"

namespace bfs = boost::filesystem;

namespace cm {

using details::sb;

SkinProviderLuca::SkinProviderLuca(const std::string& dir_path)
  : SkinProviderInterface(1)
{
  details::check_dir_validity_for_luca(dir_path); 

  details::ReportTxtData report_data;
  details::processReportTxt(dir_path, report_data);

  dx_ = report_data.x_spacing;
  dy_ = report_data.y_spacing;
  h_  = report_data.h;
  E_  = report_data.E;
  // hard-coded, not contained in report.txt
  nu_ = 0.5;
  taxelRadius_ = 0.002;

  grid_source_.reserve(report_data.x_num * report_data.y_num);
  // NOTE we're constructing "column"-wise. This is the order Luca did it
  double y = 0;
  for (size_t iy = 0; iy < report_data.y_num; ++iy, y += dy_) {
    double x = 0;
    for (size_t ix = 0; ix < report_data.x_num; ++ix, x += dx_) {
      grid_source_.push_back({x,y}); 
    }
  }

  avg_values_ = details::getAverageData(dir_path, report_data.x_num * report_data.y_num);
}

void SkinProviderLuca::impl_update(target_values_type& target_vec) const
{
  target_vec.assign(avg_values_.cbegin(), avg_values_.cend());
}

Grid* SkinProviderLuca::impl_createGrid() const
{
  return Grid::fromSensors(1, cm::Rectangle(dx_,dy_), grid_source_.cbegin(), grid_source_.cend());
}

SkinAttributes SkinProviderLuca::impl_getAttributes() const
{
  SkinAttributes ret;
  ret.h = h_;
  ret.E = E_;
  ret.nu = nu_;
  ret.taxelRadius = taxelRadius_;
  return ret;
}

namespace details {

void check_dir_validity_for_luca(const std::string& dir_path)
{
  bfs::path d_path(dir_path);
  if (!is_directory(d_path)) {
    throw std::runtime_error(sb()
      << "Luca's test files' dir, '" << dir_path << "' cannot "
      << "be analysed. It is not a directory!"
    );
  }

  bfs::path reporttxt = d_path;
  reporttxt /= "report.txt";
  if (!is_regular_file(reporttxt)) {
    throw std::runtime_error(sb()
      << "Luca's test files' dir, does not contain a regular file "
      << "named 'report.txt'"
    );
  }

  bfs::path deltadir = d_path;
  deltadir /= "delta";
  if (!is_directory(deltadir)) {
    throw std::runtime_error(sb()
      << "Luca's test files' dir does not contain a directory "
      << "named 'delta'"
    );
  }

  std::vector<std::string> delta_fails;
  for (size_t num = 0; num < 50; ++num) {
    std::string fname = sb() << num << ".txt";
    bfs::path numtxt = deltadir;
    numtxt /= fname;
    if (!is_regular_file(numtxt)) {
      delta_fails.push_back(fname);
    }
  }

  if (!delta_fails.empty()) {
    throw std::runtime_error(sb()
      << "The following files are missing from Luca's test files' dir "
      << "'delta/' directory: "
      << [&]() {
          sb buf;
          for (auto &failname : delta_fails) {
            buf << "'" << failname << "' " ;
          }
          return (std::string)buf;
        }()
    );
  }
} /* check_dir_validity_for_luca() */

std::string extractToken(std::istream& stream, const char* token)
{
  std::string line;

  ExceptionGuard<std::runtime_error> EG(std::runtime_error(sb()
    << "SkinProviderLuca: could not extract token '"
    << token << "' from report.txt"
  ));
  std::string ret;
  do {
    getline(stream, line);
    size_t pos = line.find(token);
    if (pos == std::string::npos) {
      continue;
    }
    EG.disable();
    ret = line.substr(pos+std::strlen(token));
    break;
  } while (stream);
  EG.check();
  return ret;
}

void extractResolution(std::istream& stream, ReportTxtData& save_to)
{
  const char* token = "Risoluzione griglia:";
  std::stringstream ss;
  ss.str(extractToken(stream, token));
  char dummy_times;
  ss >> save_to.x_num;
  ss >> dummy_times;
  ss >> save_to.y_num;
} /* extractResolution() */

void extractElasticModulus(std::istream& stream, ReportTxtData& save_to)
{
  const char* token = "Modulo Young:";
  std::stringstream ss;
  ss.str(extractToken(stream, token));
  ss >> save_to.E;
}

void extractSkinThickness(std::istream& stream, ReportTxtData& save_to)
{
  const char* token = "h:";
  std::stringstream ss;
  ss.str(extractToken(stream, token));
  ss >> save_to.h;
}

void extractx_spacing(std::istream& stream, ReportTxtData& save_to)
{
  const char* token = "x_spacing:";
  std::stringstream ss;
  ss.str(extractToken(stream, token));
  ss >> save_to.x_spacing;
}

void extracty_spacing(std::istream& stream, ReportTxtData& save_to)
{
  const char* token = "y_spacing:";
  std::stringstream ss;
  ss.str(extractToken(stream, token));
  ss >> save_to.y_spacing;
}

void processReportTxt(const std::string& dir_path, ReportTxtData& save_to)
{
  bfs::path report_path = dir_path;
  report_path /= "report.txt";
  std::ifstream report(report_path.native());

  extractResolution(report, save_to);
  extractElasticModulus(report, save_to);
  extractSkinThickness(report, save_to);
  extractx_spacing(report, save_to);
  extracty_spacing(report, save_to);
} /* processReportTxt() */

std::vector<double> getAverageData(const std::string& root_dir, const size_t expected_num)
{
  std::vector<double> ret;
  ret.resize(expected_num);
  for (auto& v : ret) {
    v = 0;
  }

  bfs::path root = root_dir;
  const size_t num_files = 50;
  for (size_t i = 0; i < num_files; ++i) {
    bfs::path filepath = root;
    filepath /= "delta";
    filepath /= (std::string)(sb() << i << ".txt");
    std::ifstream file(filepath.native());
    if (!file) {
      throw std::runtime_error(sb()
        << "SkinProviderLuca: could not open file "
        << filepath.native() << " to read values."
      );
    }
    std::vector<double> vals = getDataFromOneFile(file, expected_num);
    if (vals.size() != expected_num) {
      throw std::runtime_error(sb()
        << "SkinProviderLuca: not enough values when processing file "
        << filepath.native() << ". Expected number: " << expected_num << " "
        << ", actual number: " << vals.size()
      );
    }
    for (size_t i_val = 0; i_val < expected_num; ++i_val) {
      ret.at(i_val) += vals.at(i_val) / (double)num_files;
    }
  }

  return ret;
} /* getAverageData */

std::vector<double> getDataFromOneFile(std::istream& datafile, const size_t expected_num)
{
  std::vector<double> ret; 
  
  ret.reserve(expected_num);

  std::string line;
  while (datafile) {
    getline(datafile, line);
    std::stringstream ss;
    ss.str(line);
    double temp;
    while (ss) {
      ss >> temp;
      if (ss) {
       ret.push_back(temp);
      }
    }
  }
  return ret;
} /* getDataFromOneFile */


} /* namespace details */
} /* namespace cm */
