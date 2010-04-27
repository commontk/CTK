/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkVTKRenderView_h
#define __ctkVTKRenderView_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKVisualizationVTKWidgetsExport.h"

class ctkVTKRenderViewPrivate;
class vtkRenderWindowInteractor;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKRenderView : public QWidget
{
  Q_OBJECT
public:
  /// Constructors
  typedef QWidget   Superclass;
  explicit ctkVTKRenderView(QWidget* parent = 0);
  virtual ~ctkVTKRenderView();

  /// If a render has already been scheduled, this called is a no-op
  void scheduleRender();

  /// Force a render even if a render is already ocurring
  void forceRender();

  /// Set/Get window interactor
  vtkRenderWindowInteractor* interactor()const;
  void setInteractor(vtkRenderWindowInteractor* newInteractor);

  /// Set corner annotation text
  void setCornerAnnotationText(const QString& text);

  /// Set background color
  void setBackgroundColor(double r, double g, double b);

  void resetCamera();

  //virtual void setCornerText(const QString& text);
  
private:
  CTK_DECLARE_PRIVATE(ctkVTKRenderView);
}; 

#endif
