/*
 * VCPolypCand.hpp
 *
 *  Created on: Oct 13, 2008
 *      Author: haxu
 */

#ifndef VCNODEHELPER_HPP_
#define VCNODEHELPER_HPP_

#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include "boost/shared_ptr.hpp"
#include "VCPolyp.hpp"
#include "VCPredict.hpp"
#include "VCVoxel.hpp"

using namespace boost;
using namespace std;

class VCNodeHelper {
public:
  VCNodeHelper();
  virtual ~VCNodeHelper();

  // load polyp, voxel, and prediction from file
  void loadPolypCandidate(const char * polypFile_, const char * voxelFile_,
    const char * predFile_, const char * patId_, const char * studyId_);

  shared_ptr< vector<VCPolyp> > getPolyps() const {
    return _vecPolyp;
  }
  
  list<VCVoxel> getVoxels(string polypId_) const 
  {
    list<VCVoxel> retval;

    for (vector<VCVoxel>::const_iterator itor = _vecVoxel->begin(); itor != _vecVoxel->end(); itor++) {
      if (itor->getPolypId() == polypId_)
        retval.push_back(*itor);
    }

    return retval;
  }

  shared_ptr< vector<VCPredict> > getPredicts() const {
    return _vecPred;
  }

  const char * getNodeId(string polypId_) {
    map<string, string>::const_iterator iter =
      _mapPolypIdNodeId->find(polypId_);
    if (iter == _mapPolypIdNodeId->end())
      return "";
    else
		return iter->second.c_str();
  }

  bool insertPolypIdNodeId(string polypId_, string nodeId_) {
    pair< map<string, string>::iterator, bool > retval =
      _mapPolypIdNodeId->insert(pair<string, string>(polypId_, nodeId_));
    return retval.second;
  }

  const char * getLabelMapNodeId(string polypId_) {
    map<string, string>::const_iterator iter =
      _mapPolypIdLabelMapNodeId->find(polypId_);
    if (iter == _mapPolypIdLabelMapNodeId->end())
      return "";
    else
      return iter->second.c_str();
  }

  bool insertPolypIdLabelMapNodeId(string polypId_, string nodeId_) {
    pair< map<string, string>::iterator, bool > retval =
      _mapPolypIdLabelMapNodeId->insert(pair<string, string>(polypId_, nodeId_));
    return retval.second;
  }

  bool getPolyp(string polypId_, VCPolyp & polyp) {
    VCPolyp idPolyp(polypId_, 0, 0, 0);
    VCPolyp_op_less opPolyp;

    vector<VCPolyp>::iterator itor =
      lower_bound(_vecPolyp->begin(), _vecPolyp->end(), idPolyp, opPolyp);
    if (itor != _vecPolyp->end() &&
        itor->getPolypId() == polypId_) {
      polyp = *itor;
      return true;
    }
    else
      return false;
  }

  /**
   * Get PolypId according to polyp location.
   */
  string getPolypId(double center[3]) {

    const double TOL = 0.001;

    for (vector<VCPolyp>::const_iterator itor = _vecPolyp->begin();
         itor != _vecPolyp->end();
         itor++) {
      if (abs((long)(itor->getX() - center[0])) <= TOL &&
          abs((long)(itor->getY() - center[1])) <= TOL &&
          abs((long)(itor->getZ() - center[2])) <= TOL)
        return itor->getPolypId();
    }
    return "";
  }

protected:
  shared_ptr< vector<VCPolyp> > _vecPolyp;
  shared_ptr< vector<VCPredict> > _vecPred;
  shared_ptr< vector<VCVoxel> > _vecVoxel;

  shared_ptr< map<string, string> > _mapPolypIdNodeId;
  shared_ptr< map<string, string> > _mapPolypIdLabelMapNodeId;
};

#endif /* VCNODEHELPER_HPP_ */
