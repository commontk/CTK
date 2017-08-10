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

#ifndef __ctkAbstractFactory_tpp
#define __ctkAbstractFactory_tpp

// QT includes
#include <QDebug>

// CTK includes
#include "ctkAbstractFactory.h"

//----------------------------------------------------------------------------
// ctkAbstractFactoryItem methods

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType>::ctkAbstractFactoryItem()
  :Instance()
{
  this->Verbose = false;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType>::~ctkAbstractFactoryItem()
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QStringList ctkAbstractFactoryItem<BaseClassType>::instantiateErrorStrings()const
{
  return this->InstantiateErrorStrings;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::appendInstantiateErrorString(const QString& errorString)
{
  this->InstantiateErrorStrings << errorString;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::clearInstantiateErrorStrings()
{
  this->InstantiateErrorStrings.clear();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QStringList ctkAbstractFactoryItem<BaseClassType>::instantiateWarningStrings()const
{
  return this->InstantiateWarningStrings;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::appendInstantiateWarningString(const QString& msg)
{
  this->InstantiateWarningStrings << msg;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::clearInstantiateWarningStrings()
{
  this->InstantiateWarningStrings.clear();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QStringList ctkAbstractFactoryItem<BaseClassType>::loadErrorStrings()const
{
  return this->LoadErrorStrings;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::appendLoadErrorString(const QString& errorString)
{
  this->LoadErrorStrings << errorString;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::clearLoadErrorStrings()
{
  this->LoadErrorStrings.clear();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QStringList ctkAbstractFactoryItem<BaseClassType>::loadWarningStrings()const
{
  return this->LoadWarningStrings;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::appendLoadWarningString(const QString& msg)
{
  this->LoadWarningStrings << msg;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::clearLoadWarningStrings()
{
  this->LoadWarningStrings.clear();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>  
BaseClassType* ctkAbstractFactoryItem<BaseClassType>::instantiate()
{
  this->clearInstantiateErrorStrings();
  this->clearInstantiateWarningStrings();
  this->Instance = this->instanciator();
  return this->Instance;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFactoryItem<BaseClassType>::isInstantiated()const
{
  return (this->Instance != 0);
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
BaseClassType* ctkAbstractFactoryItem<BaseClassType>::instance()const
{
  return this->Instance;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::uninstantiate()
{
  if (!this->Instance)
    {
    return;
    }
  delete this->Instance;
  // Make sure the pointer is set to 0. Doing so, Will prevent attempt to
  // delete unextising object if uninstantiate() methods is called multiple times.
  this->Instance = 0; 
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::setVerbose(bool value)
{
  this->Verbose = value;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFactoryItem<BaseClassType>::verbose()const
{
  return this->Verbose;
}

//----------------------------------------------------------------------------
// ctkAbstractFactory methods

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactory<BaseClassType>::ctkAbstractFactory()
{
  this->Verbose = false;
  this->SharedRegisteredItemMap = QSharedPointer<HashType>(new HashType);
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactory<BaseClassType>::~ctkAbstractFactory()
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactory<BaseClassType>::printAdditionalInfo()
{
  qDebug() << "ctkAbstractFactory<BaseClassType> (" << this << ")";
  // TODO
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
BaseClassType* ctkAbstractFactory<BaseClassType>::instantiate(const QString& itemKey)
{
  ctkAbstractFactoryItem<BaseClassType>* _item = this->item(itemKey);
  BaseClassType* instance = 0;
  bool wasInstantiated = false;
  if (_item)
    {
    wasInstantiated = _item->isInstantiated();
    instance = wasInstantiated ? _item->instance() : _item->instantiate();
    }
  if (!wasInstantiated)
    {
    this->displayStatusMessage(instance ? QtDebugMsg : QtCriticalMsg,
                               QString("Attempt to instantiate \"%1\"").arg(itemKey),
                               instance ? "OK" : "Failed", this->verbose());
    if (_item)
      {
      if(!_item->instantiateErrorStrings().isEmpty())
        {
        qCritical().nospace() << qPrintable(QString(" ").repeated(2) + QLatin1String("Error(s):\n"))
                              << qPrintable(QString(" ").repeated(4) +
                                            _item->instantiateErrorStrings().join(
                                              QString("\n") + QString(" ").repeated(4)));
        }
      if(!_item->instantiateWarningStrings().isEmpty())
        {
        qWarning().nospace() << qPrintable(QString(" ").repeated(2) + QLatin1String("Warning(s):\n"))
                             << qPrintable(QString(" ").repeated(4) +
                                           _item->instantiateWarningStrings().join(
                                             QString("\n") + QString(" ").repeated(4)));
        }
      }
    }
  return instance;
}


//----------------------------------------------------------------------------
template<typename BaseClassType>
BaseClassType* ctkAbstractFactory<BaseClassType>::instance(const QString& itemKey)
{
  ctkAbstractFactoryItem<BaseClassType>* factoryItem = this->item(itemKey);
  return factoryItem ? factoryItem->instance() : 0;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactory<BaseClassType>::uninstantiate(const QString& itemKey)
{
  ctkAbstractFactoryItem<BaseClassType> * _item = this->item(itemKey);
  if (!_item)
    {
    return;
    }
  _item->uninstantiate();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactory<BaseClassType>::setSharedItems(const QSharedPointer<HashType>& items)
{
  this->SharedRegisteredItemMap = items;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QSharedPointer<typename ctkAbstractFactory<BaseClassType>::HashType>
ctkAbstractFactory<BaseClassType>::sharedItems()
{
  return this->SharedRegisteredItemMap;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QStringList ctkAbstractFactory<BaseClassType>::itemKeys() const
{
  // Since by construction, we checked if a name was already in the QHash,
  // there is no need to call 'uniqueKeys'
  return this->RegisteredItemMap.keys();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactory<BaseClassType>::displayStatusMessage(
    const QtMsgType& type, const QString& description, const QString& status, bool display)
{
  QString msg = QString("%1 [%2]").arg(description + " ", -70, QChar('.')).arg(status);
  if (display)
    {
    switch(type)
      {
      case QtFatalMsg:
        qFatal("%s", qPrintable(msg));
        break;
      case QtCriticalMsg:
        qCritical("%s", qPrintable(msg));
        break;
      case QtWarningMsg:
        qWarning("%s", qPrintable(msg));
        break;
#if QT_VERSION >= QT_VERSION_CHECK(5,5,0)
      case QtInfoMsg:
        qInfo("%s", qPrintable(msg));
        break;
#endif
      case QtDebugMsg:
        qDebug("%s", qPrintable(msg));
        break;
      }
    }
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFactory<BaseClassType>::registerItem(const QString& key,
  const QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > & _item)
{
  // Sanity checks
  if (!_item)
    {
    if (this->verbose())
      {
      qDebug() << __FUNCTION__ << "key is empty - item: " << _item;
      }
    return false;
    }

  QString description = QString("Attempt to register \"%1\"").arg(key);

  if (this->item(key))
    {
    this->displayStatusMessage(QtWarningMsg, description, "Already registered", this->verbose());
    return false;
    }

  if (this->sharedItem(key))
    {
    this->displayStatusMessage(QtDebugMsg, description,
                               "Already registered in other factory", this->verbose());
    return false;
    }
  
  // Attempt to load it
  if (!_item->load())
    {
    this->displayStatusMessage(QtCriticalMsg, description, "Failed", this->verbose());
    if(!_item->loadErrorStrings().isEmpty())
      {
      qCritical().nospace() << qPrintable(QString(" ").repeated(2) + QLatin1String("Error(s):\n"))
                            << qPrintable(QString(" ").repeated(4) +
                                          _item->loadErrorStrings().join(
                                            QString("\n") + QString(" ").repeated(4)));
      }
    if(!_item->loadWarningStrings().isEmpty())
      {
      qWarning().nospace() << qPrintable(QString(" ").repeated(2) + QLatin1String("Warning(s):\n"))
                           << qPrintable(QString(" ").repeated(4) +
                                         _item->loadWarningStrings().join(
                                           QString("\n") + QString(" ").repeated(4)));
      }
    return false;
    }
  
  // Store item reference using a QSharedPointer
  this->RegisteredItemMap.insert(key, _item);
  this->SharedRegisteredItemMap.data()->insert(key, _item);

  this->displayStatusMessage(QtDebugMsg, description, "OK", this->verbose());
  return true;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType> * ctkAbstractFactory<BaseClassType>::item(const QString& itemKey)const
{
  ConstIterator iter = this->RegisteredItemMap.find(itemKey);
  if ( iter == this->RegisteredItemMap.constEnd())
    {
    return 0;
    }
  return iter.value().data();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType> * ctkAbstractFactory<BaseClassType>::sharedItem(const QString& itemKey)const
{
  if(this->SharedRegisteredItemMap.isNull())
    {
    return 0;
    }
  ConstIterator iter = this->SharedRegisteredItemMap.data()->find(itemKey);
  if ( iter == this->SharedRegisteredItemMap.data()->constEnd())
    {
    return 0;
    }
  return iter.value().data();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactory<BaseClassType>::setVerbose(bool value)
{
  this->Verbose = value;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFactory<BaseClassType>::verbose()const
{
  return this->Verbose;
}

#endif
