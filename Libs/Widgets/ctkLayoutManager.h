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
#include <QStringList>
class QLayoutItem;
class QWidgetItem;

// CTK includes
#include "ctkWidgetsExport.h"
class ctkLayoutManagerPrivate;
class ctkLayoutViewFactory;

/// \ingroup Widgets
/// ctkLayoutManager is a layout manager that populates widgets (viewports)
/// with widgets described into an XML document.
/// To be used, ctkLayoutManager class must be derived and a subset of virtual
/// methods must be reimplemented to support custom views.
/// See below an example of layout XML document:
/// \code
/// <layout type=\"tab\">
///  <item>
///   <layout type=\"horizontal\" split=\"true\">
///    <item splitSize=\"500\"><view/></item>
///    <item splitSize=\"250\">
///     <layout type=\"vertical\">
///      <item><view verticalStretch=\"1\"/></item>
///      <item><view verticalStretch=\"2\"/></item>
///      <item>
///       <layout type=\"grid\">
///        <item row=\"0\" column=\"1\"><view verticalStretch=\"1\"/></item>
///        <item row=\"1\" column=\"0\"><view verticalStretch=\"1\"/></item>
///       </layout>
///      </item>
///     </layout>
///    </item>
///    <item splitSize=\"250\"><view/></item>
///   </layout>
///  </item>
///  <item><view name=\"tab2\"/></item>
///  <item><view name=\"tab3\"/></item>
/// </layout>
/// \endcode
/// The layout elements describe widget containers that embed one or multiple
/// items. Arrangement of items are specified by type attribute of the layout element;
/// supported values: vertical, horizontal, grid, tab.
/// The item elements describe widgets or layouts that are children of
/// layouts.
/// The view elements can be any type of QWidget. viewFromXML() must be
/// reimplemented to return the type(s) of QWidget(s) to use wherever the view
/// element is listed in the layout. The XML element can contain any XML
/// attribute to be parsed by viewFromXML() method.
///
/// For horizontal and vertical layouts, setting split attribute to "true" makes the
/// views resizeable. Default size can be set using splitSize attribute of child items.
///
/// Relative size of views can be adjusted by specifying stretch factors in
/// horizontalStretch and verticalStretch attributes. The stretch factor must be an
/// integer in the range of [0,255].
///
/// For layouts with multiple viewports, specify a top-level "viewports" element and add
/// the layout for each viewport as a nested "layout" element. Set a unique name to each
/// viewport using the "name" property. If name is not specified then the empty string
/// will be used as name. The empty string is a valid name, that is the default viewport.
///
/// \sa ctkSimpleLayoutManager, ctkLayoutViewFactory
class CTK_WIDGETS_EXPORT ctkLayoutManager: public QObject
{
  Q_OBJECT
  /// Spacing between the widgets in all the layouts.
  /// \sa spacing(), setSpacing()
  Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
  /// Viewport names that this widget has encountered so far.
  /// If a viewport appears or disappears from the layout
  /// then the onViewportUsageChanged() method is called.
  /// \sa viewport(), setViewport(), onViewportUsageChanged()
  Q_PROPERTY(QStringList viewportNames READ viewportNames)

public:
  /// Constructor
  ctkLayoutManager(QObject* parent = 0);
  explicit ctkLayoutManager(QWidget* viewport, QObject* parent);

  /// Destructor
  virtual ~ctkLayoutManager();

  /// Set the default viewport widget, i.e., the viewport that uses the empty string as name.
  /// If a only a single viewport is used in the layout document then it is sufficient to set this viewport.
  Q_INVOKABLE void setViewport(QWidget* widget);
  /// Get the default viewport widget.
  Q_INVOKABLE QWidget* viewport()const;

  /// Set a viewport (where the layout can be displayed in) corresponding to a viewport name.
  /// Multiple viewports can be specified by adding a "viewports" XML element and adding multiple
  /// child "layout" elements. Viewport name is specified in the "name" attribute of each "layout" element.
  Q_INVOKABLE void setViewport(QWidget* widget, const QString& viewportName);
  /// Get viewport widget by nam.
  Q_INVOKABLE QWidget* viewport(const QString& viewportName)const;
  /// Get all viewport names.
  QStringList viewportNames()const;
  /// Returns true if the viewport is visible in the current layout.
  Q_INVOKABLE bool isViewportUsedInLayout(const QString& viewportName)const;

  /// Return the spacing property value.
  /// \sa spacing
  int spacing()const;
  /// Set the spacing property value.
  /// \sa spacing
  void setSpacing(int spacing);

  void refresh();

public Q_SLOTS:

Q_SIGNALS:
  void layoutChanged();

protected:
  QScopedPointer<ctkLayoutManagerPrivate> d_ptr;
  ctkLayoutManager(ctkLayoutManagerPrivate* ptr, QWidget* viewport, QObject* parent);

  /// This method is called when a layout uses a viewport that has not been set in the layout manager yet.
  /// Derived classes must implement this method if a layout can contain viewport names that are not already set
  /// by setViewport() calls.
  virtual QWidget* createViewport(const QDomElement& layoutElement, const QString& viewportName);
  /// Called when a viewport widget is set.
  virtual void onViewportChanged();
  /// Called when a viewport is just has been added to or removed from the displayed layout.
  virtual void onViewportUsageChanged(const QString& viewportName);

  void clearLayout();
  virtual void setupLayout();
  virtual void setupViewport(const QDomElement& layoutElement, const QString& viewportName);

  virtual void setLayout(const QDomDocument& newLayout);
  const QDomDocument layout()const;

  /// Create the QLayoutItem for an XML element (e.g. "layout", "view"...)
  /// and its nested elements.
  /// \sa processLayoutElement()
  virtual QLayoutItem* processElement(QDomElement element);
  /// Create the QLayoutItem for a "layout" XML element and its nested elements.
  /// \sa processElement(), layoutFromXML(), processItemElement(), addChildItemToLayout()
  virtual QLayoutItem* processLayoutElement(QDomElement layoutElement);
  /// Create the QLayoutItem for a "layout" XML element.
  /// \sa processLayoutElement()
  virtual QLayoutItem* layoutFromXML(QDomElement layoutElement);
  /// Create the QLayoutItem(s) of the "item" XML element.
  /// \sa processItemElement()
  void                 processItemElement(QDomElement layoutElement, QLayoutItem* layoutItem);
  /// Insert a child item into a layout.
  /// \sa processLayoutElement()
  virtual void         addChildItemToLayout(QDomElement itemElement, QLayoutItem* childItem, QLayoutItem* layoutItem);
  /// Utility method that creates, setups and wraps into a QWidgetItem the widget
  /// of a view XML element.
  /// \sa widgetsItemsFromXML(), viewFromXML()
  QWidgetItem*         widgetItemFromXML(QDomElement layoutElement);
  /// Method is called each time a view is made visible into a layout.
  /// This method can be reimplemented. Sets the widget visibility to true
  /// by default.
  /// \sa viewsFromXML()
  virtual void         setupView(QDomElement layoutElement, QWidget* view);
  /// Create, setup and wrap into QWidgetItems the widgets of a view XML
  /// element.
  QList<QLayoutItem*>  widgetItemsFromXML(QDomElement layoutElement);
  /// Virtual method that returns a widget from a "view" layout element.
  /// You are ensured that the tagName of the element is "view".
  /// The XML element can contain an arbitrary number of XML attributes.
  /// Create the widget if needed or reuse it from a previous call.
  /// \sa viewsFromXML(), setupView()
  virtual QWidget*     viewFromXML(QDomElement layoutElement) = 0;
  /// Virtual method that returns a list of widgets from a "view" layout
  /// element.
  /// If the parent "item" element has a "multiple=true" XML attribute,
  /// the "view" layout element can describe many widgets instead of just one
  /// widget.
  /// The returned widgets will automatically be layout into their parent
  /// layout (e.g. boxlayout).
  /// This method can be reimplemented. Returns viewFromXML() by default.
  /// \sa viewFromXML(),
  virtual QList<QWidget*> viewsFromXML(QDomElement layoutElement);

private:
  Q_DECLARE_PRIVATE(ctkLayoutManager);
  Q_DISABLE_COPY(ctkLayoutManager);
};

#endif
