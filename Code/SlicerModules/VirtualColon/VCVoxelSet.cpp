/*
 * VCVoxelSet.cpp
 *
 *  Created on: Oct 13, 2008
 *      Author: haxu
 */

#include "VCVoxelSet.hpp"

shared_ptr< vector<VCVoxel> > VCVoxelSet::getVoxels() {
  shared_ptr< vector<VCVoxel> > retval(new vector<VCVoxel>);

  if (!_csvFile.isParsed() && _csvFile.parseFile() != 0) {
      return retval;  // err:
  }

  const vector<string> & colPolypId = _csvFile.getStringColumn(0);
  const vector<double> & colX = _csvFile.getDoubleColumn(1);
  const vector<double> & colY = _csvFile.getDoubleColumn(2);
  const vector<double> & colZ = _csvFile.getDoubleColumn(3);

  int i = 0;
  for (vector<string>::const_iterator polypIdItr = colPolypId.begin();
       polypIdItr != colPolypId.end();
       polypIdItr++, i++) {

    VCVoxel voxel(*polypIdItr, colX.at(i), colY.at(i), colZ.at(i));
    retval->push_back(voxel);
  }

  return retval;
}

shared_ptr< vector<VCVoxel> > VCVoxelSet::findVoxels(string polypId_) {
  shared_ptr< vector<VCVoxel> > retval(new vector<VCVoxel>);

  if (!_csvFile.isParsed() && _csvFile.parseFile() != 0) {
      return retval;  // err:
  }

  const vector<string> & colPolypId = _csvFile.getStringColumn(0);
  const vector<double> & colX = _csvFile.getDoubleColumn(1);
  const vector<double> & colY = _csvFile.getDoubleColumn(2);
  const vector<double> & colZ = _csvFile.getDoubleColumn(3);

  int i = 0;
  for (vector<string>::const_iterator polypIdItr = colPolypId.begin();
       polypIdItr != colPolypId.end();
       polypIdItr++, i++) {
    if (*polypIdItr == polypId_) {
      VCVoxel voxel(*polypIdItr, colX.at(i), colY.at(i), colZ.at(i));
      retval->push_back(voxel);
    }
  }

  return retval;
}
