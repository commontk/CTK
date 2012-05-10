/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

// Qt includes
#include <QDebug>
#include <QWidget>

// CTK includes
#include "ctkSimpleLayoutManager.h"
#include "ctkLayoutManager_p.h"

//-----------------------------------------------------------------------------
class ctkSimpleLayoutManagerPrivate: public ctkLayoutManagerPrivate
{
public:
  ctkSimpleLayoutManagerPrivate(ctkLayoutManager& object);
  //QMetaObject ViewMetaObject;
  ctkWidgetInstanciator* ViewInstanciator;
};

//-----------------------------------------------------------------------------
ctkSimpleLayoutManagerPrivate::ctkSimpleLayoutManagerPrivate(ctkLayoutManager& object)
  : ctkLayoutManagerPrivate(object)
{
  //this->ViewMetaObject = QWidget::staticMetaObject;
  this->ViewInstanciator = 0;
}

//-----------------------------------------------------------------------------
// ctkSimpleLayoutManager
//-----------------------------------------------------------------------------
ctkSimpleLayoutManager::ctkSimpleLayoutManager(QObject* parentObject)
  : ctkLayoutManager(new ctkSimpleLayoutManagerPrivate(*this), 0, parentObject)
{
}

//-----------------------------------------------------------------------------
ctkSimpleLayoutManager::ctkSimpleLayoutManager(QWidget* viewport, QObject* parentObject)
  : ctkLayoutManager(new ctkSimpleLayoutManagerPrivate(*this), viewport, parentObject)
{
}

//-----------------------------------------------------------------------------
ctkSimpleLayoutManager::~ctkSimpleLayoutManager()
{

}
/*
//-----------------------------------------------------------------------------
void ctkSimpleLayoutManager::setViewMetaObject(const QMetaObject& viewMetaObject)
{
  Q_D(ctkSimpleLayoutManager);
  d->ViewMetaObject = viewMetaObject;
  this->refresh();
}

//-----------------------------------------------------------------------------
const QMetaObject ctkSimpleLayoutManager::viewMetaObject()const
{
  Q_D(const ctkSimpleLayoutManager);
  return d->ViewMetaObject;
}
*/

//-----------------------------------------------------------------------------
void ctkSimpleLayoutManager::setViewInstanciator(ctkWidgetInstanciator* instanciator)
{
  Q_D(ctkSimpleLayoutManager);
  d->ViewInstanciator = instanciator;
  this->refresh();
}

//-----------------------------------------------------------------------------
ctkWidgetInstanciator* ctkSimpleLayoutManager::viewInstanciator()const
{
  Q_D(const ctkSimpleLayoutManager);
  return d->ViewInstanciator;
}

//-----------------------------------------------------------------------------
QWidget* ctkSimpleLayoutManager::viewFromXML(QDomElement viewElement)
{
  Q_UNUSED(viewElement);
  Q_D(ctkSimpleLayoutManager);
  //QObject* newView = d->ViewMetaObject.newInstance();
  //Q_ASSERT(qobject_cast<QWidget*>(newView));
  //return qobject_cast<QWidget*>(newView);
  if (!d->ViewInstanciator)
    {
    return 0;
    }
  return d->ViewInstanciator->createWidget();
}

//-----------------------------------------------------------------------------
void ctkSimpleLayoutManager::setupLayout()
{
  Q_D(ctkSimpleLayoutManager);
  if (d->ViewInstanciator)
    {
    d->ViewInstanciator->beginSetupLayout();
    }
  this->ctkLayoutManager::setupLayout();
  if (d->ViewInstanciator)
    {
    d->ViewInstanciator->endSetupLayout();
    }
}
