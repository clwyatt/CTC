/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

// command line app to register CTC images using intensity (TESTING)
 
#include <iostream>
#include <cstdlib>
using namespace std;

// ITK includes
#include "itkImageSource.h"
#include <itkBSplineDownsampleImageFilter.h>

// CTC includes
#include <ctcConfigure.h>
#include <ctcCTCImage.h>
#include <ctcCTCImageReader.h>
#include <vul_arg.h>
using namespace ctc;

class ReadAndDownsample :
  public itk::ImageSource<CTCImageType>
{
public:
  typedef ReadAndDownsample                        Self;
  typedef itk::ImageSource<CTCImageType>        Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;

  // standard ITK macros
  itkNewMacro(Self);
  itkTypeMacro(CTCImageReader, itk::ImageSource);
  
  itkGetMacro(Directory, std::string);
  itkSetMacro(Directory, std::string);
    
protected:

  ReadAndDownsample(){};
  ~ReadAndDownsample(){};
  void GenerateData() throw(itk::ExceptionObject)
  {
    clog << "Start: GenerateData" << endl;
    // read
    ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();
    reader->SetDirectory(m_Directory);
    try
      {
	reader->Update();
      }
    catch(itk::ExceptionObject & ex)
      {
	throw ex;
      }

    // downsample
    typedef itk::BSplineDownsampleImageFilter<CTCImageType, CTCImageType> 
      DownsampleFilterType;
    DownsampleFilterType::Pointer dfilter = DownsampleFilterType::New();
    dfilter->SetInput(reader->GetOutput());
    dfilter->Update();

    this->GraftOutput( dfilter->GetOutput() );
    clog << "End: GenerateData" << endl;
  }
 

private:

  ReadAndDownsample(Self&);                // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented

  std::string m_Directory;
  
};

int main(int argc, char ** argv)
{
  // parse args
  vul_arg<char const*> sourcedir(0, "Source Input DICOM directory");
  vul_arg<char const*> targetdir(0, "Target Input DICOM directory");
  vul_arg<char const*> outfile(0, "Output file");
  vul_arg_parse(argc, argv);

  // read and downsample source
  ReadAndDownsample::Pointer sourceproc = ReadAndDownsample::New();
  sourceproc->SetDirectory(string(sourcedir()));
  try
    {
      sourceproc->Update();
    }
  catch (itk::ExceptionObject & ex)
    {
      std::cout << ex << std::endl;
      abort();
    }
  
  // read and downsample target

  
  return EXIT_SUCCESS;
}
