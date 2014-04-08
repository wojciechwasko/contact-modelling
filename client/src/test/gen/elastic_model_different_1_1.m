function [] = elastic_model_different_1_1(dx,dy)
  % Generates a 1x1 matrix (i.e. 1D forces, 1D displacements) for two points separated by dx in x
  % direction and dy in y direction. Assumes both dx and dy to be larger than the epsilon used to
  % determine if points coincide.
  format long g
  h = 0.002
  G = 300000
  zz = (1/((dx^2+dy^2)^0.5) - (dx^2+dy^2+2*h^2)/((dx^2+dy^2+h^2)^1.5)) / (4*pi*G)
