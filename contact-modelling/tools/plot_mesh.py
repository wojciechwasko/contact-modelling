#!/usr/bin/env python
# -*- coding: utf-8 -*-

def getdata(fname):
  from StringIO import StringIO
  import numpy as np
  f = open(fname, 'r')
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

def plot(fname, dx, dy, x, y, v, title=''):
  if not (len(x) == len(y) == len(v)):
    raise ValueError, "x,y,v vectors have different lengths: %s,%s,%s" %(len(x),len(y),len(v))
  if dx <= 0:
    raise ValueError, "dx must be > 0"
  if dy <= 0:
    raise ValueError, "dy must be > 0"
  if dx != dy:
    print('dx != dy; since mayavi\'s barchart only plots bars with square base, we\'ll use a '
        'square with d = (dx+dy)/2, which is a pretty bad approximation.')
  d = (dx+dy)/2.0
  import numpy as np
  from mayavi import mlab      
  #mlab.barchart(x,y,v, colormap='jet', auto_scale=False, lateral_scale=dx)
  #h = mlab.barchart(1000*x, 1000*y, 50000*v, colormap='jet', auto_scale=False, lateral_scale=0.9*d*1000)
  maxx = max(x)
  minx = min(x)
  maxy = max(y)
  miny = min(y)
  maxv = max(v)
  minv = min(v)
  h = mlab.barchart(
    x, y, v,
    colormap='jet',
    auto_scale=False,
    lateral_scale=0.9*d,
    extent=[0,1,0,1,0,max((maxx-minx, maxy-miny))/(maxv-minv)]
  )
  mlab.axes(h,
    ranges=[minx, maxx, miny, maxy, minv, maxv]
  ) 
  mlab.outline(h)
  mlab.colorbar(h,orientation='vertical')
  mlab.title(title)
  mlab.show()

if __name__ == "__main__":
  import sys
  if len(sys.argv) < 2:
    sys.stderr.write("Usage: %s mesh_file [plot_title]\n" % sys.argv[0])
    sys.exit(1)

  if len(sys.argv) == 2:
    plot(sys.argv[1], *getdata(sys.argv[1]))
  else:
    plot(sys.argv[1], *getdata(sys.argv[1]), title=sys.argv[2])
