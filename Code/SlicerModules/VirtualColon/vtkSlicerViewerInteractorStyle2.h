/*
 * vtkSlicerViewerInteractorStyle2.h
 *
 *  Created on: Mar 3, 2009
 *      Author: haxu
 */

#ifndef VTKSLICERVIEWERINTERACTORSTYLE2_H_
#define VTKSLICERVIEWERINTERACTORSTYLE2_H_

#include "vtkSlicerVirtualColon.h"
#include "vtkSlicerViewerInteractorStyle.h"


/**
 * A class amend vtkSlicerViewerInteractorStyle to use vtkPropPicker to do picking operation.
 */
class VTK_VIRTUALCOLON_EXPORT vtkSlicerViewerInteractorStyle2 : public vtkSlicerViewerInteractorStyle
{
public:
  static vtkSlicerViewerInteractorStyle2 *New();
  vtkTypeRevisionMacro(vtkSlicerViewerInteractorStyle2,vtkSlicerViewerInteractorStyle);

  // override OnLeftButtonDown to use vtkPropPicker
  virtual void OnLeftButtonDown();

  virtual void Rotate();
  virtual void Spin();
  virtual void Pan();
  virtual void Dolly(double);

protected:
  vtkSlicerViewerInteractorStyle2() {};
  ~vtkSlicerViewerInteractorStyle2() {};

private:
  vtkSlicerViewerInteractorStyle2(const vtkSlicerViewerInteractorStyle&) {};  // Not implemented.
  vtkSlicerViewerInteractorStyle2 & operator=(const vtkSlicerViewerInteractorStyle&) { return *this; };  // Not implemented.

};

#endif /* VTKSLICERVIEWERINTERACTORSTYLE2_H_ */
