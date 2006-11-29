/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id: Dicom2RVF.cxx 23 2006-06-29 14:45:24Z clwyatt $
Language:  C++
*******************************************************************************/
/*
Converts DICOM images to RVF file format
*/

#include <fstream>

// ITK includes
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageFileWriter.h"

// CTC includes
#include "ctcRVFImageIO.h"

using namespace std;

int main( int argc, char* argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Usage: " << argv[0] << " DicomDirectory  outputFileName  [seriesName]" << std::endl;
    return EXIT_FAILURE;
    }

  typedef short InputPixelType;
  typedef unsigned short OutputPixelType;
  const int Dimension = 3;
  typedef itk::Image<InputPixelType,Dimension> InputImageType;
  typedef itk::Image<OutputPixelType,Dimension> OutputImageType;
  typedef itk::ImageSeriesReader< InputImageType >     ReaderType;
  typedef itk::ImageFileWriter< OutputImageType > WriterType; 
  typedef ctc::RVFImageIO ImageIOType;

  itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

  // Get the DICOM filenames from the directory
  itk::GDCMSeriesFileNames::Pointer nameGenerator = 
    itk::GDCMSeriesFileNames::New();
  nameGenerator->SetDirectory( argv[1] );

  typedef std::vector<std::string> seriesIdContainer;
  const seriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

  seriesIdContainer::const_iterator seriesItr = seriesUID.begin();
  seriesIdContainer::const_iterator seriesEnd = seriesUID.end();
  
  std::cout << std::endl << "The directory: " << std::endl;
  std::cout << std::endl << argv[1] << std::endl << std::endl;
  std::cout << "Contains the following DICOM Series: ";
  std::cout << std::endl << std::endl;

  while( seriesItr != seriesEnd )
    {
    std::cout << seriesItr->c_str() << std::endl;
    seriesItr++;
    }
  
  std::cout << std::endl << std::endl;
  std::cout << "Now reading series: " << std::endl << std::endl;

  typedef std::vector<std::string> fileNamesContainer;
  fileNamesContainer fileNames;

  if( argc < 4 ) // If no optional third argument
    {
    std::cout << seriesUID.begin()->c_str() << std::endl;
    fileNames = nameGenerator->GetFileNames(seriesUID.begin()->c_str());
    }
  else
    {
    std::cout << argv[3] << std::endl;
    fileNames = nameGenerator->GetFileNames( argv[3] );
    }
  std::cout << std::endl << std::endl;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileNames( fileNames );
  reader->SetImageIO( dicomIO );

  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  InputImageType::Pointer im = reader->GetOutput();
  InputImageType::RegionType region = im->GetRequestedRegion(); 
  OutputImageType::Pointer oim = OutputImageType::New();
  oim->SetRegions( region );
  oim->SetOrigin( im->GetOrigin() );
  oim->SetSpacing( im->GetSpacing() );

  try 
    { 
      oim->Allocate();
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      cerr << "Memory Allocation Failed, not enough memory!" << endl; 
      cerr << err << endl; 
      return EXIT_FAILURE; 
    } 

  // convert short to unsigned short using intermediate HU conversion
  // -1024 HU is the minimum CT value, and corresponds to 0 in an unsigned short
  // representation
  itk::ImageRegionIterator< InputImageType > iit(im, im->GetRequestedRegion() );
  itk::ImageRegionIterator< OutputImageType > oit(oim, oim->GetRequestedRegion() );
  for(iit.GoToBegin(), oit.GoToBegin(); !iit.IsAtEnd(); ++iit, ++oit)
    {
      short stemp = iit.Get();
      unsigned short ustemp = stemp + 1024;
      oit.Set(ustemp);
    }

  ImageIOType::Pointer rvfIO = ImageIOType::New();

  WriterType::Pointer writer = WriterType::New(); 

  writer->SetFileName( argv[2] ); 
  
  writer->SetImageIO( rvfIO );

  writer->SetInput( oim );

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

  return EXIT_SUCCESS;

}
