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

// Qt includes
#include <QList>
#include <QString>
#include <QComboBox>
#include <QUiLoader>
#include <QApplication>

// CTK includes
#include "ctkCmdLineModuleManager.h"
#include "ctkCmdLineModuleFrontendQtGui.h"
#include "ctkCmdLineModuleBackendFunctionPointer.h"
#include "ctkCmdLineModuleParameter.h"
#include "ctkCmdLineModuleParameterGroup.h"
#include "ctkCmdLineModuleDescription.h"
#include "ctkCmdLineModuleXslTransform.h"
#include "ctkCmdLineModuleFuture.h"

#include "ctkTest.h"

// ----------------------------------------------------------------------------
struct MyImageData : public ctk::CmdLineModuleBackendFunctionPointer::ImageType
{
  MyImageData(const QString& path = QString())
    : Path(path)
  {}

  QString Label;
  QString Path;
};

Q_DECLARE_METATYPE(MyImageData)
Q_DECLARE_METATYPE(const MyImageData*)

// ----------------------------------------------------------------------------
namespace ctk {
namespace CmdLineModuleBackendFunctionPointer {

template<>
QString GetParameterTypeName<MyImageData>()
{
  return "image";
}

}}

// ----------------------------------------------------------------------------
class MyImageComboBox : public QComboBox
{
  Q_OBJECT

public:

  Q_PROPERTY(QString currentLabel READ currentLabel WRITE setCurrentLabel)
  Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath)
  Q_PROPERTY(const MyImageData* currentImage READ currentImage)

  MyImageComboBox(QWidget* parent = 0)
    : QComboBox(parent)
  {
    imageData << MyImageData("/path/to/image1")
              << MyImageData("/path/to/image2")
              << MyImageData("/path/to/image3");

    this->addItem("Image 1");
    this->addItem("Image 2");
    this->addItem("Image 3");
  }

  QString currentLabel() const
  {
    return this->imageData.at(this->currentIndex()).Label;
  }

  void setCurrentLabel(const QString& label)
  {
    for(int i = 0; i < imageData.size(); ++i)
    {
      if (imageData[i].Label == label)
      {
        this->setCurrentIndex(i);
        break;
      }
    }
  }

  QString currentPath() const
  {
    return this->imageData.at(this->currentIndex()).Path;
  }

  void setCurrentPath(const QString& path)
  {
    this->imageData[this->currentIndex()].Path = path;
  }

  const MyImageData* currentImage() const
  {
    return &this->imageData.at(this->currentIndex());
  }

private:

  QList<MyImageData> imageData;
};


// ----------------------------------------------------------------------------
class MyQtGuiFrontend : public ctkCmdLineModuleFrontendQtGui
{
public:
  MyQtGuiFrontend(const ctkCmdLineModuleReference& moduleRef)
    : ctkCmdLineModuleFrontendQtGui(moduleRef)
  {}

  QUiLoader* uiLoader() const
  {
    struct MyUiLoader : public QUiLoader {
      QStringList availableWidgets() const
      {
        return QUiLoader::availableWidgets() << "MyImageComboBox";
      }
      QWidget* createWidget(const QString& className, QWidget* parent, const QString& name)
      {
        if (className == "MyImageComboBox")
        {
          MyImageComboBox* comboBox = new MyImageComboBox(parent);
          comboBox->setObjectName(name);
          comboBox->setCurrentIndex(1);
          return comboBox;
        }
        return QUiLoader::createWidget(className, parent, name);
      }
    };
    static MyUiLoader myUiLoader;
    return &myUiLoader;
  }

  ctkCmdLineModuleXslTransform* xslTransform() const
  {
    static bool initialized = false;
    ctkCmdLineModuleXslTransform* transform = ctkCmdLineModuleFrontendQtGui::xslTransform();
    if (!initialized)
    {
      transform->bindVariable("imageInputWidget", "MyImageComboBox");
      transform->bindVariable("imageValueProperty", "currentLabel");
      static QFile extraXsl(":/MyImageComboBoxTest.xsl");
      transform->setXslExtraTransformation(&extraXsl);
      initialized = true;
    }
    return transform;
  }

  QVariant value(const QString &parameter, int role = LocalResourceRole) const
  {
    if (role == UserRole)
    {
      ctkCmdLineModuleParameter param = this->moduleReference().description().parameter(parameter);
      if (param.channel() == "input" && param.tag() == "image")
      {
        return this->customValue(parameter, "currentImage");
      }
      return QVariant();
    }
    else if (role == LocalResourceRole)
    {
      return this->customValue(parameter, "currentPath");
    }
    else
    {
      return ctkCmdLineModuleFrontendQtGui::value(parameter, role);
    }
  }

  void setValue(const QString &parameter, const QVariant &value, int role = DisplayRole)
  {
    if (role == LocalResourceRole)
    {
      this->setCustomValue(parameter, value, "currentPath");
    }
    else
    {
      ctkCmdLineModuleFrontendQtGui::setValue(parameter, value, role);
    }
  }
};

// ----------------------------------------------------------------------------
class MyFpBackend : public ctkCmdLineModuleBackendFunctionPointer
{

protected:

  QList<QVariant> arguments(ctkCmdLineModuleFrontend *frontend) const
  {
    QList<QVariant> args;
    foreach(ctkCmdLineModuleParameter param, frontend->parameters())
    {
      QVariant arg = frontend->value(param.name(), ctkCmdLineModuleFrontend::UserRole);
      if (!arg.isValid())
      {
        arg = frontend->value(param.name());
      }
      args << arg;
    }

    return args;
  }
};

// ----------------------------------------------------------------------------
QString CustomImageDataPath;
void CustomImageTypeModule(const MyImageData* imageData)
{
  CustomImageDataPath = imageData->Path;
}

// ----------------------------------------------------------------------------
class ctkCmdLineModuleQtCustomizationTester: public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void testCustomization();

};

// ----------------------------------------------------------------------------
void ctkCmdLineModuleQtCustomizationTester::testCustomization()
{
  qRegisterMetaType<const MyImageData*>("const MyImageData*");

  ctkCmdLineModuleManager moduleManager;

  MyFpBackend fpBackend;
  fpBackend.registerFunctionPointer("Image Type Customization", CustomImageTypeModule);

  moduleManager.registerBackend(&fpBackend);
  QUrl url = fpBackend.registeredFunctionPointers().front();
  moduleManager.registerModule(url);

  ctkCmdLineModuleReference moduleRef = moduleManager.moduleReference(url);

  ctkCmdLineModuleFrontend* fpFrontend = new MyQtGuiFrontend(moduleRef);
  // force the creation of the frontend gui
  fpFrontend->guiHandle();

  QString expectedImageValue = "/path/to/image2";
  QString actualImageValue = fpFrontend->value("param0").toString();
  QCOMPARE(actualImageValue, expectedImageValue);

  // get a custom QVariant value holding the custom widget
  QCOMPARE(fpFrontend->value("param0", ctkCmdLineModuleFrontend::UserRole).value<const MyImageData*>()->Path,
           expectedImageValue);

  // now set the property for the "LocalResourceRole" (the default property) to something else
  expectedImageValue = "/tmp/path/to/image2";
  fpFrontend->setValue("param0", expectedImageValue, ctkCmdLineModuleFrontend::LocalResourceRole);
  QCOMPARE(fpFrontend->value("param0").toString(), expectedImageValue);

  QVERIFY(CustomImageDataPath.isEmpty());

  // run the module (function pointer) and check that is gets the tmp path
  ctkCmdLineModuleFuture future = moduleManager.run(fpFrontend);
  QTest::qSleep(500);
  QApplication::processEvents();
  future.waitForFinished();

  QCOMPARE(CustomImageDataPath, expectedImageValue);
}


// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCmdLineModuleQtCustomizationTest)
#include "moc_ctkCmdLineModuleQtCustomizationTest.cpp"
