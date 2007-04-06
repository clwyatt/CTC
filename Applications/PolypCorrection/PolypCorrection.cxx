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
#include <itkCastImageFilter.h>

// ITK/FLTK includes
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_file_chooser.H>
#include <GLSliceView.h>

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImageReader.h"
#include "vul_arg.h"

// Feature extraction methods
#include "ctcRawVolumeExtraction.h"

// GUI
#include "PolypCorrectionGUI.h"

Fl_Window *form;

struct EntryType
{
  std::string mrn;
  std::string study;
  std::string series;
  double x, y, z;
  double size;
  char true_false;
};
typedef std::list<EntryType> DBType;

// this is ugly, should be refactored 
DBType * parsedb(const char * dbname)
{
  DBType * db = new DBType;
  EntryType e;
  string entrystr;

  ifstream thefile(dbname);
  getline(thefile, entrystr); // read and discard first line
  //clog << entrystr.c_str() << endl;
  while(thefile.good()) 
    {
      string field;
      getline(thefile,entrystr);
      //clog << entry.c_str() << endl << endl;

      if(entrystr.size() < 5)
	continue;

      int start = 0;
      
      // mrn field
      int end = entrystr.find_first_of(',', start);
      if(end != string::npos)
	e.mrn = entrystr.substr(start, end-start);
      else
	throw false;

      // study field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      if(end != string::npos)
	e.study = entrystr.substr(start, end-start);
      else
	throw false;

      // series field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      if(end != string::npos)
	e.series = entrystr.substr(start, end-start);
      else
	throw false;

     // x coordinate field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      if(end != string::npos)
	e.x = atof(entrystr.substr(start, end-start).c_str());
      else
	throw false;

     // y coordinate field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      if(end != string::npos)
	e.y = atof(entrystr.substr(start, end-start).c_str());
      else
	throw false;

     // z coordinate field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      if(end != string::npos)
	e.z = atof(entrystr.substr(start, end-start).c_str());
      else
	throw false;
      
     // size field
      start = end+1;
      end = entrystr.find_first_of(',', start);
      if(end != string::npos)
	e.size = atof(entrystr.substr(start, end-start).c_str());
      else
	throw false;

     // True/False/Unknown field
      start = end+1;
      end = entrystr.size();
      if(start<end)
	e.true_false = toupper(entrystr.substr(start, end-start)[0]);
      else
	throw false;

      db->push_back(e);

    }
  if( thefile.bad() || !thefile.eof() )
    {
      delete db;
      throw false;
    }
  else
    {
      return db;
    }
};

void ExtractHistogramFeature(ofstream & out, const EntryType & e)
{
  typedef itk::Image< float, 3 > ImageType;



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

  try
    {
      filter->Update();
    }
  catch(bool bex)
    {
      clog << e.mrn.c_str() << "," 
	   << e.study.c_str() << ","
	   << e.series.c_str() << ","
	   << e.x  << ","
	   << e.y  << ","
	   << e.z  << ","
	   << e.size << ","
	   << e.true_false
	   << " (OUTSIDE) " << endl;  
      return;
    }

  ctc::CTCImageType::Pointer image = filter->GetOutput();
  ctc::CTCImageType::IndexType index;
  index[0] = 0;
  index[1] = 0;
  index[2] = 0;
  ctc::CTCImageType::RegionType region;
  region.SetIndex(index);
  ctc::CTCImageType::SizeType size = image->GetRequestedRegion().GetSize();
  region.SetSize(size);
  image->SetRegions(region);

  //std::clog << filter->GetOutput()->GetRequestedRegion() << std::endl;

  typedef itk::CastImageFilter<ctc::CTCImageType, ImageType> CasterType;
  CasterType::Pointer caster = CasterType::New();
  caster->SetInput(filter->GetOutput());
  caster->Update();

//   typedef itk::ImageFileWriter<ImageType> WriterType;
//   WriterType::Pointer writer = WriterType::New();
//   writer->SetInput(caster->GetOutput());
//   writer->SetFileName("temp.vtk");
//   writer->Update();

  //std::clog << caster->GetOutput()->GetRequestedRegion() << std::endl;

  form = make_window();


  tkWin->SetInputImage(caster->GetOutput());
  tkWin->flipY(true);
  tkWin->sliceNum(25);
  
  form->show();
  tkWin->show();
  tkWin->update();

  // force a first redraw
  Fl::check();
  tkWin->update();

  Fl::run();

  if(tkWin->numClickedPointsStored() > 0)
    {
      itk::ClickPoint p;
      tkWin->getClickedPoint(0, p);
      
      ImageType::SpacingType spacing = image->GetSpacing();
            
      // output
      out << e.mrn.c_str() << "," 
	  << e.study.c_str() << ","
	  << e.series.c_str() << ","
	  << e.x + (p.x-((double) size[0])/2)*spacing[0] << ","
	  << e.y + (p.y-((double) size[1])/2)*spacing[1] << ","
	  << e.z + (p.z-((double) size[2])/2)*spacing[2] << ","
	  << e.size << ","
	  << e.true_false
	  << endl;  
    }
  else
    {
     clog << e.mrn.c_str() << "," 
	  << e.study.c_str() << ","
	  << e.series.c_str() << ","
	  << e.x  << ","
	  << e.y  << ","
	  << e.z  << ","
	  << e.size << ","
	  << e.true_false
	  << endl;  
    }
  
};

int main( int argc, char* argv[] )
{
  // make sure base img path defined
  if(!getenv("CTC_IMG_DIR"))
    {
      cerr << "Error: Environment variable CTC_IMG_DIR not defined" << endl;
      return EXIT_FAILURE;
    }  

  // parse args
  vul_arg<char const*> outfilename("-o", 
				   "output csv file name", 
				   "corrected.csv");
  vul_arg<char const*> dbname("-db", "CSV file", "VC_findings.csv");

  vul_arg_parse(argc, argv);

  DBType * db;
  try
    {
      db = parsedb(dbname());
    }
  catch(bool stat)
    {
      cerr << "Error reading file " << dbname() << endl;
      return EXIT_FAILURE;
    }


  ofstream out(outfilename());
  out << "MRN,study_id,series_id,x,y,z,VC_size,T/F" << endl;

  DBType::iterator it = db->begin();
  int num = 0;

  while(it != db->end())
    {
      cout << num << " of " << db->size() << endl;
      ExtractHistogramFeature(out, *it);
      ++it;
      num++;
    }
  
  out.close();
  
  delete db;

  return EXIT_SUCCESS;

}
