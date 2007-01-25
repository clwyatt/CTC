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
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "ctcRVFImageIO.h"

// CTC includes
#include "ctcConfigure.h"
#include "ctcSegmentColonFilter.h"
#include "vul_arg.h"

using namespace std;

// global typedefs
typedef short InputPixelType;
typedef unsigned char OutputPixelType;
const unsigned int Dimension = 3;
typedef itk::Image< InputPixelType, Dimension > InputImageType;
typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
typedef itk::ImageFileWriter< OutputImageType > WriterType; 
typedef ctc::SegmentColonFilter< InputImageType, OutputImageType > FilterType;

int main(int argc, char ** argv)
{

  clog << "Testing ctcSegmentColonFilter version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl; 
  
  vul_arg<char const*> infilename(0, "Input filename (type short or ushort)");
  vul_arg<char const*> outfilename(0, "Output filename");
  vul_arg<int> maxiter("-i", "Maximum number of iterations", 5);
  vul_arg<int> mindist("-d", "Minimum distance considered", 10);
  vul_arg<int> thresh("-t", "Air/Tissue threshold", -800);
  vul_arg_parse(argc, argv);


  typedef ctc::RVFImageIO ImageIOType;
  ReaderType::Pointer reader = ReaderType::New(); 
  ImageIOType::Pointer rvfIO = ImageIOType::New();

  reader->SetImageIO( rvfIO );
  reader->SetFileName(infilename()); 

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
  
  FilterType::Pointer filter = FilterType::New();
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
