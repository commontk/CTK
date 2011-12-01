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


#ifndef CTKSNIPPETREPORTMANAGER_H
#define CTKSNIPPETREPORTMANAGER_H

#include <ctkPluginContext.h>
#include <ctkServiceReference.h>

#include <service/event/ctkEventAdmin.h>
#include <service/event/ctkEventHandler.h>

#include <ctkPluginFrameworkLauncher.h>

#include <QTime>

class Report
{
public:
  QString getTitle() const { return "dummy"; }
  QString getAbsolutePath() const { return "dummy"; }
};

//! [Event Handler service]
class ReportEventHandler : public QObject, public ctkEventHandler
{
  Q_OBJECT
  Q_INTERFACES(ctkEventHandler)

public:

  void handleEvent(const ctkEvent& event)
  {
    QString reportTitle = event.getProperty("title").toString();
    QString reportPath = event.getProperty("path").toString();

    // sendReportByEmail(reportTitle, reportPath);
    qDebug() << "title:" << reportTitle << "path:" << reportPath;
  }
};
//! [Event Handler service]

//! [Event Handler slot]
class ReportEventHandlerUsingSlots : public QObject
{
  Q_OBJECT

public slots:

  void handleEvent(const ctkEvent& event)
  {
    QString reportTitle = event.getProperty("title").toString();
    QString reportPath = event.getProperty("path").toString();

    // sendReportByEmail(reportTitle, reportPath);
    qDebug() << "[slot] title:" << reportTitle << "path:" << reportPath;
  }
};
//! [Event Handler slot]

class ReportManager : public QObject
{
  Q_OBJECT

public:

  //! [Register signal]
  ReportManager(ctkPluginContext* context)
  {
    ctkServiceReference ref = context->getServiceReference<ctkEventAdmin>();
    if (ref)
    {
      ctkEventAdmin* eventAdmin = context->getService<ctkEventAdmin>(ref);
      // Using Qt::DirectConnection is equivalent to ctkEventAdmin::sendEvent()
      eventAdmin->publishSignal(this, SIGNAL(reportGeneratedSignal(ctkDictionary)),
                                "com/acme/reportgenerator/GENERATED", Qt::DirectConnection);
    }
  }
  //! [Register signal]

  //! [Emit signal]
  void reportGenerated(const Report& report)
  {
    ctkDictionary properties;
    properties["title"] = report.getTitle();
    properties["path"] = report.getAbsolutePath();
    properties["time"] = QTime::currentTime();
    emit reportGeneratedSignal(properties);
  }
  //! [Emit signal]

  //! [Publish event]
  void reportGenerated(const Report& report, ctkPluginContext* context)
  {
    ctkServiceReference ref = context->getServiceReference<ctkEventAdmin>();
    if (ref)
    {
      ctkEventAdmin* eventAdmin = context->getService<ctkEventAdmin>(ref);

      ctkDictionary properties;
      properties["title"] = report.getTitle();
      properties["path"] = report.getAbsolutePath();
      properties["time"] = QTime::currentTime();

      ctkEvent reportGeneratedEvent("com/acme/reportgenerator/GENERATED", properties);
      eventAdmin->sendEvent(reportGeneratedEvent);
    }
  }
  //! [Publish event]

  void reportGeneratedAsync(const Report& report, ctkPluginContext* context)
  {
    ctkServiceReference ref = context->getServiceReference<ctkEventAdmin>();
    if (ref)
    {
      ctkEventAdmin* eventAdmin = context->getService<ctkEventAdmin>(ref);

      ctkDictionary properties;
      properties["title"] = report.getTitle();
      properties["path"] = report.getAbsolutePath();
      properties["time"] = QTime::currentTime();

      //! [Publish event async]
      ctkEvent reportGeneratedEvent("com/acme/reportgenerator/GENERATED", properties);
      eventAdmin->postEvent(reportGeneratedEvent);
      //! [Publish event async]
    }
  }

  //! [Declare signal]
signals:

  void reportGeneratedSignal(const ctkDictionary&);
  //! [Declare signal]

};

#endif // CTKSNIPPETREPORTMANAGER_H
