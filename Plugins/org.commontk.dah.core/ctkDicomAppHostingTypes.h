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

#ifndef CTKDICOMAPPHOSTINGTYPES_H
#define CTKDICOMAPPHOSTINGTYPES_H

// Qt includes
#include <QString>
#include <QList>

#ifdef ERROR
# error Try to reorder include files (this one first)       \
 or write #undef ERROR before including this header.        \
 Cause of this problem may be dcmimage.h, which indirectly  \
 includes windows.h.
#endif

#include <org_commontk_dah_core_Export.h>

/**
  * \brief Typedefs and classes defined in the interfaces of DICOM Supplement 118.
  *
  */
namespace ctkDicomAppHosting {

  //----------------------------------------------------------------------------
  enum State {
    IDLE,
    INPROGRESS,
    COMPLETED,
    SUSPENDED,
    CANCELED,
    EXIT
  };

  //----------------------------------------------------------------------------
  enum StatusType {
    INFORMATION,
    WARNING,
    ERROR,
    FATALERROR
  };

  //----------------------------------------------------------------------------
  struct Status {
    Status():statusType(INFORMATION){}
    StatusType statusType;
    QString codingSchemeDesignator;
    QString codeValue;
    QString codeMeaning;
  };

  //----------------------------------------------------------------------------
  struct ObjectLocator {
    ObjectLocator():length(0), offset(0){}
    QString locator;
    QString source;
    QString transferSyntax;
    qint64 length;
    qint64 offset;
    QString URI;
  };

  //----------------------------------------------------------------------------
  struct ObjectDescriptor {
    QString descriptorUUID;
    QString mimeType;
    QString classUID;
    QString transferSyntaxUID;
    QString modality;
  };

  //----------------------------------------------------------------------------
  typedef QList<ObjectDescriptor> ArrayOfObjectDescriptors;

  //----------------------------------------------------------------------------
  struct Series {
    QString seriesUID;
    ArrayOfObjectDescriptors objectDescriptors;
  };

  //----------------------------------------------------------------------------
  struct Study {
    QString studyUID;
    ArrayOfObjectDescriptors objectDescriptors;
    QList<Series> series;
  };

  //----------------------------------------------------------------------------
  struct Patient {
    QString name;
    QString id;
    QString assigningAuthority;
    QString sex;
    QString birthDate;
    ArrayOfObjectDescriptors objectDescriptors;
    QList<Study> studies;
  };

  //----------------------------------------------------------------------------
  struct AvailableData {
    ArrayOfObjectDescriptors objectDescriptors;
    QList<Patient> patients;
  };

//----------------------------------------------------------------------------
// Comparison operators

bool org_commontk_dah_core_EXPORT operator ==(const ctkDicomAppHosting::Status& left,
                                              const ctkDicomAppHosting::Status& right);

bool org_commontk_dah_core_EXPORT operator !=(const ctkDicomAppHosting::Status& left,
                                              const ctkDicomAppHosting::Status& right);

bool org_commontk_dah_core_EXPORT operator ==(const ctkDicomAppHosting::ObjectLocator& left,
                                              const ctkDicomAppHosting::ObjectLocator& right);

bool org_commontk_dah_core_EXPORT operator !=(const ctkDicomAppHosting::ObjectLocator& left,
                                              const ctkDicomAppHosting::ObjectLocator& right);

bool org_commontk_dah_core_EXPORT operator ==(const ctkDicomAppHosting::ObjectDescriptor& left,
                                              const ctkDicomAppHosting::ObjectDescriptor& right);

bool org_commontk_dah_core_EXPORT operator !=(const ctkDicomAppHosting::ObjectDescriptor& left,
                                              const ctkDicomAppHosting::ObjectDescriptor& right);

bool org_commontk_dah_core_EXPORT operator ==(const ctkDicomAppHosting::Series& left,
                                              const ctkDicomAppHosting::Series& right);

bool org_commontk_dah_core_EXPORT operator !=(const ctkDicomAppHosting::Series& left,
                                              const ctkDicomAppHosting::Series& right);

bool org_commontk_dah_core_EXPORT operator ==(const ctkDicomAppHosting::Study& left,
                                              const ctkDicomAppHosting::Study& right);

bool org_commontk_dah_core_EXPORT operator !=(const ctkDicomAppHosting::Study& left,
                                              const ctkDicomAppHosting::Study& right);

bool org_commontk_dah_core_EXPORT operator ==(const ctkDicomAppHosting::Patient& left,
                                              const ctkDicomAppHosting::Patient& right);

bool org_commontk_dah_core_EXPORT operator !=(const ctkDicomAppHosting::Patient& left,
                                              const ctkDicomAppHosting::Patient& right);

bool org_commontk_dah_core_EXPORT operator ==(const ctkDicomAppHosting::AvailableData& left,
                                              const ctkDicomAppHosting::AvailableData& right);

bool org_commontk_dah_core_EXPORT operator !=(const ctkDicomAppHosting::AvailableData& left,
                                              const ctkDicomAppHosting::AvailableData& right);

}
#endif // CTKDICOMAPPHOSTINGTYPES
