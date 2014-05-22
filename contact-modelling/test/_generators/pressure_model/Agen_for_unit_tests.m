function [] = Agen_for_unit_tests()
% Generates fixture for pressure.cpp unit tests
  format long
  dx = 0.010;
  a = 2*dx;
  dy = 0.005;
  b = 2*dy;
  E  = 210000;
  nu = 0.49;
  h  = 0.002;
  taxelRadius = 0;

  press_cells_locs = [-0.01, -0.01, 0.01, 0.01; -0.005, 0.005, -0.005, 0.005];
  disps_cells_locs = [-0.015, 0, 0.01; -0.007, 0, 0.005];

  mat_press2disps = zeros(size(disps_cells_locs,2), size(press_cells_locs,2));
  for ip = 1:size(press_cells_locs,2)
    for id = 1:size(disps_cells_locs,2)
      tmpx = disps_cells_locs(1,id) - press_cells_locs(1,ip);
      tmpy = disps_cells_locs(2,id) - press_cells_locs(2,ip);
      mat_press2disps(id,ip) = Zlove_w(1, dx, dy, E, nu, tmpx, tmpy, 0) ...
                             - Zlove_w(1, dx, dy, E, nu, tmpx, tmpy, h);
    end
  end

  mat_disps2press = pinv(mat_press2disps);

  % The idea is: 
  %  # given some displacements values,
  %  # calculate pressures through pinv (minimum-norm solution)
  %  # assign those displacements and those pressures to the values of meshes.
  %    Store copies in std::vector<double>
  %    That way, regardless if we (in C++) go from displacements to pressures or
  %    the other way around, the other mesh will have correct expected values.
  disps_values = [1.06638870742382e-09; 7.18837068923401e-10; 1.18425742099131e-09];
  press_values = mat_disps2press * disps_values;

  disp('');
  disp('');

  disp('struct MockCell {');
  disp('  MockCell(double x, double y) : relative_position{x,y} {};');
  disp('  std::array<double, 2> relative_position;');
  disp('};');
  disp('');
  disp('struct MockGrid {');
  disp('  std::vector<MockCell> cells;');
  disp('');
  disp('  std::unique_ptr<cm::Grid> create(size_t dim, const cm::GridCellShape& cell_shape)');
  disp('  {');
  disp('    return std::unique_ptr<cm::Grid>(cm::Grid::fromSensors(dim, cell_shape, cells.cbegin(), cells.cend()));');
  disp('  }');
  disp('};');

  disp(sprintf('\n\nstruct LoveFixture {'));
  disp('  double dx;');
  disp('  double dy;');
  disp('  cm::SkinAttributes skin_attr;');
  disp('  std::unique_ptr<cm::Grid> press_grid;');
  disp('  std::unique_ptr<cm::Grid> disps_grid;');
  disp('  arma::mat press2disps;');
  disp('  arma::mat disps2press;');
  disp('  std::vector<double> expected_disps;');
  disp('  std::vector<double> expected_press;');
  disp(sprintf('\n  LoveFixture() {'));
  disp(sprintf('    dx = %.20g;', dx));
  disp(sprintf('    dy = %.20g;', dy));

  disp('');
  disp('    MockGrid press_source;');
  for i=1:size(press_cells_locs, 2)
    disp(sprintf('    press_source.cells.push_back({%.20g,%.20g});', ...
      press_cells_locs(1,i), press_cells_locs(2,i)));
  end
  disp(sprintf('    press_grid = press_source.create(1, cm::Rectangle({2*dx,2*dy}));'));

  disp('');
  disp('    MockGrid disps_source;');
  for i=1:size(disps_cells_locs, 2)
    disp(sprintf('    disps_source.cells.push_back({%.20g,%.20g});', ...
      disps_cells_locs(1,i), disps_cells_locs(2,i)));
  end
  disp(sprintf('    disps_grid = disps_source.create(1, cm::Rectangle({0.001,0.002}));'));

  disp('');
  disp(sprintf('    skin_attr.h           = %.20g;', h ));
  disp(sprintf('    skin_attr.E           = %.20g;', E ));
  disp(sprintf('    skin_attr.nu          = %.20g;', nu));
  disp(sprintf('    skin_attr.taxelRadius = %.20g;', taxelRadius));

  disp('');
  disp(mat2armadillo_fill('press2disps', mat_press2disps, '    '));
  disp(mat2armadillo_fill('disps2press', mat_disps2press, '    '));

  disp('');
  for i=1:size(disps_values,1)
    disp(sprintf('    disps_grid->setValue(%d, 0, %.20g);', i-1, disps_values(i)));
    disp(sprintf('    expected_disps.push_back(%.20g);', disps_values(i)));
  end

  disp('');
  for i=1:size(press_values,1)
    disp(sprintf('    press_grid->setValue(%d, 0, %.20g);', i-1, press_values(i)));
    disp(sprintf('    expected_press.push_back(%.20g);', press_values(i)));
  end
  
  disp(sprintf('\n  }\n  ~LoveFixture() {}\n};'));
end

function [s] = mat2armadillo_fill(name, mat, indent)
  s = sprintf('%s%s ', indent, name);
  for r=1:size(mat,1)
    s = sprintf('%s\n%s  ', s, indent);
    for c=1:size(mat,2)
      s = sprintf('%s << %.20g', s, mat(r,c));
    end
    s = sprintf('%s << arma::endr', s);
  end
  s = sprintf('%s;',s);
end
