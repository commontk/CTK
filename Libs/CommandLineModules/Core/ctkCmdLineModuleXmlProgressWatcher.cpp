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

#include "ctkCmdLineModuleXmlProgressWatcher.h"

#include <QIODevice>
#include <QProcess>
#include <QXmlStreamReader>

#include <QDebug>

namespace {

static QString FILTER_START = "filter-start";
static QString FILTER_NAME = "filter-name";
static QString FILTER_COMMENT = "filter-comment";
static QString FILTER_PROGRESS = "filter-progress";
static QString FILTER_END = "filter-end";

}

//----------------------------------------------------------------------------
class ctkCmdLineModuleXmlProgressWatcherPrivate
{
public:

  ctkCmdLineModuleXmlProgressWatcherPrivate(QIODevice* input, ctkCmdLineModuleXmlProgressWatcher* qq)
    : input(input), process(NULL), readPos(0), q(qq), error(false), currentProgress(0)
  {
    // wrap the content in an artifical root element
    reader.addData("<module-root>");
  }

  ctkCmdLineModuleXmlProgressWatcherPrivate(QProcess* input, ctkCmdLineModuleXmlProgressWatcher* qq)
    : input(input), process(input), readPos(0), q(qq), error(false), currentProgress(0)
  {
    // wrap the content in an artifical root element
    reader.addData("<module-root>");
  }

  void _q_readyRead()
  {
    input->seek(readPos);
    reader.addData(input->readAll());
    readPos = input->pos();
    parseProgressXml();
  }

  void _q_readyReadError()
  {
    emit q->errorDataAvailable(process->readAllStandardError());
  }

  void parseProgressXml()
  {
    QXmlStreamReader::TokenType type = reader.readNext();
    QByteArray outputData;
    while(type != QXmlStreamReader::Invalid)
    {
      switch(type)
      {
      case QXmlStreamReader::NoToken: break;
      case QXmlStreamReader::Characters:
      {
        if (stack.empty())
        {
          QByteArray output(reader.text().toAscii());
          // get rid of a possible newline after the last xml end tag
          if (output.startsWith('\n')) output = output.remove(0,1);
          outputData.append(output);
          break;
        }

        if (stack.size() == 2 && stack.front() == FILTER_START)
        {
          if (stack.back() == FILTER_NAME)
          {
            currentName = reader.text().toString().trimmed();
          }
          else if (stack.back() == FILTER_COMMENT)
          {
            currentComment = reader.text().toString().trimmed();
          }
        }
        else if (stack.size() == 1 && stack.back() == FILTER_PROGRESS)
        {
          currentProgress = reader.text().toString().toFloat();
        }
        break;
      }
      case QXmlStreamReader::StartElement:
      {
        QStringRef name = reader.name();
        QString parent;
        if (!stack.empty()) parent = stack.back();

        if (name.compare("module-root") != 0)
        {
          stack.push_back(name.toString());
        }

        if (name.compare(FILTER_START, Qt::CaseInsensitive) == 0 ||
            name.compare(FILTER_PROGRESS, Qt::CaseInsensitive) == 0 ||
            name.compare(FILTER_END, Qt::CaseInsensitive) == 0)
        {
          if (!parent.isEmpty())
          {
            unexpectedNestedElement(name.toString());
            break;
          }

          if (name.compare(FILTER_START, Qt::CaseInsensitive) == 0)
          {
            currentName.clear();
            currentComment.clear();
            currentProgress = 0;
          }
        }
        break;
      }
      case QXmlStreamReader::EndElement:
      {
        QStringRef name = reader.name();

        QString curr;
        QString parent;
        if (!stack.empty())
        {
          curr = stack.back();
          stack.pop_back();
          if (!stack.empty()) parent = stack.back();
        }

        if (parent.isEmpty())
        {
          if (name.compare(FILTER_START, Qt::CaseInsensitive) == 0)
          {
            emit q->filterStarted(currentName, currentComment);
          }
          else if (name.compare(FILTER_PROGRESS, Qt::CaseInsensitive) == 0)
          {
            emit q->filterProgress(currentProgress);
          }
          else if (name.compare(FILTER_END, Qt::CaseInsensitive) == 0)
          {
            emit q->filterFinished(currentName);
          }
        }
        break;
      }
      default:
        break;
      }
      type = reader.readNext();
    }

    if (type == QXmlStreamReader::Invalid && reader.error() != QXmlStreamReader::PrematureEndOfDocumentError)
    {
      if (!error)
      {
        error = true;
        emit q->filterXmlError(QString("Error parsing XML at line %1, column %2: ")
                               .arg(reader.lineNumber()).arg(reader.columnNumber()) + reader.errorString());
      }
    }
    if (!outputData.isEmpty())
    {
      emit q->outputDataAvailable(outputData);
    }
  }

  void unexpectedNestedElement(const QString& element)
  {
    if (!error)
    {
      error = true;
      emit q->filterXmlError(QString("\"%1\" must be a top-level element, found at line %2.")
                             .arg(element).arg(reader.lineNumber()));
    }
  }

  QIODevice* input;
  QProcess* process;
  qint64 readPos;
  ctkCmdLineModuleXmlProgressWatcher* q;
  bool error;
  QXmlStreamReader reader;
  QList<QString> stack;
  QString currentName;
  QString currentComment;
  float currentProgress;
};


//----------------------------------------------------------------------------
ctkCmdLineModuleXmlProgressWatcher::ctkCmdLineModuleXmlProgressWatcher(QIODevice* input)
  : d(new ctkCmdLineModuleXmlProgressWatcherPrivate(input, this))
{
  if (d->input == NULL) return;

  if (!(d->input->openMode() & QIODevice::ReadOnly))
  {
    input->open(QIODevice::ReadOnly);
  }
  connect(d->input, SIGNAL(readyRead()), SLOT(_q_readyRead()));
}

//----------------------------------------------------------------------------
ctkCmdLineModuleXmlProgressWatcher::ctkCmdLineModuleXmlProgressWatcher(QProcess* input)
  : d(new ctkCmdLineModuleXmlProgressWatcherPrivate(input, this))
{
  if (d->input == NULL) return;

  connect(input, SIGNAL(readyReadStandardOutput()), SLOT(_q_readyRead()));
  connect(input, SIGNAL(readyReadStandardError()), SLOT(_q_readyReadError()));
}

//----------------------------------------------------------------------------
ctkCmdLineModuleXmlProgressWatcher::~ctkCmdLineModuleXmlProgressWatcher()
{
}

#include "moc_ctkCmdLineModuleXmlProgressWatcher.cxx"
