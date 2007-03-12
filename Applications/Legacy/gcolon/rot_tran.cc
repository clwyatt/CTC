// File: rot_tran.cc
// Abstract: functions to rotate and translate coordinates
//
// Created: 07/27/98 by: Chris L. Wyatt
//
//                

#include "rot_tran.h"

point Rx(point p, double angle)
{
  point p2;

  p2.x = p.x;
  p2.y = cos(angle)*p.y - sin(angle)*p.z;
  p2.z = sin(angle)*p.y + cos(angle)*p.z;

  return p2;
}


point Ry(point p, double angle)
{
  point p2;

  p2.x = cos(angle)*p.x + sin(angle)*p.z;
  p2.y = p.y;
  p2.z = -sin(angle)*p.x + cos(angle)*p.z;

  return p2;
}


point Rz(point p, double angle)
{
  point p2;

  p2.x = cos(angle)*p.x - sin(angle)*p.y;
  p2.y = sin(angle)*p.x + cos(angle)*p.y;
  p2.z = p.z;

  return p2;
}

point Txyz(point p, double dx, double dy, double dz)
{
  point p2;

  p2.x = p.x + dx;
  p2.y = p.y + dy;
  p2.z = p.z + dz;

  return p2;
}

