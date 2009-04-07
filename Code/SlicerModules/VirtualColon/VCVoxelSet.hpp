/*
 * VCVoxelSet.hpp
 *
 *  Created on: Oct 13, 2008
 *      Author: haxu
 */

#ifndef VCVOXELSET_HPP_
#define VCVOXELSET_HPP_

#include <string>
#include "boost/shared_ptr.hpp"
#include "VCVoxel.hpp"
#include "CSVReader.hpp"

using namespace std;
using namespace boost;

// 17 columns: polyp id, voxel features (15), target (0 or 1)
const string VOXEL_CSV_FORMAT = "s,"
                                "f,f,f,f,f,f,f,f,f,f,"
                                "f,f,f,f,f,d";

class VCVoxelSet {
public:
  VCVoxelSet(string voxelFile_) : _csvFile(voxelFile_, VOXEL_CSV_FORMAT) {

  }

  virtual ~VCVoxelSet() { }

  shared_ptr< vector<VCVoxel> > getVoxels();
  shared_ptr< vector<VCVoxel> > findVoxels(string polypId_);

protected:
  VCVoxelSet() : _csvFile("", "") { }

protected:
  CSVReader _csvFile;
};

#endif /* VCVOXELSET_HPP_ */
