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

#ifndef __ctkVTKOpenGLNativeWidget_h
#define __ctkVTKOpenGLNativeWidget_h

#include "ctkVisualizationVTKWidgetsExport.h"

#if CTK_USE_QVTKOPENGLWIDGET
# if CTK_HAS_QVTKOPENGLNATIVEWIDGET_H
#  include <QVTKOpenGLNativeWidget.h>
# else
#  include <QVTKOpenGLWidget.h>
# endif
#else
# include <QVTKWidget.h>
#endif

/// \ingroup Visualization_VTK_Widgets
#if CTK_USE_QVTKOPENGLWIDGET
# if CTK_HAS_QVTKOPENGLNATIVEWIDGET_H
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKOpenGLNativeWidget : public QVTKOpenGLNativeWidget
{
  Q_OBJECT
public:
  typedef QVTKOpenGLNativeWidget Superclass;
  explicit ctkVTKOpenGLNativeWidget(QWidget* parent = 0) : Superclass(parent){}
  virtual ~ctkVTKOpenGLNativeWidget(){}
private:
  Q_DISABLE_COPY(ctkVTKOpenGLNativeWidget);
};
# else
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKOpenGLNativeWidget : public QVTKOpenGLWidget
{
  Q_OBJECT
public:
  typedef QVTKOpenGLWidget Superclass;
  explicit ctkVTKOpenGLNativeWidget(QWidget* parent = 0) : Superclass(parent){}
  virtual ~ctkVTKOpenGLNativeWidget(){}
private:
  Q_DISABLE_COPY(ctkVTKOpenGLNativeWidget);
};
# endif
#endif

#endif
