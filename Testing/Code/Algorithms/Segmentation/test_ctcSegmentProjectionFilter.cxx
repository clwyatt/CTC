/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

// test for ctcSegmentProjectionFilter
 
#include <iostream>
#include <cstdlib>
#include <string>

// ITK IO includes
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImage.h"
#include "ctcSegmentProjectionFilter.h"
#include "vul_arg.h"

using namespace std;

// global typedefs
typedef itk::ImageFileReader< ctc::BinaryImageType >  ReaderType; 
typedef itk::ImageFileWriter< ctc::ProjectionImageType >  WriterType; 

int main(int argc, char ** argv)
{

  clog << "Testing ctcSegmentProjectionFilter version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl; 
  
  // make sure testing data path is defined
  if(!getenv("CTC_DATA_DIR"))
    {
      cerr << "Error: Environment variable CTC_DATA_DIR not defined" << endl;
      return EXIT_FAILURE;
    }

  vul_arg<char const*> infile(0, "Segmented file");
  vul_arg<char const*> outfile(0, "Output filename");
  vul_arg_parse(argc, argv);

  // get paths to input/output files
  string infilename = getenv("CTC_DATA_DIR");
  infilename.append("/");
  infilename.append(infile());

  // read in the DICOM series
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infile());

  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  // project
  ctc::SegmentProjectionFilter::Pointer filter = ctc::SegmentProjectionFilter::New();
  filter->SetInput( reader->GetOutput() );
  filter->Update();

  // write out binary image
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outfile()); 
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
