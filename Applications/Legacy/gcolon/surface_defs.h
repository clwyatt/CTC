// File: surface_defs.h
//
// Abstract: definition of surface point list entry
//
// Created: 07/27/98 by: Chris L. Wyatt
//


#ifndef _surface_defs_
#define _surface_defs_ 1

struct surface_point
{
  int x, y, z;          //origional point location in volume
  double gx, gy, gz;    //gradient at this point
  double K;             //curvature at this point
  double H;

  surface_point * next; //pointer to next point in list
};

struct point
{
  double x;
  double y;
  double z;
};

#endif
