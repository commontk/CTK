/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#ifndef CTKDICOMABSTRACTEXCHANGECACHE_H
#define CTKDICOMABSTRACTEXCHANGECACHE_H

#include <ctkDicomExchangeInterface.h>
#include <QScopedPointer>

#include <org_commontk_dah_core_Export.h>

class ctkDicomAbstractExchangeCachePrivate;
class ctkDicomObjectLocatorCache;

/**
 * @brief Provides a basic convenience methods for the data exchange.
 *
 * The implementation is based on the ctkDicomObjectLocatorCache.
*/
class org_commontk_dah_core_EXPORT ctkDicomAbstractExchangeCache : public QObject, public virtual ctkDicomExchangeInterface
{
 Q_OBJECT
 Q_INTERFACES(ctkDicomExchangeInterface)

public:

  /**
   * @brief Construct object.
   *
   * @param exchangeService the ctkDicomExchangeService of the other side.
  */
  ctkDicomAbstractExchangeCache();

  /**
   * @brief Destructor
   *
  */
  virtual ~ctkDicomAbstractExchangeCache();

  /**
   * @brief Gets the exchange service of the other side.
   *
   * If we are a host, this must return the exchange service
   * of the hosted app and vice versa.
   *
   * @return ctkDicomExchangeService * of the other side
  */
  virtual ctkDicomExchangeInterface* getOtherSideExchangeService() const = 0;

  /**
   * @brief Provide ctkDicomAppHosting::ObjectLocators to the other side.
   *
   * If we are a host, the other side is the hosted app and vice versa.
   *
   * @param objectUUIDs
   * @param acceptableTransferSyntaxUIDs
   * @param includeBulkData
   * @return QList<ctkDicomAppHosting::ObjectLocator>
  */
  virtual QList<ctkDicomAppHosting::ObjectLocator> getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs,
    bool includeBulkData);

  void releaseData(const QList<QUuid>& objectUUIDs);

  /**
   * @brief Return the cache for outgoing data.
   *
   * @return ctkDicomObjectLocatorCache *
  */
  ctkDicomObjectLocatorCache* objectLocatorCache() const;

  /**
   * @brief Publish data to other side
   *
   * @param availableData
   * @param lastData
   * @return bool
  */
  bool publishData(const ctkDicomAppHosting::AvailableData& availableData, bool lastData);

  // Methods to support receiving data
  /**
   * @brief Return the incoming available data.
   *
   * @return AvailableData *
  */
  const ctkDicomAppHosting::AvailableData& getIncomingAvailableData() const;

  /**
   * @brief Return whether the incoming data was marked as @a lastData.
   *
   * @return bool value of @a lastData in incoming notifyDataAvailable call
  */
  bool lastIncomingData() const;

  /**
   * @brief Receive notification from other side.
   *
  */
  bool notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData);

  /**
   * @brief Clean internal data stucture that keeps the incoming data.
   *
   * Called when other side is gone (i.e., usually the other side is a hosted app).
   *
  */
  void cleanIncomingData();

Q_SIGNALS:

  void dataAvailable();
  
private:

 Q_SIGNALS:

  void internalDataAvailable();
  
private:

  Q_DECLARE_PRIVATE(ctkDicomAbstractExchangeCache)
  const QScopedPointer<ctkDicomAbstractExchangeCachePrivate> d_ptr; /**< TODO */

};

#endif // CTKDICOMABSTRACTEXCHANGECACHE_H
