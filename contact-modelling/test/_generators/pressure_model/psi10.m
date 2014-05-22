function [ ret ] = psi10(yp,a,b,x,y,z)
  ret = (yp-y) / (r10(yp,a,b,x,y,z) + beta10(yp,a,b,x,y,z));
  if isnan(ret)
    disp('psi10: returning NaN')
  end
end