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

#include "ctkDicomAppHostingTypesHelper.h"

#include <ctkException.h>

//----------------------------------------------------------------------------
void DumpAll(const QtSoapType& type, int indent=0)
{
  if(indent==0)
    {
    QString s;
    s = "Dumping: " + type.typeName() + " " + type.name().name() + " " + type.value().toString();
    qDebug() << s;
    indent = 4;
    }
  for (int i = 0; i < type.count() ; i++)
    {
    QString s;
    s = QString(indent, ' ') + type[i].typeName() + "  " + type[i].name().name()  + " Value: " + type[i].value().toString();
    qDebug() << s;
    if(type[i].count()>0)
      DumpAll(type[i], indent+4);
    }
}

//----------------------------------------------------------------------------
void DumpQtSoapType(const QtSoapType& sstruct)
{
  qDebug() << "Dumping: " << sstruct.typeName() << " " << sstruct.name().name();
  for (int i = 0; i < sstruct.count() ; i++)
    {
    qDebug() << sstruct[i].typeName() << "  " << sstruct[i].name().name();
    }
}

//----------------------------------------------------------------------------
ctkDicomSoapRectangle::ctkDicomSoapRectangle(const QString& name,const QRect& rect)
  : QtSoapStruct(QtSoapQName(name))
{
  this->insert(new QtSoapSimpleType(QtSoapQName("Height"),
                                    rect.height()));
  this->insert(new QtSoapSimpleType(QtSoapQName("Width"),
                                    rect.width()));
  this->insert(new QtSoapSimpleType(QtSoapQName("RefPointX"),
                                    rect.x()));
  this->insert(new QtSoapSimpleType(QtSoapQName("RefPointY"),
                                    rect.y()));
}

//----------------------------------------------------------------------------
QRect ctkDicomSoapRectangle::getQRect(const QtSoapType& type)
{
  return QRect (type["RefPointX"].value().toInt(),
                type["RefPointY"].value().toInt(),
                type["Width"].value().toInt(),
                type["Height"].value().toInt());
}

//----------------------------------------------------------------------------
ctkDicomSoapState::ctkDicomSoapState(const QString& name, ctkDicomAppHosting::State s )
  : QtSoapSimpleType(QtSoapQName(name), toStringValue(s))
{}

//----------------------------------------------------------------------------
ctkDicomAppHosting::State ctkDicomSoapState::getState(const QtSoapType& type)
{
  return fromString( type.value().toString() );
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::State ctkDicomSoapState::fromString(const QString& string)
{
  if (string == "IDLE") return ctkDicomAppHosting::IDLE;
  if (string == "INPROGRESS") return ctkDicomAppHosting::INPROGRESS;
  if (string == "COMPLETED") return ctkDicomAppHosting::COMPLETED;
  if (string == "SUSPENDED") return ctkDicomAppHosting::SUSPENDED;
  if (string == "CANCELED") return ctkDicomAppHosting::CANCELED;
  if (string == "EXIT") return ctkDicomAppHosting::EXIT;
  throw ctkRuntimeException(string + "Invalid STATE:");
  return ctkDicomAppHosting::EXIT;
}

//----------------------------------------------------------------------------
QString ctkDicomSoapState::toStringValue(ctkDicomAppHosting::State state)
{
  switch(state)
    {
    case ctkDicomAppHosting::IDLE:
      return "IDLE";
    case ctkDicomAppHosting::INPROGRESS:
      return "INPROGRESS";
    case ctkDicomAppHosting::COMPLETED:
      return "COMPLETED";
    case ctkDicomAppHosting::SUSPENDED:
      return "SUSPENDED";
    case ctkDicomAppHosting::CANCELED:
      return "CANCELED";
    case ctkDicomAppHosting::EXIT:
      return "EXIT";
    default:
      throw ctkRuntimeException( "Invalid value for state" );

    }
}

//----------------------------------------------------------------------------
ctkDicomSoapStatus::ctkDicomSoapStatus(const QString& name,
                                       const ctkDicomAppHosting::Status& s)
  : QtSoapStruct(QtSoapQName(name))
{
  this->insert(new QtSoapSimpleType(QtSoapQName("StatusType"),
                                    s.statusType) );
  this->insert(new QtSoapSimpleType(
                 QtSoapQName("CodingSchemeDesignator"),
                 s.codingSchemeDesignator) );
  this->insert(new QtSoapSimpleType(
                 QtSoapQName("CodeValue"),
                 s.codeValue) );
  this->insert(new QtSoapSimpleType(
                 QtSoapQName("CodeMeaning"),
                 s.codeMeaning) );
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::Status ctkDicomSoapStatus::getStatus(const QtSoapType& type)
{
  ctkDicomAppHosting::Status s;

  s.statusType = static_cast<ctkDicomAppHosting::StatusType>
      (type["StatusType"].value().toInt());
  s.codingSchemeDesignator =
      type["CodingSchemeDesignator"].value().toString();
  s.codeValue =
      type["CodeValue"].value().toString();
  s.codeMeaning =
      type["CodeMeaning"].value().toString();
  return s;
}

//----------------------------------------------------------------------------
ctkDicomSoapUID::ctkDicomSoapUID(const QString& name, const QString& uid)
  //: QtSoapSimpleType(QtSoapQName(name), uid)
  : QtSoapStruct(QtSoapQName(name))
{
  this->insert(new QtSoapSimpleType(QtSoapQName("Uid"),uid));
}


//----------------------------------------------------------------------------
QString ctkDicomSoapUID::getUID(const QtSoapType& type)
{
  //return type.value().toString();
  QString uid;
  if(type.type()==QtSoapType::Struct)
    uid = type[0].value().toString();
  else
    uid = type.value().toString();
  return uid;

}

//----------------------------------------------------------------------------
ctkDicomSoapArrayOfUIDS::ctkDicomSoapArrayOfUIDS(const QString& name, const QList<QString>& array)
//  : QtSoapArray(QtSoapQName(name), QtSoapType::String, array.size())
  : QtSoapStruct(QtSoapQName(name))
{
  for (QList<QString>::ConstIterator it = array.constBegin();
       it < array.constEnd(); it++)
    {
    this->insert(new ctkDicomSoapUID("Uid",(*it)));
    }
}

//----------------------------------------------------------------------------
QList<QString> ctkDicomSoapArrayOfUIDS::getArray(const QtSoapType& type)
{
  QList<QString> list;
  for (int i = 0; i < type.count(); i++)
    {
    list << ctkDicomSoapUID::getUID(type[i]);
    }
  return list;
}

//----------------------------------------------------------------------------
ctkDicomSoapBool::ctkDicomSoapBool(const QString& name, bool boolean)
  : QtSoapSimpleType(QtSoapQName(name), boolean, 0)
{}

//----------------------------------------------------------------------------
bool ctkDicomSoapBool::getBool(const QtSoapType& type)
{
  return  type.value().toBool();
}

//----------------------------------------------------------------------------
ctkDicomSoapArrayOfStringType::ctkDicomSoapArrayOfStringType(const QString& typeName,
                                                             const QString& name, const QStringList& array)
//  : QtSoapArray(QtSoapQName(name), QtSoapType::String, array.size())
  : QtSoapStruct(QtSoapQName(name))
{
   QtSoapStruct *simpleStruct = new QtSoapStruct(QtSoapQName(typeName));
  for (QStringList::ConstIterator it = array.constBegin();
       it < array.constEnd(); it++)
    {

    simpleStruct->insert(new QtSoapSimpleType(QtSoapQName("Uid"),*it));
//    this->append(new QtSoapSimpleType(QtSoapQName(typeName),*it));
    }
  this->insert(simpleStruct);
}

//----------------------------------------------------------------------------
QStringList ctkDicomSoapArrayOfStringType::getArray(const QtSoapType& type)
{
  QStringList list;
  if(type.type()==QtSoapType::Struct)
  {
    const QtSoapType& type(type[0]);
    for (int i = 0; i < type.count() ; i++)
    {
      const QString str = type[i].value().toString();
      list << str;
    }
  }
  else
  {
    for (int i = 0; i < type.count() ; i++)
    {
      const QString str = type[i].value().toString();
      list << str;
    }
  }
  return list;
}

//----------------------------------------------------------------------------
ctkDicomSoapUUID::ctkDicomSoapUUID(const QString& name, const QUuid& uuid)
  : QtSoapStruct(QtSoapQName(name))
{
  QString uuidstring(uuid.toString());
  uuidstring.remove(0,1).chop(1);
  this->insert(new QtSoapSimpleType(QtSoapQName("Uuid"),uuidstring));
}

//----------------------------------------------------------------------------
QUuid ctkDicomSoapUUID::getUuid(const QtSoapType& type)
{
  QUuid uuid;
  if(type.type()==QtSoapType::Struct)
    uuid = QUuid(type[0].value().toString());
  else
    uuid = QUuid(type.value().toString());
  return uuid;
}

//----------------------------------------------------------------------------
ctkDicomSoapArrayOfUUIDS::ctkDicomSoapArrayOfUUIDS(const QString& name, const QList<QUuid>& array)
//  : QtSoapArray(QtSoapQName(name), QtSoapType::String, array.size())
  : QtSoapStruct(QtSoapQName(name))
{
  for (QList<QUuid>::ConstIterator it = array.constBegin();
       it < array.constEnd(); it++)
    {
    QString uuidstring((*it).toString());
    uuidstring.remove(0,1).chop(1);
    this->insert(new ctkDicomSoapUUID("UUID",uuidstring));
    }
}

//----------------------------------------------------------------------------
QList<QUuid> ctkDicomSoapArrayOfUUIDS::getArray(const QtSoapType& type)
{
  QList<QUuid> list;
  for (int i = 0; i < type.count(); i++)
    {
    list << ctkDicomSoapUUID::getUuid(type[i]);
    }
  return list;
}

//----------------------------------------------------------------------------
ctkDicomSoapObjectDescriptor::ctkDicomSoapObjectDescriptor(const QString& name,
                                                           const ctkDicomAppHosting::ObjectDescriptor& od)
  : QtSoapStruct(QtSoapQName(name))
{
  this->insert(new ctkDicomSoapUUID("DescriptorUuid",
                                    od.descriptorUUID) );

  /*this->insert(new QtSoapSimpleType(
                 QtSoapQName("MimeType"),
                 od.mimeType) );*/

  /*QStringList mimeType;
  mimeType.append(od.mimeType);
  this->insert(new ctkDicomSoapArrayOfStringType("MimeType",
        "MimeType",
        mimeType) );*/

  QtSoapStruct *stMimeType = new QtSoapStruct(QtSoapQName("MimeType"));
  stMimeType->insert(new QtSoapSimpleType(QtSoapQName("Type"),od.mimeType));
  this->insert(stMimeType);

  /*this->insert(new QtSoapSimpleType(
                 QtSoapQName("ClassUID"),
                 od.classUID) );*/

  this->insert(new ctkDicomSoapUID("ClassUID", od.classUID) );

  /*this->insert(new QtSoapSimpleType(
                 QtSoapQName("TransferSyntaxUID"),
                 od.transferSyntaxUID) );*/
  this->insert(new ctkDicomSoapUID("TransferSyntaxUID", od.transferSyntaxUID) );

  /*this->insert(new QtSoapSimpleType(
                 QtSoapQName("Modality"),
                 od.modality) );*/
  /*QStringList modality;
    modality.append(od.modality);
    this->insert(new ctkDicomSoapArrayOfStringType("Modality",
          "Modality",
          modality) );*/

  QtSoapStruct *structModality = new QtSoapStruct(QtSoapQName("Modality"));
  structModality->insert(new QtSoapSimpleType(QtSoapQName("Modality"),od.modality));
  this->insert(structModality);
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::ObjectDescriptor ctkDicomSoapObjectDescriptor::getObjectDescriptor(const QtSoapType& type)
{
  ctkDicomAppHosting::ObjectDescriptor od;
  od.descriptorUUID = ctkDicomSoapUUID::getUuid(type["DescriptorUuid"]).toString();
  od.mimeType =
      type["Type"].value().toString();
  //const QStringList mimeType = ctkDicomSoapArrayOfStringType::getArray(type["MimeType"]);
  //od.mimeType = mimeType[0];

  /*od.classUID =
      type["ClassUID"].value().toString();*/
  od.classUID =
    ctkDicomSoapUID::getUID(type["ClassUID"]);
  /*od.transferSyntaxUID =
      type["TransferSyntaxUID"].value().toString();*/
  od.transferSyntaxUID =
    ctkDicomSoapUID::getUID(type["TransferSyntaxUID"]);

  od.modality =
      type["Modality"].value().toString();
  return od;
}

//----------------------------------------------------------------------------
ctkDicomSoapArrayOfObjectDescriptors::ctkDicomSoapArrayOfObjectDescriptors(const QString& name,
                                                           const ctkDicomAppHosting::ArrayOfObjectDescriptors& ods)
  : QtSoapStruct(QtSoapQName(name))
{
  Q_UNUSED(ods);
  qCritical() << "ctkDicomSoapArrayOfObjectDescriptors not yet implemented";
  //this->insert(new QtSoapSimpleType(QtSoapQName("uuid"),
  //                                  od.descriptorUUID) );
  //this->insert(new QtSoapSimpleType(
  //               QtSoapQName("mimeType"),
  //               od.mimeType) );
  //this->insert(new QtSoapSimpleType(
  //               QtSoapQName("classUID"),
  //               od.classUID) );
  //this->insert(new QtSoapSimpleType(
  //               QtSoapQName("transferSyntaxUID"),
  //               od.transferSyntaxUID) );
  //this->insert(new QtSoapSimpleType(
  //               QtSoapQName("modality"),
  //               od.modality) );
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::ArrayOfObjectDescriptors ctkDicomSoapArrayOfObjectDescriptors::getArrayOfObjectDescriptors(const QtSoapType& type)
{
  ctkDicomAppHosting::ArrayOfObjectDescriptors list;
  for (int i = 0; i < type.count() ; i++)
  {
    const ctkDicomAppHosting::ObjectDescriptor od =
      ctkDicomSoapObjectDescriptor::getObjectDescriptor(type[i]);
    list.append(od);
  }
  return list;
}

//----------------------------------------------------------------------------
ctkDicomSoapSeries::ctkDicomSoapSeries(const QString& name,
                                       const ctkDicomAppHosting::Series& s)
  : QtSoapStruct(QtSoapQName(name))
{
  /*this->insert(new QtSoapSimpleType(QtSoapQName("SeriesUID"),
                                    s.seriesUID) );*/

  this->insert(new ctkDicomSoapUID("SeriesUID", s.seriesUID) );

  QtSoapArray* odescriptors = new QtSoapArray(QtSoapQName("ObjectDescriptors"), QtSoapType::Other,
                                              s.objectDescriptors.size());

  for (ctkDicomAppHosting::ArrayOfObjectDescriptors::ConstIterator it = s.objectDescriptors.constBegin();
       it < s.objectDescriptors.constEnd(); it++)
    {
    odescriptors->append(new ctkDicomSoapObjectDescriptor("ObjectDescriptor",*it));
    }
  this->insert(odescriptors);
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::Series ctkDicomSoapSeries::getSeries(const QtSoapType& type)
{
  ctkDicomAppHosting::Series s;

  //s.seriesUID = type["SeriesUID"].value().toString();
  s.seriesUID = ctkDicomSoapUID::getUID(type["SeriesUID"]);
  s.objectDescriptors = ctkDicomSoapArrayOfObjectDescriptors::getArrayOfObjectDescriptors(type["ObjectDescriptors"]);
  return s;
}

//----------------------------------------------------------------------------
ctkDicomSoapStudy::ctkDicomSoapStudy(const QString& name,
                                     const ctkDicomAppHosting::Study& s)
  : QtSoapStruct(QtSoapQName(name))
{
  //this->insert(new QtSoapSimpleType(QtSoapQName("StudyUID"),s.studyUID) );
  this->insert(new ctkDicomSoapUID("StudyUID", s.studyUID));

  QtSoapArray* odescriptors = new QtSoapArray(QtSoapQName("ObjectDescriptors"), QtSoapType::Other,
                                              s.objectDescriptors.size());

  for (ctkDicomAppHosting::ArrayOfObjectDescriptors::ConstIterator it = s.objectDescriptors.constBegin();
       it < s.objectDescriptors.constEnd(); it++)
    {
    odescriptors->append(new ctkDicomSoapObjectDescriptor("ObjectDescriptor", *it));
    }
  this->insert(odescriptors);

  QtSoapArray* series = new QtSoapArray(QtSoapQName("Series"), QtSoapType::Other,
                                        s.series.size());

  for (QList<ctkDicomAppHosting::Series>::ConstIterator it = s.series.constBegin();
       it < s.series.constEnd(); it++)
    {
    series->append(new ctkDicomSoapSeries("Series",*it));
    }
  this->insert(series);
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::Study ctkDicomSoapStudy::getStudy(const QtSoapType& type)
{
  ctkDicomAppHosting::Study s;

  //s.studyUID = type["StudyUID"].value().toString();
  s.studyUID = ctkDicomSoapUID::getUID(type["StudyUID"]);

  s.objectDescriptors = ctkDicomSoapArrayOfObjectDescriptors::getArrayOfObjectDescriptors(type["ObjectDescriptors"]);

  QList<ctkDicomAppHosting::Series> listSeries;
  const QtSoapType& seriesArray = type["Series"];
  for (int i = 0; i < seriesArray.count() ; i++)
    {
    const ctkDicomAppHosting::Series series =
        ctkDicomSoapSeries::getSeries(seriesArray[i]);
    listSeries.append(series);
    }
  s.series = listSeries;

  return s;
}

//----------------------------------------------------------------------------
ctkDicomSoapPatient::ctkDicomSoapPatient(const QString& name,
                                         const ctkDicomAppHosting::Patient& p)
  : QtSoapStruct(QtSoapQName(name))
{

  this->insert(new QtSoapSimpleType(QtSoapQName("Name"),
                                    p.name) );
  this->insert(new QtSoapSimpleType(QtSoapQName("ID"),
                                    p.id) );
  this->insert(new QtSoapSimpleType(QtSoapQName("AssigningAuthority"),
                                    p.assigningAuthority) );
  this->insert(new QtSoapSimpleType(QtSoapQName("Sex"),
                                    p.sex) );
  this->insert(new QtSoapSimpleType(QtSoapQName("DateOfBirth"),
                                    p.birthDate) );
  QtSoapArray* odescriptors = new QtSoapArray(QtSoapQName("ObjectDescriptors"), QtSoapType::Other,
                                              p.objectDescriptors.size());

  for (ctkDicomAppHosting::ArrayOfObjectDescriptors::ConstIterator it = p.objectDescriptors.constBegin();
       it < p.objectDescriptors.constEnd(); it++)
    {
    odescriptors->append(new ctkDicomSoapObjectDescriptor("ObjectDescriptor",*it));
    }
  this->insert(odescriptors);

  QtSoapArray* study = new QtSoapArray(QtSoapQName("Studies"), QtSoapType::Other,
                                       p.studies.size());

  for (QList<ctkDicomAppHosting::Study>::ConstIterator it = p.studies.constBegin();
       it < p.studies.constEnd(); it++)
  {
    study->append(new ctkDicomSoapStudy("Study",*it));
  }
  this->insert(study);
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::Patient ctkDicomSoapPatient::getPatient(const QtSoapType& type)
{
  ctkDicomAppHosting::Patient p;

  p.name = type["Name"].value().toString();
  p.id = type["ID"].value().toString();
  p.assigningAuthority = type["AssigningAuthority"].value().toString();
  p.sex = type["Sex"].value().toString();
  p.birthDate = type["DateOfBirth"].value().toString();

  p.objectDescriptors = ctkDicomSoapArrayOfObjectDescriptors::getArrayOfObjectDescriptors(type["ObjectDescriptors"]);

  QList<ctkDicomAppHosting::Study> listPatient;
  const QtSoapType& studiesArray = type["Studies"];
  for (int i = 0; i < studiesArray.count() ; i++)
    {
    const ctkDicomAppHosting::Study study =
        ctkDicomSoapStudy::getStudy(studiesArray[i]);
    listPatient.append(study);
    }
  p.studies = listPatient;
  return p;
}

//----------------------------------------------------------------------------
ctkDicomSoapAvailableData::ctkDicomSoapAvailableData(const QString& name,
                                                     const ctkDicomAppHosting::AvailableData& ad)
  : QtSoapStruct(QtSoapQName(name))
{
  QtSoapArray* odescriptors = new QtSoapArray(QtSoapQName("ObjectDescriptors"), QtSoapType::Other,
                                              ad.objectDescriptors.size());

  for (ctkDicomAppHosting::ArrayOfObjectDescriptors::ConstIterator it = ad.objectDescriptors.constBegin();
       it < ad.objectDescriptors.constEnd(); it++)
    {
    odescriptors->append(new ctkDicomSoapObjectDescriptor("ObjectDescriptor",*it));
    }
  this->insert(odescriptors);

  QtSoapArray* patients = new QtSoapArray(QtSoapQName("Patients") ,QtSoapType::Other,
                                         ad.patients.size());

  for (QList<ctkDicomAppHosting::Patient>::ConstIterator it = ad.patients.constBegin();
       it < ad.patients.constEnd(); it++)
  {
       patients->append(new ctkDicomSoapPatient("Patient",*it));
  }
  this->insert(patients);
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::AvailableData ctkDicomSoapAvailableData::getAvailableData (const QtSoapType& type)
{
  ctkDicomAppHosting::AvailableData ad;

  //const QtSoapStruct& avData = static_cast<const QtSoapStruct&> (type);
  //for (int i = 0; i < avData.count() ; i++)
  //  {
  //  qDebug() << avData[i].typeName() << "  " << avData[i].name().name();
  //  }

  ctkDicomAppHosting::ArrayOfObjectDescriptors list;

  //const QtSoapType& inputType = type["objectDescriptors"];
  //if(inputType.isValid()==false)
  //  {
  //  qCritical() << "  NotifyDataAvailable: availableData not valid. " << inputType.errorString();
  //  }
  //qDebug() << inputType.typeName() << "  " << inputType.name().name();

  ad.objectDescriptors = ctkDicomSoapArrayOfObjectDescriptors::getArrayOfObjectDescriptors(type["ObjectDescriptors"]);

  QList<ctkDicomAppHosting::Patient> listPatients;
  const QtSoapType& patientsArray = type["Patients"];
  for (int i = 0; i < patientsArray.count() ; i++)
    {
      const ctkDicomAppHosting::Patient patient =
          ctkDicomSoapPatient::getPatient(patientsArray[i]);
      listPatients.append(patient);
    }
  ad.patients = listPatients;

  return ad;
}

//----------------------------------------------------------------------------
ctkDicomSoapObjectLocator::ctkDicomSoapObjectLocator(const QString& name,
                                                     const ctkDicomAppHosting::ObjectLocator& ol)
  : QtSoapStruct(QtSoapQName(name))
{
   /*this->insert(new QtSoapSimpleType(QtSoapQName("Locator"),
                                    ol.locator) );*/

  this->insert(new QtSoapSimpleType(
                 QtSoapQName("Length"),
                 ol.length) );

  this->insert(new QtSoapSimpleType(
                 QtSoapQName("Offset"),
                 ol.offset) );

  this->insert(new ctkDicomSoapUID("TransferSyntax", ol.transferSyntax) );

  this->insert(new QtSoapSimpleType(
                 QtSoapQName("URI"),
                 ol.URI) );

  this->insert(new ctkDicomSoapUUID("Locator",
                                    ol.locator) );

   this->insert(new ctkDicomSoapUUID("Source",
                                    ol.source) );

  /*this->insert(new QtSoapSimpleType(
                 QtSoapQName("Source"),
                 ol.source) );*/

  /*this->insert(new QtSoapSimpleType(
                 QtSoapQName("TransferSyntax"),
                 ol.transferSyntax) );*/


}

//----------------------------------------------------------------------------
ctkDicomAppHosting::ObjectLocator ctkDicomSoapObjectLocator::getObjectLocator(const QtSoapType& type)
{
  ctkDicomAppHosting::ObjectLocator ol;

  ol.length =
      type["Length"].value().toInt();
  ol.offset =
      type["Offset"].value().toInt();

  //ol.transferSyntax =
    //  type["TransferSyntax"].value().toString();
  ol.transferSyntax =
    ctkDicomSoapUID::getUID(type["TransferSyntax"]);

  ol.URI =
      type["URI"].value().toString();

  //ol.locator = type["Locator"].value().toString();
  ol.locator = ctkDicomSoapUUID::getUuid(type["Locator"]).toString();

  //ol.source = type["Source"].value().toString();
  ol.source = ctkDicomSoapUUID::getUuid(type["Source"]).toString();
  //type["Source"].value().toString();



  return ol;
}

//----------------------------------------------------------------------------
ctkDicomSoapArrayOfObjectLocators::ctkDicomSoapArrayOfObjectLocators(
  const QString& name, const QList<ctkDicomAppHosting::ObjectLocator>& array)
  : QtSoapArray(QtSoapQName(name), QtSoapType::String, array.size())
{
  for (QList<ctkDicomAppHosting::ObjectLocator>::ConstIterator it = array.constBegin();
       it < array.constEnd(); it++)
    {
    this->append(new ctkDicomSoapObjectLocator("ObjectLocator",(*it)));
    }
}

//----------------------------------------------------------------------------
QList<ctkDicomAppHosting::ObjectLocator> ctkDicomSoapArrayOfObjectLocators::getArray(const QtSoapType& type)
{
  QList<ctkDicomAppHosting::ObjectLocator> list;

  for (int i = 0; i < type.count(); i++)
    {
    const ctkDicomAppHosting::ObjectLocator ol =
        ctkDicomSoapObjectLocator::getObjectLocator(type[i]);
    list << ol;
    }
  return list;
}
