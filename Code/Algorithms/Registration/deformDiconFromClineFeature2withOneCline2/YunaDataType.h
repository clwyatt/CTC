#ifndef WOOK_H
#define WOOK_H

template <class Type>
struct PathCost {
  Type x, y, z;
  Type cost;
  Type dfb;
};

template <class T>
struct COORD {
  T x;
  T y;
  T z;  
};

class VOXEL {
  public :
//    int dfb;                   // distance from boundary
     float dfb;          // modified by wook 11/22/07		  
     int mark;  
//  unsigned int dfs;       // distance from starting point
     float dfs;            // modified by wook 11/22/07
     COORD<int> current;
     COORD<int> previous;
};


class ElementLess {
  public  :
    bool operator () (const VOXEL& p, const VOXEL& q) const
    {
      return p.dfb < q.dfb;   
    }  
};


struct IndexPair {
	int SavedS_Index;
	int SavedL_Index;
	float SavedCorrel;
	float CorrelX;
	float CorrelY;
	float CorrelZ;
	int Overlap;
	int Type;  // Type =1 : YZ maximum,   Type =2 : XZ minimum,   Type =3 : ZY maximum
};

#define _OVERLAP_ 		1
#define _NON_OVERLAP_	0



class Less_Sindex {
  public  :
    bool operator () (const struct IndexPair p, const struct IndexPair q) const
    {
      return p.SavedS_Index > q.SavedS_Index;   
    }  
};

#define MARK_ON     1
#define MARK_OFF    0

#endif

