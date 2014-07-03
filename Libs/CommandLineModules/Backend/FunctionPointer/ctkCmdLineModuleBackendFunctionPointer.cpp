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

#include "ctkCmdLineModuleBackendFunctionPointer.h"

#include "ctkCmdLineModuleBackendFPUtil_p.h"
#include "ctkCmdLineModuleBackendFPDescriptionPrivate.h"
#include "ctkCmdLineModuleFunctionPointerTask_p.h"

#include "ctkCmdLineModuleFuture.h"
#include "ctkCmdLineModuleFrontend.h"

#include <QByteArray>
#include <QString>
#include <QList>
#include <QHash>
#include <QUrl>


#if (QT_VERSION < QT_VERSION_CHECK(4,7,0))
extern int qHash(const QUrl& url);
#endif

namespace ctk {
namespace CmdLineModuleBackendFunctionPointer {

//----------------------------------------------------------------------------
template<>
CTK_CMDLINEMODULEBACKENDFP_EXPORT QString GetParameterTypeName<int>()
{
  return "integer";
}

//----------------------------------------------------------------------------
template<>
CTK_CMDLINEMODULEBACKENDFP_EXPORT QString GetParameterTypeName<QList<int> >()
{
  return "integer-vector";
}

}
}

//----------------------------------------------------------------------------
#ifdef Q_OS_WIN
#include <windows.h>
void sleep_secs(int secs)
{
  Sleep(secs*1000);
}
#else
#include <time.h>
void sleep_secs(int secs)
{
  struct timespec nanostep;
  nanostep.tv_sec = secs;
  nanostep.tv_nsec = 0;
  nanosleep(&nanostep, NULL);
}
#endif

void CalculateFibonacciNumbers(int level) //, QList<int>* result)
{
  qDebug() << "Number: 0";
  if (level > 0)
  {
    sleep_secs(1);
    qDebug() << "Number: 1";
    if (level == 1) return;
  }

  int first = 0;
  int second = 1;
  for (int i = 1; i < level; ++i)
  {
    int tmp = first;
    first = second;
    second = first + tmp;
    sleep_secs(1);
    qDebug() << "Number:" << second;
  }
}

//----------------------------------------------------------------------------
ctkCmdLineModuleBackendFunctionPointer::Description::Description()
  : d(new ctkCmdLineModuleBackendFunctionPointer::DescriptionPrivate)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleBackendFunctionPointer::Description::~Description()
{
}

//----------------------------------------------------------------------------
QUrl ctkCmdLineModuleBackendFunctionPointer::Description::moduleLocation() const
{
  return d->ModuleLocation;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendFunctionPointer::Description::moduleCategory() const
{
  return d->ModuleCategory;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleBackendFunctionPointer::Description::setModuleCategory(const QString& category)
{
  d->ModuleCategory = category;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendFunctionPointer::Description::moduleTitle() const
{
  return d->ModuleTitle;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleBackendFunctionPointer::Description::setModuleTitle(const QString &title)
{
  d->ModuleTitle = title;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendFunctionPointer::Description::moduleDescription() const
{
  return d->ModuleDescription;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleBackendFunctionPointer::Description::setModuleDescription(const QString &description)
{
  d->ModuleDescription = description;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendFunctionPointer::Description::moduleVersion() const
{
  return d->ModuleVersion;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleBackendFunctionPointer::Description::setModuleVersion(const QString &version)
{
  d->ModuleVersion = version;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendFunctionPointer::Description::moduleContributor() const
{
  return d->ModuleContributor;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleBackendFunctionPointer::Description::setModuleContributor(const QString &contributor)
{
  d->ModuleContributor = contributor;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleBackendFunctionPointer::Description::Description(const QUrl &location,
                                                                 const ctk::CmdLineModuleBackendFunctionPointer::FunctionPointerProxy &fpProxy)
  : d(new ctkCmdLineModuleBackendFunctionPointer::DescriptionPrivate)
{
  d->ModuleLocation = location;
  d->FpProxy = fpProxy;
}

//----------------------------------------------------------------------------
struct ctkCmdLineModuleBackendFunctionPointerPrivate
{
  QHash<QUrl, ctkCmdLineModuleBackendFunctionPointer::Description> UrlToFpDescription;
};

//----------------------------------------------------------------------------
ctkCmdLineModuleBackendFunctionPointer::ctkCmdLineModuleBackendFunctionPointer()
  : d(new ctkCmdLineModuleBackendFunctionPointerPrivate)
{
  this->registerFunctionPointer("Fibonacci Number", CalculateFibonacciNumbers, "Count");
}

//----------------------------------------------------------------------------
ctkCmdLineModuleBackendFunctionPointer::~ctkCmdLineModuleBackendFunctionPointer()
{
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendFunctionPointer::name() const
{
  return "Function Pointer (experimental)";
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendFunctionPointer::description() const
{
  return "Calls a previously registered function pointer.";
}

//----------------------------------------------------------------------------
QList<QString> ctkCmdLineModuleBackendFunctionPointer::schemes() const
{
  static QList<QString> supportedSchemes = QList<QString>() << "fp";
  return supportedSchemes;
}

//----------------------------------------------------------------------------
qint64 ctkCmdLineModuleBackendFunctionPointer::timeStamp(const QUrl &location) const
{
  Q_UNUSED(location)
  return 0;
}

//----------------------------------------------------------------------------
QByteArray ctkCmdLineModuleBackendFunctionPointer::rawXmlDescription(const QUrl& location, int /*timeout*/)
{
  if (!d->UrlToFpDescription.contains(location)) return QByteArray();
  //qDebug() << d->UrlToFpDescription[location].d->xmlDescription();
  return QByteArray(qPrintable(d->UrlToFpDescription[location].d->xmlDescription()));
}

//----------------------------------------------------------------------------
ctkCmdLineModuleFuture ctkCmdLineModuleBackendFunctionPointer::run(ctkCmdLineModuleFrontend *frontend)
{
  QUrl url = frontend->location();

  const Description& descr = d->UrlToFpDescription[url];
  QList<QVariant> args = this->arguments(frontend);

  // Instances of ctkCmdLineModuleFunctionPointerTask are auto-deleted by the
  // thread pool
  ctkCmdLineModuleFunctionPointerTask* fpTask = new ctkCmdLineModuleFunctionPointerTask(descr, args);
  return fpTask->start();
}

//----------------------------------------------------------------------------
QList<QVariant> ctkCmdLineModuleBackendFunctionPointer::arguments(ctkCmdLineModuleFrontend *frontend) const
{
  return frontend->values().values();
}

//----------------------------------------------------------------------------
QList<QUrl> ctkCmdLineModuleBackendFunctionPointer::registeredFunctionPointers() const
{
  return d->UrlToFpDescription.keys();
}

//----------------------------------------------------------------------------
ctkCmdLineModuleBackendFunctionPointer::Description*
ctkCmdLineModuleBackendFunctionPointer::registerFunctionPointerProxy(const QString& title,
                                                                     const ctk::CmdLineModuleBackendFunctionPointer::FunctionPointerProxy& proxy,
                                                                     const QList<QString>& params)
{
  QUrl url(QString("fp://0x%1").arg(reinterpret_cast<quintptr>(proxy.FpHolder)));
  d->UrlToFpDescription[url] = Description(url, proxy);

  Description& fpDescr = d->UrlToFpDescription[url];
  fpDescr.setModuleTitle(title);
  fpDescr.d->paramDescriptions = params;
  return &fpDescr;
}
