#include <iostream>
#include <cstdlib>


#define POINTS 1000
#define FEATURES 6
using namespace std;


/* matlab MAT-4 file format header */
typedef struct {
  int type;    /* type */
  int rows;    /* row dimension */
  int cols;    /* column dimension */
  int imag;    /* flag indicating imag part */
  int namelen; /* name length (including NULL) */
} MAT4HEADER;

int main()
{ 
    double FeatureVectorArray[POINTS*FEATURES];

    /* Fill up the array
     * FeatureVectorArray[j*POINTS+i] = FeatureValue
     * Where i ---> # of points
     *       j ---> # of features
     * */

    FILE *matFile; 
	   matFile = fopen("FeatureVector.mat", "wb");

    if (matFile == 0)
    {
         std::cerr << "couldn't open file" << "\n;";
         exit(0);
    }

    MAT4HEADER matFileHeader;

    char *pname = "FeatureVector";
    matFileHeader.type = 0000;
    matFileHeader.rows = POINTS;
    matFileHeader.cols = FEATURES;
    matFileHeader.imag = 0;
    matFileHeader.namelen = 14;

    fwrite( &matFileHeader, sizeof(MAT4HEADER), 1, matFile ); // write the header
    fwrite( pname, sizeof(char), matFileHeader.namelen, matFile ); // write out name of variable
    fwrite( FeatureVectorArray, sizeof(double), matFileHeader.rows*matFileHeader.cols, matFile );
       
    fclose(matFile);


    printf("Done!\n");     
}

