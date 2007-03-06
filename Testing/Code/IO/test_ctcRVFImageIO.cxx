/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id: test_ctcRVFImageIO.cxx 23 2006-06-29 14:45:24Z clwyatt $
Language:  C++
*******************************************************************************/

// test for ctcRVFImageIO reader
 
#include <iostream>
#include <cstdlib>
#include <string>

// ITK includes
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// CTC includes
#include "ctcConfigure.h"
#include "ctcRVFImageIO.h"

#define ASSERT_EPS 0.1

using namespace std;

int main(int argc, char ** argv)
{
  clog << "Testing ctcRVFImageIO version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl;

  // make sure testing data path is defined
  if(!getenv("CTC_DATA_DIR"))
    {
      cerr << "Error: Environment variable CTC_DATA_DIR not defined" << endl;
      return EXIT_FAILURE;
    }

  
  if(argc != 3)
    {	
      cerr << "Usage: " << std::endl;
      cerr << argv[0] << " inputImageFile outputImageFile" << endl;
      return(EXIT_FAILURE);
    }

  // get paths to input/output files
  string infilename = getenv("CTC_DATA_DIR");
  infilename.append("/");
  infilename.append(argv[1]);
  string outfilename = argv[2];

  typedef unsigned short   PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< ImageType > WriterType; 
  typedef ctc::RVFImageIO ImageIOType;

  ReaderType::Pointer reader = ReaderType::New(); 
  ImageIOType::Pointer rvfIO = ImageIOType::New();

  reader->SetFileName( infilename.c_str() ); 
  
  reader->SetImageIO( rvfIO );

  try 
    { 
      reader->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      cerr << "ExceptionObject caught !" << endl; 
      cerr << err << endl; 
      return EXIT_FAILURE; 
    } 

  
  ImageType::Pointer image = reader->GetOutput();

  const ImageType::SizeType size =  image->GetBufferedRegion().GetSize();
  clog << "Size = "; clog << size[0] 
       << ", " << size[1] 
       << ", " << size[2] 
       << endl;
  assert(size[0] == 512);
  assert(size[1] == 512);
  assert(size[2] == 481);

  const ImageType::SpacingType& sp = image->GetSpacing(); 
  clog << "Spacing = "; 
  clog << sp[0] << ", " 
       << sp[1] << ", " 
       << sp[2] 
       << endl;
  assert(fabs(sp[0]-0.664062) < ASSERT_EPS);
  assert(fabs(sp[1]-0.664062) < ASSERT_EPS);
  assert(fabs(sp[2]-1) < ASSERT_EPS);

  const ImageType::PointType& orgn = image->GetOrigin(); 
  clog << "Origin = "; 
  clog << orgn[0] << ", " 
       << orgn[1] << ", " 
       << orgn[2] 
       << endl;
  assert(fabs(orgn[0]-170.6) < ASSERT_EPS);
  assert(fabs(orgn[1]-170.0) < ASSERT_EPS);
  assert(fabs(orgn[2]+532.46) < ASSERT_EPS);

  WriterType::Pointer writer = WriterType::New(); 

  writer->SetFileName( outfilename.c_str() ); 
  
  writer->SetImageIO(rvfIO);

  writer->SetInput( reader->GetOutput() );

  try 
    { 
      writer->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      cerr << "ExceptionObject caught !" << endl; 
      cerr << err << std::endl; 
      return EXIT_FAILURE; 
    } 

  return(EXIT_SUCCESS);
}
