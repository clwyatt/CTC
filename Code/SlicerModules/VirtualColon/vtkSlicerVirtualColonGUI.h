/*
 * vtkSlicerVirtualColonGUI.h
 *
 *  Created on: Oct 3, 2008
 *      Author: haxu
 */

#ifndef __vtkSlicerVirtualColonGUI_h
#define __vtkSlicerVirtualColonGUI_h

#include "vtkSlicerModuleGUI.h"
#include "vtkSlicerVirtualColon.h"
#include "vtkSlicerVirtualColonLogic.h"
#include "vtkSlicerVirtualColonNode.h"

#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWPushButton.h"
#include "vtkKWFileBrowserDialog.h"

class vtkSlicerSliceWidget;
class vtkKWFrame;
class vtkKWScaleWithEntry;
class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;

class VTK_VIRTUALCOLON_EXPORT vtkSlicerVirtualColonGUI : public vtkSlicerModuleGUI
{
public:
  static vtkSlicerVirtualColonGUI *New();
  vtkTypeMacro(vtkSlicerVirtualColonGUI,vtkSlicerModuleGUI);

  // Description: Get/Set MRML node
  vtkGetObjectMacro (Logic, vtkSlicerVirtualColonLogic);
  vtkSetObjectMacro (Logic, vtkSlicerVirtualColonLogic);


  // Description:
  // Set the logic pointer from parent class pointer.
  // Overloads implementation in vtkSlicerModulesGUI
  // to allow loadable modules.
  virtual void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic(reinterpret_cast<vtkSlicerVirtualColonLogic*> (logic));
  }

  // Description: Get/Set MRML node
  vtkGetObjectMacro (Node, vtkSlicerVirtualColonNode);

  // Description:
  // Create widgets
  virtual void BuildGUI ( );

  // Description:
  // Add obsereves to GUI widgets
  virtual void AddGUIObservers ( );

  // Description:
  // Remove obsereves to GUI widgets
  virtual void RemoveGUIObservers ( );

  virtual void ProcessGUIEvents(
      vtkObject *caller, unsigned long event, void *callData );

  virtual void ProcessMRMLEvents ( vtkObject * /*caller*/,
    unsigned long /*event*/, void * /*callData*/ );

protected:
  vtkMRMLNode * _createNode(const char * className_, const char * nodeName_);

  void _jumpToPolypSlice(const char * polypId_);

  // show a square as boundary around polyp candidate
  void _updatePolypBoundary(vtkMRMLSliceNode * sliceNode);

  // show polyp volume in a standalone vtkRenderWidget
  void _popupPolyp(string polypId_, double x_, double y_);
  void _reContourPolyp(const char * polypId_, float threshold_);
  void _showPolypArea(const char * polypId_, bool display_);

  void _showVoxels(const char * polypId_, bool display_);

protected:
  vtkSlicerVirtualColonGUI();
  virtual ~vtkSlicerVirtualColonGUI();
  vtkSlicerVirtualColonGUI(const vtkSlicerVirtualColonGUI&);
  void operator=(const vtkSlicerVirtualColonGUI&);

  vtkSlicerVirtualColonLogic * Logic;
  vtkSlicerVirtualColonNode * Node;

  // polyp candidate frame
  vtkKWLabel * _polypFeatureFileLabel;
  vtkKWEntry * _polypFeatureFileEntry;
  vtkKWPushButton * _polypFeatureFileButton;

  vtkKWLabel * _voxelFeatureFileLabel;
  vtkKWEntry * _voxelFeatureFileEntry;
  vtkKWPushButton * _voxelFeatureFileButton;

  vtkKWLabel * _predFileLabel;
  vtkKWEntry * _predFileEntry;
  vtkKWPushButton * _predFileButton;

  vtkKWLabel * _patIdLabel;
  vtkKWEntry * _patIdEntry;
  vtkKWLabel * _studyIdLabel;
  vtkKWEntry * _studyIdEntry;
  vtkKWPushButton * _applyButton;

  vtkSlicerNodeSelectorWidget* VolumeSelector;
  vtkKWMultiColumnListWithScrollbars * _tablePolypCand;

  // segmentation frame
  vtkSmartPointer<vtkKWEntry> segFileEntry;
  vtkSmartPointer<vtkSlicerNodeSelectorWidget> segVolumeSelector;
  vtkSmartPointer<vtkSlicerNodeSelectorWidget> modelSelector;
  vtkSmartPointer<vtkKWPushButton> segFileButton;
  vtkSmartPointer<vtkKWPushButton> segLoadButton;
  vtkSmartPointer<vtkKWPushButton> applyTransButton;
  vtkSmartPointer<vtkKWEntry> imageOrientationEntry;
  vtkSmartPointer<vtkKWEntry> imageOriginEntry;
  vtkSmartPointer<vtkKWEntry> contourThresholdEntry;
  vtkSmartPointer<vtkKWEntry> thresholdPIDEntry;
  vtkSmartPointer<vtkKWPushButton> thresholdButton;

  // used to hold squares as polyp boundary in 2D slice view
  vtkActor2DCollection * _polypBoundaryActors_axial;
  vtkActor2DCollection * _polypBoundaryActors_sagittal;
  vtkActor2DCollection * _polypBoundaryActors_coronal;

  static float POLYP_VOLUME_SIZE;  // size of polyp will be extracted from image volume
  static float POLYP_AREA_SIZE;  // size of area around polyp will be extracted from image volume
  map<string, vtkSmartPointer<vtkRenderer> > _mapPidRenderer;
};

#endif

