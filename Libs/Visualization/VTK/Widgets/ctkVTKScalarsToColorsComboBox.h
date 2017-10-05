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

#ifndef __ctkVTKScalarsToColorsComboBox_h
#define __ctkVTKScalarsToColorsComboBox_h

// CTK includes
#include "ctkComboBox.h"
#include "ctkVisualizationVTKWidgetsExport.h"

// VTK includes
#include <vtkSmartPointer.h>

class ctkVTKScalarsToColorsComboBoxPrivate;
class vtkScalarsToColors;

/// \ingroup Visualization_VTK_Widgets
///
/// QComboBox linked to vtkScalarsToColors objects
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarsToColorsComboBox
  : public ctkComboBox
{
  Q_OBJECT
  /// Superclass typedef
  typedef ctkComboBox Superclass;

  /// This property controls the current scalarsToColors item of the combobox.
  /// \accessors currentScalarsToColors(), setCurrentScalarsToColors()
  Q_PROPERTY(vtkScalarsToColors* currentScalarsToColors
    READ currentScalarsToColors WRITE setCurrentScalarsToColors
    NOTIFY currentScalarsToColorsChanged);
public:

  /// Constructors
  explicit ctkVTKScalarsToColorsComboBox(QWidget* parent = 0);
  virtual ~ctkVTKScalarsToColorsComboBox();

  /// Add scalars to colors function (of any type) to the combobox.
  /// Increment reference count of given function (if any)
  /// Returns the index of the added function.
  int addScalarsToColors(vtkScalarsToColors* function, const QString& text =
    QString());
  vtkScalarsToColors* getScalarsToColors(int index) const;

  /// Searches the combobox for the given scalarsToColors
  /// \sa findText()
  int findScalarsToColors(vtkScalarsToColors* scalarsToColors) const;

  void removeScalarsToColors(vtkScalarsToColors* scalarsToColors);

  /// Returns the currentScalarsToColors property value
  /// 0 if no item is selected
  vtkScalarsToColors* currentScalarsToColors() const;

public slots:
  ///
  void setCurrentScalarsToColors(vtkScalarsToColors* scalarsToColors);

signals:
  /// Signal triggered when the current scalars to colors function changes.
  void currentScalarsToColorsChanged(vtkScalarsToColors*);

protected slots:
  void onCurrentIndexChanged(int);
  void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);

protected:
  QScopedPointer<ctkVTKScalarsToColorsComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKScalarsToColorsComboBox);
  Q_DISABLE_COPY(ctkVTKScalarsToColorsComboBox);
};

#endif
