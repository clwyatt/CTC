/*
 * File:	mvVec3i.h
 *
 * Project:	MedVis Library
 *
 * Author:	David K. Ahn
 *		Virtual Endoscopy Center
 *		Wake Forest University School of Medicine
 *
 * Description:	3D Integer Vector class.
 *
 * $Id: mvVec3i.hh,v 1.1.1.1 2002/12/06 21:49:29 hongli Exp $
 *
 */

#ifndef _mvVec3i_h_
#define _mvVec3i_h_

#include <sys/types.h>

class mvVec3i {
  public:
    int32_t x;
    int32_t y;
    int32_t z;
    
  public:
  /// a constructor
    inline mvVec3i(void) {
    	x = 0;
	y = 0;
	z = 0;};

  /// a constructor with initial position
    inline mvVec3i(const int a, const int b, const int c) {
      x = a;
      y = b;
      z = c;
    }

  /// a constructor with initial position
    inline mvVec3i operator =(const mvVec3i & v) {
      x = v.x;
      y = v.y;
      z = v.z;
      return *this;
    }
    inline friend int operator ==(const mvVec3i & v1, const mvVec3i & v2) {
      return int((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z));
    }

    inline friend int operator !=(const mvVec3i & v1, const mvVec3i & v2) {
      return int((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z));
    }
    
    
};

#endif
