function [] = elastic_model_different_3_3(dx,dy)
  % Generates a 3x3 matrix (i.e. 3D forces, 3D displacements) for two points separated by dx in x
  % direction and dy in y direction. Assumes both dx and dy to be larger than the epsilon used to
  % determine if points coincide.
  format long g
  h = 0.002
  G = 300000
  xx = ((2*dx^2 + dy^2)/((dx^2 + dy^2)^1.5) - (2*dx^2 + dy^2 + h^2)/((dx^2 + dy^2 + h^2)^1.5))/(4*pi*G)
  yx = ( (dx * dy) / ((dx^2 + dy^2)^1.5) - (dx * dy) / ((dx^2 + dy^2 + h^2)^1.5)) / (4*pi*G)
  zx = - ((dx * h) / ((dx^2 + dy^2 + h^2)^1.5)) / (4*pi*G)
  xy = ( (dx*dy)/((dx^2+dy^2)^1.5) - (dx*dy)/((dx^2+dy^2+h^2)^1.5) ) / (4*pi*G)
  yy = ((dx^2+2*dy^2)/((dx^2+dy^2)^1.5) - (dx^2 + 2*dy^2 + h^2)/((dx^2 + dy^2 + h^2)^1.5)) /(4*pi*G)
  zy = - ((dy*h)/((dx^2 + dy^2 + h^2)^1.5)) / (4*pi*G)
  xz = - ((dx * h) / ((dx^2 + dy^2 + h^2)^1.5)) / (4*pi*G)
  yz = - ((dy * h) / ((dx^2 + dy^2 + h^2)^1.5)) / (4*pi*G)
  zz = (1/((dx^2+dy^2)^0.5) - (dx^2+dy^2+2*h^2)/((dx^2+dy^2+h^2)^1.5)) / (4*pi*G)
