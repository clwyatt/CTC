/*
 * vtkSlicerVirtualColonLogic.h
 *
 *  Created on: Oct 3, 2008
 *      Author: haxu
 */

#ifndef VTKSLICERVIRTUALCOLONLOGIC_H_
#define VTKSLICERVIRTUALCOLONLOGIC_H_

#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerVirtualColon.h"
#include "vtkSlicerVirtualColonNode.h"

#include "vtkSmartPointer.h"
#include "vtkContourFilter.h"

class VTK_VIRTUALCOLON_EXPORT vtkSlicerVirtualColonLogic: public vtkSlicerModuleLogic {
public:
  static vtkSlicerVirtualColonLogic * New();
  vtkTypeMacro(vtkSlicerVirtualColonLogic, vtkSlicerModuleLogic);

  // Description: Get/Set MRML node storing parameter values
  vtkGetObjectMacro(Node, vtkSlicerVirtualColonNode);
  void SetAndObserveSlicerVirtualColonNode(vtkSlicerVirtualColonNode *n)
  {
    vtkSetAndObserveMRMLNodeMacro(this->Node, n);
  }

  virtual void ProcessMRMLEvents ( vtkObject * /*caller*/,
                                  unsigned long /*event*/,
                                  void * /*callData*/ );

  // add or remove a marker (sphere) for polyp 
  void flashPolypMarker(const char * polypId_);

  // transform model node (segmented colon surface) according to image volume's image orientation and origin
  void transformModel(vtkMRMLModelNode * modelNode, double dir[3][3], double origin[3]);

  void contourSegmentationFile(const char * segFilename);

  vtkPolyData * cropAndContourPolyp(const char * polypId_);
  vtkPolyData * cropPolypFromSegmentationContour(const char * polypId_, const char * segContourName_);

  // extract image volume of a polyp and return the node name
  string extractPolypVolume(const char * polypId_, float size_);

  vtkPolyData * contourPolyp(const char * polypNodeName_, float threshold_);

  // delete vtkContourFilter which was New-ed in extractPolypVolume()
  void removePolypContour(const char * nodeName_);
  string getPolypVolumeNodeName(const char * polypId_, float size_) 
  {
    ostringstream oss; 
    oss << "polyp_" << polypId_ << "_" << size_;
    return oss.str();
  }

protected:
  vtkSlicerVirtualColonLogic() {
    this->Node = NULL;
  }
  virtual ~vtkSlicerVirtualColonLogic() {
    vtkSetMRMLNodeMacro(this->Node, NULL);
  }
  vtkSlicerVirtualColonLogic(const vtkSlicerVirtualColonLogic &);
  vtkSlicerVirtualColonLogic & operator=(const vtkSlicerVirtualColonLogic &);

  vtkSlicerVirtualColonNode * Node;

  // this map used to keep the polyp contour (vtkPolyData) in memory
  map<string, vtkSmartPointer<vtkContourFilter> > _mapPolypContourHolder;
};

#endif /* VTKSLICERVIRTUALCOLONLOGIC_H_ */
