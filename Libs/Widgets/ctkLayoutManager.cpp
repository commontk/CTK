/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

// CTK includes
#include "ctkLayoutManager.h"
#include "ctkLayoutManager_p.h"

//-----------------------------------------------------------------------------
ctkLayoutManagerPrivate::ctkLayoutManagerPrivate(ctkLayoutManager& object)
  :q_ptr(&object)
{
  this->Viewport = 0;
}

//-----------------------------------------------------------------------------
ctkLayoutManagerPrivate::~ctkLayoutManagerPrivate()
{
}

//-----------------------------------------------------------------------------
void ctkLayoutManagerPrivate::init()
{
  //Q_Q(ctkLayoutManager);
}

//-----------------------------------------------------------------------------
void ctkLayoutManagerPrivate::clearLayout(QLayout* layout)
{
  if (!layout)
    {
    return;
    }
  QLayoutItem * layoutItem = 0;
  while ((layoutItem = layout->takeAt(0)) != 0)
    {
    if (layoutItem->widget())
      {
      layoutItem->widget()->setVisible(false);
      layout->removeWidget(layoutItem->widget());
      }
    else if (layoutItem->layout())
      {
      this->clearLayout(layoutItem->layout());
      }
    }
  if (layout->parentWidget() && layout->parentWidget()->layout() == layout)
    {
    delete layout;
    }
}

//-----------------------------------------------------------------------------
// ctkLayoutManager
//-----------------------------------------------------------------------------
ctkLayoutManager::ctkLayoutManager(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkLayoutManagerPrivate(*this))
{
  Q_D(ctkLayoutManager);
  d->init();
}

//-----------------------------------------------------------------------------
ctkLayoutManager::ctkLayoutManager(QWidget* viewport, QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkLayoutManagerPrivate(*this))
{
  Q_D(ctkLayoutManager);
  d->init();
  this->setViewport(viewport);
}

//-----------------------------------------------------------------------------
ctkLayoutManager::ctkLayoutManager(ctkLayoutManagerPrivate* ptr,
                                   QWidget* viewport, QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(ptr)
{
  Q_D(ctkLayoutManager);
  d->init();
  this->setViewport(viewport);
}

//-----------------------------------------------------------------------------
ctkLayoutManager::~ctkLayoutManager()
{

}

//-----------------------------------------------------------------------------
void ctkLayoutManager::refresh()
{
  Q_D(ctkLayoutManager);
  if (!d->Viewport)
    {
    return;
    }
  // TODO: post an event on the event queue
  bool updatesEnabled = d->Viewport->updatesEnabled();
  d->Viewport->setUpdatesEnabled(false);
  this->clearLayout();
  this->setupLayout();
  d->Viewport->setUpdatesEnabled(updatesEnabled);
}

//-----------------------------------------------------------------------------
void ctkLayoutManager::clearLayout()
{
  Q_D(ctkLayoutManager);
  if (!d->Viewport)
    {
    return;
    }
  // TODO: post an event on the event queue
  d->clearLayout(d->Viewport->layout());
}

//-----------------------------------------------------------------------------
void ctkLayoutManager::setupLayout()
{
  Q_D(ctkLayoutManager);
  if (!d->Viewport || d->Layout.isNull() ||
      d->Layout.documentElement().isNull())
    {
    return;
    }
  d->Views.clear();
  Q_ASSERT(!d->Viewport->layout());
  QLayoutItem* layoutItem = this->processElement(
    d->Layout.documentElement());
  Q_ASSERT(layoutItem);
  QLayout* layout = layoutItem->layout();
  if (!layout)
    {
    QHBoxLayout* hboxLayout = new QHBoxLayout(0);
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    hboxLayout->addItem(layoutItem);
    layout = hboxLayout;
    }
  // setting the layout to the widget will reparent all the 1 level widgets.
  // Unfortunately, it has the side effect of hiding
  // (testAttribute(Qt::WA_WState_Hidden)) the widgets that were already having
  // a parent (read doc for QWidget::isHidden()).
  // we then need to manually display the widgets again. Views is not probably
  // not the best list to use to retrieve the widgets to remove the hidden flag
  // it seems to fit the bill for the moment so we can keep using it.
  d->Viewport->setLayout(layout);
  foreach(QWidget* view, d->Views)
    {
    view->setHidden(false);
    }
}

//-----------------------------------------------------------------------------
void ctkLayoutManager::setViewport(QWidget* viewport)
{
  Q_D(ctkLayoutManager);
  if (viewport == d->Viewport)
    {
    return;
    }
  this->clearLayout();
  foreach(QWidget* view, d->Views)
    {
    if (view->parent() == d->Viewport)
      {
      view->setParent(0);
      // reparenting looses the visibility attribute and we want them hidden
      view->setVisible(false);
      }
    }
  d->Viewport = viewport;
  this->onViewportChanged();
}

//-----------------------------------------------------------------------------
QWidget* ctkLayoutManager::viewport()const
{
  Q_D(const ctkLayoutManager);
  return d->Viewport;
}

//-----------------------------------------------------------------------------
void ctkLayoutManager::onViewportChanged()
{
  this->refresh();
}

//-----------------------------------------------------------------------------
void ctkLayoutManager::setLayout(const QDomDocument& newLayout)
{
  Q_D(ctkLayoutManager);
  if (newLayout == d->Layout)
    {
    return;
    }
  d->Layout = newLayout;
  this->refresh();
}

//-----------------------------------------------------------------------------
const QDomDocument ctkLayoutManager::layout()const
{
  Q_D(const ctkLayoutManager);
  return d->Layout;
}

//-----------------------------------------------------------------------------
QLayoutItem* ctkLayoutManager::processElement(QDomElement element)
{
  Q_ASSERT(!element.isNull());
  if (element.tagName() == "layout")
    {
    return this->processLayoutElement(element);
    }
  else if (element.tagName() == "view")
    {
    return this->widgetItemFromXML(element);
    }
  qDebug() << element.tagName() << element.text();
  Q_ASSERT(element.tagName() != "layout" && element.tagName() != "view");
  return 0;
}

//-----------------------------------------------------------------------------
QLayoutItem* ctkLayoutManager::processLayoutElement(QDomElement layoutElement)
{
  Q_ASSERT(layoutElement.tagName() == "layout");

  QLayoutItem* layoutItem = this->layoutFromXML(layoutElement);
  QLayout* layout = layoutItem->layout();

  if (layout)
    {
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    }
  for(QDomNode child = layoutElement.firstChild();
      !child.isNull();
      child = child.nextSibling())
    {
    // ignore children that are not QDomElement
    if (child.toElement().isNull())
      {
      continue;
      }
    this->processItemElement(child.toElement(), layoutItem);
    }
  return layoutItem;
}

//-----------------------------------------------------------------------------
QLayoutItem* ctkLayoutManager::layoutFromXML(QDomElement layoutElement)
{
  Q_ASSERT(layoutElement.tagName() == "layout");
  QString type = layoutElement.attribute("type", "horizontal");
  bool split = layoutElement.attribute("split", "false") == "true";
  if (type == "vertical")
    {
    if (split)
      {
      return new QWidgetItem(new QSplitter(Qt::Vertical));
      }
    return new QVBoxLayout();
    }
  else if (type == "horizontal")
    {
    if (split)
      {
      return new QWidgetItem(new QSplitter(Qt::Horizontal));
      }
    return new QHBoxLayout();
    }
  else if (type == "grid")
    {
    return new QGridLayout();
    }
  else if (type == "tab")
    {
    return new QWidgetItem(new QTabWidget());
    }
  return 0;
}

//-----------------------------------------------------------------------------
void ctkLayoutManager::processItemElement(QDomElement itemElement, QLayoutItem* layoutItem)
{
  Q_ASSERT(itemElement.tagName() == "item");
  Q_ASSERT(itemElement.childNodes().count() == 1);
  bool multiple = itemElement.attribute("multiple", "false") == "true";
  QList<QLayoutItem*> childrenItem;
  if (multiple)
    {
    childrenItem = this->widgetItemsFromXML(itemElement.firstChild().toElement());
    }
  else
    {
    childrenItem << this->processElement(itemElement.firstChild().toElement());
    }
  foreach(QLayoutItem* item, childrenItem)
    {
    this->addChildItemToLayout(itemElement, item, layoutItem);
    }
}

//-----------------------------------------------------------------------------
void ctkLayoutManager::addChildItemToLayout(QDomElement itemElement, QLayoutItem* childItem, QLayoutItem* layoutItem)
{
  Q_ASSERT(childItem);
  QString itemName = itemElement.attribute("name");
  if (itemName.isEmpty() && childItem->widget())
    {
    itemName = childItem->widget()->windowTitle();
    }
  QLayout* layout = layoutItem->layout();
  QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout);
  QLayout* genericLayout = qobject_cast<QLayout*>(layout);
  QTabWidget* tabWidget = qobject_cast<QTabWidget*>(layoutItem->widget());
  QSplitter* splitter = qobject_cast<QSplitter*>(layoutItem->widget());
  if (gridLayout)
    {
    int row = itemElement.attribute("row", QString::number(0)).toInt();
    int col = itemElement.attribute("column", QString::number(0)).toInt();
    int rowSpan = itemElement.attribute("rowspan", QString::number(1)).toInt();
    int colSpan = itemElement.attribute("colspan", QString::number(1)).toInt();
    gridLayout->addItem(childItem, row, col, rowSpan, colSpan);
    }
  else if (genericLayout)
    {
    genericLayout->addItem(childItem);
    }
  else if (tabWidget || splitter)
    {
    QWidget* childWidget = childItem->widget();
    if (!childWidget)
      {
      childWidget = new QWidget();
      childWidget->setLayout(childItem->layout());
      }
    if (tabWidget)
      {
      tabWidget->addTab(childWidget, itemName);
      }
    else
      {
      splitter->addWidget(childWidget);
      }
    }
}

//-----------------------------------------------------------------------------
QWidgetItem* ctkLayoutManager::widgetItemFromXML(QDomElement viewElement)
{
  Q_ASSERT(viewElement.tagName() == "view");
  QWidget* view = this->viewFromXML(viewElement);
  this->setupView(viewElement, view);
  return new QWidgetItem(view);
}

//-----------------------------------------------------------------------------
void ctkLayoutManager::setupView(QDomElement viewElement, QWidget* view)
{
  Q_UNUSED(viewElement);
  Q_D(ctkLayoutManager);
  Q_ASSERT(view);
  view->setVisible(true);
  d->Views.insert(view);
}

//-----------------------------------------------------------------------------
QList<QLayoutItem*> ctkLayoutManager::widgetItemsFromXML(QDomElement viewElement)
{
  Q_ASSERT(viewElement.tagName() == "view");
  QList<QLayoutItem*> res;
  QList<QWidget*> views = this->viewsFromXML(viewElement);
  Q_ASSERT(views.count());
  foreach(QWidget* view, views)
    {
    this->setupView(viewElement, view);
    res << new QWidgetItem(view);
    }
  return res;
}

//-----------------------------------------------------------------------------
QWidget* ctkLayoutManager::viewFromXML(QDomElement viewElement)
{
  Q_UNUSED(viewElement);
  // default, for testing purpose. Must be reimplemented
  return new QWidget(0);
}

//-----------------------------------------------------------------------------
QList<QWidget*> ctkLayoutManager::viewsFromXML(QDomElement viewElement)
{
  QList<QWidget*> res;
  res << this->viewFromXML(viewElement);
  return res;
}
