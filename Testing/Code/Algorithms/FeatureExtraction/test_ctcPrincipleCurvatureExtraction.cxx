/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
/*
Test for ctcPrincipleCurvatureExtraction class
*/

#include <cstdlib>
#include <iostream>
using std::clog;
using std::cerr;
using std::endl;

#include <string>
using std::string;

// ITK includes

// CTC includes
#include "ctcConfigure.h"
#include "vul_arg.h"
#include "ctcCTCImage.h"
#include "itkImageFileReader.h"
#include "ctcSegmentColonFilter.h"
#include "ctcPrincipleCurvatureExtraction.h"

using namespace ctc;

int main( int argc, char* argv[] )
{

  clog << "Testing ctcPricipleCurvatureExtraction version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl; 
  
  // make sure testing data path is defined
  if(!getenv("CTC_DATA_DIR"))
    {
      cerr << "Error: Environment variable CTC_DATA_DIR not defined" << endl;
      return EXIT_FAILURE;
    }

  vul_arg<char const*> datainfile(0, "input volume data file");
  vul_arg<char const*> seginfile(0, "input segmented volume file");
  vul_arg<char const*> outfilename(0, "Output feature filename");
  vul_arg_parse(argc, argv);

  // get paths to input/output files
  string rawinfilename = getenv("CTC_DATA_DIR");
  rawinfilename.append("/");
  rawinfilename.append(datainfile());
  string seginfilename = getenv("CTC_DATA_DIR");
  seginfilename.append("/");
  seginfilename.append(seginfile());


  // read data
  typedef itk::ImageFileReader< ctc::CTCImageType > RawDataReaderType;
  RawDataReaderType::Pointer rawreader = RawDataReaderType::New();
  rawreader->SetFileName(rawinfilename);

  // read segmented data
  typedef itk::ImageFileReader< ctc::BinaryImageType > SegDataReaderType;
  SegDataReaderType::Pointer segreader = SegDataReaderType::New();
  segreader->SetFileName(seginfilename);

  // invoke readers
  try
    {
      rawreader->Update();
      segreader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  // extract the features
  typedef ctc::PrincipleCurvatureExtraction FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetBinaryInput(segreader->GetOutput());
  filter->SetImageInput(rawreader->GetOutput());
  std::clog << "Computing Features ...";
  try
    {
      filter->Compute();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }
  std::clog << " Done." << std::endl;
    
  // output
  std::ofstream out(outfilename());
  FilterType::FeatureSampleType::Pointer features = filter->GetOutput();
  for(int i = 0; i < features->Size(); i++)
    {
      out << features->GetMeasurementVector(i) << std::endl;
    }
  out.close();

  return EXIT_SUCCESS;

}
