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

#include "ctkPluginGenerator_p.h"
#include "ui_ctkPluginGeneratorMainWindow.h"

#include <ctkPluginFramework.h>
#include <ctkPluginContext.h>
#include <ctkServiceReference.h>
#include <ctkPluginConstants.h>
#include <ctkPluginGeneratorCodeModel.h>
#include <ctkPluginGeneratorConstants.h>
#include <ctkPluginGeneratorOptionsDialog_p.h>
#include <ctkUtils.h>

#include <QDebug>
#include <QListWidgetItem>
#include <QDir>
#include <QFileSystemModel>
#include <QTime>
#include <QMessageBox>
#include <QSettings>

#include <stdexcept>


class ctkTemporaryDir
{
public:

  static QString create(const QString& path = QString())
  {
    QString tmpPath = path;
    if (tmpPath.isEmpty())
    {
      tmpPath = "ctkplugingenerator";
    }
    tmpPath +=  "." + QTime::currentTime().toString("hhmmsszzz");

    QDir tmp = QDir::temp();
    if (!tmp.mkdir(tmpPath))
    {
      QString msg = QString("Creating temporary directory ") + tmpPath + " in "
                    + QDir::temp().canonicalPath() + " failed.";
      throw std::runtime_error(msg.toStdString());
    }

    tmp.cd(tmpPath);

    return tmp.canonicalPath();
  }
};

ctkPluginGenerator::ctkPluginGenerator(ctkPluginFramework* framework, QWidget *parent) :
    QMainWindow(parent),
    framework(framework), ui(new Ui::ctkPluginGeneratorMainWindow),
    mode(EDIT), previewModel(0)
{
  ui->setupUi(this);

  previewModel = new QFileSystemModel(this);
  ui->previewTreeView->setModel(previewModel);
  ui->previewTreeView->hideColumn(1);
  ui->previewTreeView->hideColumn(2);
  ui->previewTreeView->hideColumn(3);

  this->setStatusBar(0);

  connect(ui->actionOptions, SIGNAL(triggered(bool)), this, SLOT(menuOptionsTriggered()));
  connect(ui->generateButton, SIGNAL(clicked()), this, SLOT(generateClicked()));
  connect(ui->previewButton, SIGNAL(clicked()), this, SLOT(previewClicked()));
  connect(ui->cancelButton, SIGNAL(clicked()), qApp, SLOT(quit()));
  connect(ui->uiExtensionList, SIGNAL(itemClicked(QListWidgetItem*)),
          this, SLOT(extensionItemClicked(QListWidgetItem*)));
  connect(ui->previewTreeView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(previewIndexChanged(QModelIndex)));

  QList<ctkServiceReference> serviceRefs = framework->getPluginContext()->
                                            getServiceReferences("ctkPluginGeneratorAbstractUiExtension");
  QListIterator<ctkServiceReference> it(serviceRefs);
  while (it.hasNext())
  {
    ctkServiceReference serviceRef = it.next();
    ctkPluginGeneratorAbstractUiExtension* extension =
        qobject_cast<ctkPluginGeneratorAbstractUiExtension*>(framework->getPluginContext()->getService(serviceRef));
    qDebug() << "Service reference found";
    if (extension)
    {
      qDebug() << "inserted";
      int ranking = serviceRef.getProperty(ctkPluginConstants::SERVICE_RANKING).toInt();
      if (ranking > 0)
      {
        uiExtensionMap.insert(ranking, extension);
      }
      else
      {
        uiExtensionMap.insert(-1, extension);
      }
    }
  }

  int id = 0;
  foreach (ctkPluginGeneratorAbstractUiExtension* extension, uiExtensionMap)
  {
    idToExtensionMap.insert(id, extension);
    ui->extensionStack->addWidget(extension->getWidget());

    connect(extension, SIGNAL(errorMessageChanged(QString)), this, SLOT(errorMessageChanged(QString)));

    extension->validate();

    (new QListWidgetItem(extension->getTitle(), ui->uiExtensionList))->setData(Qt::UserRole, id);
    ++id;
  }

  ui->uiExtensionList->setCurrentRow(0);
  extensionClicked(idToExtensionMap[0]);
}

ctkPluginGenerator::~ctkPluginGenerator()
{
  delete ui;
  if (!previewDir.isEmpty())
  {
    ctk::removeDirRecursively(previewDir);
  }
}

void ctkPluginGenerator::menuOptionsTriggered()
{
  ctkPluginGeneratorOptionsDialog optionsDialog;
  int result = optionsDialog.exec();
  if (result == QDialog::Accepted && mode == PREVIEW)
  {
    QString selPath;
    QString oldPreviewDir = previewDir;
    if (!ui->previewTreeView->selectionModel()->selection().isEmpty())
    {
      QModelIndex index = ui->previewTreeView->selectionModel()->selectedIndexes().front();
      selPath = previewModel->data(index, QFileSystemModel::FilePathRole).toString();
    }
    if (createPreview())
    {
      ui->modeStack->setCurrentWidget(ui->previewPage);
      ui->previewButton->setText(tr("<< Back"));
      ui->previewTreeView->expandAll();
      if (!selPath.isEmpty())
      {
        selPath.replace(oldPreviewDir, previewDir);
        QModelIndex index = previewModel->index(selPath);
        ui->previewTreeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
        previewIndexChanged(index);
      }
    }
  }
}

void ctkPluginGenerator::previewClicked()
{
  if (mode == EDIT)
  {
    if (createPreview())
    {
      ui->modeStack->setCurrentWidget(ui->previewPage);
      ui->previewButton->setText(tr("<< Back"));
      ui->previewTreeView->expandAll();
      if (!ui->previewTreeView->selectionModel()->selection().isEmpty())
      {
        previewIndexChanged(ui->previewTreeView->selectionModel()->selectedIndexes().front());
      }
      mode = PREVIEW;
    }
  }
  else
  {
    ui->modeStack->setCurrentWidget(ui->editPage);
    ui->previewButton->setText(tr("Preview >>"));
    mode = EDIT;

    ctk::removeDirRecursively(previewDir);
    previewDir.clear();
  }
}

void ctkPluginGenerator::generateClicked()
{
  try
  {
    createPlugin(ui->outputDirButton->directory());

    QMessageBox msgBox;
    msgBox.setText(tr("Successfully create plugin"));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
  }
  catch (const std::runtime_error& error)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Creating the plugin failed."));
    msgBox.setInformativeText(QString::fromLatin1(error.what()));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
  }
}

QString ctkPluginGenerator::createPlugin(const QString& path)
{
  ctkServiceReference codeModelRef = framework->getPluginContext()->
                                      getServiceReference("ctkPluginGeneratorCodeModel");

  ctkPluginGeneratorCodeModel* codeModel =
      qobject_cast<ctkPluginGeneratorCodeModel*>(framework->getPluginContext()->getService(codeModelRef));
  codeModel->reset();

  // set global code model info from QSettings object
  QSettings settings;
  codeModel->setLicense(settings.value(ctkPluginGeneratorConstants::PLUGIN_LICENSE_MARKER).toString());

  foreach(ctkPluginGeneratorAbstractUiExtension* extension, idToExtensionMap)
  {
    extension->updateCodeModel();
  }

  QString pluginDir = path + "/" + codeModel->getSymbolicName(true);
  if (!QDir(path).mkdir(codeModel->getSymbolicName(true)))
  {
    QString msg(tr("Creating directory \"%1\" failed.").arg(pluginDir));
    throw std::runtime_error(msg.toStdString());
  }

  codeModel->create(pluginDir);
  return pluginDir;
}

void ctkPluginGenerator::previewIndexChanged(const QModelIndex& index)
{
  QString filePath = previewModel->data(index, QFileSystemModel::FilePathRole).toString();
  ui->previewTextLabel->setText(QDir(QString(filePath).replace(previewDir, ui->outputDirButton->directory())).absolutePath());

  QFile file(filePath);
  file.open(QFile::ReadOnly);
  QTextStream textStream(&file);
  ui->previewTextEdit->setText(textStream.readAll());
}

bool ctkPluginGenerator::createPreview()
{
  try
  {
    previewDir = ctkTemporaryDir::create();

    QString tmpPluginDir = createPlugin(previewDir);

    previewModel->setRootPath(tmpPluginDir);
    ui->previewTreeView->setRootIndex(previewModel->index(previewDir));
  }
  catch (const std::runtime_error& error)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Creating the preview failed."));
    msgBox.setInformativeText(QString::fromLatin1(error.what()));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return false;
  }

  return true;
}

void ctkPluginGenerator::extensionItemClicked(QListWidgetItem* item)
{
  ctkPluginGeneratorAbstractUiExtension* extension = idToExtensionMap[item->data(Qt::UserRole).toInt()];
  extensionClicked(extension);
}

void ctkPluginGenerator::extensionClicked(ctkPluginGeneratorAbstractUiExtension* extension)
{
  ui->extensionStack->setCurrentWidget(extension->getWidget());
  ui->extensionMsgLabel->setText(extension->getTitle());
  this->errorMessageChanged(extension->getErrorMessage());
}

void ctkPluginGenerator::errorMessageChanged(const QString& errMsg)
{
  ui->extensionErrMsgLabel->setText(errMsg);

  bool enableButtons = false;
  if (errMsg.isEmpty())
  {
    enableButtons = true;
  }

  ui->previewButton->setEnabled(enableButtons);
  ui->generateButton->setEnabled(enableButtons);
}
