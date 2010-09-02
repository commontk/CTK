/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

#ifndef __ctkVTKRenderView_h
#define __ctkVTKRenderView_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKVisualizationVTKWidgetsExport.h"

class ctkVTKRenderViewPrivate;
class vtkInteractorObserver;
class vtkRenderWindowInteractor;
class vtkRenderWindow;
class vtkRenderer;
class vtkCamera; 

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKRenderView : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString cornerAnnotationText READ cornerAnnotationText WRITE setCornerAnnotationText)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(bool renderEnabled READ renderEnabled WRITE setRenderEnabled)
  Q_PROPERTY(bool orientationWidgetVisible READ orientationWidgetVisible
             WRITE setOrientationWidgetVisible)
  Q_PROPERTY(double zoomFactor READ zoomFactor WRITE setZoomFactor)
  Q_PROPERTY(int rotateDegrees READ rotateDegrees WRITE setRotateDegrees)
  Q_ENUMS(PitchDirection)
  Q_PROPERTY(PitchDirection pitchDirection READ pitchDirection WRITE setPitchDirection)
  Q_ENUMS(RollDirection)
  Q_PROPERTY(RollDirection rollDirection READ rollDirection WRITE setRollDirection)
  Q_ENUMS(YawDirection)
  Q_PROPERTY(YawDirection yawDirection READ yawDirection WRITE setYawDirection)

public:

  enum PitchDirection { PitchUp, PitchDown };
  enum RollDirection {RollLeft, RollRight};
  enum YawDirection {YawLeft, YawRight};

  /// Constructors
  typedef QWidget   Superclass;
  explicit ctkVTKRenderView(QWidget* parent = 0);
  virtual ~ctkVTKRenderView(){}

public slots:

  /// If a render has already been scheduled, this called is a no-op
  void scheduleRender();

  /// Force a render even if a render is already ocurring
  void forceRender();

  /// Set background color
  void setBackgroundColor(const QColor& newBackgroundColor);

  /// Enable/Disable rendering
  void setRenderEnabled(bool value);

  /// Set corner annotation \a text
  void setCornerAnnotationText(const QString& text);

  /// Show/Hide Orientation widget
  void setOrientationWidgetVisible(bool visible);

  /// Set absolute amount degrees the view should be either pitched, rolled or yawed with.
  /// \sa pitch setPitchDirection roll setRollDirection yaw setYawDirection
  /// \note The default is 5 degrees
  void setRotateDegrees(int newRotateDegrees);

  /// Pitch view of X degrees. X been set using setRotateDegrees
  /// \sa setRotateDegrees
  void pitch();

  /// Rool view of X degrees. X been set using setRotateDegrees
  /// \sa setRotateDegrees
  void roll();

  /// Yaw view of X degrees. X been set using setRotateDegrees
  /// \sa setRotateDegrees
  void yaw();

  /// \brief Set zoom factor
  /// zoomFactor is a value between 0.0 and 1.0
  /// \note The default value is 0.05
  void setZoomFactor(double newZoomFactor);

  /// Zoom in using the \a zoomfactor
  /// \sa setZoomFactor
  void zoomIn();

  /// Zoom out using the \a zoomfactor
  /// \sa setZoomFactor
  void zoomOut();

  /// Set the focal point
  void setFocalPoint(int x, int y, int z);

  /// \brief Reset focal point
  /// The visible scene bbox is computed, then the camera is recentered around its centroid.
  void resetFocalPoint();

public:
  /// Get underlying RenderWindow
  vtkRenderWindow* renderWindow()const;
  
  /// Set/Get window interactor
  vtkRenderWindowInteractor* interactor()const;
  void setInteractor(vtkRenderWindowInteractor* newInteractor);

  /// Get current interactor style
  vtkInteractorObserver* interactorStyle();

  /// Get corner annotation \a text
  QString cornerAnnotationText() const;

  /// Get background color
  QColor backgroundColor() const;

  /// Get Orientation widget visibility
  bool orientationWidgetVisible();

  /// Get active camera
  vtkCamera* activeCamera();

  /// Reset camera
  void resetCamera();

  /// Get a reference to the associated vtkRenderer
  vtkRenderer* renderer()const;

  /// Return if rendering is enabled
  bool renderEnabled() const;

  /// Return amount of degrees used to either pitch, roll or yaw the view
  int rotateDegrees()const;

  PitchDirection pitchDirection()const;
  void setPitchDirection(PitchDirection newPitchDirection);

  RollDirection rollDirection()const;
  void setRollDirection(RollDirection newRollDirection);

  YawDirection yawDirection()const;
  void setYawDirection(YawDirection newYawDirection);

  /// Return zoom factor
  double zoomFactor()const;
  
private:
  CTK_DECLARE_PRIVATE(ctkVTKRenderView);
}; 

#endif
