/*=============================================================================

  Library: CTK

  Copyright (c) University College London

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

#include "ctkCmdLineModuleBackendXMLChecker.h"
#include "ctkCmdLineModuleXMLCheckerTask_p.h"

#include "ctkCmdLineModuleFrontend.h"
#include "ctkCmdLineModuleFuture.h"
#include <QUrl>
#include <QFile>
#include <QString>
#include <QDateTime>
#include <ctkUtils.h>

//----------------------------------------------------------------------------
struct ctkCmdLineModuleBackendXMLCheckerPrivate
{
  QString m_HardCodedXML;
  QDateTime m_LastModified;
};


//----------------------------------------------------------------------------
ctkCmdLineModuleBackendXMLChecker::ctkCmdLineModuleBackendXMLChecker()
  : d(new ctkCmdLineModuleBackendXMLCheckerPrivate)
{
  d->m_HardCodedXML = QString("");
  d->m_LastModified = QDateTime::currentDateTime();
}


//----------------------------------------------------------------------------
ctkCmdLineModuleBackendXMLChecker::ctkCmdLineModuleBackendXMLChecker(const QString &xmlToValidate)
  : d(new ctkCmdLineModuleBackendXMLCheckerPrivate)
{
  d->m_HardCodedXML = xmlToValidate;
  d->m_LastModified = QDateTime::currentDateTime();
}


//----------------------------------------------------------------------------
ctkCmdLineModuleBackendXMLChecker::~ctkCmdLineModuleBackendXMLChecker()
{
}


//----------------------------------------------------------------------------
void ctkCmdLineModuleBackendXMLChecker::setXML(const QString& xml)
{
  d->m_HardCodedXML = xml;
  d->m_LastModified = QDateTime::currentDateTime();
}


//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendXMLChecker::xml() const
{
  return d->m_HardCodedXML;
}


//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendXMLChecker::name() const
{
  return "XML Checker";
}


//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendXMLChecker::description() const
{
  return "Fakes a backend process, returning a static piece of XML.";
}


//----------------------------------------------------------------------------
QList<QString> ctkCmdLineModuleBackendXMLChecker::schemes() const
{
  static QList<QString> supportedSchemes = QList<QString>() << "xmlchecker";
  return supportedSchemes;
}


//----------------------------------------------------------------------------
qint64 ctkCmdLineModuleBackendXMLChecker::timeStamp(const QUrl & /*location*/) const
{
  return ctk::msecsTo(QDateTime::fromTime_t(0), d->m_LastModified);
}


//----------------------------------------------------------------------------
QByteArray ctkCmdLineModuleBackendXMLChecker::rawXmlDescription(const QUrl& location, int /*timeout*/)
{
  if (location.isValid())
  {
    QFile file(location.path());
    file.open(QFile::ReadOnly);
    return file.readAll();
  }
  return d->m_HardCodedXML.toLatin1();
}


//----------------------------------------------------------------------------
ctkCmdLineModuleFuture ctkCmdLineModuleBackendXMLChecker::run(ctkCmdLineModuleFrontend* /*frontend*/)
{
  // Instances of ctkCmdLineModuleProcessTask are auto-deleted by the thread pool.
  ctkCmdLineModuleXMLCheckerTask* moduleProcess = new ctkCmdLineModuleXMLCheckerTask();
  return moduleProcess->start();
}
