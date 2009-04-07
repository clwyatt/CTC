/*
 * vtkSlicerVirtualColonLogic.cpp
 *
 *  Created on: Oct 3, 2008
 *      Author: haxu
 */

#include "vtkSlicerVirtualColonLogic.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkSlicerColorLogic.h"

#include "vtkExtractVOI.h"
#include "vtkSphereSource.h"
#include "vtkDataSetReader.h"
#include "vtkContourFilter.h"
#include "vtkClipPolyData.h"
#include "vtkBox.h"

#ifdef _WINDOWS
// for MS VC++
int round(double d) {
	return d < 0 ? (int)(d - 0.5) : (int) (d + 0.5);
}
#endif


// helper function to create a node. Caller needn't call Node->Delete()
vtkMRMLNode * _createNode(vtkSlicerLogic * logic, const char * className_, const char * nodeName_)
{
  if (!logic)
    return NULL;

  vtkMRMLScene * scene = logic->GetMRMLScene();
  if (scene == NULL)
    return NULL;

  vtkMRMLNode * n = scene->CreateNodeByClass(className_);
  if (n == NULL)
    return NULL;

  n->SetScene(scene);
  n->SetName(nodeName_);
  //if (nodeName_ == NULL || strlen(nodeName_) == 0)
  //  n->SetName(scene->GetUniqueNameByString(n->GetNodeTagName()));
  //else
  //  n->SetName(scene->GetUniqueNameByString(nodeName_));

  /*
  // set VolumeNode to be LabelMap
  vtkMRMLScalarVolumeNode * volNode = vtkMRMLScalarVolumeNode::SafeDownCast(n);
  if (volNode) {
    string nodeName(nodeName_);
    if (nodeName.find("LabelMap") != string::npos)
      volNode->LabelMapOn();
  }
  */

  scene->AddNode(n);
  n->Delete();  // decrease reference

  return n;
}




vtkSlicerVirtualColonLogic * vtkSlicerVirtualColonLogic::New() {
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret =
    vtkObjectFactory::CreateInstance("vtkSlicerVirtualColonLogic");
  if(ret) {
    return (vtkSlicerVirtualColonLogic*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkSlicerVirtualColonLogic;
}

//----------------------------------------------------------------------------
void vtkSlicerVirtualColonLogic::ProcessMRMLEvents(vtkObject * caller,
                                                   unsigned long event,
                                                   void * callData )
{

}

vtkPolyData * vtkSlicerVirtualColonLogic::cropPolypFromSegmentationContour(const char * polypId_, const char * segContourName_)
{
  /////////////////////////////////////////////////////////////////////////////
  // crop Polyp volume

  vtkMRMLModelNode * segContourModel;
  if (vtkCollection * col = this->GetMRMLScene()->GetNodesByName(segContourName_)) {
    if (col->GetNumberOfItems() == 1) {
      segContourModel = vtkMRMLModelNode::SafeDownCast(col->GetItemAsObject(0));
    }
  }

  if (!segContourModel)
    return NULL;

  double coord[3];
  if (!Node->GetPolypXYZ(polypId_, coord))
    return NULL;
  // change from DICOM coordinate (LPS) to RAS coordinate
  coord[0] *= -1;
  coord[1] *= -1;

  const int w = 25;  // crop a volume of W x W x W
  vtkClipPolyData * clipper = vtkClipPolyData::New();
  vtkSmartPointer<vtkBox> box = vtkSmartPointer<vtkBox>::New();
  box->SetBounds(coord[0] - w, coord[0] + w, coord[1] - w, coord[1] + w, coord[2] - w, coord[2] + w);
  clipper->SetClipFunction(box);
  clipper->SetInput(segContourModel->GetPolyData());
  clipper->InsideOutOn();
  clipper->Update();
  vtkPolyData * polypContour = clipper->GetOutput();

  return polypContour;  // TODO: memory leak
}

vtkPolyData * vtkSlicerVirtualColonLogic::cropAndContourPolyp(const char * polypId_) {

  double coord[3];
  if (!Node->GetPolypXYZ(polypId_, coord))
    return NULL;

  /////////////////////////////////////////////////////////////////////////////
  // crop Polyp volume

  vtkMRMLScalarVolumeNode * inVolume = vtkMRMLScalarVolumeNode::SafeDownCast(
        this->GetMRMLScene()->GetNodeByID(Node->Get_inputVolumeID()));

  vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
  inVolume->GetRASToIJKMatrix(mat);
  double vRAS[4], vIJK[4];
  vRAS[0] = -coord[0];
  vRAS[1] = -coord[1];
  vRAS[2] = coord[2];
  vRAS[3] = 1;
  mat->MultiplyPoint(vRAS, vIJK);

  int ijk[3];
  ijk[0] = round(vIJK[0]);
  ijk[1] = round(vIJK[1]);
  ijk[2] = round(vIJK[2]);

  vtkSmartPointer<vtkExtractVOI> filterVOI = vtkSmartPointer<vtkExtractVOI>::New();
  const int w = 25;  // crop a volume of 50x50x50
  filterVOI->SetVOI(ijk[0]-w, ijk[0]+w, ijk[1]-w, ijk[1]+w, ijk[2]-w, ijk[2]+w);
  filterVOI->SetSampleRate(1, 1, 1);
  filterVOI->SetInput(inVolume->GetImageData());
  filterVOI->Update();
  vtkImageData * polypVol = filterVOI->GetOutput();
  polypVol->SetOrigin(inVolume->GetOrigin());
  polypVol->SetSpacing(inVolume->GetSpacing());

  // contour the polyp volume: threshold = -800
  const float THRESHOLD = -800;
  //vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  vtkContourFilter * contourFilter = vtkContourFilter::New();
  contourFilter->SetValue(0, THRESHOLD);
  contourFilter->SetInput(polypVol);
  contourFilter->Update();
  vtkPolyData * contour = contourFilter->GetOutput();

  return contour;  // TODO: memory leak
}

// extract image volume for a polyp, and create a VolumeNode
string vtkSlicerVirtualColonLogic::extractPolypVolume(const char * polypId_, float size_) 
{
  ///////////////////////////////////////////////////////////////////////////
  // create vtkMRMLScalarVolumeNode to hold polyp volume
  string nodeName = this->getPolypVolumeNodeName(polypId_, size_);

  vtkMRMLScalarVolumeNode * polypVolume = vtkMRMLScalarVolumeNode::SafeDownCast(_createNode(this, "vtkMRMLScalarVolumeNode", nodeName.c_str()));
  if (polypVolume == NULL)
    return "";

  if (vtkMRMLScalarVolumeDisplayNode * displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(_createNode(this, "vtkMRMLScalarVolumeDisplayNode", ""))) {
    if (vtkSmartPointer<vtkSlicerColorLogic> colorLogic = vtkSmartPointer<vtkSlicerColorLogic>::New()) {
      displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
      polypVolume->SetAndObserveDisplayNodeID(displayNode->GetID());
    }
  }

  //if (Node->AddPolypNodeId(polypId.c_str(), outputVolume->GetID()) == false) {
  //  vtkDebugMacro("Add PolypNodeId failed for polyp with id = " + polypId);
  //  return;
  //}

/*
    ///////////////////////////////////////////////////////////////////////////
    // create vtkMRMLScalarVolumeNode to hold polyp label map
    string polypNodeNameLM = "LabelMap_polyp_" + polypId + "_";

    vtkMRMLScalarVolumeNode * labelMapNode =
      vtkMRMLScalarVolumeNode::SafeDownCast(
        _createNode("vtkMRMLScalarVolumeNode", polypNodeNameLM.c_str()));
    if (labelMapNode == NULL)
      return;

    if (Node->AddPolypLabelMapNodeId(polypId.c_str(), labelMapNode->GetID()) == false) {
      vtkDebugMacro("Add labelMapNode failed for polyp with id = " + polypId);
      return;
    }

    vtkMRMLLabelMapVolumeDisplayNode * displayNodeLM =
      vtkMRMLLabelMapVolumeDisplayNode::SafeDownCast(
          _createNode("vtkMRMLLabelMapVolumeDisplayNode", ""));
    if (displayNodeLM != NULL) {
      vtkSlicerColorLogic * colorLogic = vtkSlicerColorLogic::New();
      displayNodeLM->SetAndObserveColorNodeID(
          colorLogic->GetDefaultLabelMapColorNodeID());
      labelMapNode->SetAndObserveDisplayNodeID(displayNodeLM->GetID());
      colorLogic->Delete();
    }
*/

  // find input volume
  vtkMRMLScalarVolumeNode * inVolume = vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(Node->Get_inputVolumeID()));
  if (inVolume == NULL) {
    vtkErrorMacro("No input volume found");
    return "";
  }

  double coord[3];
  if (!Node->GetPolypXYZ(polypId_, coord))
    return "";

  /////////////////////////////////////////////////////////////////////////////
  // crop volume for this polyp
  vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
  inVolume->GetRASToIJKMatrix(mat);
  double vRAS[4], vIJK[4];
  vRAS[0] = -coord[0];
  vRAS[1] = -coord[1];
  vRAS[2] = coord[2];
  vRAS[3] = 1;

  mat->MultiplyPoint(vRAS, vIJK);
  //cout << "[di,dj,dk]=" << vIJK[0] << "," << vIJK[1] << "," << vIJK[2] << "," << vIJK[3] << endl;

  int ijk[3];
  ijk[0] = round(vIJK[0]);
  ijk[1] = round(vIJK[1]);
  ijk[2] = round(vIJK[2]);

  vtkSmartPointer<vtkExtractVOI> filterVOI = vtkSmartPointer<vtkExtractVOI>::New();
  int w = size_;
  filterVOI->SetVOI(ijk[0]-w, ijk[0]+w, ijk[1]-w, ijk[1]+w, ijk[2]-w, ijk[2]+w);
  filterVOI->SetSampleRate(1, 1, 1);
  filterVOI->SetInput(inVolume->GetImageData());
  filterVOI->Update();
  polypVolume->SetAndObserveImageData(filterVOI->GetOutput());
  polypVolume->CopyOrientation(inVolume);
  polypVolume->SetOrigin(inVolume->GetOrigin());
  polypVolume->SetSpacing(inVolume->GetSpacing());
  polypVolume->CalculateScalarAutoLevels();

  // need to set origin/spacing/orientation for vtkImageData
  polypVolume->GetImageData()->SetOrigin(inVolume->GetOrigin());
  polypVolume->GetImageData()->SetSpacing(inVolume->GetSpacing());
  //TODO: how set orentation in vtkImageData?
  // current approach is a trick to set spacing as a negative value
  double orient[3][3];
  inVolume->GetIJKToRASDirections(orient);
  if (orient[0][0] == 1 && orient[0][1] == 0 && orient[0][2] == 0 &&
      orient[1][0] == 0 && orient[1][1] == 1 && orient[1][2] == 0 &&
      orient[2][0] == 0 && orient[2][1] == 0 && orient[2][2] == 1) {
        ;
  }
  else if (orient[0][0] == -1 && orient[0][1] == 0 && orient[0][2] == 0 &&
           orient[1][0] == 0 && orient[1][1] == -1 && orient[1][2] == 0 &&
           orient[2][0] == 0 && orient[2][1] == 0 && orient[2][2] == 1) {
    double * spacing = polypVolume->GetImageData()->GetSpacing();
    spacing[0] = -1 * spacing[0];
    spacing[1] = -1 * spacing[1];
    polypVolume->GetImageData()->SetSpacing(spacing);
  }
  else {
    vtkErrorMacro("unknown image volume orientation.");
    return "";
  }

  return nodeName;
}

// contour the image volume of a polyp
vtkPolyData * vtkSlicerVirtualColonLogic::contourPolyp(const char * polypNodeName_, float threshold_)
{
  // get image volume of polyp
  vtkMRMLScalarVolumeNode * polypVolume = NULL;
  vtkCollection * nodes = this->GetMRMLScene()->GetNodesByName(polypNodeName_);
  if (nodes != NULL && 
      nodes->GetNumberOfItems() == 1 &&
      (polypVolume = vtkMRMLScalarVolumeNode::SafeDownCast(nodes->GetItemAsObject(0))) != NULL) 
  {
    ;
  }
  else {
    vtkWarningMacro("cannot find image volume for polyp");
    return NULL;
  }

  // remove previous contour
  map<string, vtkSmartPointer<vtkContourFilter> >::iterator itor = _mapPolypContourHolder.find(polypNodeName_);
  if (itor != _mapPolypContourHolder.end()) {
    _mapPolypContourHolder.erase(itor);
  }

  //vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  contourFilter->SetValue(0, threshold_);
  contourFilter->SetInput(polypVolume->GetImageData());
  contourFilter->Update();

  // add it to polyp contour holder, it will be deleted later
  _mapPolypContourHolder[polypNodeName_] = contourFilter;

  return contourFilter->GetOutput();
}

void vtkSlicerVirtualColonLogic::removePolypContour(const char * nodeName_) 
{
  map<string, vtkSmartPointer<vtkContourFilter> >::iterator itor = _mapPolypContourHolder.find(nodeName_);
  if (itor == _mapPolypContourHolder.end()) {
    vtkWarningMacro("cannot find the contour for the polyp.");
  }
  else {
    _mapPolypContourHolder.erase(itor);
  }

  // remove vtkVRMLScalarVolumeNode for this Polyp
  if (vtkCollection * nodes = this->GetMRMLScene()->GetNodesByName(nodeName_)) {
    if (nodes->GetNumberOfItems() == 1) {
      if (vtkMRMLScalarVolumeNode * polypVolume = vtkMRMLScalarVolumeNode::SafeDownCast(nodes->GetItemAsObject(0))) {
        this->GetMRMLScene()->RemoveNode(polypVolume);
      }
    }
  }
}

void vtkSlicerVirtualColonLogic::flashPolypMarker(const char * polypId_) {

  // remove marker if it already exists
  if (vtkCollection * nodes = this->GetMRMLScene()->GetNodesByName(polypId_)) {
    if (nodes->GetNumberOfItems() == 1) {
      if (vtkMRMLModelNode * modelNode = vtkMRMLModelNode::SafeDownCast(nodes->GetItemAsObject(0))) {
        this->GetMRMLScene()->RemoveNode(modelNode);
        return;
      }
    }
  }

  // create vtkMRMLModelMode and add it to the MRMLScene
  vtkSmartPointer<vtkMRMLModelNode> modelNode = vtkSmartPointer<vtkMRMLModelNode>::New();
  vtkSmartPointer<vtkMRMLModelDisplayNode> displayNode = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();

  modelNode->SetName(polypId_);
  this->GetMRMLScene()->SaveStateForUndo();

  modelNode->SetScene(this->GetMRMLScene());
  displayNode->SetScene(this->GetMRMLScene());
  displayNode->SetColor(255, 0, 0);

  this->GetMRMLScene()->AddNodeNoNotify(displayNode);
  modelNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  this->GetMRMLScene()->AddNode(modelNode);

  // add a sphere as polyp
  vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
  sphere->SetRadius(5);
  //sphere->SetPhiResolution(180);
  //sphere->SetThetaResolution(360);

  double xyz[3];
  if (!this->Node->GetPolypXYZ(polypId_, xyz)) {
    vtkErrorMacro("GetPolypXYZ() error.");
    return;
  }

  // convert DICOM (LPS) coordinate to RAS coordinate
  xyz[0] = - xyz[0];
  xyz[1] = - xyz[1];

  sphere->SetCenter(xyz);
  modelNode->SetAndObservePolyData(sphere->GetOutput());

}


// transform model node (segmented colon surface) according to image volume's image orientation and origin
// this is because the segmentation file does not contain image orientation information, which cause its coordinate mess up
void vtkSlicerVirtualColonLogic::transformModel(vtkMRMLModelNode * modelNode, double dir[3][3], double origin[3]) {
  ///////////////////////////////////////////////////////////////////////////
  // build transform matrix according to image orientation and origin

  vtkSmartPointer<vtkMatrix4x4> xformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  xformMatrix->Identity();

  // NOTE:
  // Image Orientation in DICOM file is -1 * x_dir, -1 * y_dir, 1 * z_dir
  // because DICOM coordinate is LPS system and dir is in RAS system

  // x direction
  if (dir[0][0] == 1 && dir[0][1] == 0 && dir[0][2] == 0) {
    // translate 2 * origin_in_RAS
    xformMatrix->SetElement(0, 0, 1);
    xformMatrix->SetElement(0, 1, 0);
    xformMatrix->SetElement(0, 2, 0);
    xformMatrix->SetElement(0, 3, 2 * origin[0]);
  }
  else if (dir[0][0] == -1 && dir[0][1] == 0 && dir[0][2] == 0) {
    // flip
    xformMatrix->SetElement(0, 0, -1);
    xformMatrix->SetElement(0, 1, 0);
    xformMatrix->SetElement(0, 2, 0);
    xformMatrix->SetElement(0, 3, 0);
  }
  else {
    vtkErrorMacro("Unknown Image Orientation: ");
    return;
  }

  // y direction
  if (dir[1][0] == 0 && dir[1][1] == 1 && dir[1][2] == 0) {
    // translate 2 * origin_in_RAS
    xformMatrix->SetElement(1, 0, 0);
    xformMatrix->SetElement(1, 1, 1);
    xformMatrix->SetElement(1, 2, 0);
    xformMatrix->SetElement(1, 3, 2 * origin[1]);
  }
  else if (dir[1][0] == 0 && dir[1][1] == -1 && dir[1][2] == 0) {
    // flip
    xformMatrix->SetElement(1, 0, 0);
    xformMatrix->SetElement(1, 1, -1);
    xformMatrix->SetElement(1, 2, 0);
    xformMatrix->SetElement(1, 3, 0);
  }
  else {
    vtkErrorMacro("Unknown Image Orientation: ");
    return;
  }

  // z direction
  if (dir[2][0] == 0 && dir[2][1] == 0 && dir[2][2] == 1) {
    // do nothing when transform between LPS and RAS
    xformMatrix->SetElement(2, 0, 0);
    xformMatrix->SetElement(2, 1, 0);
    xformMatrix->SetElement(2, 2, 1);
    xformMatrix->SetElement(2, 3, 0);
  }
  else {
    vtkErrorMacro("Unknown Image Orientation: ");
    return;
  }

  // apply transform to model node
  if (modelNode) {
    vtkSmartPointer<vtkMRMLLinearTransformNode> xformNode = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
    xformNode->ApplyTransform(xformMatrix);
    vtkMRMLTransformableNode::SafeDownCast(modelNode)->ApplyTransform(xformMatrix);
  }
  else {
    vtkErrorMacro("modelNode is NULL.");
    return;
  }
}


void vtkSlicerVirtualColonLogic::contourSegmentationFile(const char * segFilename) {
  /////////////////////////////////////////////////////////////////////////////
  // 1. load segmentation file (vktImageData): inside colon - 1, outside colon - 0
  // 2. build vtkMRMLModelNode using vtkCountourFlter and add it to scene

  vtkSmartPointer<vtkDataSetReader> reader = vtkSmartPointer<vtkDataSetReader>::New();
  reader->SetFileName(segFilename);

  vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  contourFilter->SetValue(0, 0.5);
  contourFilter->SetInput(reader->GetOutput());
  contourFilter->Update();
  vtkPolyData * contour = contourFilter->GetOutput();

  if (contour) {

    // build model node and add it to scene
    vtkMRMLScene * scene = this->Node->GetScene();
    scene->SaveStateForUndo();

    vtkSmartPointer<vtkMRMLModelNode> modelNode = vtkSmartPointer<vtkMRMLModelNode>::New();
    vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplayNode = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
    modelDisplayNode->SetOpacity(0.2);

    modelNode->SetName(segFilename);
    modelNode->SetScene(scene);
    modelDisplayNode->SetScene(scene);
    scene->AddNode(modelNode);
    scene->AddNodeNoNotify(modelDisplayNode);
    modelNode->AddAndObserveDisplayNodeID(modelDisplayNode->GetID());
    modelNode->SetAndObservePolyData(contour);
  }
}

