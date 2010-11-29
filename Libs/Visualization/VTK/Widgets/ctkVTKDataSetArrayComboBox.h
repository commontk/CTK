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

#ifndef __ctkVTKDataSetArrayComboBox_h
#define __ctkVTKDataSetArrayComboBox_h

// Qt includes
#include <QComboBox>

// CTK includes
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKDataSetArrayComboBoxPrivate;

class vtkDataArray;
class vtkDataSet;

///
/// QComboBox linked to vtkDataSet field arrays
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKDataSetArrayComboBox : public QComboBox
{
  Q_OBJECT
  
public:
  /// Superclass typedef
  typedef QComboBox Superclass;
  
  /// Constructors
  explicit ctkVTKDataSetArrayComboBox(QWidget* parent = 0);
  virtual ~ctkVTKDataSetArrayComboBox();
  
  vtkDataArray* currentArray()const;
  QString currentArrayName()const;
  vtkDataSet* dataSet()const;

public slots:
  void setDataSet(vtkDataSet* dataSet);
  /// The array must exist in the dataset
  void setCurrentArray(vtkDataArray* dataArray);
  /// the array must exist in the dataset
  void setCurrentArray(const QString& name);

signals:
  void currentArrayChanged(vtkDataArray*);
  void currentArrayChanged(const QString& name);
protected slots:
  void onCurrentIndexChanged(int);
protected:
  QScopedPointer<ctkVTKDataSetArrayComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKDataSetArrayComboBox);
  Q_DISABLE_COPY(ctkVTKDataSetArrayComboBox);
};

#endif
