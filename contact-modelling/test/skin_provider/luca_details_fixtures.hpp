#ifndef LUCA_FIXTURES_HPP
#define LUCA_FIXTURES_HPP

#include <stdexcept>
#include <fstream>
#include <boost/filesystem.hpp>

#include "custom_test_macros.hpp"

namespace bfs = boost::filesystem;

const char* sample_report_txt_data = R"SAMPLE(

Forma indenter: Sfera   Risoluzione griglia: 40x40 punti
Modulo Young: 177000 Pascal

PHI: 0.25

h: 0.002

z0: 0.00106527

h/z0: 1.87745

x_spacing: 0.00165641

y_spacing: 0.00143487
)SAMPLE";

const char* incomplete_report_txt_data = R"SAMPLE(

Forma indenter: Sfera   Risoluzione griglia: 40x40 punti
Modulo Young: 177000 Pascal

PHI: 0.25

h: 0.002

z0: 0.00106527

h/z0: 1.87745


y_spacing: 0.00143487
)SAMPLE";

struct ReportTxt {
  bfs::path luca_path;

  ReportTxt()
  {
    luca_path = bfs::unique_path("luca_report_txt-%%%%-%%%%-%%%%-%%%%");
    if (!bfs::create_directory(luca_path)) {
      throw test_error(testimpl::sb()
        << "ReportTxt: Could not create temporary directory at "
        << "'" << luca_path.native() << "'."
      );
    }
    bfs::path report_txt = luca_path;
    report_txt /= "report.txt";
    std::ofstream report(report_txt.native(), std::ofstream::trunc);
    if (!report) {
      throw test_error(testimpl::sb()
        << "ReportTxt: Could not create report.txt at "
        << "'" << report_txt.native() << "'."
      );
    }

    report << sample_report_txt_data;
  }

  ~ReportTxt()
  {
    bfs::remove_all(luca_path);  
  }
};

struct IncompleteReportTxt {
  bfs::path luca_path;

  IncompleteReportTxt()
  {
    luca_path = bfs::unique_path("luca_incomplete_report_txt-%%%%-%%%%-%%%%-%%%%");
    if (!bfs::create_directory(luca_path)) {
      throw test_error(testimpl::sb()
        << "IncompleteReportTxt: Could not create temporary directory at "
        << "'" << luca_path.native() << "'."
      );
    }
    bfs::path report_txt = luca_path;
    report_txt /= "report.txt";
    std::ofstream report(report_txt.native(), std::ofstream::trunc);
    if (!report) {
      throw test_error(testimpl::sb()
        << "IncompleteReportTxt: Could not create report.txt at "
        << "'" << report_txt.native() << "'."
      );
    }

    report << incomplete_report_txt_data;
  }

  ~IncompleteReportTxt()
  {
    bfs::remove_all(luca_path);  
  }
};

#endif /* LUCA_FIXTURES_HPP */
