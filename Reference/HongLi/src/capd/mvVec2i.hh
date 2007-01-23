/*
 * File:	mvVec2i.h
 *
 * Project:	MedVis Library
 *
 * Author:	David K. Ahn
 *		Virtual Endoscopy Center
 *		Wake Forest University School of Medicine
 *
 * Description:	3D Integer Vector class.
 *
 * $Id: mvVec2i.hh,v 1.1.1.1 2002/12/06 21:49:31 hongli Exp $
 *
 */

#ifndef _mvVec2i_h_
#define _mvVec2i_h_

#include <sys/types.h>

class mvVec2i {
  public:
    int32_t x;
    int32_t y;
    
  public:
  /// a constructor
    inline mvVec2i(void) {
    	x = 0;
	y = 0;};

  /// a constructor with initial position
    inline mvVec2i(const int a, const int b) {
      x = a;
      y = b;
    }

  /// a constructor with initial position
    inline mvVec2i operator =(const mvVec2i & v) {
      x = v.x;
      y = v.y;
      return *this;
    }
    inline friend int operator ==(const mvVec2i & v1, const mvVec2i & v2) {
      return int((v1.x == v2.x) && (v1.y == v2.y));
    }

    inline friend int operator !=(const mvVec2i & v1, const mvVec2i & v2) {
      return int((v1.x != v2.x) || (v1.y != v2.y));
    }
    
    
};

#endif
