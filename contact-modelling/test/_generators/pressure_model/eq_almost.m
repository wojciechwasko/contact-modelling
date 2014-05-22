function [ ret ] = eq_almost(p,q)
  eps = 10e-10;
  ret = 0;
  if abs(p-q) < eps
    ret = 1;
  end
end