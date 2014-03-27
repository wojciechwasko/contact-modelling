#!/usr/bin/env python
# -*- coding: utf-8 -*-

## Function which generates the sensor response at (x,y)
def response(x,y):
  from math import exp,pow
  x0 = 0.01
  y0 = -0.01
  sigmaX = 0.01
  sigmaY = 0.01
  A = 1000
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

if __name__ == "__main__":
  from sys import stdin,stdout
  stdout.write(head)
  data = stdin.readlines()
  for line in data[1:]:
    floats = [float(x) for x in line.split()]  
    if len(floats) < 11:
      continue # empty or invalid line
    # NOTE using 3D x and y.
    stdout.write("%i,\n" % response(floats[0], floats[1]))
  stdout.write(tail)

  
