/*=============================================================================

  Library: CTK

  Copyright (c) Brigham and Women's Hospital (BWH) 
  Copyright (c) University of Sheffield

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#ifndef __ctkDICOMObjectModel_h
#define __ctkDICOMObjectModel_h

// STD includes
#include <string>

// Qt includes
#include <QMetaType>
#include <QStandardItemModel>
#include <QString>

#include "ctkDICOMCoreExport.h"

class ctkDICOMObjectModelPrivate;
/// \ingroup DICOM_Core
///
/// \brief Provides a Qt MVC-compatible wrapper around a ctkDICOMItem.
///
class CTK_DICOM_CORE_EXPORT ctkDICOMObjectModel
  : public QStandardItemModel
{
  Q_OBJECT
  typedef QStandardItemModel Superclass;
  //Q_PROPERTY(setFile);

public:

  explicit ctkDICOMObjectModel(QObject* parent = 0);
  ctkDICOMObjectModel(const ctkDICOMObjectModel& other);
  virtual ~ctkDICOMObjectModel();
  Q_INVOKABLE void setFile (const QString& fileName);

protected:
  QScopedPointer<ctkDICOMObjectModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMObjectModel);
};

#endif // ctkDICOMObjectModel_h
