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


#include "ctkTrackedService_p.h"

#include "ctkPluginContext.h"
#include "ctkPluginConstants.h"
#include "ctkLDAPSearchFilter.h"

//----------------------------------------------------------------------------
template<class S, class T>
const bool ctkServiceTrackerPrivate<S,T>::DEBUG = false;

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceTrackerPrivate<S,T>::ctkServiceTrackerPrivate(
    ctkServiceTracker<S,T>* st, ctkPluginContext* context,
    const ctkServiceReference& reference,
    ctkServiceTrackerCustomizer<T>* customizer)
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
  catch (const ctkInvalidArgumentException& e)
  {
    /*
     * we could only get this exception if the ServiceReference was
     * invalid
     */
    ctkInvalidArgumentException ia(QString("unexpected ctkInvalidArgumentException exception: %1").arg(e.what()));
    throw ia;
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceTrackerPrivate<S,T>::ctkServiceTrackerPrivate(
    ctkServiceTracker<S,T>* st,
    ctkPluginContext* context, const QString& clazz,
    ctkServiceTrackerCustomizer<T>* customizer)
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
  catch (const ctkInvalidArgumentException& e)
  {
    /*
     * we could only get this exception if the clazz argument was
     * malformed
     */
    ctkInvalidArgumentException ia(
          QString("unexpected ctkInvalidArgumentException exception: %1").arg(e.what()));
    throw ia;
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceTrackerPrivate<S,T>::ctkServiceTrackerPrivate(
    ctkServiceTracker<S,T>* st,
    ctkPluginContext* context, const ctkLDAPSearchFilter& filter,
    ctkServiceTrackerCustomizer<T>* customizer)
      : context(context), filter(filter), customizer(customizer),
        listenerFilter(filter.toString()), trackReference(0),
        trackedService(0), cachedReference(0), cachedService(0), q_ptr(st)
{
  this->customizer = customizer ? customizer : q_func();
  if (context == 0)
  {
    throw ctkInvalidArgumentException("The plugin context cannot be null.");
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceTrackerPrivate<S,T>::~ctkServiceTrackerPrivate()
{

}

//----------------------------------------------------------------------------
template<class S, class T>
QList<ctkServiceReference> ctkServiceTrackerPrivate<S,T>::getInitialReferences(const QString& className,
                                                const QString& filterString)
{
  return context->getServiceReferences(className, filterString);
}

//----------------------------------------------------------------------------
template<class S, class T>
QList<ctkServiceReference> ctkServiceTrackerPrivate<S,T>::getServiceReferences_unlocked(ctkTrackedService<S,T>* t) const
{
  if (t->size() == 0)
  {
    return QList<ctkServiceReference>();
  }
  return t->getTracked();
}

//----------------------------------------------------------------------------
template<class S, class T>
QSharedPointer<ctkTrackedService<S,T> > ctkServiceTrackerPrivate<S,T>::tracked() const
{
  return trackedService;
}

//----------------------------------------------------------------------------
template<class S, class T>
void ctkServiceTrackerPrivate<S,T>::modified()
{
  cachedReference = 0; /* clear cached value */
  cachedService = 0; /* clear cached value */
  if (DEBUG)
  {
    qDebug() << "ctkServiceTracker::modified:" << filter;
  }
}
