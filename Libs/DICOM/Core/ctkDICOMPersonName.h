/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

#ifndef __ctkDICOMPersonName_h
#define __ctkDICOMPersonName_h

#include "ctkDICOMCoreExport.h"

#include <QString>
#include <QSharedDataPointer>
#include <QMetaType>

#include <string>

class ctkDICOMPersonNameData;

/// \ingroup DICOM_Core
///
/// \brief A person's name as modelled in DICOM.
///
class CTK_DICOM_CORE_EXPORT ctkDICOMPersonName
{
public:

  ctkDICOMPersonName(const QString& lastName = QString(),
              const QString& firstName = QString(),
              const QString& middleName = QString(),
              const QString& namePrefix = QString(),
              const QString& nameSuffix = QString());

  ctkDICOMPersonName(const ctkDICOMPersonName& other);
  ctkDICOMPersonName& operator=(const ctkDICOMPersonName& other);

  virtual ~ctkDICOMPersonName();
  ///
  /// \brief "Lastname, FirstName MiddleName, Suffix" (useful for alphabetical sorting)
  ///
  QString formattedName() const;

  QString lastName() const;
  QString firstName() const;
  QString middleName() const;
  QString namePrefix() const;
  QString nameSuffix() const;

  /// cast operator
  operator QString() const;
  std::string toStdString() const;

private:
  QSharedDataPointer<ctkDICOMPersonNameData> d;
};

typedef QList<ctkDICOMPersonName> ctkDICOMPersonNameList;
Q_DECLARE_METATYPE(ctkDICOMPersonName);


#endif // ctkDICOMPersonName_h
