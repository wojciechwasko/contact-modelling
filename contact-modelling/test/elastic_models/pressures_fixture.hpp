#ifndef PRESSURES_FIXTURE_HPP
#define PRESSURES_FIXTURE_HPP

struct MockCell {
  MockCell(double x, double y) : relative_position{x,y} {};
  std::array<double, 2> relative_position;
};
struct MockGrid {
  std::vector<MockCell> cells;
  std::unique_ptr<cm::Grid> create(size_t dim, const cm::GridCellShape& cell_shape)
  {
    return std::unique_ptr<cm::Grid>(cm::Grid::fromSensors(dim, cell_shape, cells.cbegin(), cells.cend()));
  }
};


struct LoveFixture {
  double dx;
  double dy;
  cm::SkinAttributes skin_attr;
  std::unique_ptr<cm::Grid> press_grid;
  std::unique_ptr<cm::Grid> disps_grid;
  arma::mat press2disps;
  arma::mat disps2press;
  std::vector<double> expected_disps;
  std::vector<double> expected_press;

  LoveFixture() {
    dx = 0.010000000000000000208;
    dy = 0.0050000000000000001041;
    MockGrid press_source;
    press_source.cells.push_back({-0.010000000000000000208,-0.0050000000000000001041});
    press_source.cells.push_back({-0.010000000000000000208,0.0050000000000000001041});
    press_source.cells.push_back({0.010000000000000000208,-0.0050000000000000001041});
    press_source.cells.push_back({0.010000000000000000208,0.0050000000000000001041});
    press_grid = press_source.create(1, cm::Rectangle({2*dx,2*dy}));
    MockGrid disps_source;
    disps_source.cells.push_back({-0.014999999999999999445,-0.0070000000000000001457});
    disps_source.cells.push_back({0,0});
    disps_source.cells.push_back({0.010000000000000000208,0.0050000000000000001041});
    disps_grid = disps_source.create(1, cm::Rectangle({0.001,0.002}));
    skin_attr.h           = 0.0020000000000000000416;
    skin_attr.E           = 210000;
    skin_attr.nu          = 0.48999999999999999112;
    skin_attr.taxelRadius = 0;
    press2disps 
       << 2.459889116577281391e-09 << -2.1610796693331085076e-10 << -3.7507481327541080257e-11 << -2.5150807594233025381e-11 << arma::endr
       << 3.137617399973065516e-10 << 3.137617399973065516e-10 << 3.1376173999729000799e-10 << 3.1376173999729993416e-10 << arma::endr
       << -5.3242187328800691569e-11 << -8.8037211199354624369e-11 << -3.98913429923369159e-10 << 2.1249779933444787564e-09 << arma::endr;
    disps2press 
       << 387011438.20855551958 << 141875358.77385008335 << -13995419.351145911962 << arma::endr
       << -189503898.19852006435 << 1336658241.4480600357 << -166734005.19556170702 << arma::endr
       << -167844429.80322176218 << 1388928221.5250482559 << -237938130.37430202961 << arma::endr
       << -29663110.2068230398 << 319669917.15979677439 << 418667554.92100584507 << arma::endr;
    disps_grid->setValue(0, 0, 1.0663887074238199299e-09);
    expected_disps.push_back(1.0663887074238199299e-09);
    disps_grid->setValue(1, 0, 7.1883706892340101108e-10);
    expected_disps.push_back(7.1883706892340101108e-10);
    disps_grid->setValue(2, 0, 1.1842574209913100869e-09);
    expected_disps.push_back(1.1842574209913100869e-09);
    press_grid->setValue(0, 0, 0.49811571517642544427);
    expected_press.push_back(0.49811571517642544427);
    press_grid->setValue(1, 0, 0.56129869239868845909);
    expected_press.push_back(0.56129869239868845909);
    press_grid->setValue(2, 0, 0.53764569052734711985);
    expected_press.push_back(0.53764569052734711985);
    press_grid->setValue(3, 0, 0.69396833936599844339);
    expected_press.push_back(0.69396833936599844339);

  }
  ~LoveFixture() {}
};


#endif /* PRESSURES_FIXTURE_HPP */
