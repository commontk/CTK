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

#ifndef __ctkLayoutFactory_h
#define __ctkLayoutFactory_h

// Qt includes
#include <QMetaObject>

// CTK includes
#include "ctkLayoutManager.h"
class ctkLayoutFactoryPrivate;

/// \ingroup Widgets
/// This class gives the ability to control externally the
/// instantiation of view widgets by registering view factories.
/// Depending on the view XML element in the layout document, the best
/// registered view factory is being used to create and setup the corresponding
/// widget.
/// \sa ctkLayoutManager, ctkViewFactory
class CTK_WIDGETS_EXPORT ctkLayoutFactory: public ctkLayoutManager
{
  Q_OBJECT
public:
  ctkLayoutFactory(QObject* parent = 0);
  explicit ctkLayoutFactory(QWidget* viewport, QObject* parent);
  virtual ~ctkLayoutFactory();

  using ctkLayoutManager::setLayout;
  using ctkLayoutManager::layout;

  /// Register a view factory.
  /// The factory is prepended to the list of factories.
  /// If the factory has no parent, ctkLayoutManager takes ownership.
  /// Otherwise you should make sure the factory is not deleted until the
  /// factory is unregisted or until the manager is deleted.
  /// \sa unregisterViewFactory(), registerViewFactories()
  void registerViewFactory(ctkLayoutViewFactory* factory);
  /// Unregister a view factory.
  /// If the factory is owned by the ctkLayoutManager, the factory is deleted.
  /// \sa registerViewFactory(), registerViewFactories()
  void unregisterViewFactory(ctkLayoutViewFactory* factory);

  /// Return the list of view factories that are registered.
  /// \sa registerViewFactory(), unregisterViewFactory()
  QList<ctkLayoutViewFactory*> registeredViewFactories()const;

protected:
  /// Call beginSetupLayout() and endSetupLayout() on all the registeredfactories.
  /// \sa setupView()
  virtual void setupLayout();
  /// Find the layoutElement factory and call viewFromXML() on it.
  /// \sa viewsFromXML(), setupView()
  virtual QWidget* viewFromXML(QDomElement layoutElement);
  /// Find the layoutElement factory and call viewsFromXML() on it.
  /// \sa viewFromXML(), setupView()
  virtual QList<QWidget*> viewsFromXML(QDomElement layoutElement);
  /// Find the layoutElement factory and setupView() on it.
  /// \sa viewFromXML(), viewsFromXML()
  virtual void setupView(QDomElement layoutElement, QWidget* view);

  /// Return all the registered factories that can handle the layoutElement.
  QList<ctkLayoutViewFactory*> viewFactories(QDomElement viewElement)const;

private:
  Q_DECLARE_PRIVATE(ctkLayoutFactory);
  Q_DISABLE_COPY(ctkLayoutFactory);
};

#endif
