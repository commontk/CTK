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


#ifndef CTKSERVICESLOTENTRY_P_H
#define CTKSERVICESLOTENTRY_P_H


#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>
#include <QExplicitlySharedDataPointer>

#include "ctkServiceEvent.h"
#include "ctkLDAPExpr_p.h"

class ctkPlugin;
class ctkServiceSlotEntryData;

class QObject;


/**
 * \ingroup PluginFramework
 *
 * Data structure for saving information about slots registered for
 * receiving service lifecycle events.
 */
class ctkServiceSlotEntry
{

public:

  ctkServiceSlotEntry(QSharedPointer<ctkPlugin> p, QObject* receiver, const char* slot,
                      const QString& filter = QString());

  ctkServiceSlotEntry(const ctkServiceSlotEntry& other);

  // default constructor for use in QSet
  ctkServiceSlotEntry();
  ~ctkServiceSlotEntry();

  // assignment operator for use in QSet
  ctkServiceSlotEntry& operator=(const ctkServiceSlotEntry& other);

  bool operator==(const ctkServiceSlotEntry& other) const;

  void invokeSlot(const ctkServiceEvent& event);

  void setRemoved(bool removed);

  bool isRemoved() const;

  QSharedPointer<ctkPlugin> getPlugin() const;

  ctkLDAPExpr getLDAPExpr() const;

  QString getFilter() const;

  ctkLDAPExpr::LocalCache& getLocalCache() const;

private:

  friend uint qHash(const ctkServiceSlotEntry& serviceSlot);

  QExplicitlySharedDataPointer<ctkServiceSlotEntryData> d;

};

/**
 * \ingroup PluginFramework
 */
uint qHash(const ctkServiceSlotEntry& serviceSlot);

#endif // CTKSERVICESLOTENTRY_P_H
