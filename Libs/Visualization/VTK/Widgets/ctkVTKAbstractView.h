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

#ifndef __ctkVTKAbstractView_h
#define __ctkVTKAbstractView_h

// Qt includes
#include <QWidget>

// VTK includes
#include <QVTKWidget.h>

// CTK includes
#include "ctkVTKObject.h"
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKAbstractViewPrivate;

class vtkCornerAnnotation;
class vtkInteractorObserver;
class vtkRenderWindowInteractor;
class vtkRenderWindow;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKAbstractView : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(QString cornerAnnotationText READ cornerAnnotationText WRITE setCornerAnnotationText)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(QColor backgroundColor2 READ backgroundColor2 WRITE setBackgroundColor)
  Q_PROPERTY(bool gradientBackground READ gradientBackground WRITE setGradientBackground)
  Q_PROPERTY(bool renderEnabled READ renderEnabled WRITE setRenderEnabled)
  /// This property controls whether a corner annotation is visible with the
  /// last frames per second (FPS) value.
  /// false by default.
  Q_PROPERTY(bool fpsVisible READ isFPSVisible WRITE setFPSVisible)
  /// This property controls whether the render window uses depth peeling or
  /// not.
  /// false by default.
  Q_PROPERTY(bool useDepthPeeling READ useDepthPeeling WRITE setUseDepthPeeling)
public:

  typedef QWidget Superclass;
  explicit ctkVTKAbstractView(QWidget* parent = 0);
  virtual ~ctkVTKAbstractView();

public Q_SLOTS:
  /// Notify QVTKWidget that the view needs to be rendered.
  /// scheduleRender() respects the desired framerate of the render window,
  /// it won't render the window more than what the current render window
  /// framerate is.
  virtual void scheduleRender();

  /// Force a render even if a render is already ocurring
  /// Be careful when calling forceRender() as it can slow down your
  /// application. It is preferable to use scheduleRender() instead.
  virtual void forceRender();

  /// Set the background color of the rendering screen.
  virtual void setBackgroundColor(const QColor& newBackgroundColor);

  /// Set the second background color of the rendering screen for gradient
  /// backgrounds.
  virtual void setBackgroundColor2(const QColor& newBackgroundColor);

  /// Set/Get whether this view should have a gradient background using the
  /// Background (top) and Background2 (bottom) colors. Default is off.
  virtual void setGradientBackground(bool enable);

  /// Enable/Disable rendering
  void setRenderEnabled(bool value);

  /// Set corner annotation \a text
  virtual void setCornerAnnotationText(const QString& text);

  /// Show/Hide the FPS annotation
  void setFPSVisible(bool show);

  /// Set the useDepthPeeling property value.
  /// \sa useDepthPeeling
  void setUseDepthPeeling(bool use);

public:
  /// Get underlying RenderWindow
  Q_INVOKABLE vtkRenderWindow* renderWindow()const;

  /// Set/Get window interactor
  Q_INVOKABLE vtkRenderWindowInteractor* interactor()const;
  /// QVTKWidget catches all render requests, and ensure the desired framerate
  /// is respected.
  /// The interactor never calls Render() on the render window.
  /// TBD: can we only set a QVTKRenderWindowInteractor ?
  virtual void setInteractor(vtkRenderWindowInteractor* interactor);

  /// Get current interactor style
  Q_INVOKABLE vtkInteractorObserver* interactorStyle()const;

  /// Get corner annotation \a text
  QString cornerAnnotationText() const;
  Q_INVOKABLE vtkCornerAnnotation* cornerAnnotation()const;

  /// Get the underlying QVTKWidget
  Q_INVOKABLE QVTKWidget * VTKWidget() const;

  /// Get background color
  virtual QColor backgroundColor() const;

  /// Get the second background color
  virtual QColor backgroundColor2() const;

  /// Is the background a gradient
  virtual bool gradientBackground() const;

  /// Return if rendering is enabled
  bool renderEnabled() const;

  /// Return true if the FPS annotation is visible, false otherwise.
  bool isFPSVisible() const;

  /// Return the current FPS
  double fps()const;

  /// Return the useDepthPeeling property value.
  /// \sa useDepthPeeling
  bool useDepthPeeling()const;

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;
  virtual bool hasHeightForWidth()const;
  virtual int heightForWidth(int width)const;

protected Q_SLOTS:
  void onRender();
  void updateFPS();

protected:
  QScopedPointer<ctkVTKAbstractViewPrivate> d_ptr;
  ctkVTKAbstractView(ctkVTKAbstractViewPrivate* pimpl, QWidget* parent);

private:
  Q_DECLARE_PRIVATE(ctkVTKAbstractView);
  Q_DISABLE_COPY(ctkVTKAbstractView);
};

#endif
