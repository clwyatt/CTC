/*
 * VCPolypSet.cpp
 *
 *  Created on: Oct 12, 2008
 *      Author: haxu
 */

#include "VCPolypSet.hpp"

shared_ptr< vector<VCPolyp> > VCPolypSet::getPolyps() {
  shared_ptr< vector<VCPolyp> > retval(new vector<VCPolyp>);

  if (!_csvFile.isParsed() && _csvFile.parseFile() != 0) {
      return retval;  // err:
  }

  const vector<string> & colPolypId = _csvFile.getStringColumn(1);
  const vector<double> & colX = _csvFile.getDoubleColumn(2);
  const vector<double> & colY = _csvFile.getDoubleColumn(3);
  const vector<double> & colZ = _csvFile.getDoubleColumn(4);

  int i = 0;
  for (vector<string>::const_iterator polypIdItr = colPolypId.begin();
       polypIdItr != colPolypId.end();
       polypIdItr++, i++) {

      VCPolyp polyp(*polypIdItr, colX.at(i), colY.at(i), colZ.at(i));
      retval->push_back(polyp);
  }

  return retval;
}
