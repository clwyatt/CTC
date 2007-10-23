#include <iostream>
#include <cstdlib>
#include <vector>
#include "MAT4Converter.h"

using namespace std;

/* matlab MAT-4 file format header */
typedef struct {
  int type;    /* type */
  int rows;    /* row dimension */
  int cols;    /* column dimension */
  int imag;    /* flag indicating imag part */
  int namelen; /* name length (including NULL) */
} MAT4HEADER;

/* File Converter Function */
/* RegionCollectorType input --- Vector of Vector */

void MAT4FeatureVector(RegionCollectorType input)
{
 
    MAT4HEADER matFileHeader;
    char *pname = "FeatureVector";

    FILE *matFile; 
	   matFile = fopen("FeatureVector.mat", "wb");

    if (matFile == 0)
    {
         std::cerr << "couldn't open file" << "\n;";
         exit(0);
    }
    
    int num_points = 0;
    int i = 0;
    int num_regions = input.size();
    GrowableRegionType::iterator iter;

    /* Have to get the number of points first */
    for(i = 0; i < num_regions; i++)
    {
          iter = input[i].begin();
          for (; iter != input[i].end(); ++iter)
          { 
                num_points++;       
          }
    }
 
    double FeatureVectorArray[9*num_points];
    int m = 0;

    /* Push each feature vector value to an array 
     *
     * E.X FeatureVectorArray[POINTS*FEATURES] = {1,4,7,10,2,5,8,11,3,6,9,12};
     * 
     * Actually the above array is acquired from following feature vectors
     *      
     * V1 (1,2,3)
     * V2 (4,5,6)
     * V3 (7,8,9)
     * V4 (10,11,12)
     *
     * The generated 2D matrix in .mat file is like this:
     * x = [1 2 3
     *      4 5 6
     *      7 8 9
     *      10 11 12]
     * */

    int num_region = 0;
    for(i = 0; i < num_regions; i++)
    {
          iter = input[i].begin();
          num_region++;
          for (; iter != input[i].end(); ++iter)
          { 
               dcmCoordinate pdcm = iter->GetDCMCoordinate();
               BinaryImageType::IndexType pindex = iter->GetVoxelIndex();
               FeatureVectorArray[m] = num_region;
               FeatureVectorArray[m + num_points] = pdcm[0];
               FeatureVectorArray[m + num_points*2] = pdcm[1];
               FeatureVectorArray[m + num_points*3] = pdcm[2];
               FeatureVectorArray[m + num_points*4] = pindex[0];
               FeatureVectorArray[m + num_points*5] = pindex[1];
               FeatureVectorArray[m + num_points*6] = pindex[2];
               FeatureVectorArray[m + num_points*7] = iter->GetSI();
               FeatureVectorArray[m + num_points*8] = iter->GetCV(); 
               m++;     
          }
    }

    /* Define header parameters */
    matFileHeader.type = 0000;
    matFileHeader.rows = num_points;
    matFileHeader.cols = 9;
    matFileHeader.imag = 0;
    matFileHeader.namelen = 14;
    
    fwrite( &matFileHeader, sizeof(MAT4HEADER), 1, matFile ); // write the header
    fwrite( pname, sizeof(char), matFileHeader.namelen, matFile ); // write out name of variable
    fwrite( FeatureVectorArray, sizeof(double), matFileHeader.rows*matFileHeader.cols, matFile );
       
    fclose(matFile);
    printf("MAT4 File is generated as FeatureVector.mat!\n"); 

}



