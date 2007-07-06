#include <cstdlib>
#include <iostream>
using std::clog;
using std::cerr;
using std::cout;
using std::endl;
#include <fstream>
using std::ifstream;
using std::ofstream;

#include <string>
using std::string;
#include <cctype>

// ITK includes
#include <itkImage.h>
#include <itkImageFileWriter.h>

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImageReader.h"
#include "vul_arg.h"


int main( int argc, char* argv[] )
{
  // parse args
  vul_arg<char const*> imagefilename(0, "Image file");
  vul_arg<char const*> infilename(0, "Input list of coordinates, comma seperated");
  vul_arg<char const*> outfilename(0, "Output list of coordinates");
  vul_arg_parse(argc, argv);

  // read image
  typedef itk::ImageFileReader< ctc::CTCImageType > RawDataReaderType;
  RawDataReaderType::Pointer rawreader = RawDataReaderType::New();
  rawreader->SetFileName(imagefilename() );
  try
    {
      rawreader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }
  
  ifstream infile( infilename() );
  ofstream outfile( outfilename() );
  float x, y, z;
  string entrystr;
  while(infile.good()) 
    {
      getline(infile,entrystr);
      clog << entrystr << endl;
      
      // x field
      int start = 0;     
      int end = entrystr.find_first_of(',', start);
      x = atof( entrystr.substr(start, end-start).c_str() );

      // y field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      y = atof( entrystr.substr(start, end-start).c_str() );

      // z field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      z = atof( entrystr.substr(start, end-start).c_str() );

      itk::Point<double, 3> dcmCoordinate;
      ctc::CTCImageType::IndexType index;
      dcmCoordinate[0] = x;
      dcmCoordinate[1] = y;
      dcmCoordinate[2] = z;

      rawreader->GetOutput()->TransformPhysicalPointToIndex(dcmCoordinate, index);

      clog << x << " " << y << " " << z << endl; 
      outfile << index[0] << "," << index[1] << "," << index[2] << endl;

    }

  infile.close();
  outfile.close();

  return EXIT_SUCCESS;

}
