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

#ifndef __ctkTestWrappedQListOfVTKObject_h
#define __ctkTestWrappedQListOfVTKObject_h

// Qt includes
#include <QObject>
#include <QList>

// VTK includes
#include <vtkTable.h>

class ctkTestWrappedQListOfVTKObject : public QObject
{
  Q_OBJECT
public:

  ctkTestWrappedQListOfVTKObject(QObject * newParent = 0) : QObject(newParent)
  {
  }

  /// Example of slot accepting a VTK object as parameter
  Q_INVOKABLE int numberOfElementInList(const QList<vtkTable*>& listOfTable)
  {
    return listOfTable.count();
  }
};

#endif
