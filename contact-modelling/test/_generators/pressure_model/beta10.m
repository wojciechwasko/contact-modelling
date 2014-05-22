function [ ret ] = beta10(yp,a,b,x,y,z)
  ret = sqrt((a-x)^2 + z^2);
  if isnan(ret)
    disp('beta10: returning NaN')
  end
end