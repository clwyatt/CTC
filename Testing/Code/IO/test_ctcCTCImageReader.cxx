/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
/*
Test for ctcCTCImageReader class
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
#include "itkCropImageFilter.h"

// CTC includes
#include "ctcConfigure.h"
#include "vul_arg.h"
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"

using namespace ctc;

int main( int argc, char* argv[] )
{

  clog << "Testing ctcCTCImageReader version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl; 

  // make sure testing data path is defined
  if(!getenv("CTC_DATA_DIR"))
    {
      cerr << "Error: Environment variable CTC_DATA_DIR not defined" << endl;
      return EXIT_FAILURE;
    }
  
  vul_arg<char const*> infile(0, "Dicom directory");
  vul_arg<char const*> outfilename(0, "Output filename");
  vul_arg<double> polypx(0, "polyp coordinate - x");
  vul_arg<double> polypy(0, "polyp coordinate - y");
  vul_arg<double> polypz(0, "polyp coordinate - z");
  vul_arg<int> cropsize(0, "half width of region to crop");

  vul_arg_parse(argc, argv);
    
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();

  // prepend CTC_DATA_DIR
  string infilename = getenv("CTC_DATA_DIR");
  infilename.append("/");
  infilename.append( infile() );

  reader->SetDirectory( infilename );

  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  // map dicom coordinates to an image index
  ctc::CTCImageType::Pointer image = reader->GetOutput();
  itk::Point<double, 3> point;
  point[0] = polypx();
  point[1] = polypy();
  point[2] = polypz();
  ctc::CTCImageType::IndexType index;
  bool inside = image->TransformPhysicalPointToIndex(point, index);

  
  if(inside)
    {
      std::clog << "Inside " << index << std::endl;
      
      // crop filter
      typedef itk::CropImageFilter< ctc::CTCImageType, ctc::CTCImageType >
	FilterType;
      FilterType::Pointer filter = FilterType::New();
      filter->SetInput(reader->GetOutput());
      
      itk::Size<3> start, end;
      ctc::CTCImageType::SizeType fullsize = 
	reader->GetOutput()->GetLargestPossibleRegion().GetSize();
      
      start[0] = index[0]-cropsize();
      start[1] = index[1]-cropsize();
      start[2] = index[2]-cropsize();
      
      end[0] = fullsize[0] - (index[0]+cropsize());
      end[1] = fullsize[1] - (index[1]+cropsize());
      end[2] = fullsize[2] - (index[2]+cropsize());

      filter->SetLowerBoundaryCropSize(start);      
      filter->SetUpperBoundaryCropSize(end);
      
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
    }
 else 
    {
      std::clog << "Outside\n" << std::endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}
