/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id: RVFInfo.cxx 26 2006-06-29 20:59:55Z clwyatt $
Language:  C++
*******************************************************************************/
/*
Print information from RVF header and compute some statistics
*/

// System includes 
#include <iostream>
#include <cstdlib>
#include <unistd.h>

// ITK includes
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"

// CTC includes
#include "ctcConfigure.h"
#include "ctcRVFImageIO.h"

using namespace std;

void usage(int nargs, char ** args)
{
  cerr << endl;
  cerr << "rvfinfo - dump to stdout information on rvf file\n";
  cerr << "Version " << CTC_VERSION_STRING << " " << CTC_REVISION_STRING << ", compiled on " << __DATE__ << "\n\n";
  cerr << "Usage:\n";
  cerr << args[0] << " [opts] filename.rvf\n";
  cerr << "where opts is one or more of\n";
  cerr << "-h : print this message and exit\n";
  cerr << "-v : print detailed information\n";
  cerr << endl;
};

int main(int nargs, char ** args)
{
  
  // parse args
  opterr = 0;
  char optchar;
  bool hflag = false;
  bool vflag = false;
 
  while ((optchar = getopt (nargs, args, "hv")) != -1)
    switch (optchar)
      {
      case 'h':
	hflag = true;
	break;
      case 'v':
	vflag = true;
	break;
      case '?':
	if (isprint (optopt))
	  cerr << "Unknown option " << optopt << endl;
	else
	  cerr << "Unknown option character" << endl;
	usage(nargs, args);
	return (EXIT_FAILURE);
      default:
	return (EXIT_FAILURE);
      }

  if((nargs-optind) < 1){
    cerr << "Error: One or more files not provided." << endl;
    usage(nargs, args);
    exit(EXIT_FAILURE);
  }

  if(hflag)
    { 
      usage(nargs, args);
      return (EXIT_SUCCESS);
    }

  if(vflag) clog << "Verbose Mode On" << endl;

  
  typedef unsigned short   PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef ctc::RVFImageIO ImageIOType;

  ReaderType::Pointer reader = ReaderType::New(); 
  ImageIOType::Pointer rvfIO = ImageIOType::New();

  reader->SetFileName( args[optind] ); 
  
  reader->SetImageIO( rvfIO );

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

  
  ImageType::Pointer image = reader->GetOutput();

  clog << "CTC_BIG_ENDIAN = " << CTC_BIG_ENDIAN << endl;

  const ImageType::SizeType size =  image->GetBufferedRegion().GetSize();
  clog << "Size = "; clog << size[0] << ", " << size[1] << ", " << size[2] << endl;

  const ImageType::SpacingType& sp = image->GetSpacing(); 
  clog << "Spacing = "; clog << sp[0] << ", " << sp[1] << ", " << sp[2] << endl;
  
  const ImageType::PointType& orgn = image->GetOrigin(); 
  clog << "Origin = "; clog << orgn[0] << ", " << orgn[1] << ", " << orgn[2] << endl;

  if(vflag)
    {

      ImageType::Pointer im = reader->GetOutput();
      itk::ImageRegionIterator< ImageType > iit(im, im->GetRequestedRegion() );
      PixelType min = itk::NumericTraits< PixelType >::max();
      PixelType max = itk::NumericTraits< PixelType >::min();
      float mean = 0;
      float N = (float) (size[0]*size[1]*size[2]);
      for(iit.GoToBegin(); !iit.IsAtEnd(); ++iit)
	{
	  unsigned short temp = iit.Get();
	  if(temp < min) min = temp;
	  if(temp > max) max = temp;
	  mean += ( (float) (temp) )/N;
	}
      
      clog << "Max Value = " << max << endl;
      clog << "Min Value = " << min << endl;
      clog << "Mean Value = " << mean << endl;
    }

  return(EXIT_SUCCESS);
}
