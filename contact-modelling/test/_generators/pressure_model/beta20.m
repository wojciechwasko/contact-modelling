function [ ret ] = beta20(yp,a,b,x,y,z)
  ret = sqrt((a+x)^2 + z^2);
  if isnan(ret)
    disp('beta20: returning NaN')
  end
end