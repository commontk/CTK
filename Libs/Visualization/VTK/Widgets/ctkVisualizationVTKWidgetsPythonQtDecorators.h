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

#ifndef __ctkVisualizationVTKWidgetsPythonQtDecorators_h
#define __ctkVisualizationVTKWidgetsPythonQtDecorators_h

// PythonQt includes
#include <PythonQt.h>

// CTK includes
#include <ctkVTKChartView.h>
#include <ctkVTKScalarsToColorsView.h>
#include <ctkVTKWidgetsUtils.h>

class vtkImageData;

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

/// \ingroup Widgets
class ctkVisualizationVTKWidgetsPythonQtDecorators : public QObject
{
  Q_OBJECT
public:

  ctkVisualizationVTKWidgetsPythonQtDecorators()
    {
    }

public Q_SLOTS:

  // ctkVTKChartView
#ifdef CTK_USE_CHARTS
  
  QList<double> chartExtent(ctkVTKChartView* view)const
  {
    double _bounds[8];
    view->chartExtent(_bounds);
    QList<double> bounds;
    for(int idx = 0; idx < 8; ++idx)
      {
      bounds << _bounds[idx];
      }
    return bounds;
  }

  QList<double> chartBounds(ctkVTKChartView* view)const
  {
    double _bounds[8];
    view->chartBounds(_bounds);
    QList<double> bounds;
    for(int idx = 0; idx < 8; ++idx)
      {
      bounds << _bounds[idx];
      }
    return bounds;
  }
  
  void setChartUserBounds(ctkVTKChartView* view, const QList<double>& bounds)
  {
    double _bounds[8];
    for(int idx = 0; idx < bounds.length() && idx < 8; ++idx)
      {
      _bounds[idx] = bounds[idx];
      }
    view->setChartUserBounds(_bounds);
  }
  
  QList<double> chartUserBounds(ctkVTKChartView* view)const
  {
    double _bounds[8];
    view->chartUserBounds(_bounds);
    QList<double> bounds;
    for(int idx = 0; idx < 8; ++idx)
      {
      bounds << _bounds[idx];
      }
    return bounds;
  }
  
  // ctkVTKScalarsToColorsView
  
  QList<double> validBounds(ctkVTKScalarsToColorsView* view)const
  {
    double _bounds[4];
    view->validBounds(_bounds);
    QList<double> bounds;
    for(int idx = 0; idx < 4; ++idx)
      {
      bounds << _bounds[idx];
      }
    return bounds;
  }

  void setValidBounds(ctkVTKScalarsToColorsView* view, const QList<double>& bounds)
  {
    double _bounds[4];
    for(int idx = 0; idx < bounds.length() && idx < 4; ++idx)
      {
      _bounds[idx] = bounds[idx];
      }
    view->setValidBounds(_bounds);
  }
#endif

};

//-----------------------------------------------------------------------------
class PythonQtWrapper_CTKVisualizationVTKWidgets : public QObject
{
  Q_OBJECT
  QVTK_OBJECT

public Q_SLOTS:

  QImage static_ctkVTKWidgetsUtils_grabVTKWidget(QWidget* widget, QRect rectangle = QRect())
  {
    return ctk::grabVTKWidget(widget, rectangle);
  }

  QImage static_ctkVTKWidgetsUtils_vtkImageDataToQImage(vtkImageData* imageData)
  {
    return ctk::vtkImageDataToQImage(imageData);
  }


  bool static_ctkVTKWidgetsUtils_qImageToVTKImageData(const QImage& image, vtkImageData* imageData)
  {
    return ctk::qImageToVTKImageData(image, imageData);
  }

};


//-----------------------------------------------------------------------------
/// \ingroup Widgets
void initCTKVisualizationVTKWidgetsPythonQtDecorators()
{
  PythonQt::self()->addDecorators(new ctkVisualizationVTKWidgetsPythonQtDecorators);

  // PythonQt doesn't support wrapping a static function and adding it to the top-level
  // ctk module.  This exposes static functions in ctk.ctkVTKWidgetsUtils, for example
  // ctk.ctkVTKWidgetsUtils.qImageToVTKImageData.
  PythonQt::self()->registerCPPClass("ctkVTKWidgetsUtils", "", "CTKVisualizationVTKWidgets", PythonQtCreateObject<PythonQtWrapper_CTKVisualizationVTKWidgets>);
}

#endif
