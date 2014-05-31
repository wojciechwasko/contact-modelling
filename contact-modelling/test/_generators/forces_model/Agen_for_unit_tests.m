function [] = Agen_for_unit_tests()
  % L - number of forces locations
  % K - number of displacements locations
  global xs_d ys_d xs_f ys_f L K
  global F11 D11 C11 Cp11
  global F13 D13 C13 Cp13
  global F31 D31 C31 Cp31
  global F33 D33 C33 Cp33

  L       =  7;
  K       =  9;

  % we generate two stripes, one at an angle of 45 deg (disps mesh)
  % and one at smaller angle (forces mesh). 
  % Something like this:
  % ______________________
  % |                    |
  % |                 +  |
  % |             +      |
  % | ********#********* |
  % |     +              |
  % | +                  |
  % |____________________|

  xs_d = linspace(-0.005, 0.005, K);
  ys_d = linspace(-0.005, 0.005, K);
  xs_f = linspace(-0.005, 0.005, L);
  ys_f = linspace(-0.002, 0.002, L);

  dx_f = xs_f(2) - xs_f(1);
  dy_f = ys_f(2) - ys_f(1);

  E = 210000;
  h = 0.002;
  s = dx_f * dy_f;
  [F11, D11, C11, Cp11] = generate_single(1,1, E, h, s);
  [F13, D13, C13, Cp13] = generate_single(1,3, E, h, s);
  [F31, D31, C31, Cp31] = generate_single(3,1, E, h, s);
  [F33, D33, C33, Cp33] = generate_single(3,3, E, h, s);

  print_result(dx_f, dy_f, E, h, ...
    F11,D11,C11,Cp11, ...
    F13,D13,C13,Cp13, ...
    F31,D31,C31,Cp31, ...
    F33,D33,C33,Cp33);

end

function [] = print_result(dxf,dyf,E,h, F11,D11,C11,Cp11, F13,D13,C13,Cp13, ...
  F31,D31,C31,Cp31, F33,D33,C33,Cp33)
  global xs_d ys_d xs_f ys_f L K

  disp('/**');
  disp(' * Generated by:');
  disp(' * (PROJECT_ROOT)/contact-modelling/test/_generators/forces_model/Agen_for_unit_tests.m');
  disp(' */');

  disp('');
  disp('');
  disp('#include <utility>'); % for std::move()

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

  disp('');
  disp('');
  disp('struct BoussinesqFixture {');
%  disp('  double dx;');
%  disp('  double dy;');
  disp('  cm::SkinAttributes skin_attr;');
  disp('  std::unique_ptr<cm::Grid> force11_grid;');
  disp('  std::unique_ptr<cm::Grid> force13_grid;');
  disp('  std::unique_ptr<cm::Grid> force31_grid;');
  disp('  std::unique_ptr<cm::Grid> force33_grid;');
  disp('  std::unique_ptr<cm::Grid> disps11_grid;');
  disp('  std::unique_ptr<cm::Grid> disps13_grid;');
  disp('  std::unique_ptr<cm::Grid> disps31_grid;');
  disp('  std::unique_ptr<cm::Grid> disps33_grid;');
  disp('  arma::mat force1_disps1;');
  disp('  arma::mat disps1_force1;');
  disp('  arma::mat force3_disps1;');
  disp('  arma::mat disps1_force3;');
  disp('  arma::mat force1_disps3;');
  disp('  arma::mat disps3_force1;');
  disp('  arma::mat force3_disps3;');
  disp('  arma::mat disps3_force3;');
  disp('  double x_for_bouss;');
  disp('  double y_for_bouss;');
  disp('  double bouss_xx_v;');
  disp('  double bouss_xy_v;');
  disp('  double bouss_xz_v;');
  disp('  double bouss_yx_v;');
  disp('  double bouss_yy_v;');
  disp('  double bouss_yz_v;');
  disp('  double bouss_zx_v;');
  disp('  double bouss_zy_v;');
  disp('  double bouss_zz_v;');


  disp('  double val_for_z0h;');
  disp('  double z0h_from_val;');

  disp('  double val_for_psi;');
  disp('  double psi_from_val;');

  disp('  double s_for_appro;');
  disp('  double appro_xx_v;');
  disp('  double appro_yy_v;');
  disp('  double appro_zz_v;');
  disp('');
  disp('  BoussinesqFixture() {');
  disp('');
  disp(sprintf('    skin_attr.h           = %.5g;', h ));
  disp(sprintf('    skin_attr.E           = %.5g;', E ));
  disp(sprintf('    skin_attr.nu          = %.5g;', 0.5));
  disp(sprintf('    skin_attr.taxelRadius = %.5g;', 1e-3));
  disp('');
  disp('    MockGrid disps_source;');
  for i=1:size(xs_d, 2)
    disp(sprintf('    disps_source.cells.push_back({%.20g,%.20g});', ...
      xs_d(i), ys_d(i)));
  end
  disp('');
  disp('    MockGrid force_source;');
  for i=1:size(xs_f, 2)
    disp(sprintf('    force_source.cells.push_back({%.20g,%.20g});', ...
      xs_f(i), ys_f(i)));
  end

  disp('');
  disp(sprintf('    force11_grid = force_source.create(1, cm::Rectangle({%.20g, %.20g}));', ...
    dxf,dyf));
  disp(sprintf('    force13_grid = force_source.create(1, cm::Rectangle({%.20g, %.20g}));', ...
    dxf,dyf));
  disp(sprintf('    force31_grid = force_source.create(3, cm::Rectangle({%.20g, %.20g}));', ...
    dxf,dyf));
  disp(sprintf('    force33_grid = force_source.create(3, cm::Rectangle({%.20g, %.20g}));', ...
    dxf,dyf));

  disp('');
  % this will make sure correct dx/dy are taken in the C++ implementation
  % disps' dx dy should not be used in the algorithm
  disp(sprintf('    disps11_grid = disps_source.create(1, cm::Rectangle({%.20g, %.20g}));', ...
    0.1*dxf, 0.1*dyf));
  disp(sprintf('    disps13_grid = disps_source.create(3, cm::Rectangle({%.20g, %.20g}));', ...
    0.1*dxf, 0.1*dyf));
  disp(sprintf('    disps31_grid = disps_source.create(1, cm::Rectangle({%.20g, %.20g}));', ...
    0.1*dxf, 0.1*dyf));
  disp(sprintf('    disps33_grid = disps_source.create(3, cm::Rectangle({%.20g, %.20g}));', ...
    0.1*dxf, 0.1*dyf));

  disp('');
  disp(grid_set_values('force11_grid', F11, '    '));

  disp('');
  disp(grid_set_values('force13_grid', F13, '    '));

  disp('');
  disp(grid_set_values('force31_grid', F31, '    '));

  disp('');
  disp(grid_set_values('force33_grid', F33, '    '));

  disp('');
  disp(grid_set_values('disps11_grid', D11, '    '));

  disp('');
  disp(grid_set_values('disps13_grid', D13, '    '));

  disp('');
  disp(grid_set_values('disps31_grid', D31, '    '));

  disp('');
  disp(grid_set_values('disps33_grid', D33, '    '));

  disp('');

  disp('');
  disp(mat2armadillo_fill('force1_disps1', C11, '    '));

  disp('');
  disp(mat2armadillo_fill('disps1_force1', Cp11, '    '));

  disp('');
  disp(mat2armadillo_fill('force1_disps3', C13, '    '));

  disp('');
  disp(mat2armadillo_fill('disps3_force1', Cp13, '    '));

  disp('');
  disp(mat2armadillo_fill('force3_disps1', C31, '    '));

  disp('');
  disp(mat2armadillo_fill('disps1_force3', Cp31, '    '));

  disp('');
  disp(mat2armadillo_fill('force3_disps3', C33, '    '));

  disp('');
  disp(mat2armadillo_fill('disps3_force3', Cp33, '    '));

  x_for_bouss = 1e-4;
  y_for_bouss = 1e-6;
  disp('');
  disp('');
  disp(sprintf('    x_for_bouss = %.20g;', x_for_bouss));
  disp(sprintf('    y_for_bouss = %.20g;', y_for_bouss));
  disp(sprintf('    bouss_xx_v = %.20g;', bouss_xx(E,h,x_for_bouss,y_for_bouss)));
  disp(sprintf('    bouss_xy_v = %.20g;', bouss_xy(E,h,x_for_bouss,y_for_bouss)));
  disp(sprintf('    bouss_xz_v = %.20g;', bouss_xz(E,h,x_for_bouss,y_for_bouss)));
  disp(sprintf('    bouss_yx_v = %.20g;', bouss_yx(E,h,x_for_bouss,y_for_bouss)));
  disp(sprintf('    bouss_yy_v = %.20g;', bouss_yy(E,h,x_for_bouss,y_for_bouss)));
  disp(sprintf('    bouss_yz_v = %.20g;', bouss_yz(E,h,x_for_bouss,y_for_bouss)));
  disp(sprintf('    bouss_zx_v = %.20g;', bouss_zx(E,h,x_for_bouss,y_for_bouss)));
  disp(sprintf('    bouss_zy_v = %.20g;', bouss_zy(E,h,x_for_bouss,y_for_bouss)));
  disp(sprintf('    bouss_zz_v = %.20g;', bouss_zz(E,h,x_for_bouss,y_for_bouss)));

  val_for_z0h = 1e-3;
  disp(sprintf('    val_for_z0h = %.20g;', val_for_z0h));
  disp(sprintf('    z0h_from_val = %.20g;', zoh(val_for_z0h)));

  val_for_psi = 1e-6;
  disp(sprintf('    val_for_psi = %.20g;', val_for_psi));
  disp(sprintf('    psi_from_val = %.20g;', psi(val_for_psi)));

  s_for_appro = 1e-3;
  disp(sprintf('    s_for_appro = %.20g;', s_for_appro));
  disp(sprintf('    appro_xx_v = %.20g;', appro_xx(E,h,s_for_appro)));
  disp(sprintf('    appro_yy_v = %.20g;', appro_yy(E,h,s_for_appro)));
  disp(sprintf('    appro_zz_v = %.20g;', appro_zz(E,h,s_for_appro)));

  disp('');
  disp('');
  disp(sprintf('\n  }\n  ~BoussinesqFixture() {}\n};'));

end

function [s] = grid_set_values(name, values, indent)
  values = values(:);
  s = '';
  s = sprintf('%s%sstd::vector<double> tmp_%s;\n', s, indent, name);
  s = sprintf('%s%stmp_%s.reserve(%d);\n', s, indent, name, size(values,1));
  for i = 1:size(values,1)
    s = sprintf('%s%stmp_%s.push_back(%.20g);\n', ...
      s, indent, name, values(i));
  end
  s = sprintf('%s%s%s->setRawValues(std::move(tmp_%s));\n', ...
    s, indent, name, name);
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

function [F, D, C, Cpinv] = generate_single(F_dim, D_dim, E, h, s)
  global xs_d ys_d xs_f ys_f L K

  F = rand(F_dim * L, 1);
  C = nan * ones(D_dim * K, F_dim * L);

  % damn this stupid Matlab's 1-indexing.

  for f=1:L
    for d=1:K
      x = xs_d(d) - xs_f(f);
      y = ys_d(d) - ys_f(f);

      switch F_dim
      case 1
        switch D_dim
        case 1
          if (x == 0 && y == 0)
            C(d, f) = appro_zz(E, h, s);
          else
            zz_b = bouss_zz(E, h, x, y);
            zz_a = appro_zz(E, h, s);
            if abs(zz_a) < abs(zz_b)
              C(d, f) = zz_a;
            else
              C(d, f) = zz_b;
            end
          end
        case 3
          if (x == 0 && y == 0)
            C(3*(d-1)+1  , f) = 0;
            C(3*(d-1)+1+1, f) = 0;
            C(3*(d-1)+1+2, f) = appro_zz(E, h, s);
          else
            zz_b = bouss_zz(E, h, x, y);
            zz_a = appro_zz(E, h, s);
            if abs(zz_a) < abs(zz_b)
              % approximate solution, other components == 0
              C(3*(d-1)+1  , f) = 0;
              C(3*(d-1)+1+1, f) = 0;
              C(3*(d-1)+1+2, f) = zz_a;
            else
              C(3*(d-1)+1  , f) = bouss_xz(E, h, x, y);
              C(3*(d-1)+1+1, f) = bouss_yz(E, h, x, y);
              C(3*(d-1)+1+2, f) = zz_b;
            end
          end
        end
      case 3
        switch D_dim
        case 1
          if (x == 0 && y == 0)
            C(d, 3*(f-1)+1  ) = 0;
            C(d, 3*(f-1)+1+1) = 0;
            C(d, 3*(f-1)+1+2) = appro_zz(E, h, s);
          else
            zz_b = bouss_zz(E, h, x, y);
            zz_a = appro_zz(E, h, s);
            if abs(zz_a) < abs(zz_b)
              % approximate solution, other components == 0
              C(d, 3*(f-1)+1  ) = 0;
              C(d, 3*(f-1)+1+1) = 0;
              C(d, 3*(f-1)+1+2) = zz_a;
            else
              C(d, 3*(f-1)+1  ) = bouss_zx(E, h, x, y);
              C(d, 3*(f-1)+1+1) = bouss_zy(E, h, x, y);
              C(d, 3*(f-1)+1+2) = zz_b;
            end
          end
        case 3
          if (x == 0 && y == 0)
            C(3*(d-1)+1  , 3*(f-1)+1  ) = appro_xx(E, h, s);
            C(3*(d-1)+1  , 3*(f-1)+1+1) = 0;
            C(3*(d-1)+1  , 3*(f-1)+1+2) = 0;
            C(3*(d-1)+1+1, 3*(f-1)+1  ) = 0;
            C(3*(d-1)+1+1, 3*(f-1)+1+1) = appro_yy(E, h, s);
            C(3*(d-1)+1+1, 3*(f-1)+1+2) = 0;
            C(3*(d-1)+1+2, 3*(f-1)+1  ) = 0;
            C(3*(d-1)+1+2, 3*(f-1)+1+1) = 0;
            C(3*(d-1)+1+2, 3*(f-1)+1+2) = appro_zz(E, h, s);
          else
            xx_b = bouss_xx(E, h, x, y);
            xx_a = appro_xx(E, h, s);
            if abs(xx_a) < abs(xx_b)
              % approximate solution, other components == 0
              C(3*(d-1)+1  , 3*(f-1)+1  ) = xx_a;
              C(3*(d-1)+1  , 3*(f-1)+1+1) = 0;
              C(3*(d-1)+1  , 3*(f-1)+1+2) = 0;
            else
              C(3*(d-1)+1  , 3*(f-1)+1  ) = xx_b;
              C(3*(d-1)+1  , 3*(f-1)+1+1) = bouss_xy(E, h, x, y);
              C(3*(d-1)+1  , 3*(f-1)+1+2) = bouss_xz(E, h, x, y);
            end
  
            yy_b = bouss_yy(E, h, x, y);
            yy_a = appro_yy(E, h, s);
            if abs(yy_a) < abs(yy_b)
              % approximate solution, other components == 0
              C(3*(d-1)+1+1, 3*(f-1)+1  ) = 0;
              C(3*(d-1)+1+1, 3*(f-1)+1+1) = yy_a;
              C(3*(d-1)+1+1, 3*(f-1)+1+2) = 0;
            else
              C(3*(d-1)+1+1, 3*(f-1)+1  ) = bouss_yx(E, h, x, y);
              C(3*(d-1)+1+1, 3*(f-1)+1+1) = yy_b;
              C(3*(d-1)+1+1, 3*(f-1)+1+2) = bouss_yz(E, h, x, y);
            end
  
            zz_b = bouss_zz(E, h, x, y);
            zz_a = appro_zz(E, h, s);
            if abs(zz_a) < abs(zz_b)
              % approximate solution, other components == 0
              C(3*(d-1)+1+2, 3*(f-1)+1  ) = 0;
              C(3*(d-1)+1+2, 3*(f-1)+1+1) = 0;
              C(3*(d-1)+1+2, 3*(f-1)+1+2) = zz_a;
            else
              C(3*(d-1)+1+2, 3*(f-1)+1  ) = bouss_zx(E, h, x, y);
              C(3*(d-1)+1+2, 3*(f-1)+1+1) = bouss_zy(E, h, x, y);
              C(3*(d-1)+1+2, 3*(f-1)+1+2) = zz_b;
            end
          end
        end
      end
    end
  end

  Cpinv = pinv(C, 1e-10);
  D = C * F;
end

function [ret] = psi(v)
  ret = (0.2431 * v - 0.1814) / v;
end

function [ret] = zoh(s)
  ret = sqrt(s*3/(2*pi));
end

function [ret] = bouss_xx(E, h, x, y)
  ret = (3/(4*pi*E)) * ( ...
      (2*x^2 + y^2)/(sqrt(x^2 + y^2)^3) ...
    - (2*x^2 + y^2 + h^2)/(sqrt(x^2+y^2+h^2)^3) ...
  );
end

function [ret] = appro_xx(E, h, s)
  z_zeroh = zoh(s);
  ret = (9/(4*pi*E)) * psi(h/z_zeroh)/z_zeroh;
end

function [ret] = bouss_yy(E, h, x, y)
  ret = (3/(4*pi*E)) * ( ...
      (x^2 + 2*y^2)/(sqrt(x^2 + y^2)^3) ...
    - (x^2 + 2*y^2 + h^2)/(sqrt(x^2+y^2+h^2)^3) ...
  );
end

function [ret] = appro_yy(E, h, s)
  z_zeroh = zoh(s);
  ret = (9/(4*pi*E)) * psi(h/z_zeroh)/z_zeroh;
end

function [ret] = bouss_zz(E, h, x, y)
  ret = (3/(4*pi*E)) * ( ...
      1/sqrt(x^2 + y^2) ...
    - (x^2 + y^2 + 2*h^2)/(sqrt(x^2+y^2+h^2)^3) ...
  );
end

function [ret] = appro_zz(E, h, s)
  z_zeroh = zoh(s);
  ret = (9/(2*pi*E)) * psi(h/z_zeroh)/z_zeroh;
end

function [ret] = bouss_xy(E, h, x, y)
  ret = (3/(4*pi*E)) * ( ...
      (x * y)/(sqrt(x^2+y^2)^3) ...
    - (x * y)/(sqrt(x^2+y^2+h^2)^3) ...
  );
end

function [ret] = bouss_xz(E, h, x, y)
  ret = (3/(4*pi*E)) * ( ...
    - (x * h)/(sqrt(x^2+y^2+h^2)^3) ...
  );
end

function [ret] = bouss_yx(E, h, x, y)
  ret = (3/(4*pi*E)) * ( ...
      (x * y)/(sqrt(x^2+y^2)^3) ...
    - (x * y)/(sqrt(x^2+y^2+h^2)^3) ...
  );
end

function [ret] = bouss_yz(E, h, x, y)
  ret = (3/(4*pi*E)) * ( ...
    - (y * h)/(sqrt(x^2+y^2+h^2)^3) ...
  );
end

function [ret] = bouss_zx(E, h, x, y)
  ret = (3/(4*pi*E)) * ( ...
    - (x * h)/(sqrt(x^2+y^2+h^2)^3) ...
  );
end

function [ret] = bouss_zy(E, h, x, y)
  ret = (3/(4*pi*E)) * ( ...
    - (y * h)/(sqrt(x^2+y^2+h^2)^3) ...
  );
end
