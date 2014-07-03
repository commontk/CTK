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


#include "ctkCmdLineModuleManager.h"
#include "ctkCmdLineModuleBackend.h"
#include "ctkException.h"
#include "ctkCmdLineModuleFuture.h"
#include "ctkCmdLineModuleReferenceResult.h"
#include <ctkCmdLineModuleConcurrentHelpers.h>
#include <ctkCmdLineModuleTimeoutException.h>

#include "ctkUtils.h"
#include "ctkTest.h"

#include <QCoreApplication>
#include <QBuffer>
#include <QDataStream>
#include <QDebug>

#if (QT_VERSION < QT_VERSION_CHECK(4,7,0))
extern int qHash(const QUrl& url);
#endif

namespace {

class BackendMockUp : public ctkCmdLineModuleBackend
{

public:

  void addModule(const QUrl& location, const QByteArray& xml, int msDelay = 0)
  {
    this->m_UrlToXmlRetrievalCount[location] = 0;
    this->m_UrlToXml[location] = xml;
    this->m_UrlToXmlOutputDelay[location] = msDelay;
  }

  void setTimestamp(const QUrl& location, qint64 timestamp)
  {
    this->m_UrlToTimestamp[location] = timestamp;
  }

  QList<QUrl> moduleLocations() const
  {
    return this->m_UrlToXml.keys();
  }

  int xmlRetrievalCount(const QUrl& location) const
  {
    QHash<QUrl,int>::ConstIterator iter = this->m_UrlToXmlRetrievalCount.find(location);
    return iter == this->m_UrlToXmlRetrievalCount.end() ? 0 : iter.value();
  }

  virtual QString name() const { return "Mockup"; }
  virtual QString description() const { return "Test Mock-up"; }
  virtual QList<QString> schemes() const { return QList<QString>() << "test"; }

  virtual qint64 timeStamp(const QUrl& location) const
  {
    QHash<QUrl,qint64>::ConstIterator iter = this->m_UrlToTimestamp.find(location);
    return iter == this->m_UrlToTimestamp.end() ? 0 : iter.value();
  }

  virtual QByteArray rawXmlDescription(const QUrl& location, int timeout)
  {
    ++m_UrlToXmlRetrievalCount[location];
    if (timeout < m_UrlToXmlOutputDelay[location])
    {
      throw ctkCmdLineModuleTimeoutException(location, "Timeout in BackendMockUp occurred");
    }
    return m_UrlToXml[location];
  }

protected:

  virtual ctkCmdLineModuleFuture run(ctkCmdLineModuleFrontend* /*frontend*/)
  {
    return ctkCmdLineModuleFuture();
  }

private:

  QHash<QUrl, qint64> m_UrlToTimestamp;
  QHash<QUrl, int> m_UrlToXmlRetrievalCount;
  QHash<QUrl, int> m_UrlToXmlOutputDelay;
  QHash<QUrl, QByteArray> m_UrlToXml;
};

}

//-----------------------------------------------------------------------------
class ctkCmdLineModuleManagerTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void initTestCase();
  void cleanup();

  void testStrictValidation();
  void testWeakValidation();
  void testSkipValidation();
  void testTimeoutHandling();
  void testCaching();

private:

  QByteArray validXml;
  QByteArray invalidXml;
  QString cachePath;
};

//-----------------------------------------------------------------------------
void ctkCmdLineModuleManagerTester::initTestCase()
{
  validXml = "<executable>\n"
                   "  <title>My Filter</title>\n"
                   "  <description>Awesome filter</description>\n"
                   "  <parameters>\n"
                   "    <label>bla</label>\n"
                   "    <description>bla</description>\n"
                   "    <integer>\n"
                   "      <name>param</name>\n"
                   "      <flag>i</flag>\n"
                   "      <description>bla</description>\n"
                   "      <label>bla</label>\n"
                   "    </integer>\n"
                   "  </parameters>\n"
                   "</executable>\n";

  invalidXml = "<executable>\n"
                   "  <description>Awesome filter</description>\n"
                   "  <title>My Filter</title>\n"
                   "  <parameters>\n"
                   "    <label>bla</label>\n"
                   "    <description>bla</description>\n"
                   "    <integer>\n"
                   "      <name>param</name>\n"
                   "      <flag>i</flag>\n"
                   "      <description>bla</description>\n"
                   "      <label>bla</label>\n"
                   "    </integer>\n"
                   "  </parameters>\n"
                   "</executable>\n";

  cachePath = QDir::tempPath() + QDir::separator() + "ctkCmdLineModuleManagerTester_cache";
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleManagerTester::cleanup()
{
  ctk::removeDirRecursively(cachePath);
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleManagerTester::testStrictValidation()
{
  BackendMockUp backend;
  backend.addModule(QUrl("test://validXml"), validXml);
  backend.addModule(QUrl("test://invalidXml"), invalidXml);

  ctkCmdLineModuleManager manager;
  manager.registerBackend(&backend);

  ctkCmdLineModuleReference moduleRef = manager.registerModule(QUrl("test://validXml"));
  QVERIFY(moduleRef);
  QVERIFY(moduleRef.xmlValidationErrorString().isEmpty());

  try
  {
    manager.registerModule(QUrl("test://invalidXml"));
    QFAIL("Succeeded in registering invalid module");
  }
  catch (const ctkInvalidArgumentException&)
  {
  }
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleManagerTester::testWeakValidation()
{
  BackendMockUp backend;
  backend.addModule(QUrl("test://validXml"), validXml);
  backend.addModule(QUrl("test://invalidXml"), invalidXml);

  ctkCmdLineModuleManager manager(ctkCmdLineModuleManager::WEAK_VALIDATION);
  manager.registerBackend(&backend);

  ctkCmdLineModuleReference moduleRef = manager.registerModule(QUrl("test://validXml"));
  QVERIFY(moduleRef);
  QVERIFY(moduleRef.xmlValidationErrorString().isEmpty());

  ctkCmdLineModuleReference moduleRef2 = manager.registerModule(QUrl("test://invalidXml"));
  QVERIFY(moduleRef2);
  QVERIFY(!moduleRef2.xmlValidationErrorString().isEmpty());
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleManagerTester::testSkipValidation()
{
  BackendMockUp backend;
  backend.addModule(QUrl("test://validXml"), validXml);
  backend.addModule(QUrl("test://invalidXml"), invalidXml);

  ctkCmdLineModuleManager manager(ctkCmdLineModuleManager::SKIP_VALIDATION);
  manager.registerBackend(&backend);

  ctkCmdLineModuleReference moduleRef = manager.registerModule(QUrl("test://validXml"));
  QVERIFY(moduleRef);
  QVERIFY(moduleRef.xmlValidationErrorString().isEmpty());

  ctkCmdLineModuleReference moduleRef2 = manager.registerModule(QUrl("test://invalidXml"));
  QVERIFY(moduleRef2);
  QVERIFY(moduleRef2.xmlValidationErrorString().isEmpty());
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleManagerTester::testTimeoutHandling()
{
  BackendMockUp backend;
  backend.addModule(QUrl("test://validXml"), validXml, 1000);
  backend.addModule(QUrl("test://validXml2"), validXml);

  ctkCmdLineModuleManager manager(ctkCmdLineModuleManager::STRICT_VALIDATION, cachePath);
  manager.setTimeOutForXMLRetrieval(2000);

  manager.registerBackend(&backend);

  // register modules with a sufficient large timeout value
  QList<ctkCmdLineModuleReferenceResult> results =
      QtConcurrent::blockingMapped(backend.moduleLocations(),
                                   ctkCmdLineModuleConcurrentRegister(&manager, true));

  QVERIFY(results.size() == 2);
  QVERIFY(results[0].m_Reference && results[0].m_RuntimeError.isEmpty());
  QVERIFY(results[1].m_Reference && results[1].m_RuntimeError.isEmpty());

  // unregister the modules
  QList<bool> unregisterResults =
      QtConcurrent::blockingMapped(backend.moduleLocations(),
                                   ctkCmdLineModuleConcurrentUnRegister(&manager));
  QVERIFY(unregisterResults.size() == 2);
  QVERIFY(unregisterResults[0] && unregisterResults[1]);

  // register modules where one runs into a timeout
  manager.setTimeOutForXMLRetrieval(500);
  results = QtConcurrent::blockingMapped(backend.moduleLocations(),
                                         ctkCmdLineModuleConcurrentRegister(&manager, true));

  QVERIFY(results.size() == 2);
  QVERIFY(!results[0].m_Reference && !results[0].m_RuntimeError.isEmpty());
  QVERIFY(results[1].m_Reference && results[1].m_RuntimeError.isEmpty());

}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleManagerTester::testCaching()
{
  QUrl location("test://validXml");
  QUrl location2("test://validXml2");

  // The code below is inside a local scopes so the manager is destroyed
  // but the cache is still available (unregistering the module would also
  // remove the cache entry, but we need to test the entry)

  {
    BackendMockUp backend;
    backend.addModule(location, validXml, 1000);

    ctkCmdLineModuleManager manager(ctkCmdLineModuleManager::STRICT_VALIDATION, cachePath);
    manager.setTimeOutForXMLRetrieval(500);

    manager.registerBackend(&backend);

    QVERIFY(backend.xmlRetrievalCount(location) == 0);

    // module runs into a timeout
    try
    {
      manager.registerModule(location);
      QFAIL("ctkCmdLineModuleTimeoutException expected");
    }
    catch (const ctkCmdLineModuleTimeoutException&)
    {}
    QVERIFY(backend.xmlRetrievalCount(location) == 1);

    // Increase the timeout and register the module again. It should
    // not have been cached and the manager is supposed to try to
    // retrieve the XML description again
    manager.setTimeOutForXMLRetrieval(2000);
    QVERIFY(manager.registerModule(location));
    QVERIFY(backend.xmlRetrievalCount(location) == 2);

    // Registering the same module again should just return the already
    // created module reference
    QVERIFY(manager.registerModule(location));
    QVERIFY(backend.xmlRetrievalCount(location) == 2);
  }

  {
    BackendMockUp backend;
    backend.addModule(location, validXml);
    backend.setTimestamp(location, 1);
    backend.addModule(location2, invalidXml);
    backend.setTimestamp(location2, 1);

    ctkCmdLineModuleManager manager(ctkCmdLineModuleManager::STRICT_VALIDATION, cachePath);
    manager.registerBackend(&backend);

    QVERIFY(manager.registerModule(location));
    QVERIFY(backend.xmlRetrievalCount(location) == 1);

    try
    {
      manager.registerModule(location2);
      QFAIL("ctkInvalidArgumentException (invalid XML) expected");
    }
    catch (const ctkInvalidArgumentException&)
    {}
    QVERIFY(backend.xmlRetrievalCount(location2) == 1);
  }

  // Do the same again but now the cache entries should be returned
  {
    BackendMockUp backend;
    backend.addModule(location, validXml);
    backend.setTimestamp(location, 1);
    backend.addModule(location2, invalidXml);
    backend.setTimestamp(location2, 1);

    ctkCmdLineModuleManager manager(ctkCmdLineModuleManager::STRICT_VALIDATION, cachePath);
    manager.registerBackend(&backend);

    QVERIFY(manager.registerModule(location));
    QVERIFY(backend.xmlRetrievalCount(location) == 0);

    try
    {
      manager.registerModule(location2);
      QFAIL("ctkInvalidArgumentException (invalid XML) expected");
    }
    catch (const ctkInvalidArgumentException&)
    {}
    QVERIFY(backend.xmlRetrievalCount(location2) == 0);
  }

  // Now test updated timestamps
  {
    BackendMockUp backend;
    backend.addModule(location, validXml);
    backend.setTimestamp(location, 2);
    // use valid XML now but keep the previous timestamp
    backend.addModule(location2, validXml);
    backend.setTimestamp(location2, 1);

    ctkCmdLineModuleManager manager(ctkCmdLineModuleManager::STRICT_VALIDATION, cachePath);
    manager.registerBackend(&backend);

    QVERIFY(manager.registerModule(location));
    QVERIFY(backend.xmlRetrievalCount(location) == 1);

    // should still throw an exception due to the cache entry
    try
    {
      manager.registerModule(location2);
      QFAIL("ctkInvalidArgumentException (invalid XML) expected");
    }
    catch (const ctkInvalidArgumentException&)
    {}
    QVERIFY(backend.xmlRetrievalCount(location2) == 0);

    // now update the timestamp and check that the valid XML is retrieved
    backend.setTimestamp(location, 2);
    QVERIFY(manager.registerModule(location));
    QVERIFY(backend.xmlRetrievalCount(location) == 1);
  }
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCmdLineModuleManagerTest)
#include "moc_ctkCmdLineModuleManagerTest.cpp"
