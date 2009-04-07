/*
 * vtkMRMLVirtualColonNode.h
 *
 *  Created on: Oct 3, 2008
 *      Author: haxu
 */

#ifndef VTKSLICERVIRTUALCOLONNODE_H_
#define VTKSLICERVIRTUALCOLONNODE_H_

#include <vtkMRMLNode.h>
#include "vtkSlicerVirtualColon.h"
#include "VCNodeHelper.hpp"

class VTK_VIRTUALCOLON_EXPORT vtkSlicerVirtualColonNode: public vtkMRMLNode {
public:
  static vtkSlicerVirtualColonNode * New();
  vtkTypeMacro(vtkSlicerVirtualColonNode, vtkMRMLNode);
  virtual vtkMRMLNode * CreateNodeInstance();
  virtual const char * GetNodeTagName() { return "VirtualColonNode"; }
  //virtual void Copy(vtkMRMLNode *node);
  //virtual void UpdateReferenceID(const char *oldID, const char *newID);
  //void PrintSelf(ostream& os, vtkIndent indent);
  //virtual void ReadXMLAttributes( const char** atts);
  //virtual void WriteXML(ostream& of, int indent);

  vtkGetStringMacro(_inputVolumeID);
  vtkSetStringMacro(_inputVolumeID);

  // load polyp, voxel, and prediction from file
  void LoadPolypCandidate(const char * polypFile_, const char * voxelFile_,
    const char * predFile_, const char * patId_, const char * studyId_) {
    return _helper.loadPolypCandidate(
        polypFile_, voxelFile_, predFile_, patId_, studyId_);
  }

  int GetNumberOfPolyps() const {
    return _helper.getPolyps()->size();
  }
  VCPolyp GetPolyp(int i) const {
    return _helper.getPolyps()->at(i);
  }

  bool GetPolypXYZ(const char * polypId_, double xyz_[3]) {
    VCPolyp polyp;
    if (_helper.getPolyp(polypId_, polyp)) {
      xyz_[0] = polyp.getX();
      xyz_[1] = polyp.getY();
      xyz_[2] = polyp.getZ();
      return true;
    }
    else
      return false;
  }

  string GetPolypId(double center[3]) {
    return _helper.getPolypId(center);
  }

  list<VCVoxel> GetVoxels(string polypId_) {
    return _helper.getVoxels(polypId_);
  }

  int GetNumberOfPredicts() const {
    return _helper.getPredicts()->size();
  }
  VCPredict GetPredict(int i) const {
    return _helper.getPredicts()->at(i);
  }

  const char * GetPolypNodeId(const char * polypId_) {
    return _helper.getNodeId(polypId_);
  }

  bool AddPolypNodeId(const char * polypId_, const char * nodeId_) {
    return _helper.insertPolypIdNodeId(polypId_, nodeId_);
  }

  const char * GetPolypLabelMapNodeId(const char * polypId_) {
    return _helper.getLabelMapNodeId(polypId_);
  }

  bool AddPolypLabelMapNodeId(const char * polypId_, const char * nodeId_) {
    return _helper.insertPolypIdLabelMapNodeId(polypId_, nodeId_);
  }

protected:
  vtkSlicerVirtualColonNode();
  virtual ~vtkSlicerVirtualColonNode();
  vtkSlicerVirtualColonNode(const vtkSlicerVirtualColonNode&);
  vtkSlicerVirtualColonNode& operator=(const vtkSlicerVirtualColonNode&);

protected:
  char * _inputVolumeID;

  VCNodeHelper _helper;
};

#endif /* VTKSLICERVIRTUALCOLONNODE_H_ */
