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

// QT includes
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMap>
#include <QMessageBox>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QVariant>

// CTKQtTesting includes
#include "ctkQtTestingUtility.h"
#include "ctkXMLEventSource.h"

//-----------------------------------------------------------------------------
// ctkXMLEventSource methods

//-----------------------------------------------------------------------------
ctkXMLEventSource::ctkXMLEventSource(QObject* testUtility)
  : Superclass(testUtility)
{
  this->Automatic = false;
  this->XMLStream = NULL;
  this->TestUtility = qobject_cast<pqTestUtility*>(testUtility);
  Q_ASSERT(this->TestUtility);
}

//-----------------------------------------------------------------------------
ctkXMLEventSource::~ctkXMLEventSource()
{
  delete this->XMLStream;
  this->TestUtility = 0;
}

//-----------------------------------------------------------------------------
void ctkXMLEventSource::setRestoreSettingsAuto(bool value)
{
  this->Automatic = value;
}

//-----------------------------------------------------------------------------
bool ctkXMLEventSource::restoreSettingsAuto() const
{
  return this->Automatic;
}

//-----------------------------------------------------------------------------
void ctkXMLEventSource::setContent(const QString& xmlfilename)
{
  delete this->XMLStream;
  this->XMLStream = NULL;

  QFile xml(xmlfilename);
  if (!xml.open(QIODevice::ReadOnly))
    {
    qCritical() << "Failed to load " << xmlfilename;
    return;
    }

  // Check if the xml file is valid
  QXmlSchema xmlSchema;
  if (!xmlSchema.load(QUrl::fromLocalFile(":/XML/XMLDescription.xsd")) ||
      !xmlSchema.isValid())
    {
    qCritical() << "Xml cannot be check.";
    return;
    }

  QXmlSchemaValidator validator(xmlSchema);
  if(!validator.validate(&xml, QUrl::fromLocalFile(xml.fileName())))
    {
    qCritical() << xmlfilename << "invalid xml file for qtTesting !";
    return;
    }

  xml.reset();
  QByteArray data = xml.readAll();
  this->XMLStream = new QXmlStreamReader(data);

  if(this->settingsRecorded())
    {
    this->OldSettings = this->recoverSettingsFromXML();
    if(!this->settingsUpToData())
      {
      this->restoreApplicationSettings();
      }
    }

  return;
}

//-----------------------------------------------------------------------------
int ctkXMLEventSource::getNextEvent(QString& widget, QString& command,
  QString& arguments, int& eventType)
{
  if (!this->XMLStream)
    {
    return EXIT_FAILURE;
    }
  if (this->XMLStream->atEnd())
    {
    return DONE;
    }
  while (!this->XMLStream->atEnd())
    {
    QXmlStreamReader::TokenType token = this->XMLStream->readNext();
    if (token == QXmlStreamReader::StartElement)
      {
      if (this->XMLStream->name() == "event")
        {
        break;
        }
      }
    }
  if (this->XMLStream->atEnd())
    {
    return DONE;
    }
  const QXmlStreamAttributes attributes = this->XMLStream->attributes();
  widget = attributes.value("widget").toString();
  command = attributes.value("command").toString();
  arguments = attributes.value("arguments").toString();
  eventType = attributes.hasAttribute("type") ?
    ctkQtTestingUtility::eventTypeFromString(this->XMLStream->attributes().value("type").toString()) :
    pqEventTypes::ACTION_EVENT;
  return SUCCESS;
}

//-----------------------------------------------------------------------------
bool ctkXMLEventSource::settingsRecorded()
{
  while(this->XMLStream->name() != "settings" && this->XMLStream->name() != "events")
    {
    this->XMLStream->readNext();
    }
  return (this->XMLStream->name() == "settings") ? true : false;
}

//-----------------------------------------------------------------------------
bool ctkXMLEventSource::settingsUpToData()
{
  QMainWindow* window = this->mainWindow();

  bool result = true;
  QMap<QObject*, QStringList> states = this->TestUtility->objectStateProperty();

  result &= (this->OldSettings.value("geometry") == QString(window->saveGeometry().toHex()));
  result &= (this->OldSettings.value("state") == QString(window->saveState().toHex()));

  QMap<QObject*, QStringList>::iterator iter;
  for(iter = states.begin() ; iter!=states.end() ; ++iter)
    {
    foreach(QString property, iter.value())
      {
      result &= (this->OldSettings.value(QString("appsetting_%1").arg(property)) ==
                iter.key()->property(property.toLatin1()).toString());
      }
    }

  return result;
}

//-----------------------------------------------------------------------------
bool ctkXMLEventSource::restoreApplicationSettings()
{
  QMainWindow* window = this->mainWindow();

  bool result = false;
  QMap<QObject*, QStringList> states = this->TestUtility->objectStateProperty();
  qDebug() << "restoreApplicationSetting" << states;

  if (!this->Automatic)
    {
    if (QMessageBox::No == QMessageBox::warning(0, tr("Playback ..."),
                                                tr("The settings are differents from the record Settings.\n"
                                                   "Do you want to restore the settings?"),
                                                QMessageBox::Yes | QMessageBox::No,
                                                QMessageBox::Yes))
      {
      return false;
      }
    }

  result = window->restoreState(
              QByteArray::fromHex(QByteArray(this->OldSettings.value("state").toLocal8Bit().constData())));
  result = window->restoreGeometry(
              QByteArray::fromHex(QByteArray(this->OldSettings.value("geometry").toLocal8Bit().constData())));

  QMap<QObject*, QStringList>::iterator iter;
  for(iter = states.begin() ; iter!=states.end() ; ++iter)
    {
    foreach(QString property, iter.value())
      {
      iter.key()->setProperty(property.toLatin1(),
                              QVariant(this->OldSettings.value(
                              QString("appsetting_%1").arg(property))));
      }
    }


  return result;
}

//-----------------------------------------------------------------------------
QMap<QString, QString> ctkXMLEventSource::recoverSettingsFromXML()
{
  // Recover the settings
  QMap<QString, QString> settings;
  while (this->XMLStream->tokenType() != QXmlStreamReader::EndElement ||
         this->XMLStream->name() != "settings")
    {
    this->XMLStream->readNext();
    if (!this->XMLStream->name().isEmpty() &&
        this->XMLStream->tokenType() == QXmlStreamReader::StartElement)
      {
      QString key = this->XMLStream->name().toString();
      // There might be multiple appsetting elements, ensure key is unique
      if (this->XMLStream->name().toString() == "appsetting")
        {
        key += "_" + this->XMLStream->attributes().value("command").toString();
        }
      settings[key].append(
                      this->XMLStream->attributes().value("arguments").toString());
      }
    }
  return settings;
}

//-----------------------------------------------------------------------------
QMainWindow* ctkXMLEventSource::mainWindow()
{
  QMainWindow* window = NULL;
  foreach(QWidget * widget, QApplication::topLevelWidgets())
    {
    window = qobject_cast<QMainWindow*>(widget);
    if (window)
      {
      return window;
      }
    }
  return 0;
}
