#ifndef _WOOK_HEADER_


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "tnt/tnt_array3d.h"
#include "itkImportImageFilter.h"
#include "myDataType.h"

using std::cerr;
using std::cout;
using std::endl; 


typedef  unsigned short   InputPixelType;
typedef  unsigned short  OutputPixelType;
const unsigned int Dimension = 3;
 
typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;

typedef itk::ImageFileReader< InputImageType  >  ReaderType;
typedef itk::ImageFileWriter< OutputImageType >  WriterType;

typedef itk::ImageRegionConstIterator< InputImageType > ConstIteratorType;
typedef itk::ImageRegionIteratorWithIndex< OutputImageType > IteratorType;

typedef itk::ImportImageFilter< OutputPixelType, Dimension >   ImportFilterType;

class ClineVoxel{
	
	private: 
	         COORD<int> *co;  	 
		  
	public: 
	          int clineSize;		 
		   int cnt;
		    COORD<float> *dump;
		   int dump_size;
	
	public:
	void SetSize_ClineVoxel(int size); 		
	void SetValue(COORD<int> r); 

	COORD<int> GetVoxelCoord(int index);
	int Get_ClineSize(void);
	
	COORD<float>* FindSliding3Dmv(int m);
	COORD<float>* MovingAvg(int m);
		
	~ClineVoxel( ){
		
//		delete []co;
//		delete []dump;
	}
};

class MatchedPair {
	public:
	          COORD<int> *src, *trg;
			  int win_size;
	 
	public:
			  void initMatchedPair(int window_size);
			  void MatchedPair::fillMatchedPair(ClineVoxel &cS, ClineVoxel &cT, int startS, int startL );
	
};


 
#define _WOOK_HEADER_  
#endif 
