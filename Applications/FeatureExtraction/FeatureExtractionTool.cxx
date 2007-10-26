/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
/*
Feature Extraction Tool - frontend to various feature extraction methods
*/

#include <cstdlib>
#include <iostream>
using std::clog;
using std::cerr;
using std::endl;
#include <fstream>
using std::ifstream;
using std::ofstream;

#include <string>
using std::string;
#include <cctype>

// ITK includes

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"
#include "vul_arg.h"

// Feature extraction methods
#include "FeatureExtractionMethods.h"
#include "ctcPrincipleCurvatureExtraction.h"
#include "ctcFeatureExtraction.h"
#include "MAT4Converter.h" 
#include "StringtoDouble.h"

// this is ugly, should be refactored 
DBType * parsedb(const char * dbname)
{
  DBType * db = new DBType;
  EntryType e;
  string entrystr;

  ifstream thefile(dbname);
  while(thefile.good())
    {
      string field;
      getline(thefile,entrystr);
      //      clog << entrystr.c_str() << endl << endl;
      if(entrystr.size() < 5)
	continue;

      int start = 0;
      
      // id field
      int end = entrystr.find_first_of(',', start);
      //clog << entrystr.substr(start, end-start) << endl;
      if(end != string::npos)
	e.id = atoi(entrystr.substr(start, end-start).c_str());
      else
	throw false;

      // x  field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      //clog << entrystr.substr(start, end-start) << endl;
      if(end != string::npos)
	e.x = atof(entrystr.substr(start, end-start).c_str());
      else
	throw false;

      // y  field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      //clog << entrystr.substr(start, end-start) << endl;
      if(end != string::npos)
	e.y = atof(entrystr.substr(start, end-start).c_str());
      else
	throw false;

      // z field
      start = end+1;
      end = entrystr.size();
      //clog << entrystr.substr(start, end-start) << endl;
      if(start < end)
	e.z = atof(entrystr.substr(start, end-start).c_str());
      else
	throw false;

      db->push_front(e);

    }
  return db;
};

int main( int argc, char* argv[] )
{
  // parse args
  vul_arg<char const*> imgdir(0, "DICOM image dir");
  vul_arg<char const*> seginfile(0, "Segmented image", "SupineSegmented.vtk/SupineSegmented.img");
  vul_arg<char const*> outfilename(0, "Output feature filename(no extension)", "PolypCandidatesExtracted");
  vul_arg_parse(argc, argv);


  /* get paths to input/output files */
  string rawinfilename = "";
  rawinfilename.append(imgdir());
  string seginfilename = "";
  seginfilename.append(imgdir());
  seginfilename.append(seginfile());
  string outputVTKname = "";
  outputVTKname.append(outfilename());

  // read in the DICOM series
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();
  reader->SetDirectory(rawinfilename);

  // read segmented data
  typedef itk::ImageFileReader< ctc::BinaryImageType > SegDataReaderType;
  SegDataReaderType::Pointer segreader = SegDataReaderType::New();
  segreader->SetFileName(seginfilename);

  try
    {
      reader->Update();
      segreader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }


  // extract the features
  typedef ctc::PrincipleCurvatureExtraction FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetBinaryInput(segreader->GetOutput());
  filter->SetImageInput(reader->GetOutput());
  std::clog << "Computing Features ...";
  try
    {
      filter->Compute();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }
  std::clog << " Done." << std::endl;

 
    typedef ctc::FeatureExtraction FeatureExtractionFilterType;  
    FeatureExtractionFilterType::Pointer FEfilter = FeatureExtractionFilterType::New();
    //FEfilter->SetSeedRegion(filter->GetSeedRegion());
    //FEfilter->SetInitialFeatureVector(filter->GetOutput());

    FEfilter->SetSegmentedImageInput(segreader->GetOutput());
    FEfilter->SetRawImageInput(reader->GetOutput());
    FEfilter->SetOutputVTK(outputVTKname);

    /* Core of feature extraction */
    FEfilter->Analyze();
  
    return EXIT_SUCCESS;

}
