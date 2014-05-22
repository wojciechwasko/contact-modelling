function [ ret ] = Zlove_w( p, dx, dy, E, nu, x, y, z )
%W Calculate normal displacement (along z) from Love's problem
%   A pressurep over a rectangle (2*dx,2*dy), the centre of which is located 
%   at (0,0) is applied and this function
%   will return the normal displacement at point (x,y,z) resulting from
%   this pressure.
%   p - value of pressure
%   dx,dy - 0.5*dimensions of the load cell
%   E - elastic modulus (aka Young's modulus)
%   nu - Poisson's ratio
%   (x,y,z) - coordinates at which to calculate the displacement

  % Calculate Lame's parameters
  % lambda = E*nu / ((1+nu)*(1-2*nu));
  % mu = E / (2*(1+nu));
    
  a = dx;
  b = dy;
  ret = 0;
  if eq_almost(z,0)
% using Lame parameters
%     yp = b;
%     ret = ret + (p/(4*pi*mu)) * (((lambda+2*mu)/(lambda+mu)) * (L1(yp,a,b,x,y,z) - L2(yp,a,b,x,y,z)));
%     yp = -b;
%     ret = ret - (p/(4*pi*mu)) * (((lambda+2*mu)/(lambda+mu)) * (L1(yp,a,b,x,y,z) - L2(yp,a,b,x,y,z)));
    yp = b;
    ret = ret + (p*(1-nu^2)/(pi*E)) * ((L1(yp,a,b,x,y,z) - L2(yp,a,b,x,y,z)));
    yp = -b;
    ret = ret - (p*(1-nu^2)/(pi*E)) * ((L1(yp,a,b,x,y,z) - L2(yp,a,b,x,y,z)));
  else
% using Lame parameters
%     yp = b;
%     ret = ret + (p/(4*pi*mu)) * (((lambda+2*mu)/(lambda+mu)) * (L1(yp,a,b,x,y,z) - L2(yp,a,b,x,y,z)) + z * (atan((a-x)*(yp-y)/(z*r10(yp,a,b,x,y,z))) + atan((a+x)*(yp-y)/(z*r20(yp,a,b,x,y,z)))));
%     yp = -b;
%     ret = ret - (p/(4*pi*mu)) * (((lambda+2*mu)/(lambda+mu)) * (L1(yp,a,b,x,y,z) - L2(yp,a,b,x,y,z)) + z * (atan((a-x)*(yp-y)/(z*r10(yp,a,b,x,y,z))) + atan((a+x)*(yp-y)/(z*r20(yp,a,b,x,y,z)))));
    
    yp = b;
    ret = ret + (p*(1-nu^2)/(pi*E)) * ((L1(yp,a,b,x,y,z) - L2(yp,a,b,x,y,z))) + (p*(1+nu)/(2*pi*E)) * z * (atan((a-x)*(yp-y)/(z*r10(yp,a,b,x,y,z))) + atan((a+x)*(yp-y)/(z*r20(yp,a,b,x,y,z))));
    yp = -b;
    ret = ret - ((p*(1-nu^2)/(pi*E)) * ((L1(yp,a,b,x,y,z) - L2(yp,a,b,x,y,z))) + (p*(1+nu)/(2*pi*E)) * z * (atan((a-x)*(yp-y)/(z*r10(yp,a,b,x,y,z))) + atan((a+x)*(yp-y)/(z*r20(yp,a,b,x,y,z)))));
  end
end

function [ ret ] = w_love(F,dx,dy,E,nu,x,y)
% Calculate the normal displacement from Love's problem in the z=0 plane,
% using a solution due to Love (1929)
  p = F/(dx*dy);
  a = dx;
  b = dy;
  ln1 = log((y+b) + sqrt((y+b)^2 + (x+a)^2)) - log((y-b) + sqrt((y-b)^2 + (x+a)^2));
  ln2 = log((x+a) + sqrt((y+b)^2 + (x+a)^2)) - log((y-b) + sqrt((y-b)^2 + (x+a)^2));
  ln3 = log((y-b) + sqrt((y-b)^2 + (x-a)^2)) - log((y+b) + sqrt((y+b)^2 + (x-a)^2));
  ln4 = log((x-a) + sqrt((y-b)^2 + (x-a)^2)) - log((x+a) + sqrt((y-b)^2 + (x+a)^2));
  
  ret = (p * (1-nu^2)/(pi*E)) * ((x+a)*ln1 + (y+b)*ln2 + (x-a)*ln3 + (y-b)*ln4);
end
