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

  void addModule(const QUrl& location, const QByteArray& xml)
  {
    this->UrlToXml[location] = xml;
  }

  virtual QString name() const { return "Mockup"; }
  virtual QString description() const { return "Test Mock-up"; }
  virtual QList<QString> schemes() const { return QList<QString>() << "test"; }
  virtual qint64 timeStamp(const QUrl& /*location*/) const { return 0; }
  virtual QByteArray rawXmlDescription(const QUrl& location)
  {
    return UrlToXml[location];
  }

protected:

  virtual ctkCmdLineModuleFuture run(ctkCmdLineModuleFrontend* /*frontend*/)
  {
    return ctkCmdLineModuleFuture();
  }

private:

  QHash<QUrl, QByteArray> UrlToXml;
};

}

//-----------------------------------------------------------------------------
class ctkCmdLineModuleManagerTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void initTestCase();

  void testStrictValidation();
  void testWeakValidation();
  void testSkipValidation();

private:

  QByteArray validXml;
  QByteArray invalidXml;
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

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCmdLineModuleManagerTest)
#include "moc_ctkCmdLineModuleManagerTest.cpp"
