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


#ifndef CTKDICOMAPPHOSTINGTYPESHELPER_H
#define CTKDICOMAPPHOSTINGTYPESHELPER_H

// Qt includes
#include <QtSoapStruct>
#include <QtSoapQName>
#include <QRect>

// CTK includes
#include <ctkDicomAppHostingTypes.h>

#include <org_commontk_dah_core_Export.h>

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapRectangle : public QtSoapStruct
{
  ctkDicomSoapRectangle(const QString& name,const QRect& rect);

  static QRect getQRect(const QtSoapType& type);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapState : public QtSoapSimpleType
{
  ctkDicomSoapState(const QString& name, ctkDicomAppHosting::State s);

  static ctkDicomAppHosting::State getState(const QtSoapType& type);
  static ctkDicomAppHosting::State fromString(const QString& string);
  static QString toStringValue(ctkDicomAppHosting::State state);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapStatus : public QtSoapStruct
{
  ctkDicomSoapStatus(const QString& name,
                     const ctkDicomAppHosting::Status& s);

  static ctkDicomAppHosting::Status getStatus(const QtSoapType& type);
};

//----------------------------------------------------------------------------
//struct org_commontk_dah_core_EXPORT ctkDicomSoapUID : public QtSoapSimpleType
struct org_commontk_dah_core_EXPORT ctkDicomSoapUID : public QtSoapStruct
{
  ctkDicomSoapUID(const QString& name, const QString& uid);

  //static QString getUID(const QtSoapType& type);
  static QString getUID(const QtSoapType& array);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapArrayOfUIDS : public QtSoapStruct
{
  ctkDicomSoapArrayOfUIDS(const QString& name, const QList<QString>& array);

  static QList<QString> getArray(const QtSoapType& array);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapBool : public QtSoapSimpleType
{
  ctkDicomSoapBool(const QString& name, bool boolean);

  static bool getBool(const QtSoapType& type);
};

//Not easy to template, will see later for other types
//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapArrayOfStringType : public QtSoapStruct
{
  ctkDicomSoapArrayOfStringType(const QString& typeName,
                                const QString& name, const QStringList& array);

  static QStringList getArray(const QtSoapType& array);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapUUID : public QtSoapStruct
{
  ctkDicomSoapUUID(const QString& name, const QUuid& uuid);

  static QUuid getUuid(const QtSoapType& array);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapArrayOfUUIDS : public QtSoapStruct
{
  ctkDicomSoapArrayOfUUIDS(const QString& name, const QList<QUuid>& array);

  static QList<QUuid> getArray(const QtSoapType& array);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapObjectDescriptor : public QtSoapStruct
{
  ctkDicomSoapObjectDescriptor(const QString& name,
                               const ctkDicomAppHosting::ObjectDescriptor& od);

  static ctkDicomAppHosting::ObjectDescriptor getObjectDescriptor(const QtSoapType& type);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapArrayOfObjectDescriptors : public QtSoapStruct
{
  ctkDicomSoapArrayOfObjectDescriptors(const QString& name,
                               const ctkDicomAppHosting::ArrayOfObjectDescriptors& ods);

  static ctkDicomAppHosting::ArrayOfObjectDescriptors getArrayOfObjectDescriptors(const QtSoapType& type);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapSeries : public QtSoapStruct
{
  ctkDicomSoapSeries (const QString& name,
                      const ctkDicomAppHosting::Series& s);

  static ctkDicomAppHosting::Series getSeries(const QtSoapType& type);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapStudy : public QtSoapStruct
{
  ctkDicomSoapStudy(const QString& name,
                    const ctkDicomAppHosting::Study& s);

  static ctkDicomAppHosting::Study getStudy(const QtSoapType& type);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapPatient : public QtSoapStruct
{
  ctkDicomSoapPatient(const QString& name,
                        const ctkDicomAppHosting::Patient& p);

  static ctkDicomAppHosting::Patient getPatient(const QtSoapType& type);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapAvailableData : public QtSoapStruct
{
  ctkDicomSoapAvailableData(const QString& name,
                            const ctkDicomAppHosting::AvailableData& ad);

  static ctkDicomAppHosting::AvailableData getAvailableData (const QtSoapType& type);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapObjectLocator : public QtSoapStruct
{
  ctkDicomSoapObjectLocator(const QString& name,
                            const ctkDicomAppHosting::ObjectLocator& ol);

  static ctkDicomAppHosting::ObjectLocator getObjectLocator(const QtSoapType& type);
};

//----------------------------------------------------------------------------
struct org_commontk_dah_core_EXPORT ctkDicomSoapArrayOfObjectLocators : public QtSoapArray
{
  ctkDicomSoapArrayOfObjectLocators(const QString& name, const QList<ctkDicomAppHosting::ObjectLocator>& array);

  static QList<ctkDicomAppHosting::ObjectLocator> getArray(const QtSoapType& array);
};

#endif // CTKDICOMAPPHOSTINGTYPESHELPER_H
