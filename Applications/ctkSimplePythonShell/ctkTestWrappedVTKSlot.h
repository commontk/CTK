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

#ifndef __ctkTestWrappedVTKSlot_h
#define __ctkTestWrappedVTKSlot_h

// Qt includes
#include <QObject>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkTable.h>

class ctkTestWrappedVTKSlot : public QObject
{
  Q_OBJECT
public:

  ctkTestWrappedVTKSlot(QObject * newParent = 0) : QObject(newParent)
    {
    this->MyTable = vtkSmartPointer<vtkTable>::New();
    }

  virtual ~ctkTestWrappedVTKSlot()
    {
    }

public Q_SLOTS:

  /// Example of slot returning a VTK object
  vtkTable* getTable() const
    {
    return this->MyTable;
    }

  /// Example ot slot accepting a VTK object as parameter
  void setTable(vtkTable * newTable)
    {
    this->MyTable = newTable;
    }

private:
  vtkSmartPointer<vtkTable> MyTable;
};

#endif
