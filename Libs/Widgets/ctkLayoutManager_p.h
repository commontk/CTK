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

#ifndef __ctkLayoutManager_p_h
#define __ctkLayoutManager_p_h

// Qt includes
#include <QDomDocument>
#include <QMap>
#include <QObject>
#include <QSet>

class QLayout;
class QWidget;

// CTK includes
#include "ctkLayoutManager.h"

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkLayoutManagerPrivate
{
  Q_DECLARE_PUBLIC(ctkLayoutManager);

protected:
  ctkLayoutManager* const q_ptr;

public:
  ctkLayoutManagerPrivate(ctkLayoutManager& object);
  virtual ~ctkLayoutManagerPrivate();

  virtual void init();
  void clearLayout(QLayout* layout);
  void clearWidget(QWidget* widget, QLayout* parentLayout = 0);
  /// Get viewport where the layout widget is displayed in.
  QWidget* viewportForWidget(QWidget* widget)const;
  /// Get viewport by name.
  QWidget* viewport(const QString& viewportName)const;
  /// Get viewport name where the layout widget is displayed in.
  QString viewportNameForWidget(QWidget* widget)const;
  /// Set viewport name where the layout widget is displayed in.
  void setViewportNameForWidget(QWidget* widget, const QString& viewportName);
  /// Return true if the viewport is used in a layout.
  /// It is stored in a custom property of the layout widget.
  bool isViewportUsedInLayout(QWidget* viewport)const;
  /// Set flag indicating whether the viewport is used in a layout.
  void setViewportUsedInLayout(QWidget* viewport, bool owned);

  /// Widgets where the layout is displayed in. Maps viewport name to viewport widget.
  /// These widgets are not creatd or owned by the ctkLayoutManager class
  /// but either in derived classes; or they can be also set externally.
  QMap<QString, QWidget*> Viewports;
  /// The XML description of the current layout.
  QDomDocument   Layout;
  /// All the QWidgets in the Viewports. The list contains the
  /// LayoutWidgets as well as the "items" of the layout.
  QSet<QWidget*> Views;
  /// All the widgets created by ctkLayoutManager.
  /// Those widgets are "layout" widgets in a sense that they are simple
  /// containers or spacers to layout the "views" of the layout.
  QSet<QWidget*> LayoutWidgets;
  /// Unique spacing used by all the inner layouts.
  int            Spacing;
};

#endif
