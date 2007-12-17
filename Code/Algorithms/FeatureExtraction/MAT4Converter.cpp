#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
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

void MAT4FeatureVector(RegionCollectorType input, char* filename)
{
 
    MAT4HEADER matFileHeader;
    char *pname = "FeatureVectorVoxels";

    FILE *matFile; 
	   matFile = fopen(filename, "wb");

    if (matFile == 0)
    {
         std::cerr << "couldn't open file" << "\n;";
         exit(0);
    }
   
    cout << "1" << endl; 
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
    cout << "2" << endl; 

    double FeatureVectorArray[9*num_points];

    cout << "3" << endl; 
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
    cout << "4" << endl; 

    /* Define header parameters */
    matFileHeader.type = 0000;
    matFileHeader.rows = num_points;
    matFileHeader.cols = 9;
    matFileHeader.imag = 0;
    matFileHeader.namelen = 20;
    
    fwrite( &matFileHeader, sizeof(MAT4HEADER), 1, matFile ); // write the header
    cout << "5" << endl; 
    fwrite( pname, sizeof(char), matFileHeader.namelen, matFile ); // write out name of variable
    cout << "6" << endl; 
    fwrite( FeatureVectorArray, sizeof(double), matFileHeader.rows*matFileHeader.cols, matFile );
    cout << "7" << endl;        

    fclose(matFile);
    printf("MAT4 File for extracted voxels is generated!\n"); 

}


void MAT4FeatureVectorPolyps(float input[][18], int num_regions, char* filename)
{
 
    MAT4HEADER matFileHeader;
    char *pname = "FeatureVectorPolyps";

    FILE *matFile; 
	   matFile = fopen(filename, "wb");

    if (matFile == 0)
    {
         std::cerr << "couldn't open file" << "\n;";
         exit(0);
    }
    
    int i = 0;

 
    double FeatureVectorArray[18*num_regions];

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

     for (int m = 0; m < num_regions; m++)
     {
          for (int j = 0; j < 18; j++)
          {
                FeatureVectorArray[m+num_regions*j] = input[m][j];
          }
     }


    /* Define header parameters */
    matFileHeader.type = 0000;
    matFileHeader.rows = num_regions;
    matFileHeader.cols = 18;
    matFileHeader.imag = 0;
    matFileHeader.namelen = 20;
    
    fwrite( &matFileHeader, sizeof(MAT4HEADER), 1, matFile ); // write the header
    fwrite( pname, sizeof(char), matFileHeader.namelen, matFile ); // write out name of variable
    fwrite( FeatureVectorArray, sizeof(double), matFileHeader.rows*matFileHeader.cols, matFile );
       
    fclose(matFile);
    printf("MAT4 File for candidate polyps is generated!\n"); 

}
