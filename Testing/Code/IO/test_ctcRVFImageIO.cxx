/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id: test_ctcRVFImageIO.cxx 23 2006-06-29 14:45:24Z clwyatt $
Language:  C++
*******************************************************************************/

// test for ctcRVFImageIO reader
 
#include <iostream>
#include <cstdlib>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "ctcRVFImageIO.h"

using namespace std;

int main(int argc, char ** argv)
{
  
  if(argc != 3)
    {	
      cerr << "Usage: " << std::endl;
      cerr << argv[0] << " inputImageFile outputImageFile" << endl;
      return(EXIT_FAILURE);
    }
  
  typedef unsigned short   PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< ImageType > WriterType; 
  typedef ctc::RVFImageIO ImageIOType;

  ReaderType::Pointer reader = ReaderType::New(); 
  ImageIOType::Pointer rvfIO = ImageIOType::New();

  reader->SetFileName( argv[1] ); 
  
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
  clog << "Size = "; clog << size[0] << ", " << size[1] << ", " << size[2] << endl;

  const ImageType::SpacingType& sp = image->GetSpacing(); 
  clog << "Spacing = "; clog << sp[0] << ", " << sp[1] << ", " << sp[2] << endl;
  
  const ImageType::PointType& orgn = image->GetOrigin(); 
  clog << "Origin = "; clog << orgn[0] << ", " << orgn[1] << ", " << orgn[2] << endl;


  WriterType::Pointer writer = WriterType::New(); 

  writer->SetFileName( argv[2] ); 
  
  writer->SetImageIO( rvfIO );

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