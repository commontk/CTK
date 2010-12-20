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


#ifndef CTKSERVICEREFERENCEPRIVATE_H
#define CTKSERVICEREFERENCEPRIVATE_H

#include <QAtomicInt>
#include <QSharedPointer>

#include "ctkPluginFramework_global.h"

class QObject;

class ctkServiceRegistrationPrivate;
class ctkPlugin;

class ctkServiceReferencePrivate
{
public:

  ctkServiceReferencePrivate(ctkServiceRegistrationPrivate* reg);

  virtual ~ctkServiceReferencePrivate() {}

  /**
    * Get the service object.
    *
    * @param plugin requester of service.
    * @return Service requested or null in case of failure.
    */
  QObject* getService(QSharedPointer<ctkPlugin> plugin);

  /**
   * Unget the service object.
   *
   * @param plugin Plugin who wants remove service.
   * @param checkRefCounter If true decrement refence counter and remove service
   *                        if we reach zero. If false remove service without
   *                        checking refence counter.
   * @return True if service was remove or false if only refence counter was
   *         decremented.
   */
  bool ungetService(QSharedPointer<ctkPlugin> plugin, bool checkRefCounter);

  /**
   * Get all properties registered with this service.
   *
   * @return A ServiceProperties object containing properties or being empty
   *         if service has been removed.
   */
  ServiceProperties getProperties() const;

  /**
   * Reference count for implicitly shared private implementation.
   */
  QAtomicInt ref;

  /**
   * Link to registration object for this reference.
   */
  ctkServiceRegistrationPrivate* const registration;
};

#endif // CTKSERVICEREFERENCEPRIVATE_H
