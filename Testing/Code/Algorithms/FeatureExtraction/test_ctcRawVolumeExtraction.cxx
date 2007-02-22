/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
/*
Test for ctcRawVolumeExtraction class
*/

#include <cstdlib>
#include <iostream>
using std::clog;
using std::cerr;
using std::endl;

#include <string>
using std::string;

// ITK includes
#include "itkImageFileWriter.h"

// CTC includes
#include "ctcConfigure.h"
#include "vul_arg.h"
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"
#include "ctcRawVolumeExtraction.h"

using namespace ctc;

int main( int argc, char* argv[] )
{

  clog << "Testing ctcRawVolumeExtraction version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl; 
  
  vul_arg<char const*> infilename(0, "Dicom directory");
  vul_arg<char const*> outfilename(0, "Output filename");
  vul_arg<double> polypx(0, "polyp coordinate - x");
  vul_arg<double> polypy(0, "polyp coordinate - y");
  vul_arg<double> polypz(0, "polyp coordinate - z");
  vul_arg<double> cropsize(0, "half width of region to crop (in mm)");

  vul_arg_parse(argc, argv);
    
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();

  reader->SetDirectory(string(infilename()));

  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  typedef ctc::RawVolumeExtraction FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(reader->GetOutput());
  itk::Point<double, 3> point;
  point[0] = polypx();
  point[1] = polypy();
  point[2] = polypz();
  filter->SetDICOMCoordinate(point);
  filter->SetCropSize(cropsize());
  filter->SetSigma(1.0);
    
  // output
  typedef itk::ImageFileWriter< ctc::CTCImageType > WriterType; 
  WriterType::Pointer writer = WriterType::New(); 
  
  writer->SetFileName( outfilename() ); 
  
  writer->SetInput( filter->GetOutput() );
  
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
