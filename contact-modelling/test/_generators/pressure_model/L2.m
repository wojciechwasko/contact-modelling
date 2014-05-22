function [ ret ] = L2(yp,a,b,x,y,z)
  dy = yp-y;
  
  if eq_almost(z,0)
    ret = safe_log(dy, (-a-x+r20(yp,a,b,x,y,z))) - dy + safe_log(-a-x, (1+psi20(yp,a,b,x,y,z))/(1-psi20(yp,a,b,x,y,z)));
  else
    if eq_almost(x,-a)
      atan_arg = psi20(yp,a,b,x,y,z);
    else
      atan_arg = z * psi20(yp,a,b,x,y,z) / (-a-x + beta20(yp,a,b,x,y,z));
    end
    ret = safe_log(dy, (-a-x+r20(yp,a,b,x,y,z))) - dy + safe_log(-a-x, (1+psi20(yp,a,b,x,y,z))/(1-psi20(yp,a,b,x,y,z))) + 2*z*atan(atan_arg);
  end
  
  if isnan(ret)
    disp('L2: returning NaN')
  end
end