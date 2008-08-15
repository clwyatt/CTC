/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
/*
Test for ctcCenterlineSpatialObject class
*/

#include <cstdlib>
#include <iostream>
using std::clog;
using std::cerr;
using std::endl;

#include <string>
using std::string;

// CTC includes
#include "ctcConfigure.h"
#include "vul_arg.h"
#include "ctcCenterlineSpatialObject.h"

using namespace ctc;

int main( int argc, char* argv[] )
{

  clog << "Testing ctcCenterlineSpatialObject version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl; 
  
  vul_arg<string> outfilename(0, "Output filename");
  vul_arg_parse(argc, argv);
    
  ctc::CenterlineSpatialObject::Pointer cline = 
    ctc::CenterlineSpatialObject::New();


  // try
  //   {
  //     reader->Update();
  //   }
  // catch (itk::ExceptionObject &ex)
  //   {
  //     std::cout << ex << std::endl;
  //     return EXIT_FAILURE;
  //   }

  return EXIT_SUCCESS;

}
