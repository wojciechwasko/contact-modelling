function [ ret ] = r20(yp,a,b,x,y,z)
  ret = sqrt((a+x)^2 + (yp-y)^2 + z^2);
  if isnan(ret)
    disp('r20: returning NaN')
  end
end