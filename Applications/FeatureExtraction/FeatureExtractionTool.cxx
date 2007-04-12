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
#include "vul_arg.h"

// Feature extraction methods
#include "FeatureExtractionMethods.h"


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

      db->push_front(e);

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
				   "features.csv");
  vul_arg<char const*> dbname("-db", "CSV file", "VC_findings.csv");
  vul_arg<bool> nonpolyp("-nonpolyp", "use non-polyp regions", false);
  vul_arg<int> Number("-N", "Number of features", 1);

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

  clog << "Size of db = " << db->size() << endl;

  ofstream out(outfilename());
  
  DBType::iterator it = db->begin();
  int num = 0;
  while(it != db->end())
    {
      if(!nonpolyp() && ((*it).true_false == 'T'))
	{
	  try
	    {
	      SingleScaleEigenstructureFeature(out, *it);
	      num += 1;	      
	    }
	  catch(int exitcode)
	    {
	      clog << "Skipping." << endl;
	    }
	}
      if(nonpolyp() && ((*it).true_false == 'F'))
	{
	  try
	    {
	      SingleScaleEigenstructureFeature(out, *it);
	      num += 1;
	    }
	  catch(int exitcode)
	    {
	      clog << "Skipping." << endl;
	    }
	}
      if(num >= Number())
	break;
      
      ++it;
    }
  
  out.close();
  
  delete db;

//   clog << "Total = " << total << endl;

  return EXIT_SUCCESS;

}
