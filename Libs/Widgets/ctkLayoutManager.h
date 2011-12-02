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

#ifndef __ctkLayoutManager_h
#define __ctkLayoutManager_h

// Qt includes
#include <QObject>
#include <QDomDocument>
class QLayoutItem;
class QWidgetItem;

// CTK includes
#include "ctkWidgetsExport.h"
class ctkLayoutManagerPrivate;

/// \ingroup Widgets
/// ctkLayoutManager is
class CTK_WIDGETS_EXPORT ctkLayoutManager: public QObject
{
  Q_OBJECT
  /// Spacing between the widgets in a layout
  Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
public:
  /// Constructor
  ctkLayoutManager(QObject* parent = 0);
  explicit ctkLayoutManager(QWidget* viewport, QObject* parent);

  /// Destructor
  virtual ~ctkLayoutManager();

  void setViewport(QWidget* widget);
  QWidget* viewport()const;

  int spacing()const;
  void setSpacing(int spacing);

  void refresh();

public Q_SLOTS:

Q_SIGNALS:
  void layoutChanged();

protected:
  QScopedPointer<ctkLayoutManagerPrivate> d_ptr;
  ctkLayoutManager(ctkLayoutManagerPrivate* ptr, QWidget* viewport, QObject* parent);

  virtual void onViewportChanged();
  void clearLayout();
  void setupLayout();

  virtual void setLayout(const QDomDocument& newLayout);
  const QDomDocument layout()const;

  virtual QLayoutItem* processElement(QDomElement element);
  virtual QLayoutItem* processLayoutElement(QDomElement layoutElement);
  virtual QLayoutItem* layoutFromXML(QDomElement layoutElement);
  void                 processItemElement(QDomElement layoutElement, QLayoutItem* layoutItem);
  virtual void         addChildItemToLayout(QDomElement itemElement, QLayoutItem* childItem, QLayoutItem* layoutItem);
  QWidgetItem*         widgetItemFromXML(QDomElement layoutElement);
  virtual void         setupView(QDomElement layoutElement, QWidget* view);
  QList<QLayoutItem*>  widgetItemsFromXML(QDomElement layoutElement);
  virtual QWidget*     viewFromXML(QDomElement layoutElement);
  virtual QList<QWidget*> viewsFromXML(QDomElement layoutElement);

private:
  Q_DECLARE_PRIVATE(ctkLayoutManager);
  Q_DISABLE_COPY(ctkLayoutManager);
};

#endif
