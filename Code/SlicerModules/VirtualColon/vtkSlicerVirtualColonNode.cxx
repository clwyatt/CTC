/*
 * vtkMRMLVirtualColonNode.cpp
 *
 *  Created on: Oct 3, 2008
 *      Author: haxu
 */

#include "vtkSlicerVirtualColonNode.h"

vtkSlicerVirtualColonNode::vtkSlicerVirtualColonNode() {
  _inputVolumeID = NULL;
  this->HideFromEditors = 1;
}

vtkSlicerVirtualColonNode::~vtkSlicerVirtualColonNode() {
}

vtkSlicerVirtualColonNode * vtkSlicerVirtualColonNode::New() {
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret =
    vtkObjectFactory::CreateInstance("vtkSlicerVirtualColonNode");
  if(ret) {
    return (vtkSlicerVirtualColonNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkSlicerVirtualColonNode();
}

vtkMRMLNode * vtkSlicerVirtualColonNode::CreateNodeInstance() {
  return this->New();
}

/*void vtkSlicerVirtualColonNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkSlicerVirtualColonNode *node = (vtkSlicerVirtualColonNode *) anode;
}
*/
