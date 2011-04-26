/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

#ifndef __ctkVTKMagnifyWidget_h
#define __ctkVTKMagnifyWidget_h

// QT includes
#include <QList>

// CTK includes
#include "ctkCursorPixmapWidget.h"
#include "ctkVisualizationVTKWidgetsExport.h"

// VTK includes
class QVTKWidget;

class ctkVTKMagnifyWidgetPrivate;

/// Gives a magnified view of a QVTKWidget around the mouse position, with
/// overlaid cursor (ex. cross-hair).  You must specify the QVTKWidget(s) to be
/// observed.
/// \sa ctkCursorPixmapWidget

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKMagnifyWidget
  : public ctkCursorPixmapWidget
{
  Q_OBJECT
  Q_PROPERTY(double magnification READ magnification WRITE setMagnification);

public:
  /// Constructors
  typedef ctkCursorPixmapWidget Superclass;
  explicit ctkVTKMagnifyWidget(QWidget* parent = 0);
  virtual ~ctkVTKMagnifyWidget();

  /// Set/get the magnification (zoom).  Looks best when the magnification and
  /// the widget size are both either even or odd.  Default 1.0.
  double magnification() const;
  void setMagnification(double newMagnification);

  /// Add a QVTKWidget to observe mouse events on.  You can call this function
  /// multiple times to observe multiple QVTKWidgets.
  /// \sa observe
  void observe(QVTKWidget * widget);

  /// Add multiple QVTKWidgets at once to observe mouse events on.  You can
  /// call this function multiple times to observe multiple QVTKWidgets.
  /// \sa observe
  void observe(QList<QVTKWidget *> widgets);

  /// Remove a QVTKWidget to observe mouse events on.  You can call this
  /// function multiple times to remove multiple QVTKWidgets.
  /// \sa remove
  void remove(QVTKWidget * widget);

  /// Remove multiple QVTKWidgets at once to observe mouse events on.  You can
  /// call this function multiple times to remove multiple QVTKWidgets.
  /// \sa unobserve
  void remove(QList<QVTKWidget *> widgets);

  /// Returns whether a QVTKWidget is observed
  bool isObserved(QVTKWidget * widget) const;

  /// Returns the number of observed QVTKWidgets
  int numberObserved()const;

protected:
  QScopedPointer<ctkVTKMagnifyWidgetPrivate> d_ptr;

  /// Handles mouse events on the observed QVTKWidgets (specifically,
  /// enterEvent, leaveEvent and mouseMoveEvent).
  virtual bool eventFilter(QObject *obj, QEvent *event);

signals:
  void enteredObservedWidget(QVTKWidget * widget);
  void leftObservedWidget(QVTKWidget * widget);

private:
  Q_DECLARE_PRIVATE(ctkVTKMagnifyWidget);
  Q_DISABLE_COPY(ctkVTKMagnifyWidget);
}; 

#endif
