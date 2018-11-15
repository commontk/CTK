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

#ifndef __ctkVTKMagnifyView_h
#define __ctkVTKMagnifyView_h

// QT includes
#include <QList>

// CTK includes
#include "ctkCrosshairLabel.h"
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKMagnifyViewPrivate;
class ctkVTKOpenGLNativeWidget;

/// \ingroup Visualization_VTK_Widgets
/// Gives a magnified view of a QVTKWidget around the mouse position, with
/// overlaid crosshair (ex. cross-hair).  You must specify the QVTKWidget(s) to be
/// observed.
/// \sa ctkCrosshairLabel

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKMagnifyView
  : public ctkCrosshairLabel
{
  Q_OBJECT
  Q_PROPERTY(double magnification READ magnification WRITE setMagnification)
  Q_PROPERTY(bool observeRenderWindowEvents
             READ observeRenderWindowEvents WRITE setObserveRenderWindowEvents)
  Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)

public:
  /// Constructors
  typedef ctkCrosshairLabel Superclass;
  explicit ctkVTKMagnifyView(QWidget* parent = 0);
  virtual ~ctkVTKMagnifyView();

  /// Set/get the magnification (zoom).  Looks best when the magnification and
  /// the widget size are both either even or odd.  Default 1.0.
  double magnification() const;
  void setMagnification(double newMagnification);

  /// Set/get whether or not to observe EndEvents emitted by the observed
  /// QVTKWidgets' vtkRenderWindows after they have rendered.  This triggers
  /// updates to the magnify widget whenever the vtkRenderWindow does a render,
  /// even if the mouse position does not move. Default true.
  bool observeRenderWindowEvents() const;
  void setObserveRenderWindowEvents(bool newObserve);

  /// Set/get a fixed interval, in milliseconds, at which this widget will update
  /// itself.  Default 20.  Specify an update interval of 0 to handle all events as
  /// they occur.
  int updateInterval() const;
  void setUpdateInterval(int newInterval);

  /// Add a QVTKWidget to observe mouse events on.  You can call this function
  /// multiple times to observe multiple QVTKWidgets.
  /// \sa observe
  void observe(ctkVTKOpenGLNativeWidget * widget);

  /// Add multiple QVTKWidgets at once to observe mouse events on.  You can
  /// call this function multiple times to observe multiple QVTKWidgets.
  /// \sa observe
  void observe(QList<ctkVTKOpenGLNativeWidget *> widgets);

  /// Remove a QVTKWidget to observe mouse events on.  You can call this
  /// function multiple times to remove multiple QVTKWidgets.
  /// \sa remove
  void remove(ctkVTKOpenGLNativeWidget * widget);

  /// Remove multiple QVTKWidgets at once to observe mouse events on.  You can
  /// call this function multiple times to remove multiple QVTKWidgets.
  /// \sa unobserve
  void remove(QList<ctkVTKOpenGLNativeWidget *> widgets);

  /// Returns whether a QVTKWidget is observed
  bool isObserved(ctkVTKOpenGLNativeWidget * widget) const;

  /// Returns the number of observed QVTKWidgets
  int numberObserved()const;

  /// Returns true if the mouse cursor is over an observed widget,
  /// false otherwise.
  bool hasCursorInObservedWidget()const;
protected:
  QScopedPointer<ctkVTKMagnifyViewPrivate> d_ptr;

  /// Handles mouse events on the observed QVTKWidgets (specifically,
  /// enterEvent, leaveEvent and mouseMoveEvent).
  virtual bool eventFilter(QObject *obj, QEvent *event);

Q_SIGNALS:
  void enteredObservedWidget(ctkVTKOpenGLNativeWidget * widget);
  void leftObservedWidget(ctkVTKOpenGLNativeWidget * widget);

private:
  Q_DECLARE_PRIVATE(ctkVTKMagnifyView)
  Q_DISABLE_COPY(ctkVTKMagnifyView)
}; 

#endif
