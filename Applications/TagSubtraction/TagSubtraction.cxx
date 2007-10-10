/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

// command line app to subtract tagged stool from images
 
#include <iostream>
#include <cstdlib>
using namespace std;

// ITK includes
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"
#include "ctcSegmentColonWithContrastFilter.h"
#include "vul_arg.h"

int main(int argc, char ** argv)
{
  // parse args
  vul_arg<char const*> infile(0, "Input DICOM directory");
  vul_arg<char const*> outfile(0, "Output DICOM File");
  vul_arg<int> replaceval("-r", 
			  "Replacement HU value for tagged regions (default -900)", 
			  -900);
  vul_arg_parse(argc, argv);

  // read in the DICOM series
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();
  reader->SetDirectory(string(infile()));
  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  // segment air + constrast
  clog << "Starting Segment";
  ctc::SegmentColonWithContrastFilter::Pointer filter = ctc::SegmentColonWithContrastFilter::New();
  filter->SetInput( reader->GetOutput() );
  clog << "Done Segmenting." << endl;

  // loop through the voxels, testing for threshold and lumen test
  // replace image values with air
  int replace = replaceval();
  typedef itk::ImageRegionIterator<ctc::CTCImageType> InputIteratorType;
  typedef itk::ImageRegionConstIterator<ctc::BinaryImageType> BinaryIteratorType;
  InputIteratorType it1(reader->GetOutput(), reader->GetOutput()->GetRequestedRegion());
  BinaryIteratorType it2(filter->GetOutput(), filter->GetOutput()->GetRequestedRegion());
  for( it1.GoToBegin(), it2.GoToBegin(); 
       !it1.IsAtEnd() || !it2.IsAtEnd(); 
       ++it1, ++it2)
    {
      if( (it2.Get() == 255) && (it1.Get() > 0) )
	{
	  it1.Set(replace);
	}
    }


  // write out modified image
  typedef itk::ImageFileWriter< ctc::CTCImageType >  WriterType; 
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outfile()); 
  writer->SetInput( reader->GetOutput() );
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
  
  return EXIT_SUCCESS;
}
