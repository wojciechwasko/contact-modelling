#!/usr/bin/env python                                                                                                                                                                                                                          
# -*- coding: utf-8 -*-

## Generate (R,G,B) tuples for colors using a mechanism similar to gnuplot's rgbformulae
def gen_colors(dz, R=7, G=5, B=15):
  import math
  rgbformulae = [
    lambda x: 0,
    lambda x: 0.5,
    lambda x: 1,
    lambda x: x,
    lambda x: x**2,
    lambda x: x**3,
    lambda x: x**4,
    lambda x: math.sqrt(x),
    lambda x: math.sqrt(math.sqrt(x)),
    lambda x: math.sin(0.5*math.pi*x),
    lambda x: math.cos(0.5*math.pi*x),
    lambda x: math.fabs(x-0.5),
    lambda x: (2*x - 1)**2,
    lambda x: math.sin(math.pi * x),
    lambda x: math.fabs(math.cos(math.pi*x)),
    lambda x: math.sin(2*math.pi*x),
    lambda x: math.cos(2*math.pi*x),
    lambda x: math.fabs(math.sin(2*math.pi*x)),
    lambda x: math.fabs(math.cos(2*math.pi*x)),
    lambda x: math.fabs(math.sin(4*math.pi*x)),
    lambda x: math.fabs(math.cos(4*math.pi*x)),
    lambda x: 3*x,
    lambda x: 3*x-1,
    lambda x: 3*x-1,
    lambda x: 3*x-2,
    lambda x: math.fabs(3*x-1),
    lambda x: math.fabs(3*x-2),
    lambda x: (3*x-1)/2.0,
    lambda x: (3*x-2)/2.0,
    lambda x: math.fabs((3*x-1)/2.0),
    lambda x: math.fabs((3*x-2)/2.0),
    lambda x: x*0.32 - 0.78125,
    lambda x: 2*x - 0.84,
  ]
  min_z = min(dz)
  max_z = max(dz)
  z_delta = float(max_z-min_z)

  scale_z = lambda z: (z - min_z)/float(r)
  def getcolors(z):
    scale_z = (z - min_z)/float(z_delta)
    # I don't think this is the right way, but I have a hard time understanding how cbrange in
    # gnuplot works. Maybe I'll come back to this later.
    def cap(v):
      if v < 0:
        return 0
      if v > 1:
        return 1
      return v
    return (
      cap(rgbformulae[R](scale_z)),
      cap(rgbformulae[G](scale_z)), 
      cap(rgbformulae[B](scale_z))
    )
  return map(getcolors, dz) 

def getdata(fname):
  from StringIO import StringIO
  import numpy as np
  f = open('ifile', 'r')
  first_line = f.readline()
  try:
    dx, dy = [float(x) for x in first_line.split()]
  except ValueError:
    raise ValueError, \
      "The first line must contain exactly two floats. Line '%s' does not." % first_line
  xs = np.loadtxt(StringIO(f.readline()))
  ys = np.loadtxt(StringIO(f.readline()))
  vals = np.loadtxt(StringIO(f.readline()))
  return (dx,dy, xs, ys, vals)

def plot(dx, dy, x, y, v, title=''):
  if not (len(x) == len(y) == len(v)):
    raise ValueError, "x,y,v vectors have different lengths: %s,%s,%s" %(len(x),len(y),len(v))
  if dx <= 0:
    raise ValueError, "dx must be > 0"
  if dy <= 0:
    raise ValueError, "dy must be > 0"
  import numpy as np
  # move the nodes to the centers of the bottom/top cuboid face
  x = np.subtract(x, dx/2.0)
  y = np.subtract(y, dy/2.0)
  import matplotlib.pyplot as plt
  from mpl_toolkits.mplot3d import Axes3D
  z = np.zeros(len(v))
  fig = plt.figure()
  ax1 = fig.add_subplot(111, projection='3d')
  ax1.bar3d(x, y, z, dx, dy, v, color=gen_colors(v))
  ax1.set_xlabel('x')
  ax1.set_ylabel('y')
  ax1.set_title(title)
  plt.show()

if __name__ == "__main__":
  import sys
  if len(sys.argv) < 2:
    sys.stderr.write("Usage: %s mesh_file [plot_title]\n" % sys.argv[0])
    sys.exit(1)

  if len(sys.argv) == 2:
    plot(*getdata(sys.argv[1]))
  else:
    plot(*getdata(sys.argv[1]), title=sys.argv[2])
