/*
 * VCPredictSet.hpp
 *
 *  Created on: Oct 11, 2008
 *      Author: haxu
 */

#ifndef VCPREDICTSET_HPP_
#define VCPREDICTSET_HPP_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "VCPredict.hpp"
#include "CSVReader.hpp"

using namespace std;
using namespace boost;

const string PREDICT_CSV_FORMAT = "s,s,d,f,f";

class VCPredictSet {
public:
  VCPredictSet(string predFilename_) :
    _csvFile(predFilename_, PREDICT_CSV_FORMAT) {

  }
  virtual ~VCPredictSet() { };

  /**
   * Search Predicts for patient. If not found, return an empty vector.
   */
  //const vector<VCPredict> & findPredict(string patId_);


  /**
   * Search Predicts for study. If not found, return an empty vector.
   */
  shared_ptr< vector<VCPredict> > findPredict(string patId_, string studyId_);

  /**
   * Search Predict for a polyp. If not found, an invalid VCPredict object will
   * be returned.
   */
  //VCPredict findPredict(string patId_, string studyId_, string polypId_);

protected:
  VCPredictSet() : _csvFile("", "") { }

protected:
  CSVReader _csvFile;
};

#endif /* VCPREDICTSET_HPP_ */
