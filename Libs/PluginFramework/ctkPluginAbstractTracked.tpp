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


#include "ctkPluginAbstractTracked_p.h"

#include <QDebug>

//----------------------------------------------------------------------------
template<class S, class T, class R>
const bool ctkPluginAbstractTracked<S,T,R>::DEBUG = false;

//----------------------------------------------------------------------------
template<class S, class T, class R>
ctkPluginAbstractTracked<S,T,R>::ctkPluginAbstractTracked()
{
  closed = false;
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
ctkPluginAbstractTracked<S,T,R>::~ctkPluginAbstractTracked()
{

}

//----------------------------------------------------------------------------
template<class S, class T, class R>
bool ctkPluginAbstractTracked<S,T,R>::wait(unsigned long timeout)
{
  return waitCond.wait(this, timeout);
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
void ctkPluginAbstractTracked<S,T,R>::wakeAll()
{
  waitCond.wakeAll();
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
void ctkPluginAbstractTracked<S,T,R>::setInitial(const QList<S>& list)
{
  foreach (S item, list)
  {
    if (DEBUG)
    {
      qDebug() << "ctkPluginAbstractTracked::setInitial:" << item;
    }
    initial.push_back(item);
  }
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
void ctkPluginAbstractTracked<S,T,R>::trackInitial()
{
  while (true)
  {
    S item(0);
    {
      QMutexLocker lock(this);
      if (closed || (initial.size() == 0))
      {
        /*
         * if there are no more initial items
         */
        return; /* we are done */
      }
      /*
       * move the first item from the initial list to the adding list
       * within this synchronized block.
       */
      item = initial.takeFirst();
      if (tracked.value(item))
      {
        /* if we are already tracking this item */
        if (DEBUG)
        {
          qDebug() << "ctkPluginAbstractTracked::trackInitial[already tracked]: " << item;
        }
        continue; /* skip this item */
      }
      if (adding.contains(item))
      {
        /*
         * if this item is already in the process of being added.
         */
        if (DEBUG)
        {
          qDebug() << "ctkPluginAbstractTracked::trackInitial[already adding]: " << item;
        }
        continue; /* skip this item */
      }
      adding.push_back(item);
    }
    if (DEBUG)
    {
      qDebug() << "ctkPluginAbstractTracked::trackInitial: " << item;
    }
    trackAdding(item, R());
    /*
     * Begin tracking it. We call trackAdding
     * since we have already put the item in the
     * adding list.
     */
  }
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
void ctkPluginAbstractTracked<S,T,R>::close()
{
  closed = true;
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
void ctkPluginAbstractTracked<S,T,R>::track(S item, R related)
{
  T object(0);
  {
    QMutexLocker lock(this);
    if (closed)
    {
      return;
    }
    object = tracked.value(item);
    if (!object)
    { /* we are not tracking the item */
      if (adding.contains(item))
      {
        /* if this item is already in the process of being added. */
        if (DEBUG)
        {
          qDebug() << "ctkPluginAbstractTracked::track[already adding]: " << item;
        }
        return;
      }
      adding.push_back(item); /* mark this item is being added */
    }
    else
    { /* we are currently tracking this item */
      if (DEBUG)
      {
        qDebug() << "ctkPluginAbstractTracked::track[modified]: " << item;
      }
      modified(); /* increment modification count */
    }
  }

  if (!object)
  { /* we are not tracking the item */
    trackAdding(item, related);
  }
  else
  {
    /* Call customizer outside of synchronized region */
    customizerModified(item, related, object);
    /*
     * If the customizer throws an unchecked exception, it is safe to
     * let it propagate
     */
  }
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
void ctkPluginAbstractTracked<S,T,R>::untrack(S item, R related)
{
  T object(0);
  {
    QMutexLocker lock(this);
    if (initial.removeOne(item))
    { /* if this item is already in the list
       * of initial references to process
       */
      if (DEBUG)
      {
        qDebug() << "ctkPluginAbstractTracked::untrack[removed from initial]: " << item;
      }
      return; /* we have removed it from the list and it will not be
               * processed
               */
    }

    if (adding.removeOne(item))
    { /* if the item is in the process of
       * being added
       */
      if (DEBUG)
      {
        qDebug() << "ctkPluginAbstractTracked::untrack[being added]: " << item;
      }
      return; /*
           * in case the item is untracked while in the process of
           * adding
           */
    }
    object = tracked.take(item); /*
                     * must remove from tracker before
                     * calling customizer callback
                     */
    if (!object)
    { /* are we actually tracking the item */
      return;
    }
    modified(); /* increment modification count */
  }
  if (DEBUG)
  {
    qDebug() << "ctkPluginAbstractTracked::untrack[removed]: " << item;
  }
  /* Call customizer outside of synchronized region */
  customizerRemoved(item, related, object);
  /*
   * If the customizer throws an unchecked exception, it is safe to let it
   * propagate
   */
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
int ctkPluginAbstractTracked<S,T,R>::size() const
{
  return tracked.size();
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
bool ctkPluginAbstractTracked<S,T,R>::isEmpty() const
{
  return tracked.isEmpty();
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
T ctkPluginAbstractTracked<S,T,R>::getCustomizedObject(S item) const
{
  return tracked.value(item);
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
QList<S> ctkPluginAbstractTracked<S,T,R>::getTracked() const
{
  return tracked.keys();
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
void ctkPluginAbstractTracked<S,T,R>::modified()
{
  trackingCount.ref();
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
int ctkPluginAbstractTracked<S,T,R>::getTrackingCount() const
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
  return trackingCount;
#else
  return trackingCount.load();
#endif
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
QMap<S,T> ctkPluginAbstractTracked<S,T,R>::copyEntries(QMap<S,T>& map) const
{
  typename QHash<S,T>::ConstIterator end = tracked.end();
  for (typename QHash<S,T>::ConstIterator it = tracked.begin();
       it != end; ++it)
  {
    map.insert(it.key(), it.value());
  }
  return map;
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
bool ctkPluginAbstractTracked<S,T,R>::customizerAddingFinal(S item, const T& custom)
{
  QMutexLocker lock(this);
  if (adding.removeOne(item) && !closed)
  {
    /*
     * if the item was not untracked during the customizer
     * callback
     */
    if (custom)
    {
      tracked.insert(item, custom);
      modified(); /* increment modification count */
      waitCond.wakeAll(); /* notify any waiters */
    }
    return false;
  }
  else
  {
    return true;
  }
}

//----------------------------------------------------------------------------
template<class S, class T, class R>
void ctkPluginAbstractTracked<S,T,R>::trackAdding(S item, R related)
{
  if (DEBUG)
  {
    qDebug() << "ctkPluginAbstractTracked::trackAdding:" << item;
  }
  T object(0);
  bool becameUntracked = false;
  /* Call customizer outside of synchronized region */
  try
  {
    object = customizerAdding(item, related);
    becameUntracked = this->customizerAddingFinal(item, object);
  }
  catch (...)
  {
    /*
     * If the customizer throws an exception, it will
     * propagate after the cleanup code.
     */
    this->customizerAddingFinal(item, object);
    throw;
  }

  /*
   * The item became untracked during the customizer callback.
   */
  if (becameUntracked && object)
  {
    if (DEBUG)
    {
      qDebug() << "ctkPluginAbstractTracked::trackAdding[removed]: " << item;
    }
    /* Call customizer outside of synchronized region */
    customizerRemoved(item, related, object);
    /*
     * If the customizer throws an unchecked exception, it is safe to
     * let it propagate
     */
  }
}
