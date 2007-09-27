#include <iostream>
#include <cstdlib>


#define POINTS 4
#define FEATURES 3
using namespace std;

void MAT4FeatureVector(double *vals, int numV, int numF);

/* matlab MAT-4 file format header */
typedef struct {
  int type;    /* type */
  int rows;    /* row dimension */
  int cols;    /* column dimension */
  int imag;    /* flag indicating imag part */
  int namelen; /* name length (including NULL) */
} MAT4HEADER;


/* File Converter Function */
/* vals --- the array holding the feature vectors
 * numV --- number of detected voxels in candidate polyps
 * numF --- number of features for each voxel */
void MAT4FeatureVector(double *vals, int numV, int numF)
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

    matFileHeader.type = 0000;
    matFileHeader.rows = numV;
    matFileHeader.cols = numF;
    matFileHeader.imag = 0;
    matFileHeader.namelen = 14;

    
    fwrite( &matFileHeader, sizeof(MAT4HEADER), 1, matFile ); // write the header
    fwrite( pname, sizeof(char), matFileHeader.namelen, matFile ); // write out name of variable
    fwrite( vals, sizeof(double), matFileHeader.rows*matFileHeader.cols, matFile );
       
    fclose(matFile);
    printf("MAT4 File is generated as FeatureVector.mat!\n"); 

}


int main()
{ 
    /* Fill up the array
     * FeatureVectorArray[j*POINTS+i] = FeatureValue
     * Where i ---> # of points
     *       j ---> # of features
     */
    double FeatureVectorArray[POINTS*FEATURES] = {1,4,7,10,2,5,8,11,3,6,9,12};

    /* Actually the above array is acquired from following feature vectors
     * V1 (1,2,3)
     * V2 (4,5,6)
     * V3 (7,8,9)
     * V4 (10,11,12)
     */

    /* The generated 2D matrix in .mat file is like this:
     * x = [1 2 3
     *      4 5 6
     *      7 8 9
     *      10 11 12]*/

    MAT4FeatureVector(FeatureVectorArray, POINTS, FEATURES); 
}

