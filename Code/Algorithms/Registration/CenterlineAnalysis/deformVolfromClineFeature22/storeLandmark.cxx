#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkDeformationFieldSource.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "MyHeader.h"
//#include <fstream>
//#include <string.h>
#include <stdio.h>


//#define	PRINT_FEATURES
using namespace std;

void storeLandmarks(const string &fileName, int validCnt,  IndexPair* mIndexPair, MatchedPair *matchedPair, int window_size)
{

	const     unsigned int   Dimension = 3;
	typedef   float          VectorComponentType;
	typedef   itk::Vector< VectorComponentType, Dimension >    VectorType;
	typedef   itk::Image< VectorType,  Dimension >   DeformationFieldType;
	
	typedef itk::DeformationFieldSource< DeformationFieldType >  DeformationSourceType;
	typedef DeformationSourceType::LandmarkPointType          LandmarkPointType;
	
	LandmarkPointType sourcePoint;
	LandmarkPointType targetPoint;

//  std::ofstream pointsFile;
//  pointsFile.open( fileName.c_str() );

	FILE *fp;
	if((fp=fopen(fileName.c_str(),"wt"))==NULL){
		
		cout << "Landmark file open error!"<<endl;
	}
	
	int current_set = 0;
	int k,j,i, value=0;
	int feature_size;
	feature_size = window_size;
	//feature_size = 2; // for test
	
	for(i=0; i<validCnt; i++){
		if(current_set == 0){
			if((i == (validCnt-1)) ){ // Because the final pair doesn't have the next pair.
				
				current_set = 0;
				for(j=0; j<feature_size; j++){
					sourcePoint[0] = (float)matchedPair[i].src[j].x;    targetPoint[0] =(float)matchedPair[i].trg[j].x; 								
					sourcePoint[1] = (float)matchedPair[i].src[j].y;    targetPoint[1] = (float)matchedPair[i].trg[j].y;  								
					sourcePoint[2] = (float)matchedPair[i].src[j].z;    targetPoint[2] = (float)matchedPair[i].trg[j].z;								
					//sourcePoint[0] = 1.3;  		 targetPoint[0] = 112.3;
					//sourcePoint[1] = -4.8;           targetPoint[1] =  -10.0;
 					//sourcePoint[2] = 9.9;            targetPoint[2] =  -0.3;
					
					//pointsFile << sourcePoint;
					//pointsFile << targetPoint;
					fprintf(fp,"%f %f %f\n",sourcePoint[0], sourcePoint[1], sourcePoint[2]);
					fprintf(fp,"%f %f %f\n",targetPoint[0], targetPoint[1], targetPoint[2]);
					
#ifdef PRINT_FEATURES
					cout<<j<<" sourcePoint: "<<sourcePoint<<" targetPoint: "<<targetPoint<<endl;
#endif

				}  // for - j		
//cout <<i<< " indexS= "<<mIndexPair[i].SavedS_Index<<" , indexL= "<<mIndexPair[i].SavedL_Index<<endl;
				
			}
			else if( ((mIndexPair[i].SavedS_Index + window_size)<mIndexPair[i+1].SavedS_Index) &&
			   ( (mIndexPair[i].SavedL_Index + window_size)<mIndexPair[i+1].SavedL_Index) ){
		   
				current_set = 0;
				for(j=0; j<feature_size; j++){
					sourcePoint[0] = (float)matchedPair[i].src[j].x;    targetPoint[0] =(float)matchedPair[i].trg[j].x; 								
					sourcePoint[1] = (float)matchedPair[i].src[j].y;    targetPoint[1] = (float)matchedPair[i].trg[j].y;  								
					sourcePoint[2] = (float)matchedPair[i].src[j].z;    targetPoint[2] = (float)matchedPair[i].trg[j].z;								
					//sourcePoint[0] = 1.3;  		 targetPoint[0] = 112.3;
					//sourcePoint[1] = -4.8;           targetPoint[1] =  -10.0;
 					//sourcePoint[2] = 9.9;            targetPoint[2] =  -0.3;
					
					//pointsFile << sourcePoint;
					//pointsFile << targetPoint;
					fprintf(fp,"%f %f %f\n",sourcePoint[0], sourcePoint[1], sourcePoint[2]);
					fprintf(fp,"%f %f %f\n",targetPoint[0], targetPoint[1], targetPoint[2]);
#ifdef PRINT_FEATURES
					cout<<j<<" sourcePoint: "<<sourcePoint<<" targetPoint: "<<targetPoint<<endl;
#endif

				}  // for - j		
//cout <<i<< " indexS= "<<mIndexPair[i].SavedS_Index<<" , indexL= "<<mIndexPair[i].SavedL_Index<<endl;
			 }
			else { 
				if(mIndexPair[i].SavedCorrel > mIndexPair[i+1].SavedCorrel){
					current_set = 1;
					for(j=0; j<feature_size; j++){
						sourcePoint[0] = matchedPair[i].src[j].x;    targetPoint[0] = matchedPair[i].trg[j].x; 
						sourcePoint[1] = matchedPair[i].src[j].y;    targetPoint[1] = matchedPair[i].trg[j].y; 
						sourcePoint[2] = matchedPair[i].src[j].z;    targetPoint[2] = matchedPair[i].trg[j].z;
						//pointsFile << sourcePoint;
						//pointsFile << targetPoint;
						fprintf(fp,"%f %f %f\n",sourcePoint[0], sourcePoint[1], sourcePoint[2]);
						fprintf(fp,"%f %f %f\n",targetPoint[0], targetPoint[1], targetPoint[2]);
#ifdef PRINT_FEATURES
						cout<<j<<" sourcePoint: "<<sourcePoint<<" targetPoint: "<<targetPoint<<endl;
#endif
					}// for - j	
					current_set = 0;
//cout <<i<< " indexS= "<<mIndexPair[i].SavedS_Index<<" , indexL= "<<mIndexPair[i].SavedL_Index<<endl;
				}  
			}  // else
	       } // current_set == 0
		else if(current_set !=0) current_set = 0;
	}  // for - i  
  
	fclose(fp);
}

void storeImage(TNT::Array3D<int> &Volume, string &fileName, int sizeX, int sizeY, int sizeZ, ReaderType::Pointer reader)
{
	ImportFilterType::Pointer importFilter = ImportFilterType::New();
	ImportFilterType::SizeType  size;

	size[0]  = sizeX;  // size along X
	size[1]  = sizeY;  // size along Y
	size[2]  = sizeZ;  // size along Z

	ImportFilterType::IndexType start;
	start.Fill( 0 );

	ImportFilterType::RegionType region;
	region.SetIndex( start );
	region.SetSize(  size  );

	importFilter->SetRegion( region );

	cout <<"&&&  Origin = " << reader->GetOutput()->GetOrigin();
	cout <<" Spacing = " <<reader->GetOutput()->GetSpacing() << endl;
	
	importFilter->SetOrigin( reader->GetOutput()->GetOrigin() );
	importFilter->SetSpacing( reader->GetOutput()->GetSpacing() );
	
	unsigned int numOfPixels =  size[0] * size[1] * size[2];

	OutputPixelType *localBuffer = new OutputPixelType[numOfPixels];
	OutputPixelType * it = localBuffer;	
	int i,j,k, value;
	
	for(k=0; k < sizeZ; k++){
		for(j=0; j < sizeY; j++){
			for(i=0; i < sizeX; i++){
				
				value = Volume[i][j][k];
				if(value > 0) *it = 255;
				else *it = 0;
				it++;      
				
			}   
		}
	}		
	
	const bool importImageFilterWillOwnTheBuffer = true;
	importFilter->SetImportPointer( localBuffer, numOfPixels,
                                  importImageFilterWillOwnTheBuffer );
	
	WriterType::Pointer writer = WriterType::New();

	writer->SetFileName( fileName.c_str() );
	writer->SetInput(  importFilter->GetOutput()  );

	//cout <<"test--------"<<endl;
	
	try
	{
		writer->Update();
	}
	catch( itk::ExceptionObject & exp )
	{
		std::cerr << "Exception caught ! in the storeImage()" << std::endl;
		std::cerr << exp << std::endl;
	}

}
