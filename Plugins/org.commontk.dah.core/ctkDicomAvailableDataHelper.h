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


#ifndef CTKDICOMAVAILABLEDATAHELPER_H
#define CTKDICOMAVAILABLEDATAHELPER_H

// Qt includes
#include <QObject>
#include <QString>

// CTK includes
#include <org_commontk_dah_core_Export.h>
#include <ctkDicomAppHostingTypes.h>

class ctkDicomObjectLocatorCache;
class ctkDICOMDataset;

namespace ctkDicomAvailableDataHelper {

//----------------------------------------------------------------------------
class ctkDicomAvailableDataAccessorPrivate;
class org_commontk_dah_core_EXPORT ctkDicomAvailableDataAccessor : public QObject
{
public:
  ctkDicomAvailableDataAccessor(ctkDicomAppHosting::AvailableData& ad);
  virtual ~ctkDicomAvailableDataAccessor();
  
  /**
   * Method used to retrieve information about a specific patient, giving a patient struct with the ID field already 
   * defined.
   * \return the struct with patient information if patient is present inside available data, otherwise return NULL.
   */
  ctkDicomAppHosting::Patient* getPatient(const ctkDicomAppHosting::Patient& patient) const;

  /**
   * Method used to retrieve information about a specific study, giving Study UID.
   * \return the struct with study information if study is present inside available data, otherwise return NULL.
   */
  ctkDicomAppHosting::Study* getStudy(const QString& studyUID) const;

  /**
   * Method used to retrieve information about a specific series, giving series UID.
   * \return the struct with series information if series is present inside available data, otherwise return NULL.
   */
  ctkDicomAppHosting::Series* getSeries(const QString& seriesUID) const;

  void find(const ctkDicomAppHosting::Patient& patient, 
                                         const QString& studyUID, 
                                         const QString& seriesUID,
                                         ctkDicomAppHosting::Patient*& patientResult, 
                                         ctkDicomAppHosting::Study*& studyResult, 
                                         ctkDicomAppHosting::Series*& seriesResult) const;

protected:
  QScopedPointer<ctkDicomAvailableDataAccessorPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDicomAvailableDataAccessor);
};

//----------------------------------------------------------------------------
bool org_commontk_dah_core_EXPORT addToAvailableData(ctkDicomAppHosting::AvailableData& data, 
                        ctkDicomObjectLocatorCache* objectLocatorCache, 
                        const ctkDICOMDataset& dataset, 
                        long length, 
                        long offset, 
                        const QString& uri);

//----------------------------------------------------------------------------
bool org_commontk_dah_core_EXPORT addToAvailableData(ctkDicomAppHosting::AvailableData& data, 
                        ctkDicomObjectLocatorCache* objectLocatorCache, 
                        const QString& filename);

//----------------------------------------------------------------------------
bool org_commontk_dah_core_EXPORT addNonDICOMToAvailableData(ctkDicomAppHosting::AvailableData& data, 
                        ctkDicomObjectLocatorCache* objectLocatorCache, 
                        const ctkDICOMDataset& dataset, 
                        long length, 
                        long offset, 
                        const QString& uri);

//----------------------------------------------------------------------------
bool appendToAvailableData(ctkDicomAppHosting::AvailableData& dest,
                        const ctkDicomAppHosting::AvailableData& src);


} //end namespace ctkDicomAvailableDataHelper

#endif // CTKDICOMAVAILABLEDATAHELPER_H
