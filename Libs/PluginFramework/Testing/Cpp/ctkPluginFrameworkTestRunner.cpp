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


#include "ctkPluginFrameworkTestRunner.h"

#include "ctkTestSuiteInterface.h"

#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFramework.h>
#include <ctkPluginFrameworkLauncher.h>
#include <ctkPluginContext.h>
#include <ctkPluginException.h>

#include <QStringList>
#include <QPair>
#include <QCoreApplication>
#include <QDirIterator>
#include <QTest>
#include <QThread>
#include <QDebug>


//----------------------------------------------------------------------------
class TestRunner : public QThread
{
public:

  typedef QPair<long, ctkPlugin::StartOptions> StartPluginPair;

  TestRunner(ctkPluginContext* context, const QSet<StartPluginPair>& startPlugins, int argc, char** argv)
    : context(context), startPluginInfos(startPlugins),
      argc(argc), argv(argv)
  {

  }

  void run()
  {
    // start the specified plugins which may register the test suites (QObject classes)
    foreach(StartPluginPair pluginInfo, startPluginInfos)
    {
      QSharedPointer<ctkPlugin> plugin = context->getPlugin(pluginInfo.first);
      plugin->start(pluginInfo.second);
    }

    QList<ctkServiceReference> refs = context->getServiceReferences<ctkTestSuiteInterface>();

    int result = 0;
    int count = 0;
    foreach(ctkServiceReference ref, refs)
    {
      result = QTest::qExec(context->getService(ref), argc, argv);
      if (result > 0) break;
      ++count;
    }

    // stop the framework
    QSharedPointer<ctkPluginFramework> fw = qSharedPointerCast<ctkPluginFramework>(context->getPlugin(0));
    fw->stop();
    // wait for 30 secs
    ctkPluginFrameworkEvent event = fw->waitForStop(30000);

    if (result == 0 && event.getType() == ctkPluginFrameworkEvent::FRAMEWORK_WAIT_TIMEDOUT)
    {
      qWarning() << "Framework shutdown wait timed out";
      result = 1;
    }

    qDebug() << "#########" << count << "out of" << refs.size() << "test suites passed #########";
    if (result > 0)
    {
      QCoreApplication::exit(result);
      qApp->thread()->wait();
    }
  }

private:

  ctkPluginContext* context;
  QSet<StartPluginPair> startPluginInfos;
  int argc;
  char** argv;
};

//----------------------------------------------------------------------------
class ctkPluginFrameworkTestRunnerPrivate
{
public:

  typedef QPair<QString, bool> PluginPathPair;
  QList<PluginPathPair> pluginPaths;

  typedef QPair<QString, QString> InstallCandPair;
  QList<InstallCandPair> installCandidates;

  typedef QPair<QString, ctkPlugin::StartOptions> ActivatePair;
  QList<ActivatePair> activatePlugins;

  typedef QPair<long, ctkPlugin::StartOptions> StartPluginPair;
  QSet<StartPluginPair> startPlugins;

  ctkPluginContext* context;

  ctkPluginFrameworkFactory* fwFactory;

  //----------------------------------------------------------------------------
  ctkPluginFrameworkTestRunnerPrivate()
    : context(0), fwFactory(0)
  {
    pluginLibFilter << "*.dll" << "*.so" << "*.dylib";
  }

  //----------------------------------------------------------------------------
  void installPlugins(const QString& path)
  {
    QDirIterator dirIter(path, pluginLibFilter, QDir::Files);
    while(dirIter.hasNext())
    {
      dirIter.next();
      try
      {
        QSharedPointer<ctkPlugin> plugin = context->installPlugin(QUrl::fromLocalFile(dirIter.filePath()));
        long pluginId = plugin->getPluginId();
        QString symbolicName = plugin->getSymbolicName();
        foreach(ActivatePair activatePlugin, activatePlugins)
        {
          if (activatePlugin.first == symbolicName)
          {
            startPlugins.insert(qMakePair(pluginId, activatePlugin.second));
            activatePlugins.removeAll(activatePlugin);
            break;
          }
        }
      }
      catch (const ctkPluginException& e)
      {
        qCritical() << e.what();
      }
    }
  }

  //----------------------------------------------------------------------------
  void installPlugin(const QString& path, const QString& name)
  {
    QDirIterator dirIter(path, pluginLibFilter, QDir::Files);
    while(dirIter.hasNext())
    {
      dirIter.next();
      QString fileName = dirIter.fileName().mid(3); // strip the "lib" prefix
      fileName.truncate(fileName.lastIndexOf('.')); // remove the suffix
      if (fileName == name)
      {
        try
        {
          QSharedPointer<ctkPlugin> plugin = context->installPlugin(QUrl::fromLocalFile(dirIter.filePath()));
          QString symbolicName = plugin->getSymbolicName();
          long pluginId = plugin->getPluginId();
          foreach(ActivatePair activatePlugin, activatePlugins)
          {
            if (activatePlugin.first == symbolicName)
            {
              startPlugins.insert(qMakePair(pluginId, activatePlugin.second));
              activatePlugins.removeAll(activatePlugin);
              break;
            }
          }
          break;
        }
        catch (const ctkPluginException& e)
        {
          qCritical() << e.what();
        }
      }
    }
  }

private:

  QStringList pluginLibFilter;
};

//----------------------------------------------------------------------------
ctkPluginFrameworkTestRunner::ctkPluginFrameworkTestRunner()
  : d_ptr(new ctkPluginFrameworkTestRunnerPrivate())
{

}

//----------------------------------------------------------------------------
ctkPluginFrameworkTestRunner::~ctkPluginFrameworkTestRunner()
{
  Q_D(ctkPluginFrameworkTestRunner);
  delete d->fwFactory;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestRunner::addPluginPath(const QString& path, bool install)
{
  Q_D(ctkPluginFrameworkTestRunner);
  d->pluginPaths.push_back(qMakePair(path, install));
  ctkPluginFrameworkLauncher::appendPathEnv(path);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestRunner::addPlugin(const QString &path, const QString &name)
{
  Q_D(ctkPluginFrameworkTestRunner);
  d->installCandidates.push_back(qMakePair(path, name));
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestRunner::startPluginOnRun(const QString& pluginId, ctkPlugin::StartOptions opts)
{
  Q_D(ctkPluginFrameworkTestRunner);
  d->activatePlugins.push_back(qMakePair(pluginId, opts));
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestRunner::init(const ctkProperties& fwProps)
{
  Q_D(ctkPluginFrameworkTestRunner);

  d->fwFactory = new ctkPluginFrameworkFactory(fwProps);
  QSharedPointer<ctkPluginFramework> framework = d->fwFactory->getFramework();
  framework->start();

  d->context = framework->getPluginContext();

  foreach(ctkPluginFrameworkTestRunnerPrivate::PluginPathPair path,
          d->pluginPaths)
  {
    QCoreApplication::addLibraryPath(path.first);
    if (path.second) d->installPlugins(path.first);
  }

  foreach(ctkPluginFrameworkTestRunnerPrivate::InstallCandPair candidate,
          d->installCandidates)
  {
    qDebug() << "Installing" << candidate.first << "," << candidate.second;
    d->installPlugin(candidate.first, candidate.second);
  }
}

//----------------------------------------------------------------------------
int ctkPluginFrameworkTestRunner::run(int argc, char** argv)
{
  Q_D(ctkPluginFrameworkTestRunner);

  if (!d->activatePlugins.isEmpty())
  {
    qCritical() << "The following plugins will not be started, because"
                << "they could not be installed:";
    foreach(ctkPluginFrameworkTestRunnerPrivate::ActivatePair p,
            d->activatePlugins)
    {
      qCritical() << "  -" << p.first;
    }

    return EXIT_FAILURE;
  }

  TestRunner runner(d->context, d->startPlugins, argc, argv);
  runner.connect(&runner, SIGNAL(finished()), qApp, SLOT(quit()));
  runner.start();

  return qApp->exec();
}
