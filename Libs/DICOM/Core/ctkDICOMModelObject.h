/*=============================================================================

  Library: CTK

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

#ifndef __ctkDICOMModelObject_h
#define __ctkDICOMModelObject_h

// Qt includes
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QMetaType>

#include "ctkDICOMCoreExport.h"

#include <string>

//class ctkDICOMModelObjectData;
class ctkDICOMModelObjectPrivate;
/// \ingroup DICOM_Core
///
/// \brief .
///
class CTK_DICOM_CORE_EXPORT ctkDICOMModelObject
//class  ctkDICOMModelObject
	: public QStandardItemModel
{

  typedef QStandardItemModel Superclass;

public:

  explicit ctkDICOMModelObject(QObject* parent = 0);
  ctkDICOMModelObject(const ctkDICOMModelObject& other);
  virtual ~ctkDICOMModelObject();
  void setFile (const QString& fileName);

protected:
  QScopedPointer<ctkDICOMModelObjectPrivate> d_ptr;

private:
Q_DECLARE_PRIVATE(ctkDICOMModelObject);
};


#endif // ctkDICOMModelObject_h
