/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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


#include "ctkServiceTrackerPrivate.h"
#include "ctkTrackedService_p.h"

#include "ctkPluginConstants.h"
#include "ctkLDAPSearchFilter.h"
#include "ctkServiceTracker.h"

#include <stdexcept>

const bool ctkServiceTrackerPrivate::DEBUG	= true;

ctkServiceTrackerPrivate::ctkServiceTrackerPrivate(
    ctkServiceTracker* st, ctkPluginContext* context,
    const ctkServiceReference& reference,
    ctkServiceTrackerCustomizer* customizer)
  : context(context), customizer(customizer), trackReference(reference),
    trackedService(0), cachedReference(0), cachedService(0), q_ptr(st)
{
  this->customizer = customizer ? customizer : q_func();
  this->listenerFilter = QString("(") + ctkPluginConstants::SERVICE_ID +
                         "=" + reference.getProperty(ctkPluginConstants::SERVICE_ID).toString() + ")";
  try
  {
    this->filter = ctkLDAPSearchFilter(listenerFilter);
  }
  catch (const std::invalid_argument& e)
  {
    /*
     * we could only get this exception if the ServiceReference was
     * invalid
     */
    std::invalid_argument ia(std::string("unexpected std::invalid_argument exception: ") + e.what());
    throw ia;
  }
}

ctkServiceTrackerPrivate::ctkServiceTrackerPrivate(
    ctkServiceTracker* st,
    ctkPluginContext* context, const QString& clazz,
    ctkServiceTrackerCustomizer* customizer)
      : context(context), customizer(customizer), trackClass(clazz),
        trackReference(0), trackedService(0), cachedReference(0),
        cachedService(0), q_ptr(st)
{
  this->customizer = customizer ? customizer : q_func();
  this->listenerFilter = QString("(") + ctkPluginConstants::OBJECTCLASS + "="
                        + clazz + ")";
  try
  {
    this->filter = ctkLDAPSearchFilter(listenerFilter);
  }
  catch (const std::invalid_argument& e)
  {
    /*
     * we could only get this exception if the clazz argument was
     * malformed
     */
    std::invalid_argument ia(
        std::string("unexpected std::invalid_argument exception: ") + e.what());
    throw ia;
  }
}

ctkServiceTrackerPrivate::ctkServiceTrackerPrivate(
    ctkServiceTracker* st,
    ctkPluginContext* context, const ctkLDAPSearchFilter& filter,
    ctkServiceTrackerCustomizer* customizer)
      : context(context), filter(filter), customizer(customizer),
        listenerFilter(filter.toString()), trackReference(0),
        trackedService(0), cachedReference(0), cachedService(0), q_ptr(st)
{
  this->customizer = customizer ? customizer : q_func();
  if (context == 0)
  {
    throw std::invalid_argument("The plugin context cannot be null.");
  }
}

ctkServiceTrackerPrivate::~ctkServiceTrackerPrivate()
{
  if (customizer != q_func())
  {
    delete customizer;
  }
}

QList<ctkServiceReference> ctkServiceTrackerPrivate::getInitialReferences(const QString& className,
                                                const QString& filterString)
{
  return context->getServiceReferences(className, filterString);
}

QSharedPointer<ctkTrackedService> ctkServiceTrackerPrivate::tracked() const
{
  return trackedService;
}

void ctkServiceTrackerPrivate::modified()
{
  cachedReference = 0; /* clear cached value */
  cachedService = 0; /* clear cached value */
  if (DEBUG)
  {
    qDebug() << "ctkServiceTracker::modified:" << filter;
  }
}
