/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef CTKDICOMINDEXERPRIVATE_H
#define CTKDICOMINDEXERPRIVATE_H

#include <QObject>

#include "ctkDICOMIndexer.h"

//------------------------------------------------------------------------------
class ctkDICOMIndexerPrivate : public QObject
{
  Q_OBJECT

  Q_DECLARE_PUBLIC(ctkDICOMIndexer);

protected:
  ctkDICOMIndexer* const q_ptr;

public:
  ctkDICOMIndexerPrivate(ctkDICOMIndexer&);
  ~ctkDICOMIndexerPrivate();

public:
  ctkDICOMAbstractThumbnailGenerator* thumbnailGenerator;
  bool                    Canceled;

  // Incremented each time startIndexing is called
  // and decremented when endIndexing is called.
  // This makes sure that when a batch of indexing
  // operations are performed, at any level
  // (file, folder, or set of folders) then
  // batch processing initialization and finalization
  // are performed exactly once.
  int                     StartedIndexing;
};


#endif // CTKDICOMINDEXERPRIVATE_H
