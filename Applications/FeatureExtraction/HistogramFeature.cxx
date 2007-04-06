/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
/*
Histogram as Feature
*/

#include "FeatureExtractionMethods.h"

#include "ctcCTCImageReader.h"
#include "ctcRawVolumeExtraction.h"
#include "itkScalarImageToHistogramGenerator.h"

using namespace std;

void ExtractHistogramFeature(ofstream & out, const EntryType & e)
{

  // construct the directory name
  string dir = getenv("CTC_IMG_DIR");
  dir.append(e.mrn);
  dir.append("/");

  char temp[100];
  sprintf(&(temp[0]), "%06i.STU/", atoi(e.study.c_str()));
  dir.append(temp);
  sprintf(&(temp[0]), "%06i.SER/000001.ACQ", atoi(e.series.c_str()));
  dir.append(temp);
  
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();

  reader->SetDirectory(dir);

  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      std::clog << reader << std::endl;
      throw EXIT_FAILURE;
    }

  typedef ctc::RawVolumeExtraction FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(reader->GetOutput());
  itk::Point<double, 3> point;
  point[0] = e.x;
  point[1] = e.y;
  point[2] = e.z;
  filter->SetDICOMCoordinate(point);
  filter->SetCropSize(25.0);
  filter->Update();

  typedef itk::Statistics::ScalarImageToHistogramGenerator< ctc::CTCImageType > 
    HistFilterType;
  HistFilterType::Pointer hfilter = HistFilterType::New();
  hfilter->SetInput(filter->GetOutput());
  hfilter->SetNumberOfBins(10);
  hfilter->Compute();
  typedef HistFilterType::HistogramType  HistogramType;

  const HistogramType * histogram = hfilter->GetOutput();

  const unsigned int histogramSize = histogram->Size();
  
  // output
  out << e.mrn.c_str() << "," 
      << e.series.c_str() << ","
      << e.x << ","
      << e.y << ","
      << e.z;

  float total = 0;
  unsigned int bin;
  for( bin=0; bin < histogramSize; bin++ )
    {
      total += histogram->GetFrequency(bin);
    }

  for( bin=0; bin < histogramSize; bin++ )
    {
      out << "," << histogram->GetFrequency(bin)/total;
    }

  out << endl;  
  
};
