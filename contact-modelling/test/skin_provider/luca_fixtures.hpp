#ifndef LUCA_FIXTURES_HPP
#define LUCA_FIXTURES_HPP

#include <stdexcept>
#include <fstream>
#include <boost/filesystem.hpp>

#include "custom_test_macros.hpp"


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

  GoodLuca()
  {
    luca_path = bfs::unique_path("luca_only_txt-%%%%-%%%%-%%%%-%%%%");
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
      std::ofstream report(file.native(), std::ofstream::trunc);
      if (!report) {
        throw test_error(testimpl::sb()
          << "GoodLuca: Could not create input file at "
          << "'" << file.native() << "'."
        );
      }
    }
  }

  ~GoodLuca()
  {
    bfs::remove_all(luca_path);  
  }
};


#endif /* LUCA_FIXTURES_HPP */
