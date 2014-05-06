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

// CTK includes
#include "ctkDicomAvailableDataHelper.h"
#include "ctkDicomAvailableDataHelper.h"
#include "ctkDicomObjectLocatorCache.h"
#include <ctkDICOMItem.h>

// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>

namespace ctkDicomAvailableDataHelper {

//------------------------------------------------------------------------------
class ctkDicomAvailableDataAccessorPrivate
{
//  Q_DECLARE_PUBLIC(ctkDicomAvailableDataAccessor);
protected:
  
public:
  ctkDicomAvailableDataAccessorPrivate(ctkDicomAppHosting::AvailableData& availableData) : 
      m_AvailableData(availableData) { };

  ctkDicomAppHosting::AvailableData& m_AvailableData;
};

//----------------------------------------------------------------------------
ctkDicomAvailableDataAccessor::ctkDicomAvailableDataAccessor(ctkDicomAppHosting::AvailableData& ad)
  : d_ptr(new ctkDicomAvailableDataAccessorPrivate(ad))
{
}

ctkDicomAvailableDataAccessor::~ctkDicomAvailableDataAccessor() {};

//----------------------------------------------------------------------------
ctkDicomAppHosting::Patient* ctkDicomAvailableDataAccessor::getPatient(const ctkDicomAppHosting::Patient& patient) const
{
  const Q_D(ctkDicomAvailableDataAccessor);
  ctkDicomAppHosting::AvailableData & ad(d->m_AvailableData);
  for (QList<ctkDicomAppHosting::Patient>::Iterator pit = ad.patients.begin();
    pit < ad.patients.end(); pit++)
    {
      if(pit->id==patient.id)
        return &(*pit);
    }
  return NULL;
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::Study* ctkDicomAvailableDataAccessor::getStudy(const QString& studyUID) const
{
  const Q_D(ctkDicomAvailableDataAccessor);
  ctkDicomAppHosting::AvailableData & ad(d->m_AvailableData);
  for (QList<ctkDicomAppHosting::Patient>::Iterator pit = ad.patients.begin();
    pit < ad.patients.end(); pit++)
    {
    for (QList<ctkDicomAppHosting::Study>::Iterator sit = pit->studies.begin();
      sit < pit->studies.end(); sit++)
      {
          if(sit->studyUID==studyUID)
            return &(*sit);
      }
    }
  return NULL;
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::Series* ctkDicomAvailableDataAccessor::getSeries(const QString& seriesUID) const
{
  const Q_D(ctkDicomAvailableDataAccessor);
  ctkDicomAppHosting::AvailableData & ad(d->m_AvailableData);
  for (QList<ctkDicomAppHosting::Patient>::Iterator pit = ad.patients.begin();
    pit < ad.patients.end(); pit++)
    {
    for (QList<ctkDicomAppHosting::Study>::Iterator sit = pit->studies.begin();
      sit < pit->studies.end(); sit++)
      {
      for (QList<ctkDicomAppHosting::Series>::Iterator seit = sit->series.begin();
        seit < sit->series.end(); seit++)
        {
          if(seit->seriesUID==seriesUID)
            return &(*seit);
        }
      }
    }
  return NULL;
}

//----------------------------------------------------------------------------
void ctkDicomAvailableDataAccessor::find(const ctkDicomAppHosting::Patient& patient, 
                                         const QString& studyUID, 
                                         const QString& seriesUID,
                                         ctkDicomAppHosting::Patient*& patientResult, 
                                         ctkDicomAppHosting::Study*& studyResult, 
                                         ctkDicomAppHosting::Series*& seriesResult) const
{
  const Q_D(ctkDicomAvailableDataAccessor);
  ctkDicomAppHosting::AvailableData & ad(d->m_AvailableData);
  patientResult=NULL;
  studyResult=NULL;
  seriesResult=NULL;
  for (QList<ctkDicomAppHosting::Patient>::Iterator pit = ad.patients.begin();
    pit < ad.patients.end(); pit++)
    {
    if(pit->id==patient.id)
      {
      patientResult = &(*pit);
      for (QList<ctkDicomAppHosting::Study>::Iterator sit = pit->studies.begin();
        sit < pit->studies.end(); sit++)
        {
        if(sit->studyUID==studyUID)
          {
          studyResult = &(*sit);
          for (QList<ctkDicomAppHosting::Series>::Iterator seit = sit->series.begin();
            seit < sit->series.end(); seit++)
            {
            if(seit->seriesUID==seriesUID)
              {
              seriesResult=&(*seit);
              return;
              }
            }
            return;
          }
        }
      return;
      }
    }
}

//----------------------------------------------------------------------------
bool addNonDICOMToAvailableData(ctkDicomAppHosting::AvailableData& data, 
                        ctkDicomObjectLocatorCache* objectLocatorCache, 
                        long length, 
                        long offset, 
                        const QString& uri)
{
  if(objectLocatorCache == NULL)
    return false;
  
  ctkDicomAppHosting::ObjectDescriptor objectDescriptor;
  ctkDicomAppHosting::Study study;
  ctkDicomAppHosting::Series series;
  ctkDicomAppHosting::Patient patient;
 

  QFileInfo fileinfo(uri);
  QString ext = fileinfo.suffix();

  QString uuid = QUuid::createUuid().toString();
  objectDescriptor.descriptorUUID = uuid;
  //objectDescriptor.mimeType = "text/plain";
  objectDescriptor.classUID = "";
  objectDescriptor.transferSyntaxUID = "";
  objectDescriptor.modality = "";
  
  //the default mime type is set to plain text
  if ( ext.compare("xml") == 0)
	  objectDescriptor.mimeType = "text/xml";
  else if ( ext.compare("txt") == 0)
	  objectDescriptor.mimeType = "text/plain";
  else if ( (ext.compare("jpg") == 0) || (ext.compare("jpeg") == 0))
	  objectDescriptor.mimeType = "image/jpeg";
  else if ( ext.compare("bmp") == 0)
	  objectDescriptor.mimeType = "image/bmp";
  else
	   objectDescriptor.mimeType = "text/plain"; //default

  //ctkDicomAppHosting::Patient* ppatient;
  //ctkDicomAppHosting::Study* pstudy;
  //ctkDicomAppHosting::Series* pseries;

  data.objectDescriptors.append(objectDescriptor);

  ctkDicomAppHosting::ObjectLocator locator;
  locator.locator = objectDescriptor.descriptorUUID;
  locator.source = objectDescriptor.descriptorUUID;
  locator.offset = offset;
  locator.length = length;
  locator.transferSyntax = objectDescriptor.transferSyntaxUID;
  locator.URI = uri;

  objectLocatorCache->insert(objectDescriptor.descriptorUUID, locator);
  return true;
}


bool addToAvailableData(ctkDicomAppHosting::AvailableData& data, 
                        ctkDicomObjectLocatorCache* objectLocatorCache, 
                        const ctkDICOMItem& dataset, 
                        long length, 
                        long offset, 
                        const QString& uri)
{
  if(objectLocatorCache == NULL)
    return false;
  
  ctkDicomAppHosting::ObjectDescriptor objectDescriptor;
  ctkDicomAppHosting::Study study;
  ctkDicomAppHosting::Series series;
  ctkDicomAppHosting::Patient patient;
  patient.name = dataset.GetElementAsString(DCM_PatientName);
  qDebug()<<"Patient:  " << patient.name;
  patient.id = dataset.GetElementAsString(DCM_PatientID);
  patient.assigningAuthority = dataset.GetElementAsString(DCM_IssuerOfPatientID);
  patient.sex = dataset.GetElementAsString(DCM_PatientSex);
  patient.birthDate = dataset.GetElementAsString(DCM_PatientBirthDate);


  study.studyUID = dataset.GetElementAsString(DCM_StudyInstanceUID);
  series.seriesUID = dataset.GetElementAsString(DCM_SeriesInstanceUID);

  QString uuid = QUuid::createUuid().toString();
  objectDescriptor.descriptorUUID = uuid;
  objectDescriptor.mimeType = "application/dicom";
  objectDescriptor.classUID = dataset.GetElementAsString(DCM_SOPClassUID);
  objectDescriptor.transferSyntaxUID = dataset.GetElementAsString(DCM_TransferSyntaxUID);
  objectDescriptor.modality = dataset.GetElementAsString(DCM_Modality);
  


  ctkDicomAppHosting::Patient* ppatient;
  ctkDicomAppHosting::Study* pstudy;
  ctkDicomAppHosting::Series* pseries;
  
  ctkDicomAvailableDataAccessor(data).find(patient, study.studyUID, series.seriesUID,
    ppatient, pstudy, pseries);

  if(pseries==NULL)
  {
    series.objectDescriptors.append(objectDescriptor);
    if(pstudy==NULL)
    {
      study.series.append(series);
      if(ppatient==NULL)
      {
        patient.studies.append(study);
        data.patients.append(patient);
      }
      else
      {
        ppatient->studies.append(study);
      }
    }
    else
    {
      pstudy->series.append(series);
    }
  }
  else
  {
    pseries->objectDescriptors.append(objectDescriptor);
  }

  ctkDicomAppHosting::ObjectLocator locator;
  locator.locator = objectDescriptor.descriptorUUID;
  locator.source = objectDescriptor.descriptorUUID;
  locator.offset = offset;
  locator.length = length;
  locator.transferSyntax = objectDescriptor.transferSyntaxUID;
  locator.URI = uri;

  objectLocatorCache->insert(objectDescriptor.descriptorUUID, locator);
  return true;
}

//----------------------------------------------------------------------------
bool addToAvailableData(ctkDicomAppHosting::AvailableData& data,
                        ctkDicomObjectLocatorCache* objectLocatorCache, 
                        const QString& filename)
{
  QFileInfo fileinfo(filename);
  qDebug() << filename << " " << fileinfo.exists();
  
  
  QString uri("file:///");
  uri.append(fileinfo.absoluteFilePath());

  //first check if its a non-DICOM file, if so add it first.
  QString ext = fileinfo.suffix();
  if ( (ext.compare("txt") == 0) || (ext.compare("xml") ==0) || (ext.compare("jpg") ==0) || 
       (ext.compare("bmp") ==0) || (ext.compare("csv") ==0) || (ext.compare("nii") ==0) || 
       (ext.compare("nrrd") ==0) )
  {
  	  qDebug() << "adding Non DICOM File";
      return addNonDICOMToAvailableData(data, objectLocatorCache, fileinfo.size(), 0, uri);
  }
  //this could be a DICOM file then
  ctkDICOMItem ctkdataset;
  ctkdataset.InitializeFromFile(filename, EXS_Unknown, EGL_noChange, 400);

  return addToAvailableData(data, objectLocatorCache, ctkdataset, fileinfo.size(), 0, uri);

}

//----------------------------------------------------------------------------
bool appendToAvailableData(ctkDicomAppHosting::AvailableData& dest,
                        const ctkDicomAppHosting::AvailableData& src)
{
  dest.objectDescriptors.append(src.objectDescriptors);
  dest.patients.append(src.patients);
  return true;
}

//----------------------------------------------------------------------------
void appendAllUuids(const ctkDicomAppHosting::Patient& patient, QList<QUuid> & uuidlist)
{
  // Loop over patient level object descriptors
  foreach(const ctkDicomAppHosting::ObjectDescriptor& objectDescriptor, patient.objectDescriptors)
    {
    uuidlist.append(objectDescriptor.descriptorUUID);
    }

  // Loop over studies
  foreach(const ctkDicomAppHosting::Study& study, patient.studies)
    {
    // Loop over study level object descriptors
    foreach(const ctkDicomAppHosting::ObjectDescriptor& objectDescriptor, study.objectDescriptors)
      {
      uuidlist.append(objectDescriptor.descriptorUUID);
      }
    // Loop over series
    foreach(const ctkDicomAppHosting::Series& series, study.series)
      {
      // Loop over series level object descriptors
      foreach(const ctkDicomAppHosting::ObjectDescriptor& objectDescriptor, series.objectDescriptors)
        {
        uuidlist.append(objectDescriptor.descriptorUUID);
        }
      }
    }
}

//----------------------------------------------------------------------------
QList<QUuid> getAllUuids(const ctkDicomAppHosting::Patient& patient)
{
  QList<QUuid> uuidlist;
  appendAllUuids(patient, uuidlist);
  return uuidlist;
}

//----------------------------------------------------------------------------
QList<QUuid> getAllUuids(const ctkDicomAppHosting::AvailableData& availableData)
{
  QList<QUuid> uuidlist;

  // Loop over top level object descriptors
  foreach(const ctkDicomAppHosting::ObjectDescriptor& objectDescriptor, availableData.objectDescriptors)
    {
    uuidlist.append(objectDescriptor.descriptorUUID);
    }

   // Loop over patients
  foreach(const ctkDicomAppHosting::Patient& patient, availableData.patients)
    {
    appendAllUuids(patient, uuidlist);
    }
  return uuidlist;
}

} //end namespace ctkDicomAvailableDataHelper
