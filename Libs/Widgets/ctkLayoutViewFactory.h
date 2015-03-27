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

#ifndef __ctkLayoutViewFactory_h
#define __ctkLayoutViewFactory_h

// Qt includes
#include <QObject>
#include <QDomDocument>
class QLayoutItem;
class QWidgetItem;

// CTK includes
#include "ctkWidgetsExport.h"
class ctkLayoutViewFactoryPrivate;

/// \ingroup Widgets
/// ctkLayoutViewFactory is an abstract class that creates and setups widgets for a
/// specific view XML element from ctkLayoutManager.
/// See ctkTemplateLayoutViewFactory for a factory that can instantiate
/// any Qt widget.
/// This class is meant to be derived with at least the method createViewFromXML()
/// being overwritten.
/// \sa ctkLayoutManager, ctkTemplateLayoutViewFactory, createViewFromXML()
class CTK_WIDGETS_EXPORT ctkLayoutViewFactory: public QObject
{
  Q_OBJECT
  /// This property controls whether the views are cached and reused
  /// from a previous layout. True by default.
  /// \sa useCachedViews(), setUseCachedViews()
  Q_PROPERTY(bool useCachedViews READ useCachedViews WRITE setUseCachedViews);
public:
  /// Constructor
  ctkLayoutViewFactory(QObject* parent = 0);

  /// Destructor
  virtual ~ctkLayoutViewFactory();

  /// Returns the list of element names supported by the factory (e.g. "view",
  /// "myview"...). Returns ["view"] by default.
  /// Can be reimplemented to support other element names.
  /// \warning It is not possible to support the "layout" and "item" XML
  /// elements.
  /// \sa isSupportedElement()
  virtual QStringList supportedElementNames()const;

  /// Returns true if the layout element can be created and setup by the
  /// factory. By default, returns true if the layout element name is
  /// contained in supportedElementNames().
  /// \sa supportedElementNames()
  virtual bool isElementSupported(QDomElement layoutElement)const;

  /// Return the useCachedViews property value.
  /// \sa useCachedViews
  bool useCachedViews()const;
  /// Set the useCachedViews property value.
  /// \sa useCachedViews
  void setUseCachedViews(bool cache);

  /// Called by the layout factory before a layout is setup.
  /// \sa endSetupLayout()
  virtual void beginSetupLayout();
  /// Called by the layout factory after a layout is setup.
  /// \sa beginSetupLayout()
  virtual void endSetupLayout();

  /// Method is called each time a view is made visible into a layout.
  /// This method can be reimplemented. Sets the widget visibility to true
  /// and register the view by default.
  /// \sa viewsFromXML()
  virtual void setupView(QDomElement layoutElement, QWidget* view);
  /// Virtual method that returns a widget from a "view" layout element.
  /// You are ensured that the tagName of the element is one of the element
  /// from the supportedElementNames() list.
  /// The XML element can contain an arbitrary number of XML attributes.
  /// Returns previously registered or cached view if any, otherwise create
  /// a new view using createViewFromXML(). Must be reimplemented.
  /// \sa viewsFromXML(), setupView(), registeredViews(), useCachedView,
  /// createViewFromXML()
  virtual QWidget* viewFromXML(QDomElement layoutElement);
  /// Virtual method that returns a list of widgets from a "view" layout
  /// element.
  /// If the parent "item" element has a "multiple=true" XML attribute,
  /// the "view" layout element can describe many widgets instead of just one
  /// widget.
  /// The returned widgets will automatically be layout into their parent
  /// layout (e.g. boxlayout).
  /// Returns previously registered or cached views if any, otherwise create
  /// new views using createViewsFromXML().
  /// \sa viewFromXML(), registeredViews(), 
  virtual QList<QWidget*> viewsFromXML(QDomElement layoutElement);

  /// Return all the widgets that have been registered.
  /// Used internally for testing.
  /// \sa registeredViews()
  QList<QWidget*> registeredViews()const;
protected:
  QScopedPointer<ctkLayoutViewFactoryPrivate> d_ptr;

  /// Create a new view from a layoutElement. Returns 0 by default
  /// \sa viewFromXML
  virtual QWidget* createViewFromXML(QDomElement layoutElement);
  /// Create new views from a layoutElement. Returns createViewFromXML()
  /// by default.
  /// \sa viewsFromXML(), createViewFromXML()
  virtual QList<QWidget*> createViewsFromXML(QDomElement layoutElement);

  /// Return the list of widgets that have already been created by
  /// view(s)FromXML().
  /// \sa registerView(), unregisterView()
  QList<QWidget*> registeredViews(const QDomElement& layoutElement)const;

  /// Save the view to reuse it later on (in view(s)FromXML() ).
  /// \sa registerView(), registeredViews()
  virtual void registerView(QDomElement layoutElement, QWidget* view);

  /// Forget about the view. The view is not being deleted.
  /// \sa registerView(), registeredViews()
  virtual void unregisterView(QDomElement layoutElement, QWidget* view);

  /// Forget about the view. The view is not being deleted.
  /// \sa registerView(), unregisterView()
  virtual void unregisterView(QWidget* view);

  /// Return the layout element registered with the view or a null element
  /// if the view is not registered.
  /// \sa registerView(), unregisterView()
  QDomElement layoutElement(QWidget* view)const;

private:
  Q_DECLARE_PRIVATE(ctkLayoutViewFactory);
  Q_DISABLE_COPY(ctkLayoutViewFactory);
};

/// \ingroup Widgets
/// Instantiate a template QWidget anytime a view element is listed
/// inside a layout element.
/// \code
/// QString tabLayout(
///   "<layout type=\"tab\">"
///   " <item><view name=\"tab1\"/></item>"
///   " <item><view name=\"tab2\"/></item>"
///   " <item><view name=\"tab3\"/></item>"
///   "</layout>");
/// QWidget tab;
/// tab.setWindowTitle("Tab Layout");
/// ctkTemplateLayoutViewFactory<QPushButton>* buttonFactory =
///   new ctkTemplateLayoutViewFactory<QPushButton>(&tab);
/// ctkLayoutFactory tabLayoutManager;
/// tabLayoutManager.registerViewFactory(buttonFactory);
/// tabToSimpleLayoutManager.setLayout(tabLayoutDoc);
/// tabToSimpleLayoutManager.setViewport(&tab);
/// tabToSimple.show();
/// \endcode
/// \sa ctkLayoutViewFactory
template<class T>
class ctkTemplateLayoutViewFactory: public ctkLayoutViewFactory
{
public:
  ctkTemplateLayoutViewFactory(QObject* parent = 0)
    : ctkLayoutViewFactory(parent)
  {
    this->setUseCachedViews(true);
  }
  virtual QWidget* createViewFromXML(QDomElement layoutElement){
    Q_UNUSED(layoutElement);
    return new T;
    }
};

#endif
