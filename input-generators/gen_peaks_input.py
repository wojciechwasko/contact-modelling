#!/usr/bin/env python
# -*- coding: utf-8 -*-

class Response(object):
  def __init__(self):
##    self.h = 0.002
##    self.E = 210000
    self.f = [
      (0.023 , -0.023 , 20) ,
      (0.015 , 0.005  , 15) ,
    ]
  def __call__(self, coordinates, **kwargs):
    for c in coordinates:
      yield self.calc_deformation(*c, h=kwargs["h"],E=kwargs["E"])

  def calc_deformation(self, x, y, h, E):
    import math
    res = 0
    pi = math.pi
    for f in self.f:
      dx = x - f[0]
      dy = y - f[1]
      if math.fabs(dx) < 0.0001 and math.fabs(dy) < 0.0001:
        psi = 0.25
        node_area = 0.002
        z0h = sqrt(3/(2*pi) * node_area) # eq (20), Muscari et al. 2013
        zz = 9/(2*pi*G) * psi/z0h
      else:
        zz = - 3 * (1/((dx**2+dy**2)**0.5) - (dx**2+dy**2+2*h**2)/((dx**2+dy**2+h**2)**1.5)) / (4*pi*E)
      res += zz * f[2]
#      print("zz: %s, f[2]: %s" % (zz, f[2]))
#    print("res: %s" % res)
    return res

if __name__ == "__main__":
  from common import run
  run(Response())
