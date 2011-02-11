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

#ifndef CTKDICOMABSTRACTAPP_H
#define CTKDICOMABSTRACTAPP_H

#include <ctkDicomAppInterface.h>
#include <QScopedPointer>
#include <org_commontk_dah_app_Export.h>

class ctkDicomAbstractAppPrivate;
class ctkDicomHostInterface;
class ctkPluginContext;
class ctkDicomObjectLocatorCache;

/**
  * Provides a basic implementation for an application app.
  *
  * TODO: provide helper/convenience methods to ease application development
  *
  * The methods of the ctkDicomAppInterface have to be implemented for the business logic,
  *
  */
class org_commontk_dah_app_EXPORT ctkDicomAbstractApp : public QObject, public ctkDicomAppInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkDicomAppInterface)

public:

  ctkDicomAbstractApp(ctkPluginContext* context);
  virtual ~ctkDicomAbstractApp();

  /**
    * Method triggered by the host. Changes the state of the hosted application.
    * @return true if state received and not illegal in the transition diagram from the reference, false if illegal or not recognized.
    */
  virtual bool setState(ctkDicomAppHosting::State newState);
  virtual ctkDicomAppHosting::State getState();
  virtual QList<ctkDicomAppHosting::ObjectLocator> getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs,
    bool includeBulkData);

  ctkDicomObjectLocatorCache* objectLocatorCache()const;

  bool publishData(const ctkDicomAppHosting::AvailableData& availableData, bool lastData);

protected:
  virtual ctkDicomHostInterface* getHostInterface() const;
 void setInternalState(ctkDicomAppHosting::State state);
signals:
  void startProgress();
  void resumeProgress();
  void suspendProgress();
  void cancelProgress();
  void exitHostedApp();
  void releaseResources();

private:
  Q_DECLARE_PRIVATE(ctkDicomAbstractApp)
  const QScopedPointer<ctkDicomAbstractAppPrivate> d_ptr;
};

#endif // CTKDICOMABSTRACTAPP_H
