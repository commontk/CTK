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
#include <QList>
#include <QWidget>

// CTK includes
#include "ctkLayoutFactory.h"
#include "ctkLayoutManager_p.h"
#include "ctkLayoutViewFactory.h"

//-----------------------------------------------------------------------------
class ctkLayoutFactoryPrivate: public ctkLayoutManagerPrivate
{
public:
  ctkLayoutFactoryPrivate(ctkLayoutManager& object);

  /// List of factories to generate views from XML element.
  QList<ctkLayoutViewFactory*> ViewFactories;

  /// Save the factory that was used to generate the view
  QHash<QWidget*, ctkLayoutViewFactory*> ViewFactory;
};

//-----------------------------------------------------------------------------
ctkLayoutFactoryPrivate::ctkLayoutFactoryPrivate(ctkLayoutManager& object)
  : ctkLayoutManagerPrivate(object)
{
}

//-----------------------------------------------------------------------------
// ctkLayoutFactory
//-----------------------------------------------------------------------------
ctkLayoutFactory::ctkLayoutFactory(QObject* parentObject)
  : ctkLayoutManager(new ctkLayoutFactoryPrivate(*this), 0, parentObject)
{
}

//-----------------------------------------------------------------------------
ctkLayoutFactory::ctkLayoutFactory(QWidget* viewport, QObject* parentObject)
  : ctkLayoutManager(new ctkLayoutFactoryPrivate(*this), viewport, parentObject)
{
}

//-----------------------------------------------------------------------------
ctkLayoutFactory::~ctkLayoutFactory()
{
}

//-----------------------------------------------------------------------------
void ctkLayoutFactory::registerViewFactory(ctkLayoutViewFactory* factory)
{
  Q_D(ctkLayoutFactory);
  if (!factory)
    {
    return;
    }
  if (factory->parent() == 0)
    {
    factory->setParent(this);
    }
  d->ViewFactories.push_front(factory);
}

//-----------------------------------------------------------------------------
void ctkLayoutFactory::unregisterViewFactory(ctkLayoutViewFactory* factory)
{
  Q_D(ctkLayoutFactory);
  bool removed = d->ViewFactories.removeOne(factory);
  if (removed && factory->parent() == this)
    {
    factory->deleteLater();
    }
}

//-----------------------------------------------------------------------------
QList<ctkLayoutViewFactory*> ctkLayoutFactory::registeredViewFactories()const
{
  Q_D(const ctkLayoutFactory);
  return d->ViewFactories;
}

//-----------------------------------------------------------------------------
void ctkLayoutFactory::setupLayout()
{
  Q_D(ctkLayoutFactory);
  foreach(ctkLayoutViewFactory* factory, d->ViewFactories)
    {
    factory->beginSetupLayout();
    }
  this->ctkLayoutManager::setupLayout();
  foreach(ctkLayoutViewFactory* factory, d->ViewFactories)
    {
    factory->endSetupLayout();
    }
}

//-----------------------------------------------------------------------------
QWidget* ctkLayoutFactory::viewFromXML(QDomElement viewElement)
{
  Q_D(ctkLayoutFactory);
  QWidget* res = 0;
  QList<ctkLayoutViewFactory*> factories = this->viewFactories(viewElement);
  foreach(ctkLayoutViewFactory* factory, factories)
    {
    res = factory->viewFromXML(viewElement);
    if (res)
      {
      d->ViewFactory[res] = factory;
      break;
      }
    }
  return res;
}

//-----------------------------------------------------------------------------
QList<QWidget*> ctkLayoutFactory::viewsFromXML(QDomElement viewElement)
{
  QWidgetList res;
  QList<ctkLayoutViewFactory*> factories = this->viewFactories(viewElement);
  foreach(ctkLayoutViewFactory* factory, factories)
    {
    res = factory->viewsFromXML(viewElement);
    if (!res.isEmpty())
      {
      break;
      }
    }
  if (res.isEmpty())
    {
    res = this->ctkLayoutManager::viewsFromXML(viewElement);
    }
  return res;
}

//-----------------------------------------------------------------------------
void ctkLayoutFactory::setupView(QDomElement viewElement, QWidget* view)
{
  Q_D(ctkLayoutFactory);
  ctkLayoutViewFactory* factory = d->ViewFactory[view];
  if (factory)
    {
    factory->setupView(viewElement, view);
    d->Views.insert(view);
    }
  else
    {
    this->ctkLayoutManager::setupView(viewElement, view);
    }
}

//-----------------------------------------------------------------------------
QList<ctkLayoutViewFactory*> ctkLayoutFactory::viewFactories(QDomElement viewElement)const
{
  Q_D(const ctkLayoutFactory);
  QList<ctkLayoutViewFactory*> res;
  foreach(ctkLayoutViewFactory* factory, d->ViewFactories)
    {
    if (factory->isElementSupported(viewElement))
      {
      res << factory;
      }
    }
  return res;
}
