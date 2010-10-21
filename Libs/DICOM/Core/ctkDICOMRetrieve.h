/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkDICOMRetrieve_h
#define __ctkDICOMRetrieve_h


// Qt includes 
#include <QObject>
#include <QDir>

#include "ctkDICOMCoreExport.h"

class ctkDICOMRetrievePrivate;
class CTK_DICOM_CORE_EXPORT ctkDICOMRetrieve : public QObject
{
  Q_OBJECT
public:
  explicit ctkDICOMRetrieve();
  virtual ~ctkDICOMRetrieve();

  /// Set methods for connectivity
  void setCallingAETitle ( QString callingAETitle );
  const QString& callingAETitle();
  void setCalledAETitle ( QString calledAETitle );
  const QString& calledAETitle();
  void setHost ( QString host );
  const QString& host();
  void setCallingPort ( int port );
  int callingPort();
  void setCalledPort ( int port );
  int calledPort();
  void setMoveDestinationAETitle ( QString moveDestinationAETitle );
  const QString& moveDestinationAETitle();

  // Could be a slot...
  void retrieveSeries ( QString seriesInstanceUID, QDir directory );
  void retrieveStudy ( QString studyInstanceUID, QDir directory );

protected:
  QScopedPointer<ctkDICOMRetrievePrivate> d_ptr;
  
private:
  void retrieve ( QDir directory );
  
  Q_DECLARE_PRIVATE(ctkDICOMRetrieve);
  Q_DISABLE_COPY(ctkDICOMRetrieve);

};


#endif
