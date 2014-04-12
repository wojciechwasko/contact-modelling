#!/usr/bin/env python
# -*- coding: utf-8 -*-

## Function which generates the sensor response at (x,y)
def response(x,y):
  from math import exp,pow
  x0 = 0.01
  y0 = -0.01
  sigmaX = 0.012
  sigmaY = 0.012
  A = 0.001
  v = A * exp(-pow(x-x0,2)/(2*sigmaX*sigmaX) - pow(y-y0,2)/(2*sigmaY*sigmaY))
  if v > 0:
    return v
  return 0

head = """
#ifndef SIMULATED_INPUT_H
#define SIMULATED_INPUT_H

static uint16_t simulated_input[] = {
"""

tail = """0 // dummy element because actual last element has a comma after it
};
#endif /* SIMULATED_INPUT_H */
"""

head_stderr = """
nodes:
"""

tail_stderr = """
attributes:
  h: 0.0002
  E: 3000000
"""

if __name__ == "__main__":
  from sys import stdin,stdout, stderr
  import math

  d1 = 0.002
  e0 = 8.85 * math.pow(10, -12)
  er = 4.37
  r_tax = 0.002
  A = math.pow(r_tax, 2) * 3.14159

  stdout.write(head)
  stderr.write(head_stderr)

  data = stdin.readlines()
  for line in data[1:]:
    floats = [float(x) for x in line.split()]  
    if len(floats) < 11:
      continue # empty or invalid line

    d2 = d1 - response(floats[0], floats[1])
    #stderr.write("%f\n" % d2)
    v = (math.pow(10,15) / 0.32 ) * e0 * er * A * ((d1 - d2)/ (d1 * d2))
    # NOTE using 3D x and y.
    stdout.write("%i,\n" % v)
    stderr.write("  - relative_position:\n")
    stderr.write("      - %f\n" % floats[0])
    stderr.write("      - %f\n" % floats[1])
    stderr.write("    value: %i\n" % v)
  stdout.write(tail)
  stderr.write(tail_stderr)

  
