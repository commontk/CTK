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
#include <QLayout>
#include <QMainWindow>
#include <QVariant>

// CTKQtTesting includes
#include "ctkQtTestingUtility.h"
#include "ctkXMLEventObserver.h"

//-----------------------------------------------------------------------------
// ctkXMLEventObserver methods

//-----------------------------------------------------------------------------
ctkXMLEventObserver::ctkXMLEventObserver(QObject* p)
  : pqEventObserver(p)
{
  this->XMLStream = NULL;
  this->TestUtility = qobject_cast<pqTestUtility*>(p);
  Q_ASSERT(this->TestUtility);
}

//-----------------------------------------------------------------------------
ctkXMLEventObserver::~ctkXMLEventObserver()
{
  delete this->XMLStream;
  this->TestUtility = 0;
}

//-----------------------------------------------------------------------------
void ctkXMLEventObserver::recordApplicationSettings()
{
  Q_ASSERT(this->TestUtility);
  if (!this->XMLStream)
  {
    return;
  }
  this->XMLStream->writeStartElement("settings");

  // Information about the application
  this->recordApplicationSetting("name","qApp", "applicationName",
                                 QCoreApplication::applicationName());
  this->recordApplicationSetting("version" , "qApp", "applicationVersion",
                                 QCoreApplication::applicationVersion());

  // save Geometry and State of the application
  QMainWindow* window = NULL;
  foreach(QWidget * widget, QApplication::topLevelWidgets())
  {
    window = qobject_cast<QMainWindow*>(widget);
    if (window)
    {
      this->recordApplicationSetting("geometry" , "MainWindow", "mainWindowGeometry",
                                     QString(window->saveGeometry().toHex()));

      this->recordApplicationSetting("state" , "MainWindow", "mainWindowState",
                                     QString(window->saveState().toHex()));
      break;
    }
  }

  // Save extra properties from the application
  QMap<QObject*, QStringList> states = this->TestUtility->objectStateProperty();
  QMap<QObject*, QStringList>::iterator iter;
  for(iter = states.begin() ; iter!=states.end() ; ++iter)
  {
    foreach(QString property, iter.value())
    {
      this->recordApplicationSetting(
        QString("appsetting"),
        iter.key()->metaObject()->className(),
        property,
        iter.key()->property(property.toLatin1()).toString()
      );
    }
  }

  this->XMLStream->writeEndElement();
}

//-----------------------------------------------------------------------------
void ctkXMLEventObserver::recordApplicationSetting(const QString &startElement,
                                                   const QString &attribute1,
                                                   const QString &attribute2,
                                                   const QString &attribute3)
{
  this->XMLStream->writeStartElement(startElement);
  this->XMLStream->writeAttribute("widget", attribute1);
  this->XMLStream->writeAttribute("command", attribute2);
  this->XMLStream->writeAttribute("arguments", attribute3);
  this->XMLStream->writeEndElement();
}

//-----------------------------------------------------------------------------
void ctkXMLEventObserver::setStream(QTextStream* stream)
{
  if (this->XMLStream)
  {
    this->XMLStream->writeEndElement();
    this->XMLStream->writeEndElement();
    this->XMLStream->writeEndDocument();
    delete this->XMLStream;
    this->XMLStream = NULL;
  }
  if (this->Stream)
  {
    *this->Stream << this->XMLString;
  }
  this->XMLString = QString();
  pqEventObserver::setStream(stream);
  if (this->Stream)
  {
    this->XMLStream = new QXmlStreamWriter(&this->XMLString);
    this->XMLStream->setAutoFormatting(true);
    this->XMLStream->writeStartDocument();
    this->XMLStream->writeStartElement("QtTesting");
    this->recordApplicationSettings();
    this->XMLStream->writeStartElement("events");
  }
}

//-----------------------------------------------------------------------------
/// Return a copy of \a str with characters that are illegal in XML 1.0
/// attribute values removed.
///
/// XML 1.0 only allows: #x9 (tab), #xA (LF), #xD (CR), and #x20 and above.
/// Characters in the range #x1–#x8, #xB–#xC, #xE–#x1F are prohibited even
/// as numeric character references (e.g. &#8; is invalid).  QXmlStreamWriter
/// passes them through as raw bytes, producing malformed XML that fails
/// validation (QtXmlPatterns FODC0002).
///
/// Carriage return (#xD) is legal and QXmlStreamWriter encodes it as &#13;
/// so it round-trips correctly through the XML reader.
static QString xmlSafeArguments(const QString& str)
{
  QString safe;
  safe.reserve(str.size());
  for (const QChar& ch : str)
  {
    const ushort code = ch.unicode();
    // Keep: tab (#x9), LF (#xA), CR (#xD), and everything >= space (#x20)
    if (code == 0x09 || code == 0x0A || code == 0x0D || code >= 0x20)
    {
      safe.append(ch);
    }
    // All other control characters (0x01–0x08, 0x0B–0x0C, 0x0E–0x1F) are
    // dropped.  For key events, the key() code field already identifies the
    // key; the text() field is only meaningful for printable characters.
  }
  return safe;
}

//-----------------------------------------------------------------------------
void ctkXMLEventObserver::onRecordEvent(const QString& widget,
                                        const QString& command,
                                        const QString& arguments,
                                        const int& eventType)
{
  if(this->XMLStream)
  {
    this->XMLStream->writeStartElement("event");
    this->XMLStream->writeAttribute("widget", widget);
    this->XMLStream->writeAttribute("command", command);
    this->XMLStream->writeAttribute("arguments", xmlSafeArguments(arguments));
    this->XMLStream->writeAttribute("type", ctkQtTestingUtility::eventTypeToString(eventType));
    this->XMLStream->writeEndElement();
    if (this->Stream)
    {
      *this->Stream << this->XMLString;
    }
    this->XMLString = QString();
    emit this->eventRecorded(widget, command, arguments, eventType);
  }
}
