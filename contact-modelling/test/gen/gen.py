#!/usr/bin/env python
# -*- coding: utf-8 -*-

node1 = (0.1, 0.1)
node2_diff = (0.105, 0.107)
node2_same = (0.1, 0.1)
h = 0.002
G = 300000

dx_diff = node2_diff[0]-node1[0]
dy_diff = node2_diff[1]-node1[1]

dx_same = node2_same[0]-node1[0]
dy_same = node2_same[1]-node1[1]

xx_diff = lambda dx,dy,h,pi,G : -3 * (-(2*dx**2 + dy**2)/((dx**2 + dy**2)**1.5) + (2*dx**2 + dy**2 + h**2)/((dx**2 + dy**2 + h**2)**1.5))/(4*pi*G)
yx_diff = lambda dx,dy,h,pi,G : -3 * (-(dx * dy) / ((dx**2 + dy**2)**1.5) + (dx * dy) / ((dx**2 + dy**2 + h**2)**1.5)) / (4*pi*G)
zx_diff = lambda dx,dy,h,pi,G : -3 * ((dx * h) / ((dx**2 + dy**2 + h**2)**1.5)) / (4*pi*G)
xy_diff = lambda dx,dy,h,pi,G : -3 * (-(dx*dy)/((dx**2+dy**2)**1.5) + (dx*dy)/((dx**2+dy**2+h**2)**1.5) ) / (4*pi*G)
yy_diff = lambda dx,dy,h,pi,G : -3 * (-(dx**2+2*dy**2)/((dx**2+dy**2)**1.5) + (dx**2 + 2*dy**2 + h**2)/((dx**2 + dy**2 + h**2)**1.5)) /(4*pi*G)
zy_diff = lambda dx,dy,h,pi,G : -3 * ((dy*h)/((dx**2 + dy**2 + h**2)**1.5)) / (4*pi*G)
xz_diff = lambda dx,dy,h,pi,G : -3 * ((dx * h) / ((dx**2 + dy**2 + h**2)**1.5)) / (4*pi*G)
yz_diff = lambda dx,dy,h,pi,G : -3 * ((dy * h) / ((dx**2 + dy**2 + h**2)**1.5)) / (4*pi*G)
zz_diff = lambda dx,dy,h,pi,G : -3 * (-1/((dx**2+dy**2)**0.5) + (dx**2+dy**2+2*h**2)/((dx**2+dy**2+h**2)**1.5)) / (4*pi*G)

xx_same = lambda pi,G : 9/(4*pi*G) * 0.25/0.0309019361618552
yx_same = lambda pi,G : 0
zx_same = lambda pi,G : 0
xy_same = lambda pi,G : 0
yy_same = lambda pi,G : 9/(4*pi*G) * 0.25/0.0309019361618552
zy_same = lambda pi,G : 0
xz_same = lambda pi,G : 0
yz_same = lambda pi,G : 0
zz_same = lambda pi,G : 9/(2*pi*G) * 0.25/0.0309019361618552

vec_head = "std::vector<double> expected_%s = {"
vec_tail = "};"

import math
print(
vec_head % "diff_3_3" +
"%s,%s,%s, %s,%s,%s, %s,%s,%s" % (
  xx_diff(dx_diff, dy_diff, h, math.pi, G),
  yx_diff(dx_diff, dy_diff, h, math.pi, G),
  zx_diff(dx_diff, dy_diff, h, math.pi, G),
  xy_diff(dx_diff, dy_diff, h, math.pi, G),
  yy_diff(dx_diff, dy_diff, h, math.pi, G),
  zy_diff(dx_diff, dy_diff, h, math.pi, G),
  xz_diff(dx_diff, dy_diff, h, math.pi, G),
  yz_diff(dx_diff, dy_diff, h, math.pi, G),
  zz_diff(dx_diff, dy_diff, h, math.pi, G),
  )
+ vec_tail)

print(
vec_head % "diff_1_3"+
"%s,%s,%s" % (
  zx_diff(dx_diff, dy_diff, h, math.pi, G),
  zy_diff(dx_diff, dy_diff, h, math.pi, G),
  zz_diff(dx_diff, dy_diff, h, math.pi, G),
  )
+ vec_tail)

print(
vec_head % "diff_3_1" +
"%s,%s,%s" % (
  xz_diff(dx_diff, dy_diff, h, math.pi, G),
  yz_diff(dx_diff, dy_diff, h, math.pi, G),
  zz_diff(dx_diff, dy_diff, h, math.pi, G),
  )
+ vec_tail)

print(
vec_head % "diff_1_1" +
"%s" % (
  zz_diff(dx_diff, dy_diff, h, math.pi, G),
  )
+ vec_tail)

print(
vec_head % "same_3_3" +
"%s,%s,%s, %s,%s,%s, %s,%s,%s" % (
  xx_same(math.pi, G),
  yx_same(math.pi, G),
  zx_same(math.pi, G),
  xy_same(math.pi, G),
  yy_same(math.pi, G),
  zy_same(math.pi, G),
  xz_same(math.pi, G),
  yz_same(math.pi, G),
  zz_same(math.pi, G),
  )
+ vec_tail)

print(
vec_head % "same_1_3" +
"%s,%s,%s" % (
  zx_same(math.pi, G),
  zy_same(math.pi, G),
  zz_same(math.pi, G),
  )
+ vec_tail)

print(
vec_head % "same_3_1" +
"%s,%s,%s" % (
  xz_same(math.pi, G),
  yz_same(math.pi, G),
  zz_same(math.pi, G),
  )
+ vec_tail)

print(
vec_head % "same_1_1" +
"%s" % (
  zz_same(math.pi, G),
  )
+ vec_tail)

