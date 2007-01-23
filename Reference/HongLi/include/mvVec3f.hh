/***************************************************************************
File: mvVec3f.hh
Abstract:  3D Float Vector class.
Created: 09/2001
Author:      David K. Ahn
             Virtual Endoscopy Center
             Wake Forest University School of Medicine

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ on $Date: 2002/12/06 21:49:29 $
***************************************************************************/

#ifndef _mvVec3f_hh_
#define _mvVec3f_hh_

#include <math.h>
/**
* A 3D vector class 
*/ 
class mvVec3f {
  public:
  /// x coordinate
    float x;
  /// y coordinate
    float y;
  /// z coordinate
    float z;

  public:
  /// a constructor
    inline mvVec3f(void) {
    	x = 0;
	y = 0;
	z = 0;};

  /// a constructor with initial position
    inline mvVec3f(const float a, const float b, const float c) {
      x = a;
      y = b;
      z = c;
    }

  /// a constructor with initial position
    inline mvVec3f operator =(const mvVec3f & v) {
      x = v.x;
      y = v.y;
      z = v.z;
      return *this;
    }

    inline mvVec3f operator +=(const mvVec3f & v) {
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
    }
    
    inline mvVec3f operator -=(const mvVec3f & v) {
      x -= v.x;
      y -= v.y;
      z -= v.z;
      return *this;
    }

    inline mvVec3f operator *=(const float a) {
      x *= a;
      y *= a;
      z *= a;
      return *this;
    }

    inline mvVec3f operator /=(const float a) {
      x /= a;
      y /= a;
      z /= a;
      return *this;
    }


    inline mvVec3f operator *(const float a) {
      return mvVec3f(x*a, y*a,z*a);
    }

    inline mvVec3f operator /(const float a) {
      return mvVec3f(x/a, y/a,z/a);
    }

    inline friend mvVec3f operator +(const mvVec3f & v1, const mvVec3f & v2) {
      return mvVec3f(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }


    inline friend mvVec3f operator -(const mvVec3f & v1, const mvVec3f & v2) {
      return mvVec3f(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    inline friend int operator ==(const mvVec3f & v1, const mvVec3f & v2) {
      return int((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z));
    }

    inline friend int operator !=(const mvVec3f & v1, const mvVec3f & v2) {
      return int((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z));
    }

    inline void normalize(void) {
      double d;
      d = sqrt(x*x + y*y + z*z);
      if (d > 0.0) {
        x /= d;
        y /= d;
        z /= d;
      }
    }

    inline mvVec3f crossProduct(const mvVec3f & v2) {
      mvVec3f v;
      v.x = y * v2.z - z * v2.y;
      v.y = z * v2.x - x * v2.z;
      v.z = x * v2.y - y * v2.x;
      return v;
    }

    inline double innerProduct(const mvVec3f & v2) {
      return double(x) * double(v2.x) +
             double(y) * double(v2.y) +
             double(z) * double(v2.z);
    }

    inline double get_Distance(const mvVec3f & v2) {
      mvVec3f v;
      v = (*this) - v2;
      return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    inline double get_Length(void) {
      return sqrt(x * x + y * y + z * z);
    }

    inline double get_Angle(mvVec3f & v) {
      double l1, l2;

      l1 = this->get_Length();

      if (l1 == 0.0) return 0.0;

      l2 = v.get_Length();

      if (l2 == 0.0) return 0.0;

      return acos(this->innerProduct(v) / l1 / l2 );
    }

    inline mvVec3f xRotate(const double & angle) {
      mvVec3f v;
      double cosr, sinr;
      cosr = cos(angle);
      sinr = sin(angle);
      v.x = x;
      v.y = y * cosr - z * sinr;
      v.z = y * sinr + z * cosr;
      return v;
    }

    inline mvVec3f yRotate(const double & angle) {
      mvVec3f v;
      double cosr, sinr;
      cosr = cos(angle);
      sinr = sin(angle);
      v.x = x * cosr + z * sinr;
      v.y = y;
      v.z = -x * sinr + z * cosr;
      return v;
    }

    inline mvVec3f zRotate(const double & angle) {
      mvVec3f v;
      double cosr, sinr;
      cosr = cos(angle);
      sinr = sin(angle);
      v.x = x * cosr - y * sinr;
      v.y = x * sinr + y * cosr;
      v.z = z;
      return v;
    }

    inline mvVec3f uRotate(const mvVec3f & u, const double & angle) {
      double cosr, sinr;
      mvVec3f v;
      cosr = cos(angle);
      sinr = sin(angle);
      v.x = x * (u.x * u.x + cosr * (1 - u.x * u.x)) +
            y * (u.x * u.y * (1 - cosr) - u.z * sinr) +
            z * (u.x * u.z * (1 - cosr) + u.y * sinr);

      v.y = x * (u.x * u.y * (1 - cosr) + u.z * sinr) +
            y * (u.y * u.y + cosr * (1 - u.y * u.y)) +
            z * (u.y * u.z * (1 - cosr) - u.x * sinr);

      v.z = x * (u.x * u.z * (1 - cosr) - u.y * sinr) +
            y * (u.y * u.z * (1 - cosr) + u.x * sinr) +
            z * (u.z * u.z + cosr * (1 - u.z * u.z));
      return v;
    }
};

#endif
