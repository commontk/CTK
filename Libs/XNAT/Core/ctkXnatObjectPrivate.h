/*=============================================================================

  Library: XNAT/Core

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

#ifndef CTKXNATOBJECTPRIVATE_H
#define CTKXNATOBJECTPRIVATE_H

#include <QDateTime>
#include <QMap>
#include <QString>

class ctkXnatObject;
class ctkXnatConnection;

class ctkXnatObjectPrivate
{

public:

  typedef QMap<QString,QString> PropertyMap;
  typedef QMap<QString,QString>::Iterator PropertyMapIterator;
  typedef QMap<QString,QString>::ConstIterator PropertyMapConstInterator;

  explicit ctkXnatObjectPrivate();
  virtual ~ctkXnatObjectPrivate();

private:

  friend class ctkXnatObject;

  QMap<QString, QString> properties;

  QList<ctkXnatObject*> children;

  // Description of the object that can be used e.g. as tooltip
  QString description;

  QDateTime lastModifiedTime;

  bool fetched;

  ctkXnatObject* parent;
};

#endif // CTKXNATOBJECTPRIVATE_H
