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

template<class Item, class Related>
const bool ctkPluginAbstractTracked<Item,Related>::DEBUG	= true;

template<class Item, class Related>
ctkPluginAbstractTracked<Item,Related>::ctkPluginAbstractTracked()
{
  closed = false;
}

template<class Item, class Related>
ctkPluginAbstractTracked<Item,Related>::~ctkPluginAbstractTracked()
{

}

template<class Item, class Related>
bool ctkPluginAbstractTracked<Item,Related>::wait(unsigned long timeout)
{
  return waitCond.wait(this, timeout);
}

template<class Item, class Related>
void ctkPluginAbstractTracked<Item,Related>::wakeAll()
{
  waitCond.wakeAll();
}

template<class Item, class Related>
void ctkPluginAbstractTracked<Item,Related>::setInitial(const QList<Item>& list)
{
  foreach (Item item, list)
  {
    if (DEBUG)
    {
      qDebug() << "ctkPluginAbstractTracked::setInitial:" << item;
    }
    initial.push_back(item);
  }
}

//template<class Item, class Related>
//void ctkPluginAbstractTracked<Item,Related>::setInitial(const QList<QSharedPointer<Item> >& list)
//{
//  foreach (Item* item, list)
//  {
//    if (item == 0)
//    {
//      continue;
//    }
//    if (DEBUG)
//    {
//      qDebug() << "ctkPluginAbstractTracked::setInitial:" << item;
//    }
//    initial.push_back(item);
//  }
//}

template<class Item, class Related>
void ctkPluginAbstractTracked<Item,Related>::trackInitial()
{
  while (true)
  {
    Item item(0);
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
      if (tracked.value(item) != 0)
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
    trackAdding(item, Related());
    /*
     * Begin tracking it. We call trackAdding
     * since we have already put the item in the
     * adding list.
     */
  }
}

template<class Item, class Related>
void ctkPluginAbstractTracked<Item,Related>::close()
{
  closed = true;
}

template<class Item, class Related>
void ctkPluginAbstractTracked<Item,Related>::track(Item item, Related related)
{
  QVariant object;
  {
    QMutexLocker lock(this);
    if (closed)
    {
      return;
    }
    object = tracked.value(item);
    if (object.isNull())
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

  if (object.isNull())
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

template<class Item, class Related>
void ctkPluginAbstractTracked<Item,Related>::untrack(Item item, Related related)
{
  QVariant object;
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
    if (object.isNull())
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

template<class Item, class Related>
int ctkPluginAbstractTracked<Item,Related>::size() const
{
  return tracked.size();
}

template<class Item, class Related>
QVariant ctkPluginAbstractTracked<Item,Related>::getCustomizedObject(Item item) const
{
  return tracked.value(item);
}

template<class Item, class Related>
QList<Item> ctkPluginAbstractTracked<Item,Related>::getTracked() const
{
  return tracked.keys();
}

template<class Item, class Related>
void ctkPluginAbstractTracked<Item,Related>::modified()
{
  trackingCount.ref();
}

template<class Item, class Related>
int ctkPluginAbstractTracked<Item,Related>::getTrackingCount() const
{
  return trackingCount;
}

template<class Item, class Related>
bool ctkPluginAbstractTracked<Item,Related>::customizerAddingFinal(Item item, const QVariant& custom)
{
  QMutexLocker lock(this);
  if (adding.removeOne(item) && !closed)
  {
    /*
     * if the item was not untracked during the customizer
     * callback
     */
    if (!custom.isNull())
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

template<class Item, class Related>
void ctkPluginAbstractTracked<Item,Related>::trackAdding(Item item, Related related)
{
  if (DEBUG)
  {
    qDebug() << "ctkPluginAbstractTracked::trackAdding:" << item;
  }
  QVariant object;
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
  if (becameUntracked && (!object.isNull()))
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
