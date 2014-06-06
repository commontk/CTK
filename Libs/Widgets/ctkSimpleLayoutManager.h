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

#ifndef __ctkSimpleLayoutManager_h
#define __ctkSimpleLayoutManager_h

// Qt includes
#include <QMetaObject>

// CTK includes
#include "ctkLayoutManager.h"
class ctkSimpleLayoutManagerPrivate;

/// \ingroup Widgets
struct ctkWidgetInstanciator
{
  virtual ~ctkWidgetInstanciator() {}
  virtual void beginSetupLayout(){}
  virtual void endSetupLayout(){}
  virtual QWidget* createWidget() = 0;
};

/// \ingroup Widgets
template<class T>
struct ctkTemplateInstanciator:public ctkWidgetInstanciator
{
  virtual QWidget* createWidget() {return new T;}
};

/// \ingroup Widgets
/// Utility class to access control on the DomDocument layout
class CTK_WIDGETS_EXPORT ctkSimpleLayoutManager: public ctkLayoutManager
{
  Q_OBJECT
public:
  ctkSimpleLayoutManager(QObject* parent = 0);
  explicit ctkSimpleLayoutManager(QWidget* viewport, QObject* parent);
  virtual ~ctkSimpleLayoutManager();

  using ctkLayoutManager::setLayout;
  using ctkLayoutManager::layout;

  // Note the default constructor of the class must be declared with
  // Q_INVOKABLE.
  //void setViewMetaObject(const QMetaObject& viewMetaObject);
  //const QMetaObject viewMetaObject()const;
  void setViewInstanciator(ctkWidgetInstanciator* instanciator);
  ctkWidgetInstanciator* viewInstanciator()const;

protected:
  virtual QWidget* viewFromXML(QDomElement viewElement);
  virtual void setupLayout();
private:
  Q_DECLARE_PRIVATE(ctkSimpleLayoutManager);
  Q_DISABLE_COPY(ctkSimpleLayoutManager);
};

#endif
