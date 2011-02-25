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

#ifndef __ctkVTKAbstractView_h
#define __ctkVTKAbstractView_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKAbstractViewPrivate;

class vtkCornerAnnotation;
class vtkInteractorObserver;
class vtkRenderWindowInteractor;
class vtkRenderWindow;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKAbstractView : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString cornerAnnotationText READ cornerAnnotationText WRITE setCornerAnnotationText)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(bool renderEnabled READ renderEnabled WRITE setRenderEnabled)
public:

  typedef QWidget Superclass;
  explicit ctkVTKAbstractView(QWidget* parent = 0);
  virtual ~ctkVTKAbstractView();

public slots:

  /// If a render has already been scheduled, this called is a no-op
  void scheduleRender();

  /// Force a render even if a render is already ocurring
  void forceRender();

  /// Set background color
  virtual void setBackgroundColor(const QColor& newBackgroundColor) = 0;

  /// Enable/Disable rendering
  void setRenderEnabled(bool value);

  /// Set corner annotation \a text
  virtual void setCornerAnnotationText(const QString& text);

public:
  /// Get underlying RenderWindow
  vtkRenderWindow* renderWindow()const;

  /// Set/Get window interactor
  vtkRenderWindowInteractor* interactor()const;
  virtual void setInteractor(vtkRenderWindowInteractor* interactor);

  /// Get current interactor style
  vtkInteractorObserver* interactorStyle()const;

  /// Get corner annotation \a text
  QString cornerAnnotationText() const;
  vtkCornerAnnotation* cornerAnnotation()const;

  /// Get background color
  virtual QColor backgroundColor() const = 0;

  /// Return if rendering is enabled
  bool renderEnabled() const;

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;
  virtual bool hasHeightForWidth()const;
  virtual int heightForWidth(int width)const;

protected:
  QScopedPointer<ctkVTKAbstractViewPrivate> d_ptr;
  ctkVTKAbstractView(ctkVTKAbstractViewPrivate* pimpl, QWidget* parent);

private:
  Q_DECLARE_PRIVATE(ctkVTKAbstractView);
  Q_DISABLE_COPY(ctkVTKAbstractView);
};

#endif
