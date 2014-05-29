#include "cm/skin_provider/luca.hpp"

#include <stdexcept>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>

#include "cm/details/string.hpp"
#include "cm/details/exception.hpp"

namespace bfs = boost::filesystem;

namespace cm {

using details::sb;

SkinProviderLuca::SkinProviderLuca(const std::string& dir_path)
  : SkinProviderInterface(1)
{
  details::check_dir_validity_for_luca(dir_path); 
}

void SkinProviderLuca::impl_update(target_values_type& target_vec) const
{

}

Grid* SkinProviderLuca::impl_createGrid() const
{

}

SkinAttributes SkinProviderLuca::impl_getAttributes() const
{

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

} /* namespace details */
} /* namespace cm */
