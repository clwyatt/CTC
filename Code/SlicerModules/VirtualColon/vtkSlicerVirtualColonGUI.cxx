
#include "vtkCubeSource.h"
#include "vtkExtractEdges.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkProperty.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLLabelMapVolumeDisplayNode.h"
#include "vtkMRMLStorageNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkSlicerColorLogic.h"
#include "vtkSlicerViewerInteractorStyle2.h"
#include "vtkSlicerVirtualColonGUI.h"
#include "vtkSlicerApplication.h"


#include "VCPredictSet.hpp"
#include "VCPolypSet.hpp"
#include "VCVoxelSet.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/filesystem.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace boost;

float DEFAULT_THRESHOLD = -574; //median threshold in Hong Li's paper
//float DEFAULT_THRESHOLD = -814; //near-air threshold in Hong Li's paper

float vtkSlicerVirtualColonGUI::POLYP_VOLUME_SIZE = 15;  // size of polyp will be extracted from image volume
float vtkSlicerVirtualColonGUI::POLYP_AREA_SIZE = 35;  // size of area around polyp will be extracted from image volume

// helper function to find Axial/Sagittal/Coronal slice node (vtkMRMLSliceNode)
vector<vtkMRMLSliceNode *> _getMRMLSliceNodes(vtkMRMLNode * node);

vtkSlicerVirtualColonGUI* vtkSlicerVirtualColonGUI::New()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkSlicerVirtualColonGUI");
  if(ret)
    {
      return (vtkSlicerVirtualColonGUI*)ret;
    }
  return new vtkSlicerVirtualColonGUI;
}


vtkSlicerVirtualColonGUI::vtkSlicerVirtualColonGUI()
{
  this->Logic = NULL;
  this->Node = NULL;
  _polypBoundaryActors_axial = vtkActor2DCollection::New();
  _polypBoundaryActors_sagittal = vtkActor2DCollection::New();
  _polypBoundaryActors_coronal = vtkActor2DCollection::New();
}

vtkSlicerVirtualColonGUI::~vtkSlicerVirtualColonGUI()
{
  this->SetLogic (NULL);
  vtkSetMRMLNodeMacro(this->Node, NULL);
  _polypBoundaryActors_axial->Delete();
  _polypBoundaryActors_sagittal->Delete();
  _polypBoundaryActors_coronal->Delete();

  // polyp candidate frame
  if (_tablePolypCand) {
    _tablePolypCand->SetParent(NULL);
    _tablePolypCand->Delete();
    _tablePolypCand = NULL;
  }

  if (_polypFeatureFileLabel) {
    _polypFeatureFileLabel->SetParent(NULL);
    _polypFeatureFileLabel->Delete();
    _polypFeatureFileLabel = NULL;
  }
  if (_polypFeatureFileEntry) {
    _polypFeatureFileEntry->SetParent(NULL);
    _polypFeatureFileEntry->Delete();
    _polypFeatureFileEntry = NULL;
  }
  if (_polypFeatureFileButton) {
    _polypFeatureFileButton->SetParent(NULL);
    _polypFeatureFileButton->Delete();
    _polypFeatureFileButton = NULL;
  }

  if (_voxelFeatureFileLabel) {
    _voxelFeatureFileLabel->SetParent(NULL);
    _voxelFeatureFileLabel->Delete();
    _voxelFeatureFileLabel = NULL;
  }
  if (_voxelFeatureFileEntry) {
    _voxelFeatureFileEntry->SetParent(NULL);
    _voxelFeatureFileEntry->Delete();
    _voxelFeatureFileEntry = NULL;
  }
  if (_voxelFeatureFileButton) {
    _voxelFeatureFileButton->SetParent(NULL);
    _voxelFeatureFileButton->Delete();
    _voxelFeatureFileButton = NULL;
  }

  if (_predFileLabel) {
    _predFileLabel->SetParent(NULL);
    _predFileLabel->Delete();
    _predFileLabel = NULL;
  }
  if (_predFileEntry) {
    _predFileEntry->SetParent(NULL);
    _predFileEntry->Delete();
    _predFileEntry = NULL;
  }
  if (_predFileButton) {
    _predFileButton->SetParent(NULL);
    _predFileButton->Delete();
    _predFileButton = NULL;
  }

  if (_patIdLabel) {
    _patIdLabel->SetParent(NULL);
    _patIdLabel->Delete();
    _patIdLabel = NULL;
  }
  if (_patIdEntry) {
    _patIdEntry->SetParent(NULL);
    _patIdEntry->Delete();
    _patIdEntry = NULL;
  }

  if (_studyIdLabel) {
    _studyIdLabel->SetParent(NULL);
    _studyIdLabel->Delete();
    _studyIdLabel = NULL;
  }
  if (_studyIdEntry) {
    _studyIdEntry->SetParent(NULL);
    _studyIdEntry->Delete();
    _studyIdEntry = NULL;
  }

  if (_applyButton) {
    _applyButton->SetParent(NULL);
    _applyButton->Delete();
    _applyButton = NULL;
  }
}


void vtkSlicerVirtualColonGUI::BuildGUI ( )
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  vtkSlicerVirtualColonNode* n = vtkSlicerVirtualColonNode::New();
  this->Logic->GetMRMLScene()->RegisterNodeClass(n);
  n->Delete();
  n->SetScene(this->Logic->GetMRMLScene());
  // set an observe new node in Logic
  vtkSetAndObserveMRMLNodeMacro(this->Node, n);
  this->Logic->SetAndObserveSlicerVirtualColonNode(n);
  this->Logic->GetMRMLScene()->RegisterNodeClass(n);

  this->UIPanel->AddPage ( "SlicerVirtualColon", "SlicerVirtualColon", NULL );
  // ---
  // MODULE GUI FRAME
  // ---
 // Define your help text and build the help frame here.
  const char *help = "The SlicerVirtualColon module....";
  const char *about = "This work was supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See <a>http://www.slicer.org</a> for details. ";
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "SlicerVirtualColon" );
  this->BuildHelpAndAboutFrame ( page, help, about );

  /////////////////////////////////////////////////////////////////////////////
  // build polyp candidate frame
  {
    vtkSlicerModuleCollapsibleFrame *moduleFrame = vtkSlicerModuleCollapsibleFrame::New ( );
    moduleFrame->SetParent ( this->UIPanel->GetPageWidget ( "SlicerVirtualColon" ) );
    moduleFrame->Create ( );
    moduleFrame->SetLabelText ("Polyp Candidate");
    moduleFrame->ExpandFrame ( );
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                  moduleFrame->GetWidgetName(), this->UIPanel->GetPageWidget("SlicerVirtualColon")->GetWidgetName());

    // polyp candidate frame
    vtkKWFrame *predFileFrame = vtkKWFrame::New();
    predFileFrame->SetParent ( moduleFrame->GetFrame() );
    predFileFrame->Create();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
        predFileFrame->GetWidgetName() );

    _predFileLabel = vtkKWLabel::New();
    _predFileLabel->SetParent (predFileFrame);
    _predFileLabel->Create();
    _predFileLabel->SetText("Prediction file: ");
    _predFileEntry = vtkKWEntry::New();
    _predFileEntry->SetParent (predFileFrame);
    _predFileEntry->Create();
    _predFileButton = vtkKWPushButton::New();
    _predFileButton->SetParent (predFileFrame);
    _predFileButton->Create();
    _predFileButton->SetText("Browse ...");

    app->Script("pack %s %s %s -side left -anchor w -padx 4 -pady 2",
                _predFileLabel->GetWidgetName(),
                _predFileEntry->GetWidgetName(),
                _predFileButton->GetWidgetName());

    this->VolumeSelector = vtkSlicerNodeSelectorWidget::New();
    this->VolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
    this->VolumeSelector->SetParent( moduleFrame->GetFrame() );
    this->VolumeSelector->Create();
    this->VolumeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
    this->VolumeSelector->UpdateMenu();

    this->VolumeSelector->SetBorderWidth(2);
    this->VolumeSelector->SetLabelText( "Input Volume: ");
    this->VolumeSelector->SetBalloonHelpString("select an input volume from the current mrml scene.");
    app->Script("pack %s -side top -anchor w -padx 4 -pady 2",
                  this->VolumeSelector->GetWidgetName());

    //
    vtkKWFrame *polypFileFrame = vtkKWFrame::New();
    polypFileFrame->SetParent ( moduleFrame->GetFrame() );
    polypFileFrame->Create();
    app->Script ( "pack %s -side top -anchor w -fill x -padx 2 -pady 2",
                  polypFileFrame->GetWidgetName() );

    _polypFeatureFileLabel = vtkKWLabel::New();
    _polypFeatureFileLabel->SetParent (polypFileFrame);
    _polypFeatureFileLabel->Create();
    _polypFeatureFileLabel->SetText("Polyp feature file: ");
    _polypFeatureFileEntry = vtkKWEntry::New();
    _polypFeatureFileEntry->SetParent (polypFileFrame);
    _polypFeatureFileEntry->Create();
    _polypFeatureFileButton = vtkKWPushButton::New();
    _polypFeatureFileButton->SetParent (polypFileFrame);
    _polypFeatureFileButton->Create();
    _polypFeatureFileButton->SetText("Browse");
    app->Script("pack %s %s %s -side left -anchor w -padx 4 -pady 2",
                _polypFeatureFileLabel->GetWidgetName(),
                _polypFeatureFileEntry->GetWidgetName(),
                _polypFeatureFileButton->GetWidgetName());

    vtkKWFrame *voxelFileFrame = vtkKWFrame::New();
    voxelFileFrame->SetParent ( moduleFrame->GetFrame() );
    voxelFileFrame->Create();
    app->Script ( "pack %s -side top -anchor w -fill x -padx 2 -pady 2",
        voxelFileFrame->GetWidgetName() );

    _voxelFeatureFileLabel = vtkKWLabel::New();
    _voxelFeatureFileLabel->SetParent (voxelFileFrame);
    _voxelFeatureFileLabel->Create();
    _voxelFeatureFileLabel->SetText("Voxel feature file: ");
    _voxelFeatureFileEntry = vtkKWEntry::New();
    _voxelFeatureFileEntry->SetParent (voxelFileFrame);
    _voxelFeatureFileEntry->Create();
    _voxelFeatureFileButton = vtkKWPushButton::New();
    _voxelFeatureFileButton->SetParent (voxelFileFrame);
    _voxelFeatureFileButton->Create();
    _voxelFeatureFileButton->SetText("Browse ...");
    app->Script("pack %s %s %s -side left -anchor w -padx 4 -pady 2",
                _voxelFeatureFileLabel->GetWidgetName(),
                _voxelFeatureFileEntry->GetWidgetName(),
                _voxelFeatureFileButton->GetWidgetName());

    vtkKWFrame *patIdFrame = vtkKWFrame::New();
    patIdFrame->SetParent ( moduleFrame->GetFrame() );
    patIdFrame->Create();
    app->Script ( "pack %s -side top -anchor w -fill x -padx 2 -pady 2",
        patIdFrame->GetWidgetName() );

    _patIdLabel = vtkKWLabel::New();
    _patIdLabel->SetParent (patIdFrame);
    _patIdLabel->Create();
    _patIdLabel->SetText("Patient: ");
    _patIdEntry = vtkKWEntry::New();
    _patIdEntry->SetParent (patIdFrame);
    _patIdEntry->Create();
    app->Script("pack %s %s -side left -anchor w -padx 4 -pady 2",
                _patIdLabel->GetWidgetName(),
                _patIdEntry->GetWidgetName());

    vtkKWFrame *studyIdFrame = vtkKWFrame::New();
    studyIdFrame->SetParent ( moduleFrame->GetFrame() );
    studyIdFrame->Create();
    app->Script ( "pack %s -side top -anchor w -fill x -padx 2 -pady 2",
        studyIdFrame->GetWidgetName() );

    _studyIdLabel = vtkKWLabel::New();
    _studyIdLabel->SetParent (studyIdFrame);
    _studyIdLabel->Create();
    _studyIdLabel->SetText("Study: ");
    _studyIdEntry = vtkKWEntry::New();
    _studyIdEntry->SetParent (studyIdFrame);
    _studyIdEntry->Create();
    app->Script("pack %s -side left -anchor w -padx 4 -pady 2", _studyIdLabel->GetWidgetName());
    app->Script("pack %s -side left -anchor w -fill x -padx 4 -pady 2", _studyIdEntry->GetWidgetName());

    _applyButton = vtkKWPushButton::New();
    _applyButton->SetParent (moduleFrame->GetFrame());
    _applyButton->Create();
    _applyButton->SetText("Apply");
    app->Script("pack %s -side top -anchor e -padx 4 -pady 2",
                _applyButton->GetWidgetName());

    // polyp candidates table
    _tablePolypCand = vtkKWMultiColumnListWithScrollbars::New ( );
    _tablePolypCand->SetParent (moduleFrame->GetFrame());
    _tablePolypCand->Create ( );
    _tablePolypCand->SetHeight(10);
    _tablePolypCand->GetWidget()->SetSelectionTypeToRow();
    _tablePolypCand->GetWidget()->SetSelectionModeToSingle();
    _tablePolypCand->GetWidget()->MovableRowsOff();
    _tablePolypCand->GetWidget()->MovableColumnsOff();


    // set up the columns of data for each point
    // refer to the header file for order
    _tablePolypCand->GetWidget()->AddColumn("Polyp Id");
    _tablePolypCand->GetWidget()->AddColumn("X");
    _tablePolypCand->GetWidget()->AddColumn("Y");
    _tablePolypCand->GetWidget()->AddColumn("Z");
    _tablePolypCand->GetWidget()->AddColumn("Pred 0");
    _tablePolypCand->GetWidget()->AddColumn("Pred 1");
    _tablePolypCand->GetWidget()->AddColumn("Target");

    // now set the attributes that are equal across the columns
    for (int col = 0; col < _tablePolypCand->GetWidget()->GetNumberOfColumns(); col++)
    {
      _tablePolypCand->GetWidget()->SetColumnWidth(col, 8);
      _tablePolypCand->GetWidget()->SetColumnAlignmentToLeft(col);
      //_tablePolypCand->GetWidget()->ColumnEditableOn(col);
    }

    app->Script ( "pack %s -fill both -expand true", _tablePolypCand->GetWidgetName());
    //_tablePolypCand->GetWidget()->SetCellUpdatedCommand(this, "UpdateElement");

    polypFileFrame->Delete();
    voxelFileFrame->Delete();
    predFileFrame->Delete();
    moduleFrame->Delete();
    patIdFrame->Delete();
    studyIdFrame->Delete();
    }

  /////////////////////////////////////////////////////////////////////////////
  // build segmentation frames
  {
    vtkSmartPointer<vtkSlicerModuleCollapsibleFrame> moduleFrame = vtkSmartPointer<vtkSlicerModuleCollapsibleFrame>::New ( );
    moduleFrame->SetParent ( this->UIPanel->GetPageWidget ( "SlicerVirtualColon" ) );
    moduleFrame->Create ( );
    moduleFrame->SetLabelText ("Segmentation");
    moduleFrame->ExpandFrame ( );
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                  moduleFrame->GetWidgetName(), this->UIPanel->GetPageWidget("SlicerVirtualColon")->GetWidgetName());

    // segmentation file
    vtkSmartPointer<vtkKWFrame> segFrame = vtkSmartPointer<vtkKWFrame>::New();
    segFrame->SetParent ( moduleFrame->GetFrame() );
    segFrame->Create();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2", segFrame->GetWidgetName() );

    // controls
    vtkSmartPointer<vtkKWLabel> segFileLabel = vtkSmartPointer<vtkKWLabel>::New();
    segFileEntry = vtkSmartPointer<vtkKWEntry>::New();
    segFileButton = vtkSmartPointer<vtkKWPushButton>::New();
    segFileLabel->SetParent (segFrame);
    segFileLabel->Create();
    segFileLabel->SetText("File (.vtk): ");
    segFileEntry->SetParent (segFrame);
    segFileEntry->Create();
    segFileButton->SetParent (segFrame);
    segFileButton->Create();
    segFileButton->SetText("Browse");
    app->Script("pack %s %s %s -side left -anchor w -padx 4 -pady 2",
                segFileLabel->GetWidgetName(),
                segFileEntry->GetWidgetName(),
                segFileButton->GetWidgetName());

    segLoadButton = vtkSmartPointer<vtkKWPushButton>::New();
    segLoadButton->SetParent (segFrame);
    segLoadButton->Create();
    segLoadButton->SetText("Load & Contour");
    app->Script("pack %s -side left -anchor w -padx 4 -pady 2", segLoadButton->GetWidgetName());

    // volume selector
    segVolumeSelector = vtkSmartPointer<vtkSlicerNodeSelectorWidget>::New();
    segVolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
    segVolumeSelector->SetParent( moduleFrame->GetFrame() );
    segVolumeSelector->Create();
    segVolumeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
    segVolumeSelector->UpdateMenu();
    segVolumeSelector->SetBorderWidth(2);
    segVolumeSelector->SetLabelText( "Image Volume: ");
    segVolumeSelector->SetBalloonHelpString("select an input volume from the current mrml scene.");
    app->Script("pack %s -side top -anchor w -padx 4 -pady 2", segVolumeSelector->GetWidgetName());

    // model selector (segmentation suface model)
    modelSelector = vtkSmartPointer<vtkSlicerNodeSelectorWidget>::New();
    modelSelector->SetNodeClass("vtkMRMLModelNode", NULL, NULL, NULL);
    modelSelector->SetParent( moduleFrame->GetFrame() );
    modelSelector->Create();
    modelSelector->SetMRMLScene(this->Logic->GetMRMLScene());
    modelSelector->UpdateMenu();
    modelSelector->SetBorderWidth(2);
    modelSelector->SetLabelText( "Segmentation Model: ");
    modelSelector->SetBalloonHelpString("select an model from the current mrml scene.");
    app->Script("pack %s -side top -anchor w -padx 4 -pady 2", modelSelector->GetWidgetName());

    // image orientation and origin info
    vtkSmartPointer<vtkKWFrame> orientFrame = vtkSmartPointer<vtkKWFrame>::New();
    orientFrame->SetParent ( moduleFrame->GetFrame() );
    orientFrame->Create();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2", orientFrame->GetWidgetName() );

    vtkSmartPointer<vtkKWLabel> imageOrientationLabel = vtkSmartPointer<vtkKWLabel>::New();
    imageOrientationEntry = vtkSmartPointer<vtkKWEntry>::New();
    imageOrientationLabel->SetParent (orientFrame);
    imageOrientationLabel->Create();
    imageOrientationLabel->SetText("XYZ axis in RAS: ");
    imageOrientationEntry->SetParent (orientFrame);
    imageOrientationEntry->Create();
    app->Script("pack %s %s -side left -anchor w -padx 4 -pady 2",
                imageOrientationLabel->GetWidgetName(),
                imageOrientationEntry->GetWidgetName());

    // image orientation info
    vtkSmartPointer<vtkKWFrame> originFrame = vtkSmartPointer<vtkKWFrame>::New();
    originFrame->SetParent ( moduleFrame->GetFrame() );
    originFrame->Create();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2", originFrame->GetWidgetName() );

    vtkSmartPointer<vtkKWLabel> imageOriginLabel = vtkSmartPointer<vtkKWLabel>::New();
    imageOriginEntry = vtkSmartPointer<vtkKWEntry>::New();
    imageOriginLabel->SetParent (originFrame);
    imageOriginLabel->Create();
    imageOriginLabel->SetText("Image Origin in RAS: ");
    imageOriginEntry->SetParent (originFrame);
    imageOriginEntry->Create();
    app->Script("pack %s %s -side left -anchor w -padx 4 -pady 2",
                imageOriginLabel->GetWidgetName(),
                imageOriginEntry->GetWidgetName());

    applyTransButton = vtkSmartPointer<vtkKWPushButton>::New();
    applyTransButton->SetParent (moduleFrame->GetFrame());
    applyTransButton->Create();
    applyTransButton->SetText("Transform");

    app->Script("pack %s -side top -anchor w -padx 4 -pady 2", applyTransButton->GetWidgetName());


    // contour threshold
    vtkSmartPointer<vtkKWFrame> contourThresholdFrame = vtkSmartPointer<vtkKWFrame>::New();
    contourThresholdFrame->SetParent ( moduleFrame->GetFrame() );
    contourThresholdFrame->Create();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2", contourThresholdFrame->GetWidgetName() );

    vtkSmartPointer<vtkKWLabel> thresholdPIDLabel = vtkSmartPointer<vtkKWLabel>::New();
    thresholdPIDEntry = vtkSmartPointer<vtkKWEntry>::New();
    thresholdPIDLabel->SetParent (contourThresholdFrame);
    thresholdPIDLabel->Create();
    thresholdPIDLabel->SetText("polyp id: ");
    thresholdPIDEntry->SetParent (contourThresholdFrame);
    thresholdPIDEntry->Create();
    app->Script("pack %s %s -side left -anchor w -padx 4 -pady 2",
                thresholdPIDLabel->GetWidgetName(),
                thresholdPIDEntry->GetWidgetName());

    vtkSmartPointer<vtkKWLabel> contourThresholdLabel = vtkSmartPointer<vtkKWLabel>::New();
    contourThresholdEntry = vtkSmartPointer<vtkKWEntry>::New();
    contourThresholdLabel->SetParent (contourThresholdFrame);
    contourThresholdLabel->Create();
    contourThresholdLabel->SetText("contour threshold: ");
    contourThresholdEntry->SetParent (contourThresholdFrame);
    contourThresholdEntry->Create();
    app->Script("pack %s %s -side left -anchor w -padx 4 -pady 2",
                contourThresholdLabel->GetWidgetName(),
                contourThresholdEntry->GetWidgetName());

    thresholdButton = vtkSmartPointer<vtkKWPushButton>::New();
    thresholdButton->SetParent (moduleFrame->GetFrame());
    thresholdButton->Create();
    thresholdButton->SetText("threshold");

    app->Script("pack %s -side top -anchor w -padx 4 -pady 2", thresholdButton->GetWidgetName());


    applyTransButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    segLoadButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *) this->GUICallbackCommand);
    segFileButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    segVolumeSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *) this->GUICallbackCommand);
    thresholdButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->GetRenderWindowInteractor()->AddObserver(vtkCommand::KeyPressEvent, (vtkCommand*)this->GUICallbackCommand);

    // change the interactor style of 3D viewer
    vtkSmartPointer<vtkSlicerViewerInteractorStyle2> istyle = vtkSmartPointer<vtkSlicerViewerInteractorStyle2>::New();
    vtkRenderWindowInteractor * rwi = this->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->GetRenderWindowInteractor();
    vtkSlicerViewerInteractorStyle *origStyle = vtkSlicerViewerInteractorStyle::SafeDownCast(rwi->GetInteractorStyle());
    istyle->SetViewerWidget(origStyle->GetViewerWidget());
    istyle->SetApplicationLogic (origStyle->GetApplicationLogic());
    rwi->SetInteractorStyle (istyle);
  }
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualColonGUI::AddGUIObservers ( )
{
  _polypFeatureFileButton->AddObserver(
      vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  _voxelFeatureFileButton->AddObserver(
      vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  _predFileButton->AddObserver(
      vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  _applyButton->AddObserver(
      vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  VolumeSelector->AddObserver(
      vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
      (vtkCommand *)this->GUICallbackCommand);
  _tablePolypCand->GetWidget()->AddObserver(
      vtkKWMultiColumnList::SelectionChangedEvent,
      (vtkCommand *)this->GUICallbackCommand);

  vtkAbstractPicker * picker = vtkAbstractPicker::SafeDownCast(this->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->GetRenderWindowInteractor()->GetPicker());
  picker->AddObserver(vtkCommand::EndPickEvent, (vtkCommand *)this->GUICallbackCommand);
  //vtkSlicerViewerInteractorStyle * istyle = vtkSlicerViewerInteractorStyle::SafeDownCast(this->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->GetRenderWindowInteractor()->GetInteractorStyle());
  //istyle->AddObserver(vtkSlicerViewerInteractorStyle::SelectRegionEvent, (vtkCommand *) this->GUICallbackCommand);

}



//---------------------------------------------------------------------------
void vtkSlicerVirtualColonGUI::RemoveGUIObservers ( )
{
  _polypFeatureFileButton->RemoveObservers(
      vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  _voxelFeatureFileButton->RemoveObservers(
      vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  _predFileButton->RemoveObservers(
      vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  _applyButton->RemoveObservers(
      vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  VolumeSelector->RemoveObservers(
      vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
      (vtkCommand *)this->GUICallbackCommand);
  _tablePolypCand->GetWidget()->RemoveObservers(
      vtkKWMultiColumnList::SelectionChangedEvent,
      (vtkCommand *)this->GUICallbackCommand);
}



//--------------------------------------------------------------------------
void vtkSlicerVirtualColonGUI::ProcessGUIEvents(
  vtkObject *caller, unsigned long event, void *callData )
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWPushButton * button = vtkKWPushButton::SafeDownCast(caller);
  vtkSlicerNodeSelectorWidget *selector = vtkSlicerNodeSelectorWidget::SafeDownCast(caller);
  vtkKWMultiColumnList * table = vtkKWMultiColumnList::SafeDownCast(caller);
  vtkRenderWindowInteractor * iren = vtkRenderWindowInteractor::SafeDownCast(caller);

  if (iren != NULL && event == vtkCommand::KeyPressEvent) {
    if (iren->GetKeyCode() == 'a')
      this->_showPolypArea(thresholdPIDEntry->GetValue(), true);
    else if (iren->GetKeyCode() == 'b')
      this->_showPolypArea(thresholdPIDEntry->GetValue(), false);
    else if (iren->GetKeyCode() == 'v')
      this->_showVoxels(thresholdPIDEntry->GetValue(), true);
    else if (iren->GetKeyCode() == 'c')
      this->_showVoxels(thresholdPIDEntry->GetValue(), false);
  }
  // apply button
  if (event == vtkKWPushButton::InvokedEvent &&
      button == _applyButton) {

    _tablePolypCand->GetWidget()->DeleteAllRows();

    // load prediction, polyp candiates and voxels
    Node->LoadPolypCandidate(
      _polypFeatureFileEntry->GetValue(),
      _voxelFeatureFileEntry->GetValue(),
      _predFileEntry->GetValue(),
      _patIdEntry->GetValue(),
      _studyIdEntry->GetValue());

    if (Node->GetNumberOfPolyps() != Node->GetNumberOfPredicts()) {
      vtkErrorMacro("Node->GetNumberOfPolys() != Node->GetNumberOfPredicts()."
          "\n");
      return;
    }

    vtkKWMultiColumnList * tabPlpCand = _tablePolypCand->GetWidget();
    for (int row = 0; row < Node->GetNumberOfPolyps(); row++) {
      tabPlpCand->AddRow();
      VCPolyp polyp = Node->GetPolyp(row);
      VCPredict pred = Node->GetPredict(row);

      // column 0 - polyp id
      tabPlpCand->SetCellText(row, 0, polyp.getPolypId().c_str());
      // column 1,2,3 - X, Y, Z
      tabPlpCand->SetCellTextAsFormattedDouble(row, 1, polyp.getX(), 4);
      tabPlpCand->SetCellTextAsFormattedDouble(row, 2, polyp.getY(), 4);
      tabPlpCand->SetCellTextAsFormattedDouble(row, 3, polyp.getZ(), 4);

      if (pred.getPolypId() != polyp.getPolypId()) {
        vtkDebugMacro("VirtualColon: Incomplete prediction. polypId = "
            << polyp.getPolypId() << " has no prediction.\n");
        return;
      }

      // column 4,5 - prediction for class 0, 1
      vector<double> probs = pred.getPredProb();
      tabPlpCand->SetCellTextAsFormattedDouble(row, 4, probs.at(0), 4);
      tabPlpCand->SetCellTextAsFormattedDouble(row, 5, probs.at(1), 4);

      // column 6 - target
      tabPlpCand->SetCellTextAsInt(row, 6, pred.getTarget());
    }

    // observe vtkMRMLSliceNode::ModifiedEvent
    vector<vtkMRMLSliceNode*> sliceNodes = _getMRMLSliceNodes(Node);
    for (unsigned int n = 0; n < sliceNodes.size(); n++) {
      vtkMRMLSliceNode * node = sliceNodes.at(n);
      vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
      events->InsertNextValue(vtkCommand::ModifiedEvent);
      this->MRMLObserverManager->AddObjectEvents(node, events);
    }
  }
  // file browse buttons
  else if (event == vtkKWPushButton::InvokedEvent &&
      (button == _polypFeatureFileButton ||
       button == _voxelFeatureFileButton ||
       button == _predFileButton) ||
       button == segFileButton) {

    // Create the file browser dialog
    vtkKWFileBrowserDialog *dlg = vtkKWFileBrowserDialog::New();
    dlg->SetApplication(app);
    dlg->Create();
    dlg->SetTitle(app->GetName());

    // Invoke the dialog
    dlg->RetrieveLastPathFromRegistry("LastPath");
    dlg->Invoke();

    if (dlg->GetStatus()==vtkKWDialog::StatusOK)
    {
      dlg->SaveLastPathToRegistry("LastPath");
      string fname = dlg->GetFileName();
      if (button == _polypFeatureFileButton)
        _polypFeatureFileEntry->SetValue(fname.c_str());
      else if (button == _voxelFeatureFileButton)
        _voxelFeatureFileEntry->SetValue(fname.c_str());
      else if (button == _predFileButton)
        _predFileEntry->SetValue(fname.c_str());
      else if (button== segFileButton)
        segFileEntry->SetValue(fname.c_str());
    }
    dlg->Delete();

    // save state for undo
    //this->MRMLScene->SaveStateForUndo();

  }
  else if (selector == VolumeSelector &&
           event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent &&
           VolumeSelector->GetSelected() != NULL) {
    // update text for Polyp/VoxelFeatureFile, patientID, studyID
    this->_polypFeatureFileEntry->SetValue("");
    this->_voxelFeatureFileEntry->SetValue("");
    this->_patIdEntry->SetValue("");
    this->_studyIdEntry->SetValue("");

    vtkMRMLScalarVolumeNode * volume =
      vtkMRMLScalarVolumeNode::SafeDownCast(VolumeSelector->GetSelected());
    if (volume != NULL) {
      string fname = volume->GetStorageNode()->GetFullNameFromFileName();
      boost::filesystem::path f(fname);
      if (f.has_parent_path() && f.parent_path().has_parent_path()) {
        string studyId = f.parent_path().filename();
        string patId = f.parent_path().parent_path().filename();
        string polypFeatureFilename = "";
        string voxelFeatureFilename = "";
        boost::filesystem::path fPolyp = f.parent_path().parent_path() /
            (studyId + "_ExtractedPolyps.mat.matched.1.csv");
        if (boost::filesystem::exists(fPolyp))
          polypFeatureFilename = fPolyp.string();
        boost::filesystem::path fVoxel = f.parent_path().parent_path() /
            (studyId + "_ExtractedVoxels.mat.matched.1.csv");
        if (boost::filesystem::exists(fVoxel))
          voxelFeatureFilename = fVoxel.string();

        this->_polypFeatureFileEntry->SetValue(polypFeatureFilename.c_str());
        this->_voxelFeatureFileEntry->SetValue(voxelFeatureFilename.c_str());
        this->_patIdEntry->SetValue(patId.c_str());
        this->_studyIdEntry->SetValue(studyId.c_str());

        // set node parameters from GUI widgets
        Node->Set_inputVolumeID(volume->GetID());
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////////////
  // selection of polyp candidate changed

  else if (table == _tablePolypCand->GetWidget() &&
           event == vtkKWMultiColumnList::SelectionChangedEvent) {

    // save node parameters for Undo
//    this->GetLogic()->GetMRMLScene()->SaveStateForUndo(n);

    string polypId("");
    int rowIdxSelected = table->GetIndexOfFirstSelectedRow();
    if (rowIdxSelected >= 0 && rowIdxSelected < table->GetNumberOfRows()) {
      polypId = table->GetCellText(rowIdxSelected, 0);

      thresholdPIDEntry->SetValue(polypId.c_str());

      // show slicer in axial, sagittal and coronal views
      _jumpToPolypSlice(polypId.c_str());

      this->Logic->flashPolypMarker(polypId.c_str());
    }
  }
  /////////////////////////////////////////////////////////////////////////////
  // transform colon surface model
  //
  else if (event == vtkKWPushButton::InvokedEvent &&
           button == applyTransButton) {
    // image orientation string should be "(x,x,x),(x,x,x),(x,x,x)"
    // image origin string should be "(x,x,x)"
    istringstream issOrient(imageOrientationEntry->GetValue());
    istringstream issOrigin(imageOriginEntry->GetValue());
    double dir[3][3], origin[3];
    if (issOrient.good() && issOrigin.good()) {
      char c;

      if (!(issOrient >> c >> dir[0][0] >> c >> dir[0][1] >> c >> dir[0][2] >> c >> c
                    >> c >> dir[1][0] >> c >> dir[1][1] >> c >> dir[1][2] >> c >> c
                    >> c >> dir[2][0] >> c >> dir[2][1] >> c >> dir[2][2] >> c) ||
          issOrient.peek() != EOF) {
        vtkErrorMacro("Failed to retrieve image orientation.");
        return;
      }

      if (!(issOrigin >> c >> origin[0] >> c >> origin[1] >> c >> origin[2] >> c) ||
          issOrigin.peek() != EOF) {
        vtkErrorMacro("Failed to retrieve image origin.");
        return;
      }
    }
    else {
      vtkErrorMacro("Error in image orientation and/or origin");
      return;
    }

    // apply transform to model node
    if (vtkMRMLModelNode * modelNode = vtkMRMLModelNode::SafeDownCast(modelSelector->GetSelected())) {
      this->Logic->transformModel(modelNode, dir, origin);
    }
  }
  /////////////////////////////////////////////////////////////////////////////
  // load and contour segmentation file
  //
  else if (event == vtkKWPushButton::InvokedEvent &&
           button == segLoadButton) {
    this->Logic->contourSegmentationFile(segFileEntry->GetValue());
  }
  /////////////////////////////////////////////////////////////////////////////
  // in segmentation, fill up image orientation and image origin edit box.
  //
  else if (selector == segVolumeSelector &&
           event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent) {
    // if image volume node is changed in segmentation frame,
    // update Image Orientation (XYZ in RAS) info
    this->imageOrientationEntry->SetValue("");

    vtkMRMLScalarVolumeNode * volNode = vtkMRMLScalarVolumeNode::SafeDownCast(segVolumeSelector->GetSelected());
    if (volNode!= NULL) {
      double dir[3][3], origin[3];
      volNode->GetIJKToRASDirections(dir);
      volNode->GetOrigin(origin);

      ostringstream ossOrient;
      ossOrient << "(" << dir[0][0] << "," << dir[0][1] << "," << dir[0][2] << "), "
             << "(" << dir[1][0] << "," << dir[1][1] << "," << dir[1][2] << "), "
             << "(" << dir[2][0] << "," << dir[2][1] << "," << dir[2][2] << ")";
      this->imageOrientationEntry->SetValue(ossOrient.str().c_str());

      ostringstream ossOrigin;
      ossOrigin << "(" << origin[0] << "," << origin[1] << "," << origin[2] << ")";
      this->imageOriginEntry->SetValue(ossOrigin.str().c_str());
    }
  }
  else if (event == vtkCommand::EndPickEvent) {
    if (vtkPropPicker * picker = vtkPropPicker::SafeDownCast(caller)) {

      vtkKWRenderWidget * renwidget = this->GetApplicationGUI()->GetViewerWidget()->GetMainViewer();
      vtkSlicerViewerInteractorStyle2 * istyle = vtkSlicerViewerInteractorStyle2::SafeDownCast(renwidget->GetRenderWindowInteractor()->GetInteractorStyle());
      // if click on renderer other than the first render (main render which show colon shape)
      if (istyle->GetCurrentRenderer() != renwidget->GetRenderer())
        return;

      double * pick = picker->GetPickPosition();
      double * selection = picker->GetSelectionPoint();

      // TODO: use GetAssemblyPath rather than GetActor() here
      if (vtkActor * actor = picker->GetActor()) {
        double * polypCenter = actor->GetCenter();
        // change from RAS coordinate to DICOM coordinate
        polypCenter[0] *= -1;
        polypCenter[1] *= -1;
        string polypId = this->Node->GetPolypId(polypCenter);
        this->_popupPolyp(polypId, selection[0], selection[1]);

        //double * pick = picker->GetPickPosition();
        //cout << "EndPickEvent: " << endl;
        //cout << polypId << ": " << *pick << "," << *(pick + 1) <<"," << *(pick + 2) << endl;
      }
    }
  }
  /////////////////////////////////////////////////////////////////////////////
  // set contour threshold
  //
  else if (event == vtkKWPushButton::InvokedEvent &&
           button == thresholdButton) {

    istringstream iss(contourThresholdEntry->GetValue());
    float threshold = 0;
    if((iss >> threshold).fail()) {
      vtkErrorMacro("failed to retrive threshold as a float number.");
      return;
    }

    _reContourPolyp(thresholdPIDEntry->GetValue(), threshold);
  }

}



void vtkSlicerVirtualColonGUI::ProcessMRMLEvents (
    vtkObject * caller, unsigned long event, void * callData )
{
  vtkMRMLSliceNode * sliceNode;
  if ((sliceNode = vtkMRMLSliceNode::SafeDownCast(caller)) &&
      event == vtkCommand::ModifiedEvent)
  {
    // show a square as boundary around polyp candidate when vtkSlicerSliceNode was modified
    _updatePolypBoundary(sliceNode);
  }
}

void vtkSlicerVirtualColonGUI::_jumpToPolypSlice(const char * polypId_)
{
  // find input volume
    vtkMRMLScalarVolumeNode * inVolume =
      vtkMRMLScalarVolumeNode::SafeDownCast(
          this->GetMRMLScene()->GetNodeByID(Node->Get_inputVolumeID()));
  if (inVolume == NULL)
  {
    vtkErrorMacro("No input volume found");
    return;
  }

  double coord[3];
  if (!Node->GetPolypXYZ(polypId_, coord)) {
    vtkErrorMacro("Node->GetPolypXYZ() failed.");
    return;
  }
  //coord[2] = -34.0;  // for WRAMC_VC-019_less_image

  //////////////////////////////////////////////////////////////////////
  // jump to slice

  // find Axial/Sagittal/Coronal slice node (vtkMRMLSliceNode) and jump to the
  // slice according to polyp location (DICOM coordinate)
  vector<vtkMRMLSliceNode*> sliceNodes = _getMRMLSliceNodes(Node);
  for (unsigned int n = 0; n < sliceNodes.size(); n++) {
    vtkMRMLSliceNode * node = sliceNodes.at(n);
    if (node != NULL) {
      vtkMatrix4x4 * m = node->GetSliceToRAS();
      double r = m->GetElement(0, 3);
      double a = m->GetElement(1, 3);
      double s = m->GetElement(2, 3);

      // transfer from DICOM coordinate (LPS) to RAS coordinate:
      //   r = -x, a = -y, s = z
      const string & AXIAL = "Axial";
      const string & SAGITTAL = "Sagittal";
      const string & CORONAL = "Coronal";
      if (AXIAL == node->GetOrientationString())
        s = coord[2];
      else if (SAGITTAL == node->GetOrientationString())
        r = -coord[0];
      else if (CORONAL == node->GetOrientationString())
        a = -coord[1];
      else {
        vtkErrorMacro("node->GetOrientationString() == " <<
            node->GetOrientationString());
        return;
      }

      node->JumpSlice(r, a, s);
    }
  }

}

//////////////////////////////////////////////////////////////////////
// show a square as boundary around polyp candidate when vtkSlicerSliceNode
// was modified
void vtkSlicerVirtualColonGUI::_updatePolypBoundary(vtkMRMLSliceNode * sliceNode) {
  if (!sliceNode)
    return;

  vtkMatrix4x4 * sliceToRAS = sliceNode->GetSliceToRAS();
  vtkSmartPointer<vtkMatrix4x4> rasToXY = vtkSmartPointer<vtkMatrix4x4>::New();
  rasToXY->DeepCopy(sliceNode->GetXYToRAS());
  rasToXY->Invert();

  const string & AXIAL = "Axial";
  const string & SAGITTAL = "Sagittal";
  const string & CORONAL = "Coronal";
  const double & SIZE =  15;  // 15mm in RAS coordinate

  // create a collection of vtkActor2D as polyp boundary
  vtkSmartPointer<vtkActor2DCollection> actors = vtkSmartPointer<vtkActor2DCollection>::New();
  for (int i = 0; i < Node->GetNumberOfPolyps(); i++) {
    VCPolyp polyp = Node->GetPolyp(i);
    double coord[4];
    // transfer polyp location from DICOM coord (LPS) to RAS coordinate
    coord[0] = - polyp.getX();
    coord[1] = - polyp.getY();
    coord[2] = polyp.getZ();
    coord[3] = 1;

    // if polyp is not close to current slice in Z direction (depth, not Z axis in RAS coordinate),
    //   continue
    if (AXIAL == sliceNode->GetOrientationString()) {
      if (fabs(coord[2] - sliceToRAS->GetElement(2, 3)) > SIZE / 2)
        continue;
    }
    else if (SAGITTAL == sliceNode->GetOrientationString()) {
      if (fabs(coord[0] - sliceToRAS->GetElement(0, 3)) > SIZE / 2)
        continue;
    }
    else if (CORONAL == sliceNode->GetOrientationString()) {
      if (fabs(coord[1] - sliceToRAS->GetElement(1, 3)) > SIZE / 2)
        continue;
    }
    else {
      vtkErrorMacro("sliceNode->GetOrientationString() == " <<
          sliceNode->GetOrientationString());
      return;
    }

    // map XYZ coord to XY in viewport(display) coodinate
    //cout << coord[0] << "," << coord[1] << "," << coord[2] << "," << coord[3] << endl;
    rasToXY->MultiplyPoint(coord, coord);
    //cout << coord[0] << "," << coord[1] << "," << coord[2] << "," << coord[3] << endl;
    //cout << "GetOrientationString(): " << sliceNode->GetOrientationString() << endl;

    vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
    cube->SetCenter(coord[0], coord[1], coord[2]);
    // set cube length to SIZE mm in RAS coordinate
    double sizeInXY0[4] = {0, 0, 0, 1};
    double sizeInXY15[4] = {SIZE, SIZE, SIZE, 1};
    rasToXY->MultiplyPoint(sizeInXY0, sizeInXY0);
    rasToXY->MultiplyPoint(sizeInXY15, sizeInXY15);
    cube->SetXLength(fabs(sizeInXY15[0] - sizeInXY0[0]));
    cube->SetYLength(fabs(sizeInXY15[1] - sizeInXY0[1]));
    //cube->SetZLength(sizeInXY15[0] - sizeInXY0[0]);

    vtkSmartPointer<vtkExtractEdges> cubeEdges = vtkSmartPointer<vtkExtractEdges>::New();
    cubeEdges->SetInputConnection(cube->GetOutputPort());

    // create actor and mapper, add them to renderer
    //vtkSmartPointer<vtkCoordinate> coords = vtkCoordinate::New();
    //coords->SetCoordinateSystemToViewport();

    vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    mapper->SetInputConnection(cubeEdges->GetOutputPort());
    //mapper->SetInputConnection(cube->GetOutputPort());
    //mapper->SetTransformCoordinate(coords);

    vtkSmartPointer<vtkActor2D> actor = vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(mapper);
    actors->AddItem(actor);
  }

  // find the viewer (axial, sagittal or coronal)
  string layoutName;
  vtkActor2DCollection * polypBoundaryActors;
  if (AXIAL == sliceNode->GetOrientationString()) {
    layoutName = "Red";
    polypBoundaryActors = _polypBoundaryActors_axial;
  }
  else if (SAGITTAL == sliceNode->GetOrientationString()) {
    layoutName = "Yellow";
    polypBoundaryActors = _polypBoundaryActors_sagittal;
  }
  else if (CORONAL == sliceNode->GetOrientationString()) {
    layoutName = "Green";
    polypBoundaryActors = _polypBoundaryActors_coronal;
  }
  else {
    vtkErrorMacro("sliceNode->GetOrientationString() == " <<
        sliceNode->GetOrientationString());
    return;
  }

  if (vtkSlicerApplicationGUI * appGui = this->GetApplicationGUI()) {
    if (vtkSlicerSliceGUI * sliceGui = appGui->GetMainSliceGUI(const_cast<char*>(layoutName.c_str()))) {
      if (vtkSlicerSliceViewer * sliceViewer = sliceGui->GetSliceViewer()) {
        if (vtkRenderer * renderer = sliceViewer->GetRenderWidget()->GetRenderer()) {

          // remove previous polyp boundary actors
          polypBoundaryActors->InitTraversal();
          for (vtkActor2D * actor = polypBoundaryActors->GetNextActor2D();
               actor != NULL;
               actor = polypBoundaryActors->GetNextActor2D()) {
            renderer->RemoveActor(actor); // remove actors
          }
          polypBoundaryActors->RemoveAllItems();

          // add new polyp boundary actors
          actors->InitTraversal();
          for (vtkActor2D * actor = actors->GetNextActor2D();
               actor != NULL;
               actor = actors->GetNextActor2D()) {
            renderer->AddActor(actor);
            polypBoundaryActors->AddItem(actor);
          }
          actors->RemoveAllItems();
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// show polyp volume in a standalone vtkRenderWidget
//
void vtkSlicerVirtualColonGUI::_popupPolyp(string polypId_, double x_, double y_)
{
  vtkKWRenderWidget * renwidget = this->GetApplicationGUI()->GetViewerWidget()->GetMainViewer();

  // if there is already a Renderer shown for this polyp, remove it
  map<string, vtkSmartPointer<vtkRenderer> >::iterator itor = _mapPidRenderer.find(polypId_);
  if (itor != _mapPidRenderer.end()) {
    vtkRenderer * renderer = itor->second;
    renwidget->RemoveRenderer(renderer);
    _mapPidRenderer.erase(itor);

    // remove polyp contour
    string polypVolemeNode = Logic->getPolypVolumeNodeName(polypId_.c_str(), POLYP_VOLUME_SIZE);
    this->Logic->removePolypContour(polypVolemeNode.c_str());

    // if polyp area contour/actor is still displayed, remove it
    string nodeName = Logic->getPolypVolumeNodeName(polypId_.c_str(), POLYP_AREA_SIZE);
    Logic->removePolypContour(nodeName.c_str());
    //renderer->RemoveActor(actor); actor is a smartpointer, will be deleted as Rednerer is being deleted

    return;
  }

  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

  // calculate the position (in screen unit) of the vtkRenderer
  int * winsize = renwidget->GetRenderWindow()->GetSize();
  const double OFFSET = 5.0;  // 5 pixels
  const double WIDTH = 200.0 / winsize[0];  // 200 pixel width
  const double HEIGHT = 200.0 / winsize[1];  // 200 pixel height
  double xmin = x_ / winsize[0] + OFFSET / winsize[0];
  double ymin = y_ / winsize[1] + OFFSET / winsize[1];
  renderer->SetBackground(0.0, 0.0, 0.0);
  renderer->SetViewport(xmin, ymin, xmin + WIDTH, ymin + HEIGHT);

  // get source and add Actor to the vtkRenderer
  //vtkPolyData * polypContour = this->Logic->cropAndContourPolyp(polypId_.c_str());
  //vtkPolyData * polypContour = this->Logic->cropPolypFromSegmentationContour(polypId_.c_str(), segFileEntry->GetValue());
  string polypVolumeNode = Logic->extractPolypVolume(polypId_.c_str(), POLYP_VOLUME_SIZE);

  // get contour threshold
  istringstream iss(contourThresholdEntry->GetValue());
  float threshold = 0;
  if((iss >> threshold).fail()) {
    threshold = DEFAULT_THRESHOLD;
  }
  vtkPolyData * polypContour = Logic->contourPolyp(polypVolumeNode.c_str(), threshold);

  //vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  mapper->SetInput(polypContour);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(1.0, 0.0, 0.0);  // red
  renderer->AddActor(actor);

  // add vtkRenderer to vtkKWRenderWidget
  renderer->SetRenderWindow(renwidget->GetRenderWindow());
  renwidget->AddRenderer(renderer);

  _mapPidRenderer[polypId_] = renderer;
}

void vtkSlicerVirtualColonGUI::_reContourPolyp(const char * polypId_, float threshold_) 
{
  // replace actor with new contour
  vtkRenderer * renderer = _mapPidRenderer[polypId_];
  if (!renderer)
    return;

  // re-contour polyp volume
  {
    string nodeName = Logic->getPolypVolumeNodeName(polypId_, POLYP_VOLUME_SIZE);

    renderer->RemoveAllViewProps();  //TODO: remove contour actor only
    vtkPolyData * polypContour  = this->Logic->contourPolyp(nodeName.c_str(), threshold_);
    if (polypContour) {
      vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
      mapper->SetInput(polypContour);
      actor->SetMapper(mapper);
      renderer->AddActor(actor);
    }
  }

  // re-contour polyp area volume
  {
    string nodeName = Logic->getPolypVolumeNodeName(polypId_, POLYP_AREA_SIZE);

    renderer->RemoveAllViewProps();  //TODO: remove contour actor only
    vtkPolyData * polypContour  = this->Logic->contourPolyp(nodeName.c_str(), threshold_);
    if (polypContour) {
      vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
      mapper->SetInput(polypContour);
      actor->SetMapper(mapper);
      renderer->AddActor(actor);
    }
  }
}

void vtkSlicerVirtualColonGUI::_showPolypArea(const char * polypId_, bool display_) 
{
  static map<string, vtkSmartPointer<vtkActor> > _mapPidAreaActor;  // AreaActor: polyp area surface

  vtkRenderer * renderer = _mapPidRenderer[polypId_];
  if (!renderer)
    return;

  if (!display_) {
    string nodeName = Logic->getPolypVolumeNodeName(polypId_, POLYP_AREA_SIZE);
    Logic->removePolypContour(nodeName.c_str());
    if (vtkActor * actor = _mapPidAreaActor[polypId_]) {
      renderer->RemoveActor(actor);
      _mapPidAreaActor.erase(polypId_);
    }
  }
  else {
    // if already shown, return
    if (vtkActor * actor = _mapPidAreaActor[polypId_])
      return;

    // get contour threshold
    istringstream iss(contourThresholdEntry->GetValue());
    float threshold = 0;
    if((iss >> threshold).fail()) {
      threshold = DEFAULT_THRESHOLD; 
    }

    string nodeName = Logic->extractPolypVolume(polypId_, POLYP_AREA_SIZE);
    vtkPolyData * polypArea = Logic->contourPolyp(nodeName.c_str(), threshold);
    if (!polypArea)
      return;

    //vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
    vtkSmartPointer<vtkPolyDataMapper> mapperLarge = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> actorLarge = vtkSmartPointer<vtkActor>::New();
    mapperLarge->SetInput(polypArea);
    actorLarge->SetMapper(mapperLarge);
    actorLarge->GetProperty()->SetColor(0.5, 0.5, 0.5);  // gray
    renderer->AddActor(actorLarge);

    _mapPidAreaActor[polypId_] = actorLarge;
  }

  renderer->Render();
}

void vtkSlicerVirtualColonGUI::_showVoxels(const char * polypId_, bool display_) 
{
  vtkRenderer * renderer = _mapPidRenderer[polypId_];
  if (renderer == NULL)
    return;

  static map<string, vtkSmartPointer<vtkActor>> mapPidVoxels;

  if (display_) {
    vtkSmartPointer<vtkAppendPolyData> appender = vtkSmartPointer<vtkAppendPolyData>::New();

    list<VCVoxel> voxels = Node->GetVoxels(polypId_);
    for (list<VCVoxel>::const_iterator itor = voxels.begin(); itor != voxels.end(); itor++) {
      vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
      sphere->SetRadius(0.1);
      //sphere->SetPhiResolution(180);
      //sphere->SetThetaResolution(360);
      sphere->SetCenter(-1 * itor->getX(), -1 * itor->getY(), itor->getZ());
      appender->AddInput(sphere->GetOutput());
    }

    appender->Update();
    vtkPolyData * polydata = appender->GetOutput();
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

    mapper->SetInput(appender->GetOutput());
    actor->SetMapper(mapper);
    renderer->AddActor(actor);
    mapPidVoxels[polypId_] = actor;
  }
  else {
    if (vtkActor * actor = mapPidVoxels[polypId_]) {
      renderer->RemoveActor(actor);
      mapPidVoxels.erase(polypId_);
    }
  }
}


// find Axial/Sagittal/Coronal slice node (vtkMRMLSliceNode)
vector<vtkMRMLSliceNode *> _getMRMLSliceNodes(vtkMRMLNode * node)
{
  vector<vtkMRMLSliceNode*> retval;

  vtkMRMLScene * scene = node->GetScene();
  if (scene == NULL) {
    ostringstream oss;
    oss << "ERROR: In " __FILE__ ", line " << __LINE__ << endl
        << "vtkMRMLScene::GetActiveScene() == NULL" << endl;
    throw oss.str();
  }

  int nnodes = scene->GetNumberOfNodesByClass("vtkMRMLSliceNode");
  for (int n=0; n<nnodes; n++) {
    vtkMRMLSliceNode * node = vtkMRMLSliceNode::SafeDownCast (
          scene->GetNthNodeByClass(n, "vtkMRMLSliceNode"));
    if (node != NULL) {
      retval.push_back(node);
    }
  }

  return retval;
}



