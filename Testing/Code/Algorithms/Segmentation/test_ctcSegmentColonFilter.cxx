/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

// test for ctcSegmentColonFilter
 
#include <iostream>
#include <cstdlib>

// ITK IO includes
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"
#include "ctcSegmentColonFilter.h"
#include "vul_arg.h"

using namespace std;

// global typedefs
typedef itk::ImageFileWriter< ctc::BinaryImageType >  WriterType; 

int main(int argc, char ** argv)
{

  clog << "Testing ctcSegmentColonFilter version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl; 
  
  // make sure testing data path is defined
  if(!getenv("CTC_DATA_DIR"))
    {
      cerr << "Error: Environment variable CTC_DATA_DIR not defined" << endl;
      return EXIT_FAILURE;
    }

  vul_arg<char const*> infile(0, "Dicom directory");
  vul_arg<char const*> outfilename(0, "Output filename");
  vul_arg<int> maxiter("-i", "Maximum number of iterations", 5);
  vul_arg<int> mindist("-d", "Minimum distance considered", 10);
  vul_arg<int> thresh("-t", "Air/Tissue threshold", -800);
  vul_arg_parse(argc, argv);

  // get paths to input/output files
  string infilename = getenv("CTC_DATA_DIR");
  infilename.append("/");
  infilename.append(infile());

  // read in the DICOM series
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();
  
  reader->SetDirectory(infilename);

  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  ctc::SegmentColonFilter::Pointer filter = ctc::SegmentColonFilter::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetThreshold(thresh());
  filter->SetMaxIterations(maxiter());
  filter->SetMinDistanceThreshold(mindist());
  
  WriterType::Pointer writer = WriterType::New(); 
  writer->SetFileName(outfilename()); 
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
