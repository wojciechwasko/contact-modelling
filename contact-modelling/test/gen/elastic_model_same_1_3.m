function [] = elastic_model_same_1_3(dx,dy)
  % Generates a 1x3 matrix (i.e. 3D forces, 1D displacements) for two points separated by dx in x
  % direction and dy in y direction. Assumes both dx and dy to be smaller than the epsilon used to
  % determine if points coincide.
  format long g
  h = 0.002
  G = 300000
  psi = 0.25
  node_area = 0.002

  z0h = sqrt(3/(2*pi) * node_area) % eq (20), Muscari et al. 2013

  zx = 0
  zy = 0
  zz = 9/(2*pi*G) * psi/z0h
