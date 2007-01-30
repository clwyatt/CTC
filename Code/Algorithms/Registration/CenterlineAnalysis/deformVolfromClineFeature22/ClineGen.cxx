#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "tnt/tnt_array3d.h"

/* Dijstra_heap.cpp  */
extern unsigned int Dijkstra_main3(TNT::Array3D<int> bVoxel, int startX, int startY, int startZ, int dimx, int dimy, int dimz);
extern void storeCenterline(TNT::Array3D<int> Cline, int sizeX, int sizeY, int sizeZ, int CentValue, int dilate);
                                                                                                   
/** distancehook2.cpp  **/
extern void distancehook(TNT::Array3D<int> &vin, int dimx,int dimy,int dimz);



int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputFileName DilateSize OutputFileName CenterlineValue  ";
 
    std::cerr << std::endl;  
    return EXIT_FAILURE;
    }

  typedef  unsigned short   InputPixelType;
  typedef  unsigned short  OutputPixelType;
  const unsigned int Dimension = 3;

  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;
                        
  typedef itk::ImageFileReader< InputImageType  >  ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );  reader->Update();

  InputImageType::RegionType::SizeType  Imagesize;

  Imagesize =  reader->GetOutput()->GetRequestedRegion().GetSize();
  std::cout<<"sizeX="<<Imagesize[0]<<" sizeY="<<Imagesize[1]<<" sizeZ="<<Imagesize[2]<<std::endl;

  int sizeX, sizeY, sizeZ;
  sizeX =Imagesize[0];     sizeY = Imagesize[1];    sizeZ = Imagesize[2];
  std::cout<<"sizeX="<< sizeX<<" sizeY="<<sizeY<<" sizeZ="<<sizeZ<<std::endl;

  TNT::Array3D<int> bVoxel(sizeX, sizeY, sizeZ, 0);


  /************************************************************************************/
  typedef itk::ImageRegionConstIterator< InputImageType > ConstIteratorType;
  InputImageType::RegionType inputFixedRegion;
  InputImageType::RegionType::SizeType  Fixedsize;
  
  Fixedsize =  reader->GetOutput()->GetRequestedRegion().GetSize();

  InputImageType::RegionType::IndexType inputFixedStart;
  inputFixedStart[0]=0;   inputFixedStart[1]=0;  inputFixedStart[2]=0;
  std::cout<<"sizeX="<<Fixedsize[0]<<" sizeY="<<Fixedsize[1]<<" sizeZ="<<Fixedsize[2]<<std::endl;

  inputFixedRegion.SetSize( Fixedsize );
  inputFixedRegion.SetIndex( inputFixedStart );
  
  ConstIteratorType inputFixedIt(   reader->GetOutput(), inputFixedRegion  );
  int value1 = 0;
  int i,j,k;
  
  for(k=0; k < sizeZ; k++){
    for(j=0; j < sizeY; j++){
      for(i=0; i < sizeX; i++){

          value1 = inputFixedIt.Get();   //if(value1 != 0) std::cout<< "value="<<value1<<" at"<<i<<", "<<j<<", "<<k<<std::endl;
          bVoxel[i][j][k] = value1;
          ++inputFixedIt;
          
      }
    }
  }

    distancehook(bVoxel, sizeX, sizeY, sizeZ);
    unsigned int size=0; 
    int startX=0, startY=0, startZ=0;

    //startX=121; startY=110; startZ=35; // 443074_2146_104.rvf
    //startX=76; startY=112; startZ=90; // 443074_2146_102.rvf
    //startX=127; startY=151; startZ=17; //anus hole of  443074_2146_102.rvf
    //startX=126; startY=126; startZ=20; //anus hole of  443074_2146_104tube.rvf
    //startX=126; startY=134; startZ=20;
    //startX=126; startY=151; startZ=20;  // ok
    startX=127; startY=151; startZ=17;
                                                           
    size = Dijkstra_main3(bVoxel, startX, startY, startZ, sizeX, sizeY, sizeZ);
    int CentValue, DilateSize;
    CentValue = atoi(argv[4]);
    DilateSize = atoi(argv[2]);
               
    TNT::Array3D<int> Cline(sizeX, sizeY, sizeZ, 0);
    storeCenterline(Cline, sizeX, sizeY, sizeZ, CentValue, DilateSize);
                                                           
    OutputImageType::IndexType Wstart;  Wstart[0]=0; Wstart[1]=0; Wstart[2]=0;
    OutputImageType::SizeType Wsize;    Wsize[0]= sizeX;  Wsize[1]= sizeY;  Wsize[2]= sizeZ;
    OutputImageType::RegionType Wregion;

       Wregion.SetSize(Wsize);
       Wregion.SetIndex(Wstart);
    
    OutputImageType::Pointer OutputImage = OutputImageType::New();
                                                
        OutputImage->SetRegions(Wregion);
        OutputImage->Allocate();
 

 /////////////////////////////////////////////////////////////////////////////////////////
      typedef itk::ImageRegionIteratorWithIndex< OutputImageType > IteratorType;
                                                     
      IteratorType  OutputIt(  OutputImage,  OutputImage->GetRequestedRegion() );
      
      for(k=0; k < sizeZ; k++){
        for(j=0; j < sizeY; j++){
          for(i=0; i < sizeX; i++){

              value1 = Cline[i][j][k];
              OutputIt.Set(value1);   //if(value1 != 0) std::cout<< "value="<<value1<<" at"<<i<<", "<<j<<", "<<k<<std::endl;
              ++OutputIt;             
          }   
        }
      }
     
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput(OutputImage);
  
  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return -1;
    }
    

   return EXIT_SUCCESS;
}

