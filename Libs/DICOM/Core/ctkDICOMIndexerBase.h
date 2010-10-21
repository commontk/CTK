/*=========================================================================

  Library:   CTK

  Copyright (c) 2010

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

#ifndef __ctkDICOMIndexerBase_h
#define __ctkDICOMIndexerBase_h

// Qt includes 
#include <QObject>
#include <QSqlDatabase>

#include "ctkDICOMCoreExport.h"

class ctkDICOMIndexerBasePrivate;
class DcmDataset;

class CTK_DICOM_CORE_EXPORT ctkDICOMIndexerBase : public QObject
{
  Q_OBJECT
public:
  explicit ctkDICOMIndexerBase();
  virtual ~ctkDICOMIndexerBase();
  void setDatabase ( QSqlDatabase database );
  const QSqlDatabase& database() const;

  /**
   * Will create an entry in the appropriate tables for this dataset.
   */
  void insert ( DcmDataset* dataset, QString filename );
  /**
   * Insert into the database if not already exsting.
   */
  void insert ( DcmDataset *dataset );

protected:
  QScopedPointer<ctkDICOMIndexerBasePrivate> d_ptr;
  
private:
  Q_DECLARE_PRIVATE(ctkDICOMIndexerBase);
  Q_DISABLE_COPY(ctkDICOMIndexerBase);
};

#endif
