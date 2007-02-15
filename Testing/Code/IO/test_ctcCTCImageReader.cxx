/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
/*
Test for ctcCTCImageReader class
*/

#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;

#include <string>
using std::string;

// ITK includes
#include "itkImageFileWriter.h"

// CTC includes
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"

using namespace ctc;

int main( int argc, char* argv[] )
{
  
  if( argc != 3 )
    {
      cerr << "Usage: " << argv[0] 
	   << " DicomDirectory OutputFile" 
	   << endl;
      return EXIT_FAILURE;
    }
  
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();

  reader->SetDirectory(string(argv[1]));

  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  typedef itk::ImageFileWriter< ctc::CTCImageType > WriterType; 
  WriterType::Pointer writer = WriterType::New(); 

  writer->SetFileName( argv[2] ); 

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

  return EXIT_SUCCESS;

}
