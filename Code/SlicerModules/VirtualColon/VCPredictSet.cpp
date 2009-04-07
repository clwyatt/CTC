/*
 * VCPredictSet.cpp
 *
 *  Created on: Oct 11, 2008
 *      Author: haxu
 */

#include "VCPredictSet.hpp"

shared_ptr< vector<VCPredict> > VCPredictSet::findPredict(
  string patId_, string studyId_)
{
  shared_ptr< vector<VCPredict> > retval(new vector<VCPredict>);

  if (!_csvFile.isParsed() && _csvFile.parseFile() != 0) {
      return retval;  // err:
  }

  const vector<string> & colPatId = _csvFile.getStringColumn(0);
  const vector<string> & colPolypId = _csvFile.getStringColumn(1);
  const vector<int> & colTarget = _csvFile.getIntColumn(2);
  const vector<double> & colPred_0 = _csvFile.getDoubleColumn(3);
  const vector<double> & colPred_1 = _csvFile.getDoubleColumn(4);

  int i = 0;
  for (vector<string>::const_iterator patIdItr = colPatId.begin();
       patIdItr != colPatId.end();
       patIdItr++, i++) {
    if (patIdItr->find(patId_ + "/" + studyId_) == 0) {
      VCPredict pred(*patIdItr,
                     colPolypId.at(i),
                     colTarget.at(i),
                     colPred_0.at(i),
                     colPred_1.at(i));
      retval->push_back(pred);
    }
  }

  return retval;
}

/*
VCPredict VCPredictSet::findPredict(
  string patId_, string studyId_, string polypId_)
{
  VCPredict retval;

  if (!_csvFile.isParsed() && _csvFile.parseFile() != 0) {
      return retval;  // err:
  }

  const vector<string> & colPatId = _csvFile.getStringColumn(0);
  const vector<string> & colPolypId = _csvFile.getStringColumn(1);
  const vector<int> & colTarget = _csvFile.getIntColumn(2);
  const vector<double> & colPred_0 = _csvFile.getDoubleColumn(3);
  const vector<double> & colPred_1 = _csvFile.getDoubleColumn(4);

  int i = 0;
  for (vector<string>::const_iterator patIdItr = colPatId.begin();
       patIdItr != colPatId.end();
       patIdItr++, i++) {
    if (patIdItr->find_first_of(patId_ + "/" + studyId_) == 0 &&
        colPolypId.at(i) == polypId_) {
      retval = VCPredict(*patIdItr,
                     colPolypId.at(i),
                     colTarget.at(i),
                     colPred_0.at(i),
                     colPred_1.at(i));
    }
  }

  return retval;
}
*/
