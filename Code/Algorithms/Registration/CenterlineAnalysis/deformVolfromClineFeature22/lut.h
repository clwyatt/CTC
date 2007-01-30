#ifndef LUT_H
#define LUT_H

#include "tnt/tnt_array3d.h"

template <class T>
void LUT(const TNT::Array3D<T> &DT, vector< localMaximum<T> > &LMList)
{
  typename vector< localMaximum<T> >::iterator idx;
  T r;
  int x,y,z;
  
  for (idx = LMList.begin(); idx != LMList.end(); idx++) {

    r = (*idx).radius;
    x = (int)(*idx).x;
    y = (int)(*idx).y;
    z = (int)(*idx).z;
    if ( r == 3 ) {
                                    /* if any a-neighbor is >= 4 */
      if ((DT[x][y][z-1] >= 4) ||
	  (DT[x][y-1][z] >= 4) ||
	  (DT[x-1][y][z] >= 4) ||
	  (DT[x+1][y][z] >= 4) ||
	  (DT[x][y+1][z] >= 4) ||
	  (DT[x][y][z+1] >= 4)) {
	idx = LMList.erase(idx);    /* remove the point from the list */
	idx--;	                    /* set up to test next point */
      }
                                    /* or if any b-neighbor is >= 5 */
      else if ((DT[x][y-1][z-1] >= 5) ||
	       (DT[x-1][y][z-1] >= 5) ||
	       (DT[x+1][y][z-1] >= 5) ||
	       (DT[x][y+1][z-1] >= 5) ||
	       (DT[x-1][y-1][z] >= 5) ||
	       (DT[x+1][y-1][z] >= 5) ||
	       (DT[x-1][y+1][z] >= 5) ||
	       (DT[x+1][y+1][z] >= 5) ||
	       (DT[x][y-1][z+1] >= 5) ||
	       (DT[x-1][y][z+1] >= 5) ||
	       (DT[x+1][y][z+1] >= 5) ||
	       (DT[x][y+1][z+1] >= 5)) {
	                            
	idx = LMList.erase(idx);    /* remove the point from the list */
	idx--;                      /* set up to test next point */
      }
                                    /* or if any c-neighbor is >= 6 */
      else if ((DT[x-1][y-1][z-1] >= 6) ||
	       (DT[x+1][y-1][z-1] >= 6) ||
	       (DT[x-1][y+1][z-1] >= 6) ||
	       (DT[x+1][y+1][z-1] >= 6) ||
	       (DT[x-1][y-1][z+1] >= 6) ||
	       (DT[x+1][y-1][z+1] >= 6) ||
	       (DT[x-1][y+1][z+1] >= 6) ||
	       (DT[x+1][y+1][z+1] >= 6)) {
	idx = LMList.erase(idx);    /* remove the point from the list */
	idx--;                      /* set up to test next point */
      }
    }
  }
}
  
#endif
  
