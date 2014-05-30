#ifndef LUCA_FIXTURES_HPP
#define LUCA_FIXTURES_HPP

#include <vector>
#include <stdexcept>
#include <fstream>
#include <boost/filesystem.hpp>

#include "custom_test_macros.hpp"

const char* report_txt_data = R"SAMPLE(

Forma indenter: Sfera   Risoluzione griglia: 10x10 punti
Modulo Young: 177000 Pascal

PHI: 0.25

h: 0.002

z0: 0.00106527

h/z0: 1.87745

x_spacing: 0.001

y_spacing: 0.002
)SAMPLE";

namespace bfs = boost::filesystem;

struct BadLucaEmptyDir {
  bfs::path luca_path;

  BadLucaEmptyDir()
  {
    luca_path = bfs::unique_path("luca_empty-%%%%-%%%%-%%%%-%%%%");
    if (!bfs::create_directory(luca_path)) {
      throw std::runtime_error(testimpl::sb()
        << "BadLucaEmptyDir: Could not create temporary directory at "
        << "'" << luca_path.native() << "'."
      );
    }
  }

  ~BadLucaEmptyDir()
  {
    bfs::remove_all(luca_path);  
  }
};

struct BadLucaOnlyReportTxt {
  bfs::path luca_path;

  BadLucaOnlyReportTxt()
  {
    luca_path = bfs::unique_path("luca_only_txt-%%%%-%%%%-%%%%-%%%%");
    if (!bfs::create_directory(luca_path)) {
      throw test_error(testimpl::sb()
        << "BadLucaOnlyReportTxt: Could not create temporary directory at "
        << "'" << luca_path.native() << "'."
      );
    }
    bfs::path report_txt = luca_path;
    report_txt /= "report.txt";
    std::ofstream report(report_txt.native(), std::ofstream::trunc);
    if (!report) {
      throw test_error(testimpl::sb()
        << "BadLucaOnlyReportTxt: Could not create report.txt at "
        << "'" << report_txt.native() << "'."
      );
    }
  }

  ~BadLucaOnlyReportTxt()
  {
    bfs::remove_all(luca_path);  
  }
};

struct BadLucaSomeMissing {
  bfs::path luca_path;

  BadLucaSomeMissing()
  {
    luca_path = bfs::unique_path("luca_only_txt-%%%%-%%%%-%%%%-%%%%");
    if (!bfs::create_directory(luca_path)) {
      throw test_error(testimpl::sb()
        << "BadLucaSomeMissing: Could not create temporary directory at "
        << "'" << luca_path.native() << "'."
      );
    }
    bfs::path report_txt = luca_path;
    report_txt /= "report.txt";
    std::ofstream report(report_txt.native(), std::ofstream::trunc);
    if (!report) {
      throw test_error(testimpl::sb()
        << "BadLucaSomeMissing: Could not create report.txt at "
        << "'" << report_txt.native() << "'."
      );
    }
    
    bfs::path delta = luca_path;
    delta /= "delta";
    if (!bfs::create_directory(delta)) {
      throw test_error(testimpl::sb()
        << "BadLucaSomeMissing: Could not create temporary directory at "
        << "'" << delta.native() << "'."
      );
    }

    for (size_t num = 4; num < 45; ++num) {
      bfs::path file = delta;
      file /= (std::string)(testimpl::sb() << num << ".txt");
      std::ofstream report(file.native(), std::ofstream::trunc);
      if (!report) {
        throw test_error(testimpl::sb()
          << "BadLucaSomeMissing: Could not create input file at "
          << "'" << file.native() << "'."
        );
      }
    }
  }

  ~BadLucaSomeMissing()
  {
    bfs::remove_all(luca_path);  
  }
};

struct GoodLuca {
  bfs::path luca_path;

  std::vector<double> generateValues()
  {
    std::vector<double> ret;
    const double delta_val = 1e-2;
    const double val_zero = 0.1;
    ret.reserve(10*10);
    for (size_t i = 0; i < 10*10; ++i) {
      ret.push_back(val_zero + delta_val * i);
    }
    return ret;
  }

  void writeValuesToFile(std::ostream& out, const std::vector<double>& vals)
  {
    if (vals.size() != 100) {
      throw test_error(testimpl::sb()
        << "writeValuesToFile: values size, " << vals.size() << " "
        << "is not equal to the desired size, 100 (10*10)"
      );
    }
    for (size_t iy = 0; iy < 10; ++iy) {
      for (size_t ix = 0; ix < 10; ++ix) {
        out << vals.at(iy*10+ix) << " ";
      }
      out << std::endl;
    }
  }

  GoodLuca()
  {
    luca_path = bfs::unique_path("luca_good-%%%%-%%%%-%%%%-%%%%");
    if (!bfs::create_directory(luca_path)) {
      throw test_error(testimpl::sb()
        << "GoodLuca: Could not create temporary directory at "
        << "'" << luca_path.native() << "'."
      );
    }
    bfs::path report_txt = luca_path;
    report_txt /= "report.txt";
    std::ofstream report(report_txt.native(), std::ofstream::trunc);
    if (!report) {
      throw test_error(testimpl::sb()
        << "GoodLuca: Could not create report.txt at "
        << "'" << report_txt.native() << "'."
      );
    }
    report << report_txt_data;
    
    bfs::path delta = luca_path;
    delta /= "delta";
    if (!bfs::create_directory(delta)) {
      throw test_error(testimpl::sb()
        << "GoodLuca: Could not create temporary directory at "
        << "'" << delta.native() << "'."
      );
    }

    for (size_t num = 0; num < 50; ++num) {
      bfs::path file = delta;
      file /= (std::string)(testimpl::sb() << num << ".txt");
      std::ofstream vals_file(file.native(), std::ofstream::trunc);
      if (!vals_file) {
        throw test_error(testimpl::sb()
          << "GoodLuca: Could not create input file at "
          << "'" << file.native() << "'."
        );
      }
      writeValuesToFile(vals_file, generateValues());
    }
  }

  ~GoodLuca()
  {
    bfs::remove_all(luca_path);  
  }
};


#endif /* LUCA_FIXTURES_HPP */
