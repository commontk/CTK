/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkVTKRenderView_h
#define __ctkVTKRenderView_h

// CTK includes
#include <ctkAxesWidget.h>
#include "ctkVTKAbstractView.h"
class ctkVTKRenderViewPrivate;

class vtkCamera;
class vtkRenderer;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKRenderView : public ctkVTKAbstractView
{
  Q_OBJECT
  Q_PROPERTY(bool orientationWidgetVisible READ orientationWidgetVisible
             WRITE setOrientationWidgetVisible)
  Q_PROPERTY(double zoomFactor READ zoomFactor WRITE setZoomFactor)
  Q_PROPERTY(double pitchRollYawIncrement READ pitchRollYawIncrement WRITE setPitchRollYawIncrement)
  Q_ENUMS(RotateDirection)
  Q_PROPERTY(RotateDirection pitchDirection READ pitchDirection WRITE setPitchDirection)
  Q_PROPERTY(RotateDirection rollDirection READ rollDirection WRITE setRollDirection)
  Q_PROPERTY(RotateDirection yawDirection READ yawDirection WRITE setYawDirection)
  Q_PROPERTY(RotateDirection spinDirection READ spinDirection WRITE setSpinDirection)
  Q_PROPERTY(bool spinEnabled READ spinEnabled WRITE setSpinEnabled)
  Q_PROPERTY(double spinIncrement READ spinIncrement WRITE setSpinIncrement)
  Q_PROPERTY(int animationIntervalMs READ animationIntervalMs WRITE setAnimationIntervalMs)
  Q_PROPERTY(bool rockEnabled READ rockEnabled WRITE setRockEnabled)
  Q_PROPERTY(int rockLength READ rockLength WRITE setRockLength)

public:

  enum RotateDirection { PitchUp, PitchDown, RollLeft, RollRight, YawLeft, YawRight };

  typedef ctkVTKAbstractView Superclass;
  explicit ctkVTKRenderView(QWidget* parent = 0);
  virtual ~ctkVTKRenderView();

public Q_SLOTS:
  /// Show/Hide Orientation widget
  void setOrientationWidgetVisible(bool visible);

  /// Set absolute amount degrees the view should be either pitched, rolled or yawed with.
  /// \sa pitch setPitchDirection roll setRollDirection yaw setYawDirection
  /// \note The default is 5 degrees
  void setPitchRollYawIncrement(double newPitchRollYawIncrement);

  /// Pitch view of X degrees. X been set using setPitchRollYawIncrement
  /// \sa setPitchRollYawIncrement setPitchDirection
  void pitch();

  /// Rool view of X degrees. X been set using setPitchRollYawIncrement
  /// \sa setPitchRollYawIncrement setRollDirection
  void roll();

  /// Yaw view of X degrees. X been set using setPitchRollYawIncrement
  /// \sa setPitchRollYawIncrement setYawDirection
  void yaw();

  /// Enable or Disbled the animated spin of the view
  void setSpinEnabled(bool enabled);

  /// Set number of degrees in spin increment
  /// \sa setSpinDirection setSpinIntervalMs
  void setSpinIncrement(double newSpinIncrement);

  /// Amount of wait time between spin or rock increments
  /// \sa setSpinIncrement setRockIncrement
  void setAnimationIntervalMs(int ms);

  /// Enable or Disbled the animated rock of the view
  void setRockEnabled(bool enabled);

  /// Set length of a complete rock period (in number of animation steps)
  void setRockLength(int newRockLength);

  /// Set current rocking position
  /// Rocking will be centered around the position where increment is set to 0.
  void setRockIncrement(int newRockIncrement);

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
  void setFocalPoint(double x, double y, double z);

  /// \brief Reset focal point
  /// The visible scene bbox is computed, then the camera is recentered around its centroid.
  void resetFocalPoint();

  /// \brief Change camera to look from a given axis to the focal point
  /// Translate/Rotate the camera to look from a given axis
  /// The Field of View (fov) controls how far from the focal point the
  /// camera must be (final_pos = focal_point + 3*fov).
  void lookFromAxis(const ctkAxesWidget::Axis& axis, double fov = 10.);

public:

  /// Get Orientation widget visibility
  bool orientationWidgetVisible();

  /// Get active camera
  vtkCamera* activeCamera();

  /// Reset camera
  void resetCamera();

  /// Get a reference to the associated vtkRenderer
  vtkRenderer* renderer()const;

  /// Set window interactor
  /// Reimplemented to propagate interaction to Orientation widget
  virtual void setInteractor(vtkRenderWindowInteractor* interactor);

  /// Return pitch, roll or yaw increment (in degree)
  double pitchRollYawIncrement()const;

  /// Return if animated spin is enabled
  bool spinEnabled() const;

  /// Return spin increment (in degrees)
  /// \sa setSpinIncrement
  double spinIncrement()const;

  /// Amount of waiting time between spin or rock increment
  /// \sa setAnimationIntervalMs
  int animationIntervalMs()const;

  /// Return if animated rock is enabled
  bool rockEnabled() const;

  /// Return length of the rock animation
  int rockLength() const;

  /// Return current increment value of animated rock.
  /// Rocking will be centered around the position where increment is set to 0.
  int rockIncrement() const;

  RotateDirection pitchDirection()const;
  void setPitchDirection(RotateDirection newPitchDirection);

  RotateDirection rollDirection()const;
  void setRollDirection(RotateDirection newRollDirection);

  RotateDirection yawDirection()const;
  void setYawDirection(RotateDirection newYawDirection);

  RotateDirection spinDirection()const;
  void setSpinDirection(RotateDirection newSpinDirection);

  /// Return zoom factor
  double zoomFactor()const;

private:
  Q_DECLARE_PRIVATE(ctkVTKRenderView);
  Q_DISABLE_COPY(ctkVTKRenderView);
};

#endif
