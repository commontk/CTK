/*===================================================================
  
BlueBerry Platform

Copyright (c) German Cancer Research Center, 
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without 
even the implied warranty of MERCHANTABILITY or FITNESS FOR 
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "ctkCmdLineModuleXmlProgressWatcher.h"

#include <QIODevice>
#include <QXmlStreamReader>

#include <QDebug>

namespace {

static QString FILTER_START = "filter-start";
static QString FILTER_NAME = "filter-name";
static QString FILTER_COMMENT = "filter-comment";
static QString FILTER_PROGRESS = "filter-progress";
static QString FILTER_END = "filter-end";

}

class ctkCmdLineModuleXmlProgressWatcherPrivate
{
public:

  ctkCmdLineModuleXmlProgressWatcherPrivate(QIODevice* input, ctkCmdLineModuleXmlProgressWatcher* qq)
    : input(input), q(qq), error(false)
  {}

  void _q_readyRead()
  {
    QByteArray ba = input->readAll();
    qDebug() << input->pos() << " [" << input->bytesAvailable() << "]:" << ba;
    //reader.addData(ba);
    //parseProgressXml();
  }

  void parseProgressXml()
  {
    QXmlStreamReader::TokenType type = reader.readNext();
    while(type != QXmlStreamReader::Invalid)
    {
      switch(type)
      {
      case QXmlStreamReader::NoToken: break;
      case QXmlStreamReader::StartElement:
      {
        QStringRef name = reader.name();
        if (name.compare(FILTER_START, Qt::CaseInsensitive) == 0)
        {
          stack.push_back(FILTER_START);
          currentName.clear();
          currentComment.clear();
        }
        else if (name.compare(FILTER_NAME, Qt::CaseInsensitive) == 0)
        {
          if (stack.back() == FILTER_START || stack.back() == FILTER_END)
          {
            currentName = reader.name().toString().trimmed();
          }
        }
        else if (name.compare(FILTER_COMMENT, Qt::CaseInsensitive) == 0)
        {
          if (stack.back() == FILTER_START)
          {
            currentComment = reader.name().toString().trimmed();
          }
        }
        else if (name.compare(FILTER_PROGRESS, Qt::CaseInsensitive) == 0)
        {
          if (!stack.empty())
          {
            if (!error)
            {
              emit q->filterXmlError(QString("\"%1\" must be a top-level element, found at line %2.")
                                     .arg(FILTER_PROGRESS).arg(reader.lineNumber()));
            }
            continue;
          }
          emit q->filterProgress(reader.text().toString().toFloat());
        }
        type = reader.readNext();
        break;
      }
      case QXmlStreamReader::EndElement:
      {
        QStringRef name = reader.name();
        if (name.compare(FILTER_START, Qt::CaseInsensitive) == 0)
        {
          if (stack.back() != FILTER_START)
          {
            if (!error)
            {
              emit q->filterXmlError(QString("Unexpected end tag \"%1\" found at line %2.")
                                     .arg(FILTER_END).arg(reader.lineNumber()));
            }
            continue;
          }
          stack.pop_back();
          emit q->filterStarted(currentName, currentComment);
        }
        else if (name.compare(FILTER_END, Qt::CaseInsensitive) == 0)
        {
          if (!stack.empty())
          {
            if (!error)
            {
              emit q->filterXmlError(QString("\"%1\" must be a top-level element, found at line %2.")
                                     .arg(FILTER_PROGRESS).arg(reader.lineNumber()));
            }
            continue;
          }
          stack.pop_back();
          emit q->filterFinished(currentName);
        }
        type = reader.readNext();
        break;
      }
      default:
        type = reader.readNext();
      }
    }
  }

  QIODevice* input;
  ctkCmdLineModuleXmlProgressWatcher* q;
  bool error;
  QXmlStreamReader reader;
  QList<QString> stack;
  QString currentName;
  QString currentComment;
};

ctkCmdLineModuleXmlProgressWatcher::ctkCmdLineModuleXmlProgressWatcher(QIODevice* input)
  : d(new ctkCmdLineModuleXmlProgressWatcherPrivate(input, this))
{
  if (d->input == NULL) return;

  if (!(d->input->openMode() & QIODevice::ReadOnly))
  {
    input->open(QIODevice::ReadOnly);
  }
  connect(d->input, SIGNAL(readyRead()), SLOT(_q_readyRead()));

  // start parsing
  d->_q_readyRead();
}

ctkCmdLineModuleXmlProgressWatcher::~ctkCmdLineModuleXmlProgressWatcher()
{
}

#include "moc_ctkCmdLineModuleXmlProgressWatcher.cxx"
