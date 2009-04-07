/*
 * VCPolypSet.hpp
 *
 *  Created on: Oct 12, 2008
 *      Author: haxu
 */

#ifndef VCPOLYPSET_HPP_
#define VCPOLYPSET_HPP_

#include <string>
#include "boost/shared_ptr.hpp"
#include "VCPolyp.hpp"
#include "CSVReader.hpp"

using namespace std;
using namespace boost;

// 95 columns: study id, polyp id, polyp features (92), target (0 or 1)
const string POLYP_CSV_FORMAT = "s,s,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,d";

class VCPolypSet {
public:
  VCPolypSet(string polypFile_) : _csvFile(polypFile_, POLYP_CSV_FORMAT) {

  }

  virtual ~VCPolypSet() { }

  shared_ptr< vector<VCPolyp> > getPolyps();

protected:
  VCPolypSet() : _csvFile("", "") { }

protected:
  CSVReader _csvFile;
};

#endif /* VCPOLYPSET_HPP_ */
