/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#include <iostream>
#include "ColonSegmenterCLP.h"

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"
#include "ctcSegmentColonFilter.h"
#include "ctcSegmentColonWithContrastFilter.h"
#include "ctcSegmentProjectionFilter.h"

#include "itkImageFileWriter.h"

using namespace std;

int main(int argc, char * argv [])
{
  // CLP magic
  PARSE_ARGS;
  
  /// Get the source image.
  typedef itk::ImageFileReader< ctc::CTCImageType >  ReaderType;
  ReaderType::Pointer inputReader = ReaderType::New();
  inputReader->SetFileName(inputVolume );
  
  try
    {
      inputReader->Update();
    }
  catch(itk::ExceptionObject &e)
    {
      cerr << "Reading of Source Image Failed." << endl;
      cerr << e << endl;
      exit(EXIT_FAILURE);
    }
  
  
  return EXIT_SUCCESS;
}

