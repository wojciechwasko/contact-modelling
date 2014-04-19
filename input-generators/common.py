#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import print_function

skin_attr = {'h':0.002, 'E':210000}

head_c = """
#ifndef SIMULATED_INPUT_H
#define SIMULATED_INPUT_H

static uint16_t simulated_input[] = {
"""

tail_c = """0 // dummy element because actual last element has a comma after it
};
#endif /* SIMULATED_INPUT_H */
"""

head_yaml = """
nodes:
"""

tail_yaml = """
attributes:
  h: 0.002
  E: 210000
  taxelArea: 1.2566e-05
"""


## A simple validator that accepts a list of allowable values
class AllowedValues(object):
  def __init__(self, allowed):
    self.allowed_ = allowed
  def __call__(self, v):
    if v not in self.allowed_:
      import argparse
      raise argparse.ArgumentTypeError('Value "%s" not in the list of allowed values: %s'
        % (v, self.allowed_))
    return v

def load_calib_cache(fname):
  f = open(fname, 'r')
  coords = []
  for line in f:
    floats = [float(x) for x in line.split()]  
    if len(floats) < 11:
      continue # empty or invalid line
    coords.append((floats[0], floats[1]))
  return coords

def output(form, coords, values):
#  if len(coords) != len(values):
#    raise RuntimeError(
#      'Lenghts of coordinates list(%s) and of values returned by callback(%s) do not match!'
#      % (len(coords), len(values)))
  if form == 'none':
    for c,v in zip(coords, values):
      continue
  elif form == 'yaml':
    print(head_yaml, end="")
    for c,v in zip(coords, values):
      print("  - relative_position:\n", end="")
      print("      - %f\n" % c[0], end="")
      print("      - %f\n" % c[1], end="")
      print("    value: %f\n" % v, end="")
    print(tail_yaml, end="")
# Disabling since we'd have to make it "raw" (uint16_t) first
#  elif form == 'c_header':
#    print(head_c, end="")
#    for c,v in zip(coords, values):
#      print("%i,\n" % v, end="")
#    print(tail_c, end="")
  else:
    raise ValueError("Unrecognised output format: %s" % form)

## \param   callback  a callable which takes a list of two-element tuples/lists (x,y) coordinates
##                    of sensors and will output a list of *skin thickness* values (floating-point)
##                    which will be later translated to raw (uint16_t) values by the run() function
def run(callback):
  import argparse
  parser = argparse.ArgumentParser(description='Generate simulated raw data')
  parser.add_argument('cache_file',
      nargs=1,
      help='Input file (SkinWare\'s calibration cache')
  parser.add_argument('formats',
      type=AllowedValues(('yaml','none')),
      nargs=1,
      help='Output format, allowed values: "yaml", "none"')
  args = parser.parse_args()

  coords = load_calib_cache(args.cache_file[0])
  values = callback(coords, **skin_attr)

  for out_format in args.formats:
    output(out_format, coords, values)
  

