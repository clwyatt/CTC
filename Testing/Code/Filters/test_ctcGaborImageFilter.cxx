/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

// test for ctcGaborImageFilter
 
#include <iostream>
#include <cstdlib>

// ITK IO includes
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// CTC includes
#include "ctcConfigure.h"
#include "ctcGaborImageFilter.h"
#include "vul_arg.h"

using namespace std;

// global typedefs
typedef short InputPixelType;
typedef float OutputPixelType;
const unsigned int Dimension = 3;
typedef itk::Image< InputPixelType, Dimension > InputImageType;
typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
typedef itk::ImageFileWriter< OutputImageType > WriterType; 
typedef ctc::GaborImageFilter< InputImageType, OutputImageType > FilterType;

int main(int argc, char ** argv)
{

  clog << "Testing ctcGaborImageFilter version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl; 
  
  // make sure testing data path is defined
  if(!getenv("CTC_DATA_DIR"))
    {
      cerr << "Error: Environment variable CTC_DATA_DIR not defined" << endl;
      return EXIT_FAILURE;
    }

  vul_arg<char const*> infile(0, "Input filename (type short or ushort)");
  vul_arg<char const*> outfilename(0, "Output filename");
  vul_arg<float> sigma("-s", "sigma value", 1.0);
  vul_arg_parse(argc, argv);


  // get paths to input/output files
  string infilename = getenv("CTC_DATA_DIR");
  infilename.append("/");
  infilename.append(infile());

  ReaderType::Pointer reader = ReaderType::New(); 
  reader->SetFileName(infilename.c_str()); 

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
  filter->SetSigma(sigma());
  
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
