/***************************************************************************
File: mvVec2f.hh
Abstract:  2D Float Vector class.
Created: 09/2001
Author:      David K. Ahn
             Virtual Endoscopy Center
             Wake Forest University School of Medicine

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ on $Date: 2002/12/06 21:49:29 $
***************************************************************************/

#ifndef _mvVec2f_hh_
#define _mvVec2f_hh_

#include <math.h>
/**
* A 2D vector class 
*/ 
class mvVec2f {
  public:
  /// x coordinate
    float x;
  /// y coordinate
    float y;

  public:
  /// a constructor
    inline mvVec2f(void) {
    	x = 0;
	y = 0;};

  /// a constructor with initial position
    inline mvVec2f(const float a, const float b) {
      x = a;
      y = b;
    }

  /// a constructor with initial position
    inline mvVec2f operator =(const mvVec2f & v) {
      x = v.x;
      y = v.y;
      return *this;
    }

    inline mvVec2f operator +=(const mvVec2f & v) {
      x += v.x;
      y += v.y;
      return *this;
    }
    
    inline mvVec2f operator -=(const mvVec2f & v) {
      x -= v.x;
      y -= v.y;
      return *this;
    }

    inline mvVec2f operator *=(const float a) {
      x *= a;
      y *= a;
      return *this;
    }

    inline mvVec2f operator /=(const float a) {
      x /= a;
      y /= a;
      return *this;
    }


    inline mvVec2f operator *(const float a) {
      return mvVec2f(x*a, y*a);
    }

    inline mvVec2f operator /(const float a) {
      return mvVec2f(x/a, y/a);
    }

    inline friend mvVec2f operator +(const mvVec2f & v1, const mvVec2f & v2) {
      return mvVec2f(v1.x + v2.x, v1.y + v2.y);
    }


    inline friend mvVec2f operator -(const mvVec2f & v1, const mvVec2f & v2) {
      return mvVec2f(v1.x - v2.x, v1.y - v2.y);
    }

    inline friend int operator ==(const mvVec2f & v1, const mvVec2f & v2) {
      return int((v1.x == v2.x) && (v1.y == v2.y));
    }

    inline friend int operator !=(const mvVec2f & v1, const mvVec2f & v2) {
      return int((v1.x != v2.x) || (v1.y != v2.y));
    }

    inline void normalize(void) {
      double d;
      d = sqrt(x*x + y*y);
      if (d > 0.0) {
        x /= d;
        y /= d;
      }
    }
};

#endif
