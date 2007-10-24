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
  vul_arg<char const*> datainfile(0, "input volume data file", "SupineData.vtk");
  vul_arg<char const*> seginfile(0, "segmented image", "SupineSegmented.vtk");
  vul_arg<char const*> outfilename(0, "Output feature filename", "PolypCandidatesExtractedInSegfile.vtk");
  //vul_arg<char const*> pfile("-p", "polyp coordinate file", "polyps");
  //vul_arg<char const*> outfilename("-o", 
		//		   "output csv file name", 
		//		   "features.csv");
  vul_arg_parse(argc, argv);


  /* get paths to input/output files */
  string rawinfilename = "";
  rawinfilename.append(imgdir());
  rawinfilename.append("/");
  rawinfilename.append(datainfile());
  string seginfilename = "";
  seginfilename.append(imgdir());
  seginfilename.append("/");
  seginfilename.append(seginfile());
  string outputVTKname = "";
  outputVTKname.append(outfilename());

  // read data
  //ctc::CTCImageReader::Pointer rawreader = ctc::CTCImageReader::New();
  //rawreader->SetDirectory(string(imgdir()));
  //rawreader->SetFileName(rawinfilename);
  typedef itk::ImageFileReader< ctc::CTCImageType > RawDataReaderType;
  RawDataReaderType::Pointer rawreader = RawDataReaderType::New();
  rawreader->SetFileName(rawinfilename);
  

  // read segmented data
  typedef itk::ImageFileReader< ctc::BinaryImageType > SegDataReaderType;
  SegDataReaderType::Pointer segreader = SegDataReaderType::New();
  segreader->SetFileName(seginfilename);

  // invoke readers
  try
    {
      rawreader->Update();
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
  filter->SetImageInput(rawreader->GetOutput());
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

  // read the polyps file
 
  
  /* Will uncomment soon */
  /*
  DBType * db;
  try
    {
      db = parsedb(pfile());
    }
  catch(bool stat)
    {
      cerr << "Error reading file " << pfile() << endl;
      return EXIT_FAILURE;
    }
  */

  /* Previous Work */
  /*
  // output
  ofstream out(outfilename());

  out << "voxelx" << "," 
      << "voxely" << ","
      << "voxelz" << ","
      << "kappa1" << ","
      << "kappa2" << ","
      << "shape_index" << ","
      << "curvedness" << ","
      << "gauusian_curv" << ","
      << "mean_curv" << ","
      << "dist_to_closest" << ","
      << "id_of_closest"
      << endl;

  FilterType::FeatureSampleType::Pointer features = filter->GetOutput();
  FilterType::FeatureType f;
  DBType::iterator it;
  for(int i = 0; i < features->Size(); i++)
    {
       f = features->GetMeasurementVector(i);
       int id = 0;
       float mindist = pow(512,6);
       for(it = db->begin(); it != db->end(); ++it)
	 {
	   float dx = f[0] - it->x;
	   float dy = f[1] - it->y;
	   float dz = f[2] - it->z;
	   float dist = dx*dx + dy*dy + dz*dz;
	   if( dist < mindist)
	     {
	       mindist = dist;
	       id = it->id;
	     }
	 }
       mindist = sqrt(mindist);

       //note the si is slightly different, since I use atan2 in place of atan
       float si = 0.5 + 0.1592*atan2(f[3] + f[4], f[3] - f[4]);
       float cv = 0.6366*log(sqrt((f[3]*f[3] + f[4]*f[4])/2));
       float K = f[3]*f[4];
       float H = 0.5*(f[3] + f[4]);

       out << f[0] << "," 
	   << f[1] << ","
	   << f[2] << ","
	   << f[3] << ","
	   << f[4] << ","
	   << si << ","
	   << cv << ","
	   << K << ","
	   << H << ","
	   << mindist << ","
	   << id
	   << endl;
    }  
  out.close();*/
 
    typedef ctc::FeatureExtraction FeatureExtractionFilterType;  
    FeatureExtractionFilterType::Pointer FEfilter = FeatureExtractionFilterType::New();
//  FEfilter->SetInitialFeatureVector(filter->GetOutput());

    FEfilter->SetSegmentedImageInput(segreader->GetOutput());
    FEfilter->SetRawImageInput(rawreader->GetOutput());
    FEfilter->SetOutputVTK(outputVTKname);

    /* Core of feature extraction */
    FEfilter->Analyze();


//  delete db;

  return EXIT_SUCCESS;

}
