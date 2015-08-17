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

#ifndef __ctkVTKDataSetArrayComboBox_h
#define __ctkVTKDataSetArrayComboBox_h

// Qt includes
#include <QComboBox>

// CTK includes
#include "ctkVisualizationVTKWidgetsExport.h"
#include "ctkVTKDataSetModel.h"
class ctkVTKDataSetArrayComboBoxPrivate;

class vtkAbstractArray;
class vtkDataSet;

/// \ingroup Visualization_VTK_Widgets
///
/// QComboBox linked to vtkDataSet field arrays
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKDataSetArrayComboBox
  : public QComboBox
{
  Q_OBJECT
  Q_PROPERTY(ctkVTKDataSetModel::AttributeTypes attributeTypes READ attributeTypes WRITE setAttributeTypes)
  Q_PROPERTY(bool noneEnabled READ noneEnabled WRITE setNoneEnabled)

public:
  /// Superclass typedef
  typedef QComboBox Superclass;

  /// Constructors
  explicit ctkVTKDataSetArrayComboBox(QWidget* parent = 0);
  virtual ~ctkVTKDataSetArrayComboBox();

  vtkAbstractArray* currentArray()const;
  QString currentArrayName()const;
  vtkDataSet* dataSet()const;

  /// Return the current array location, i.e. whether it's a point data
  /// array or a cell data array.
  /// \sa currentArrayChanged(int), ctkVTKDataSetModel::locationFromItem()
  int currentArrayLocation()const;

  ctkVTKDataSetModel::AttributeTypes attributeTypes()const;
  void setAttributeTypes(const ctkVTKDataSetModel::AttributeTypes& attributeTypes);

  /// Set/Get NoneEnabled flags
  /// An additional empty item is added into the list, where the user can select.
  /// It is recommended to enable this if the combobox is used to select active scalar of the
  /// observed VTK data set, because if there is no None option is available then the combobox selects
  /// the first array automatically if an array becomes available, causing unintended change of the VTK data set
  /// (and often infinite loop of widget/MRML node updates).
  void setNoneEnabled(bool enable);
  bool noneEnabled()const; 

  /// Return a pointer to the model used to populate the combobox.
  /// \sa dataSet()
  ctkVTKDataSetModel* dataSetModel()const;

public Q_SLOTS:
  /// Set the dataset to observe. It is used to as the "model" for the combobox.
  /// \sa dataSet(), dataSetModel()
  void setDataSet(vtkDataSet* dataSet);
  /// The array must exist in the dataset
  void setCurrentArray(vtkAbstractArray* dataArray);
  /// the array must exist in the dataset
  void setCurrentArray(const QString& name);

Q_SIGNALS:
  void currentArrayChanged(vtkAbstractArray*);
  void currentArrayChanged(const QString& name);

protected Q_SLOTS:
  void onCurrentIndexChanged(int);
protected:
  QScopedPointer<ctkVTKDataSetArrayComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKDataSetArrayComboBox);
  Q_DISABLE_COPY(ctkVTKDataSetArrayComboBox);
};

#endif
