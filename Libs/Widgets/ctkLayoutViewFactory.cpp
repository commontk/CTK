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
#include <QDomElement>
#include <QList>
#include <QPair>
#include <QVector>
#include <QWidget>

// CTK includes
#include "ctkLayoutViewFactory.h"

class ctkLayoutViewFactoryPrivate
{
  Q_DECLARE_PUBLIC(ctkLayoutViewFactory);
public:
  ctkLayoutViewFactoryPrivate(ctkLayoutViewFactory& obj);
  ~ctkLayoutViewFactoryPrivate();
  void init();

protected:
  ctkLayoutViewFactory* q_ptr;
  typedef QPair<QDomElement, QWidget*> ViewFactory;
  QVector<ViewFactory> Views;
  bool UseCachedViews;
  int NumberOfViewsInCurrentLayout;
};

//-----------------------------------------------------------------------------
ctkLayoutViewFactoryPrivate::ctkLayoutViewFactoryPrivate(ctkLayoutViewFactory& object)
  : q_ptr(&object)
  , UseCachedViews(true)
  , NumberOfViewsInCurrentLayout( 0 )
{
}

//-----------------------------------------------------------------------------
ctkLayoutViewFactoryPrivate::~ctkLayoutViewFactoryPrivate()
{
}

//-----------------------------------------------------------------------------
void ctkLayoutViewFactoryPrivate::init()
{
}

//-----------------------------------------------------------------------------
// ctkLayoutViewFactory
//-----------------------------------------------------------------------------
ctkLayoutViewFactory::ctkLayoutViewFactory(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkLayoutViewFactoryPrivate(*this))
{
  Q_D(ctkLayoutViewFactory);
  d->init();
}

//-----------------------------------------------------------------------------
ctkLayoutViewFactory::~ctkLayoutViewFactory()
{
}

//-----------------------------------------------------------------------------
QStringList ctkLayoutViewFactory::supportedElementNames()const
{
  return QStringList() << "view";
}

//-----------------------------------------------------------------------------
bool ctkLayoutViewFactory::isElementSupported(QDomElement layoutElement)const
{
  return this->supportedElementNames().contains(layoutElement.tagName());
}

//-----------------------------------------------------------------------------
bool ctkLayoutViewFactory::useCachedViews()const
{
  Q_D(const ctkLayoutViewFactory);
  return d->UseCachedViews;
}

//-----------------------------------------------------------------------------
void ctkLayoutViewFactory::setUseCachedViews(bool cache)
{
  Q_D(ctkLayoutViewFactory);
  d->UseCachedViews = cache;
}

//-----------------------------------------------------------------------------
void ctkLayoutViewFactory::beginSetupLayout()
{
  Q_D(ctkLayoutViewFactory);
  // A new layout is set, reset the number of views being used in the layout.
  d->NumberOfViewsInCurrentLayout = 0;
}

//-----------------------------------------------------------------------------
void ctkLayoutViewFactory::endSetupLayout()
{
}

//-----------------------------------------------------------------------------
void ctkLayoutViewFactory::setupView(QDomElement viewElement, QWidget* view)
{
  Q_ASSERT(view);
  view->setVisible(true);
  this->registerView(viewElement, view);
}

//-----------------------------------------------------------------------------
QWidget* ctkLayoutViewFactory::viewFromXML(QDomElement layoutElement)
{
  Q_D(ctkLayoutViewFactory);
  QWidgetList views = this->registeredViews(layoutElement);
  // The same XML element has already been processed, reuse the view associated
  // to it.
  if (views.count())
    {
    return views[0];
    }
  // The layout element does not match any existing one, however we can just reuse
  // one that was registered for a different layout element.
  if (this->useCachedViews() &&
      d->NumberOfViewsInCurrentLayout >= 0 && 
      d->NumberOfViewsInCurrentLayout < d->Views.count())
    {
    QWidget* view = d->Views[d->NumberOfViewsInCurrentLayout].second;
    return view;
    }
  return this->createViewFromXML(layoutElement);
}

//-----------------------------------------------------------------------------
QWidget* ctkLayoutViewFactory::createViewFromXML(QDomElement layoutElement)
{
  Q_UNUSED(layoutElement);
  return 0;
}

//-----------------------------------------------------------------------------
QList<QWidget*> ctkLayoutViewFactory::viewsFromXML(QDomElement layoutElement)
{
  Q_D(ctkLayoutViewFactory);
  QWidgetList views = this->registeredViews(layoutElement);
  if (views.count())
    {
    return views;
    }
  // The layout element does not match any existing one, however we can just reuse
  // one that was registered for a different layout element.
  if (this->useCachedViews() &&
      d->NumberOfViewsInCurrentLayout >= 0 && 
      d->NumberOfViewsInCurrentLayout < d->Views.count())
    {
    for (int i = d->NumberOfViewsInCurrentLayout; i < d->Views.count(); ++i)
      {
      views << d->Views[i].second;
      }
    return views;
    }
  return this->createViewsFromXML(layoutElement);
}

//-----------------------------------------------------------------------------
QList<QWidget*> ctkLayoutViewFactory::createViewsFromXML(QDomElement layoutElement)
{
  QWidgetList views;
  QWidget* view = this->createViewFromXML(layoutElement);
  if (view)
    {
    views << view;
    }
  return views;
}

//-----------------------------------------------------------------------------
QList<QWidget*> ctkLayoutViewFactory::registeredViews()const
{
  Q_D(const ctkLayoutViewFactory);
  QWidgetList res;
  foreach(ctkLayoutViewFactoryPrivate::ViewFactory p, d->Views)
    {
    res << p.second;
    }
  return res;
}

//-----------------------------------------------------------------------------
QList<QWidget*> ctkLayoutViewFactory
::registeredViews(const QDomElement& layoutElement)const
{
  Q_D(const ctkLayoutViewFactory);
  QWidgetList res;
  foreach(ctkLayoutViewFactoryPrivate::ViewFactory p, d->Views)
    {
    if (p.first == layoutElement)
      {
      res << p.second;
      }
    }
  return res;
}

//-----------------------------------------------------------------------------
void ctkLayoutViewFactory::registerView(QDomElement layoutElement, QWidget* view)
{
  Q_D(ctkLayoutViewFactory);
  QDomElement viewElement = this->layoutElement(view);
  if (!viewElement.isNull())
    { // replace the current view element with the new layout element.
    ctkLayoutViewFactoryPrivate::ViewFactory item(viewElement, view);
    int index = d->Views.indexOf(item);
    Q_ASSERT(index >= 0);
    d->Views[index].first = layoutElement;
    }
  else
    {
    d->Views.push_back(ctkLayoutViewFactoryPrivate::ViewFactory(layoutElement, view));
    }
  ++d->NumberOfViewsInCurrentLayout;
}

//-----------------------------------------------------------------------------
void ctkLayoutViewFactory::unregisterView(QDomElement layoutElement, QWidget* view)
{
  Q_D(ctkLayoutViewFactory);
  ctkLayoutViewFactoryPrivate::ViewFactory itemToRemove(layoutElement, view);
  for (int index = d->Views.indexOf(itemToRemove) ; index >= 0 ;
       index = d->Views.indexOf(itemToRemove))
    {
    d->Views.remove(index);
    }
}

//-----------------------------------------------------------------------------
void ctkLayoutViewFactory::unregisterView(QWidget* view)
{
  for (QDomElement viewElement = this->layoutElement(view);
       !viewElement.isNull();
       viewElement = this->layoutElement(view))
    {
    this->unregisterView(viewElement, view);
    }
}

//-----------------------------------------------------------------------------
QDomElement ctkLayoutViewFactory::layoutElement(QWidget* view)const
{
  Q_D(const ctkLayoutViewFactory);
  for (int index = 0 ; index < d->Views.count(); ++index)
    {
    if (d->Views[index].second == view)
      {
      return d->Views[index].first;
      }
    }
  return QDomElement();
}
