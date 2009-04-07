/*
 * VCPolypCand.cpp
 *
 *  Created on: Oct 13, 2008
 *      Author: haxu
 */

#include "VCNodeHelper.hpp"
#include "VCPredictSet.hpp"
#include "VCPolypSet.hpp"
#include "VCVoxelSet.hpp"

VCNodeHelper::VCNodeHelper() {
  // TODO: memory leak after loadPolypCandidate(...)?
  _vecPolyp = shared_ptr< vector<VCPolyp> >(new vector<VCPolyp>());
  _vecPred = shared_ptr< vector<VCPredict> >(new vector<VCPredict>());
  _vecVoxel = shared_ptr< vector<VCVoxel> >(new vector<VCVoxel>());

  _mapPolypIdNodeId =
    shared_ptr< map<string, string> >(new map<string, string>());

  _mapPolypIdLabelMapNodeId =
    shared_ptr< map<string, string> >(new map<string, string>());
}

VCNodeHelper::~VCNodeHelper() {
  // TODO Auto-generated destructor stub
}


// load
void VCNodeHelper::loadPolypCandidate(
  const char * polypFile_, const char * voxelFile_,
  const char * predFile_, const char * patId_, const char * studyId_) {

  // load prediction, polyp candiates and voxels
  VCPredictSet predSet(predFile_);
  VCPolypSet polypSet(polypFile_);
  VCVoxelSet voxelSet(voxelFile_);

  _vecPolyp = polypSet.getPolyps();
  VCPolyp_op_less opPolyp;
  sort(_vecPolyp->begin(), _vecPolyp->end(), opPolyp);

  _vecPred = predSet.findPredict(patId_, studyId_);
  VCPredict_op_less opPred;
  sort(_vecPred->begin(), _vecPred->end(), opPred);

  _vecVoxel = voxelSet.getVoxels();
  // todo: sort
}

