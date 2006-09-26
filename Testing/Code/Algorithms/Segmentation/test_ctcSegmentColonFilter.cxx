/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

// test for ctcSegmentColonFilter
 
#include <iostream>
#include <cstdlib>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "ctcConfigure.h"
#include "ctcSegmentColonFilter.h"

using namespace std;

int main(int argc, char ** argv)
{
  
  if(argc != 3)
    {	
      cerr << "Testing ctcSegmentColonFilter version ";
      cerr <<  CTC_VERSION_STRING << CTC_REVISION_STRING << std::endl; 
      cerr << "Usage: " << std::endl;
      cerr << argv[0] << " inputImageFile outputImageFile" << endl;
      return(EXIT_FAILURE);
    }
  
  typedef unsigned short   PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< ImageType > WriterType; 

  typedef ctc::SegmentColonFilter< ImageType > FilterType;

  ReaderType::Pointer reader = ReaderType::New(); 
  reader->SetFileName( argv[1] ); 

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );

  WriterType::Pointer writer = WriterType::New(); 
  writer->SetFileName( argv[2] ); 
  writer->SetInput( filter->GetOutput() );

  try 
    { 
      writer->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      cerr << "ExceptionObject caught !" << endl; 
      cerr << err << endl; 
      return EXIT_FAILURE; 
    } 

  return(EXIT_SUCCESS);
}
