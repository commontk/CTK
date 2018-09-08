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

// CTK includes
#include "ctkVTKOpenGLNativeWidget.h"
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
  /// Set a maximum rate (in frames per second) for rendering.
  Q_PROPERTY(double maximumUpdateRate READ maximumUpdateRate WRITE setMaximumUpdateRate)

public:

  typedef QWidget Superclass;
  explicit ctkVTKAbstractView(QWidget* parent = 0);
  virtual ~ctkVTKAbstractView();

public Q_SLOTS:
  /// Notify QVTKWidget that the view needs to be rendered.
  /// scheduleRender() respects the maximum update rate of the view,
  /// it won't render the window more frequently than what the maximum
  /// update rate is.
  /// \sa setMaximumUpdateRate
  virtual void scheduleRender();

  /// Force a render even if a render is already ocurring
  /// Be careful when calling forceRender() as it can slow down your
  /// application. It is preferable to use scheduleRender() instead.
  /// \sa scheduleRender
  virtual void forceRender();

  /// Calls pauseRender() if pause is true or resumeRender() if pause is false
  /// When pause render count is greater than 0, prevents requestRender() from calling forceRender()
  /// Callers are responsible for calling both setPauseRender(true) and setPauseRender(false)
  /// Ex.
  /// \code{.cpp}
  ///  view->pauseRender() // Or setPauseRender(true)
  /// // Perform operations that may call view->scheduleRender().
  /// view->resumeRender(); // Or setPauseRender(false)
  /// \endcode
  ///
  /// If the pause render count reaches zero when calling resumeRender(), scheduleRender() will be
  /// called if a scheduleRender() was invoked while rendering was paused.
  /// Rendering can still be triggered while the paused with forceRender()
  /// 
  /// This behaviour is different from renderEnabled(), which will prevent all rendering calls
  /// from both scheduleRender() and forceRender(), and will not invoke either when re-enabled.
  /// \sa renderEnabled
  virtual int setRenderPaused(bool pause);

  /// Increments the pause render count
  /// \sa setPauseRender
  virtual int pauseRender();

  /// De-increments the pause render count and calls scheduleRender() if one is currently pending
  /// \sa setPauseRender  
  virtual int resumeRender();

  /// Returns true if the current pause render count is greater than 0
  /// \sa setPauseRender
  virtual bool isRenderPaused()const;

  /// Set maximum rate for rendering (in frames per second).
  /// If rendering is requested more frequently than this rate using scheduleRender,
  /// actual rendering will happen at this rate.
  /// This mechanism prevents repeated rendering caused by cluster of rendering requests.
  ///
  /// If maximum update rate is set to 0 then it indicates that rendering is done next time
  /// the application is idle, i.e., pending timer events are processed. This option should be used
  /// with caution, as policy of timer event processing may differ between operating systems.
  /// Specifically, on macOS, timer events may be indefinitely delayed if user interface continuously
  /// generates events.
  ///
  /// RenderWindow's DesiredUpdateRate property is intended for determining rendering quality settings,
  /// and is not suitable to be used as maximum update rate. The main reason is that VTK usually makes the
  /// rendering much faster and lower quality than DesiredUpdateRate would dictate, and so it would
  /// unnecessarily decrease the actual refresh rate.
  ///
  /// By default maximum update rate is set to 60FPS, which allows smooth updates, while effectively
  /// suppressing repeated update requests (after a rendering has been completed,
  /// repeated rendering requests will be ignored for 17 milliseconds).
  ///
  /// \sa scheduleRender
  void setMaximumUpdateRate(double fps);

  /// Set the background color of the rendering screen.
  virtual void setBackgroundColor(const QColor& newBackgroundColor);

  /// Set the second background color of the rendering screen for gradient
  /// backgrounds.
  virtual void setBackgroundColor2(const QColor& newBackgroundColor);

  /// Set whether this view should have a gradient background using the
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
  Q_INVOKABLE ctkVTKOpenGLNativeWidget * VTKWidget() const;

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

  /// Returns maximum rate for rendering (in frames per second).
  /// \\sa setMaximumUpdateRate
  double maximumUpdateRate()const;

  /// Returns true if depth peeling is enabled.
  /// \sa setUseDepthPeeling
  bool useDepthPeeling()const;

  /// Set the default number of multisamples to use. Note that a negative
  /// value means "auto", which means the renderer will attempt to select
  /// the maximum number (but is not guaranteed to work).
  ///
  /// WARNING: Multisampling should be set *before* creation of the
  /// OpenGL context (e.g., initializing the rendering window) in order
  /// to have an effect. Consider using setMultisamples before
  /// instantiating ctkVTKAbstractView objects.
  /// \sa multiSamples
  static void setMultiSamples(int);

  /// Return the current multisamples default
  /// \sa setMultiSamples()
  static int multiSamples();

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;
  virtual bool hasHeightForWidth()const;
  virtual int heightForWidth(int width)const;

protected Q_SLOTS:
  void onRender();
  void updateFPS();

  /// Calls forceRender if the rendering has not been paused from pauseRender()
  /// \sa pauseRender
  virtual void requestRender();

protected:
  QScopedPointer<ctkVTKAbstractViewPrivate> d_ptr;
  ctkVTKAbstractView(ctkVTKAbstractViewPrivate* pimpl, QWidget* parent);

private:
  Q_DECLARE_PRIVATE(ctkVTKAbstractView);
  Q_DISABLE_COPY(ctkVTKAbstractView);
};

#endif
