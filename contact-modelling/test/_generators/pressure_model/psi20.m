function [ ret ] = psi20(yp,a,b,x,y,z)
  ret = (yp-y) / (r20(yp,a,b,x,y,z) + beta20(yp,a,b,x,y,z));
  if isnan(ret)
    disp('psi20: returning NaN')
  end
end