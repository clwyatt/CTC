/*
 * vtkSlicerViewerInteractorStyle.cxx
 *
 *  Created on: Mar 3, 2009
 *      Author: haxu
 */

#include "vtkRenderWindowInteractor.h"
#include "vtkAssemblyPath.h"
#include "vtkAbstractPropPicker.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkMath.h"

#include "vtkSlicerViewerInteractorStyle2.h"
#include "vtkMRMLInteractionNode.h"

vtkCxxRevisionMacro(vtkSlicerViewerInteractorStyle2, "$Revision: 1.32 $");
vtkStandardNewMacro(vtkSlicerViewerInteractorStyle2);

void vtkSlicerViewerInteractorStyle2::OnLeftButtonDown() {

  vtkRenderWindowInteractor *rwi = this->Interactor;
  if ( this->GetCameraNode() != NULL ) {
    if (vtkMRMLInteractionNode * interactionNode = vtkMRMLInteractionNode::SafeDownCast (this->GetCameraNode()->GetScene()->GetNthNodeByClass(0,"vtkMRMLInteractionNode"))) {
      int mouseInteractionMode = interactionNode->GetCurrentInteractionMode();

      // PickManupilate in vtkSlicerViewerInteractorStyle
      if (!rwi->GetShiftKey() && !rwi->GetControlKey() && mouseInteractionMode == vtkMRMLInteractionNode::PickManipulate) {

        // this is copied from vtkInteractorStyle::OnChar():: case: 'p' 'P':
        if (this->State == VTKIS_NONE) {
          vtkAssemblyPath *path = NULL;
          int *eventPos = rwi->GetEventPosition();
          this->FindPokedRenderer(eventPos[0], eventPos[1]);
          rwi->StartPickCallback();
          vtkAbstractPropPicker *picker =
            vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
          if ( picker != NULL ) {
            picker->Pick(eventPos[0], eventPos[1],
                         0.0, this->CurrentRenderer);
            path = picker->GetPath();
          }
          if ( path == NULL ) {
            this->HighlightProp(NULL);
            this->PropPicked = 0;
          }
          else {
            this->HighlightProp(path->GetFirstNode()->GetViewProp());
            this->PropPicked = 1;
          }
          rwi->EndPickCallback();
        }
      }
    }
  }


  this->vtkSlicerViewerInteractorStyle::OnLeftButtonDown();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void vtkSlicerViewerInteractorStyle2::Rotate()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkRenderWindowInteractor *rwi = this->Interactor;

  int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

  double delta_elevation = -20.0 / size[1];
  double delta_azimuth = -20.0 / size[0];

  double rxf = (double)dx * delta_azimuth * this->MotionFactor;
  double ryf = (double)dy * delta_elevation * this->MotionFactor;

  vtkCamera *camera = NULL;
    camera = this->CurrentRenderer->GetActiveCamera();

  camera->Azimuth(rxf);
  camera->Elevation(ryf);
  camera->OrthogonalizeViewUp();

  if (this->AutoAdjustCameraClippingRange)
    {
    this->CurrentRenderer->ResetCameraClippingRange();
    }

  if (rwi->GetLightFollowCamera())
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

  // release the camera
  camera = NULL;
  rwi->Render();
}



//----------------------------------------------------------------------------
void vtkSlicerViewerInteractorStyle2::Spin()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkRenderWindowInteractor *rwi = this->Interactor;

  double *center = this->CurrentRenderer->GetCenter();

  double newAngle =
    atan2((double)rwi->GetEventPosition()[1] - (double)center[1],
          (double)rwi->GetEventPosition()[0] - (double)center[0]);

  double oldAngle =
    atan2((double)rwi->GetLastEventPosition()[1] - (double)center[1],
          (double)rwi->GetLastEventPosition()[0] - (double)center[0]);

  newAngle *= vtkMath::RadiansToDegrees();
  oldAngle *= vtkMath::RadiansToDegrees();

  vtkCamera *camera = NULL;
    camera = this->CurrentRenderer->GetActiveCamera();

  camera->Roll(newAngle - oldAngle);
  camera->OrthogonalizeViewUp();

  // release the camera
  camera = NULL;

  rwi->Render();
}

//----------------------------------------------------------------------------
void vtkSlicerViewerInteractorStyle2::Pan()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkRenderWindowInteractor *rwi = this->Interactor;

  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];

  // Calculate the focal depth since we'll be using it a lot
  vtkCamera *camera = NULL;
    camera = this->CurrentRenderer->GetActiveCamera();

  camera->GetFocalPoint(viewFocus);
  this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2],
                              viewFocus);
  focalDepth = viewFocus[2];

  this->ComputeDisplayToWorld((double)rwi->GetEventPosition()[0],
                              (double)rwi->GetEventPosition()[1],
                              focalDepth,
                              newPickPoint);

  // Has to recalc old mouse point since the viewport has moved,
  // so can't move it outside the loop

  this->ComputeDisplayToWorld((double)rwi->GetLastEventPosition()[0],
                              (double)rwi->GetLastEventPosition()[1],
                              focalDepth,
                              oldPickPoint);

  // Camera motion is reversed

  motionVector[0] = oldPickPoint[0] - newPickPoint[0];
  motionVector[1] = oldPickPoint[1] - newPickPoint[1];
  motionVector[2] = oldPickPoint[2] - newPickPoint[2];

  camera->GetFocalPoint(viewFocus);
  camera->GetPosition(viewPoint);
  camera->SetFocalPoint(motionVector[0] + viewFocus[0],
                        motionVector[1] + viewFocus[1],
                        motionVector[2] + viewFocus[2]);

  camera->SetPosition(motionVector[0] + viewPoint[0],
                      motionVector[1] + viewPoint[1],
                      motionVector[2] + viewPoint[2]);

  if (rwi->GetLightFollowCamera())
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

  // release the camera
  camera = NULL;

  rwi->Render();
}


//----------------------------------------------------------------------------
void vtkSlicerViewerInteractorStyle2::Dolly(double factor)
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkCamera *camera = NULL;
    camera = this->CurrentRenderer->GetActiveCamera();

  if (camera->GetParallelProjection())
    {
    camera->SetParallelScale(camera->GetParallelScale() / factor);
    }
  else
    {
    camera->Dolly(factor);
    if (this->AutoAdjustCameraClippingRange)
      {
      this->CurrentRenderer->ResetCameraClippingRange();
      }
    }

  if (this->Interactor->GetLightFollowCamera())
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

  this->Interactor->Render();

  camera = NULL;
}



