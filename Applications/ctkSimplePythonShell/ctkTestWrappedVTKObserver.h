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

#ifndef __ctkTestWrappedVTKObserver_h
#define __ctkTestWrappedVTKObserver_h

// Qt includes
#include <QObject>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkTable.h>

class ctkTestWrappedVTKObserver : public QObject
{
  Q_OBJECT
public:

  ctkTestWrappedVTKObserver(QObject * newParent = 0) : QObject(newParent)
    {
    this->MyTable = vtkSmartPointer<vtkTable>::New();
    }

  virtual ~ctkTestWrappedVTKObserver()
    {
    }

public Q_SLOTS:

  /// Example of slot returning a VTK object
  vtkTable* getTable() const
    {
    return this->MyTable;
    }

private:
  vtkSmartPointer<vtkTable> MyTable;
};

#endif
