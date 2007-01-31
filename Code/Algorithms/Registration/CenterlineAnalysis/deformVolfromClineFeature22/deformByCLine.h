#ifndef _deformByCLine_HEADER_

#include <stdio.h>
#include "myDataType.h"

#define FEATURED_POINT			10
#define NON_FEATURED_POINT	21
#define NON_DISTANCE_SETTING	999.99

#define _TARGET_CLINE_	55
#define _SOURCE_CLINE_  77
using namespace std;

class DField4ClineVoxel{
	
	public:
	          COORD<int> position;
  		   COORD<float> DField;
		
		   int nFeatured;
		   float lDistanceFromFeaturePoint;       
};

class DField4Clines{
	public:
	          DField4ClineVoxel *DFieldCline;
		   DField4ClineVoxel *auxDFieldCline;  // auxiliary DField for source cline cal.  
		    int cline_size; 
		    int auxCline_size;                          // auxiliary DField for source cline cal.  
                  int featurePair_size;
		    int window_size;
	public:
		   void fillFeaturedCline(int validCnt, int win_size, int ClineSize, ClineVoxel &cS, ClineVoxel &cT, MatchedPair *matchedPair, IndexPair* mIndexPair);
		   void fillFeaturedCline2(int validCnt, int win_size, int ClineSize, int AuxClineSize, ClineVoxel &cS, ClineVoxel &cT, MatchedPair *matchedPair, IndexPair* mIndexPair);
		   void storeClineDField(string &ClineVectorFile, ReaderType::Pointer reader, IndexPair* mIndexPair, int ClineSize);
		   void computeNeighborClineDField(float sigmaSquare, float dist, int currentIndex, int baseIndex);
		   void computeNeighborClineAuxDField(float sigmaSquare, float dist, int currentIndex, int baseIndex);
		   void computeNeighborClineDFieldUsingLinerInterpolation(int LowFeaturedIndex, int HighFeaturedIndex,  int currentIndex);
		   void computeNeighborClineAuxDFieldUsingLinerInterpolation(int LowFeaturedIndex, int HighFeaturedIndex,  int currentIndex);
		   ~DField4Clines(){}
};

class ObjVoxel{
	public:
	         COORD<int> position;
		  COORD<float> DField;
		  float closestDistToCvoxel;
		  int closestClineIndex;
		  
		  float closestDistToAuxCvoxel;    // auxiliary DField for source cline cal.  
		  int closestAuxClineIndex;		  // auxiliary DField for source cline cal.  
		  
		  int whichClosestCline;
		  int mark;
};

class GroupObjVoxel{
	public:
	         ObjVoxel *groupObjVoxel;
		  unsigned long int size;	 
			 
	 public:
		  void fillGroupObjVoxel(unsigned long int num, TNT::Array3D<int> &bVoxel, int dimx, int dimy, int dimz);
		  void fillGroupObjVoxelObj(int num, TNT::Array3D<int> &Src, TNT::Array3D<int> &Trg,int dimx, int dimy, int dimz);
		  void distFromClosestCline(DField4Clines &VecCline);
		  void distFromClosestCline2(DField4Clines &VecCline);
		  void DFieldFromClosestCline(DField4Clines &VecCline);
		  void DFieldFromClosestCline2(DField4Clines &VecCline);
		  void smootheDField(ReaderType::Pointer reader);
		  void storeVolumeDField(ReaderType::Pointer reader, string &VolumeVectorFile);
		  int CountSrcAndTrgObj(TNT::Array3D<int> &Src, TNT::Array3D<int> &Trg, int dimx, int dimy, int dimz);
};



#define _deformByCLine_HEADER_
#endif 
