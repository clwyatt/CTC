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
#include <itkImageSeriesWriter.h>
#include <itkNumericSeriesFileNames.h>
#include <itksys/SystemTools.hxx>
#include <itkGDCMImageIO.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkMetaDataObject.h>
#include <itkMetaDataDictionary.h>
#include "gdcmUtil.h"

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
  vul_arg<char const*> outfile(0, "Output DICOM directory");
  vul_arg<int> replaceval("-r", 
			  "Replacement HU value for tagged regions (default -900)", 
			  -900);
  vul_arg_parse(argc, argv);

  // test if outfile exists, if so bail out
  if(itksys::SystemTools::FileExists(outfile()))
    {
      if(itksys::SystemTools::FileIsDirectory(outfile()))
	{
	  cerr << "Error: directory " << outfile() << " exists. Halting." << endl;
	}
      else
	{
	  cerr << "Error: file " << outfile() << " exists. Halting." << endl;
	}
      return EXIT_FAILURE;
    }

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

  ctc::CTCImageType::ConstPointer inputImage = reader->GetOutput(); 
  ctc::CTCImageType::RegionType region = inputImage->GetLargestPossibleRegion(); 
  ctc::CTCImageType::IndexType start = region.GetIndex(); 
  ctc::CTCImageType::SizeType size = region.GetSize(); 

  // segment air + constrast
  clog << "Starting Segment";
  ctc::SegmentColonWithContrastFilter::Pointer filter = 
    ctc::SegmentColonWithContrastFilter::New();
  filter->SetInput( reader->GetOutput() );
  filter->Update();
  clog << " Done Segmenting." << endl;

  // loop through the voxels, testing for threshold and lumen test
  // replace image values with air
  clog << "Starting Mask";
  int replace = replaceval();
  typedef itk::ImageRegionIterator<ctc::CTCImageType> InputIteratorType;
  typedef itk::ImageRegionConstIterator<ctc::BinaryImageType> BinaryIteratorType;
  InputIteratorType it1(reader->GetOutput(), 
			reader->GetOutput()->GetRequestedRegion());
  BinaryIteratorType it2(filter->GetOutput(), 
			 filter->GetOutput()->GetRequestedRegion());

  for( it1.GoToBegin(), it2.GoToBegin(); 
       !it1.IsAtEnd() || !it2.IsAtEnd(); 
       ++it1, ++it2)
    {
      if( (it2.Get() == 255) && (it1.Get() > -800) )
	{
	  it1.Set(replace);
	}
    }
  clog << " Done Masking" << endl;

  // write out modified image
  typedef itk::Image< short, 2 > Image2DType; 
  typedef itk::ImageSeriesWriter< ctc::CTCImageType, Image2DType > WriterType; 
  WriterType::Pointer writer = WriterType::New(); 
  writer->SetInput( reader->GetOutput() );

  // modify series number, use old StudyInstanceUID,
  // alter old SeriesInstanceUId, and add Series description
  ctc::CTCImageReader::DictionaryArrayRawPointer dictarray = 
    reader->GetMetaDataDictionaryArray();

  assert(size[2] == dictarray->size());

  for(int slice = 0; 
      slice < dictarray->size(); 
      slice++)
    {

      std::string SeriesNumberTag = "0020|0011";
      std::string SeriesNumberValue = "90";
      std::string SeriesDescriptionTag = "0008|103e";
      std::string SeriesInstanceUIDTag = "0020|000e";
      std::string SOPInstanceUIDTag = "0008|0018";
      std::string SeriesInstanceUIDValue;
      std::string NewSeriesInstanceUIDValue;
      std::string SOPInstanceUIDValue;
      std::string NewSOPInstanceUIDValue;

      ctc::CTCImageReader::DictionaryRawPointer dict = 
	(*(reader->GetMetaDataDictionaryArray()))[slice];
      
      itk::ExposeMetaData<std::string>(*dict, 
				       SeriesInstanceUIDTag, 
				       SeriesInstanceUIDValue);

      itk::ExposeMetaData<std::string>(*dict, 
				       SOPInstanceUIDTag, 
				       SOPInstanceUIDValue);
      
      itk::EncapsulateMetaData<std::string>(*dict, 
					    SeriesNumberTag, 
					    SeriesNumberValue);
      
      std::string SeriesDescriptionValue = "Derived from SeriesInstance UID " + SeriesInstanceUIDValue;
      itk::EncapsulateMetaData<std::string>(*dict, 
					    SeriesDescriptionTag, 
					    SeriesDescriptionValue);

      //append after the last . to make unique
      // if it has a null at the end, strip it and add it back at the end
      if( *(SeriesInstanceUIDValue.rbegin()) == 0)
	{
	  NewSeriesInstanceUIDValue = SeriesInstanceUIDValue.substr(0, SeriesInstanceUIDValue.size() - 1) + ".1";
	  NewSeriesInstanceUIDValue.append(1,0);
	}
      else
	{
	  NewSeriesInstanceUIDValue = SeriesInstanceUIDValue + ".1";
	}

      itk::EncapsulateMetaData<std::string>(*dict, 
 					    SeriesInstanceUIDTag, 
 					    NewSeriesInstanceUIDValue);

      //append to make unique SOPInstanceUID, must be of even length padded by null
      // if it has a null at the end, strip it and add it back at the end
      if( *(SOPInstanceUIDValue.rbegin()) == 0)
	{
	  NewSOPInstanceUIDValue = SOPInstanceUIDValue.substr(0, SOPInstanceUIDValue.size() - 1) + ".1";
	  NewSOPInstanceUIDValue.append(1,0);
	}
      else
	{
	  NewSOPInstanceUIDValue = SOPInstanceUIDValue + ".1";
	}


      // clog <<  SOPInstanceUIDValue << "(" << SOPInstanceUIDValue.size() << ")" << endl;
      // clog <<  NewSOPInstanceUIDValue << endl;


      itk::EncapsulateMetaData<std::string>(*dict, 
 					    SOPInstanceUIDTag, 
 					    NewSOPInstanceUIDValue);

    }

  writer->SetMetaDataDictionaryArray( dictarray ); 

  itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
  dicomIO->SetKeepOriginalUID(true);

  writer->SetImageIO( dicomIO );

  typedef itk::NumericSeriesFileNames NameGeneratorType; 
  NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
  if( !itksys::SystemTools::MakeDirectory(outfile()) )
    {
      cerr << "Error: could not create output directory" << endl;
      return EXIT_FAILURE;
    }

  std::string format = outfile(); 
  format += "/%03d.dcm"; 
  nameGenerator->SetSeriesFormat( format.c_str() );


  const unsigned int firstSlice = start[2]; 
  const unsigned int lastSlice = start[2] + size[2] - 1; 
  nameGenerator->SetStartIndex( firstSlice ); 
  nameGenerator->SetEndIndex( lastSlice ); 
  nameGenerator->SetIncrementIndex( 1 ); 
  writer->SetFileNames( nameGenerator->GetFileNames() );

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
