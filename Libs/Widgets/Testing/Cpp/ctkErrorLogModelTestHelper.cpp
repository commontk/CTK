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
#include <QDateTime>
#include <QFile>
#include <QMutexLocker>
#include <QSharedPointer>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <QThread>

// CTK includes
#include "ctkErrorLogModel.h"

namespace
{
//-----------------------------------------------------------------------------
// Utility function

//-----------------------------------------------------------------------------
QString checkRowCount(int line, int currentRowCount, int expectedRowCount)
{
  if (currentRowCount != expectedRowCount)
    {
    QString errorMsg("Line %1 - Expected rowCount: %2 - Current rowCount: %3\n");
    return errorMsg.arg(line).arg(expectedRowCount).arg(currentRowCount);
    }
  return QString();
}

//-----------------------------------------------------------------------------
QString checkSpyCount(int line, int currentSpyCount, int expectedSpyCount)
{
  if (currentSpyCount != expectedSpyCount)
    {
    QString errorMsg("Line %1 - Expected spyCount: %2 - Current spyCount: %3\n");
    return errorMsg.arg(line).arg(expectedSpyCount).arg(currentSpyCount);
    }
  return QString();
}

//-----------------------------------------------------------------------------
QString checkTextMessages(int line, const QStringList& currentMessages, const QStringList& expectedMessages)
{
  for(int i = 0; i < expectedMessages.count(); ++i)
    {
    if (!expectedMessages.contains(currentMessages.at(i)))
      {
      QString errorMsg("Line %1 - Problem with logged messages !\n"
                       "\tMessage [%2] hasn't been logged !\n");
      return errorMsg.arg(line).arg(expectedMessages.value(i));
      }
    }
  return QString();
}

//-----------------------------------------------------------------------------
QString checkTextMessages(int line, const ctkErrorLogModel& model, const QStringList& expectedMessages)
{
  QStringList currentMessages;
  for(int i = 0; i < expectedMessages.count(); ++i)
    {
    QModelIndex descriptionIndex = model.index(i, ctkErrorLogModel::DescriptionColumn);
    currentMessages << descriptionIndex.data(ctkErrorLogModel::DescriptionTextRole).toString();
    }
  return checkTextMessages(line, currentMessages, expectedMessages);
}

//-----------------------------------------------------------------------------
void printTextMessages(const ctkErrorLogModel& model)
{
  fprintf(stdout, "%s", "ErrorLogModel rows:\n");
  QString text("\trow %1 => [%2]\n");
  for (int i=0; i < model.rowCount(); ++i)
    {
    QString description =
        model.index(i, ctkErrorLogModel::DescriptionColumn).data().toString();
    fprintf(stdout, "%s", qPrintable(text.arg(i).arg(description)));
    }
  fflush(stdout);
}

//-----------------------------------------------------------------------------
void printErrorMessage(const QString& errorMessage)
{
  fprintf(stderr, "%s", qPrintable(errorMessage));
  fflush(stderr);
}

//-----------------------------------------------------------------------------
QString checkInteger(int line, const char* valueName, int current, int expected)
{
  if (current != expected)
    {
    QString errorMsg("Line %1 - Expected %2: %3 - Current %4: %5\n");
    return errorMsg.arg(line).arg(valueName).
        arg(expected).arg(valueName).arg(current);
    }
  return QString();
}

//-----------------------------------------------------------------------------
QString checkBoolean(int line, const char* valueName, bool current, bool expected)
{
  if (current != expected)
    {
    QString errorMsg("Line %1 - Expected %2: %3 - Current %4: %5\n");
    return errorMsg.arg(line).arg(valueName).
        arg(static_cast<int>(expected)).arg(valueName).arg(static_cast<int>(current));
    }
  return QString();
}

//-----------------------------------------------------------------------------
QString checkString(int line, const char* valueName, QString current, QString expected)
{
  if (current != expected)
    {
    QString errorMsg("Line %1 - Expected %2: %3 - Current %4: %5\n");
    return errorMsg.arg(line).arg(valueName).
        arg(static_cast<QString>(expected)).arg(valueName).arg(static_cast<QString>(current));
    }
  return QString();
}

//-----------------------------------------------------------------------------
void processEvents(int durationInMSecs)
{
  QTimer timer;
  timer.setSingleShot(true);
  timer.start(durationInMSecs);
  while(timer.isActive())
    {
    QCoreApplication::processEvents();
    }
}

//-----------------------------------------------------------------------------
QMutex AppendToFileMutex;

//-----------------------------------------------------------------------------
void appendToFile(const QString& fileName, const QString& text)
{
  QMutexLocker locker(&AppendToFileMutex);
  QFile f(fileName);
  f.open(QFile::Append);
  QTextStream s(&f);
  s << QDateTime::currentDateTime().toString() << " - " << text << "\n";
  f.close();
}

//-----------------------------------------------------------------------------
QStringList readFile(const QString& filePath)
{
  QStringList lines;
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
    return lines;
    }
   QTextStream in(&file);
   while(!in.atEnd())
     {
     lines << in.readLine();
     }
   file.close();
   return lines;
}

//-----------------------------------------------------------------------------
class LogMessageThread : public QThread
{
public:
  LogMessageThread(int id, int maxIteration) :
    Id(id), MaxIteration(maxIteration), Counter(0){}

protected:
  void run();

  virtual void logMessage(const QDateTime& dateTime, int threadId, int counterIdx) = 0;
private:
  int Id;
  int MaxIteration;
  int Counter;
};

//-----------------------------------------------------------------------------
void LogMessageThread::run()
{
  while(this->Counter < this->MaxIteration)
    {
    this->logMessage(QDateTime::currentDateTime(), this->Id, this->Counter);
    ++this->Counter;
    }
}

////-----------------------------------------------------------------------------
//template<class LogMessageThreadType>
//class SynchronousLogMessageStarterThread : public QThread
//{
//public:
//  SynchronousLogMessageStarterThread(ctkErrorLogModel * errorLogModel, int threadCount, int maxIteration) :
//    ErrorLogModel(errorLogModel), ThreadCount(threadCount), MaxIteration(maxIteration)
//  {
//    this->ErrorLogModel->setAsynchronousLogging(false);
//  }

//protected:
//  void run();

//private:
//  QList<QSharedPointer<LogMessageThread> > ThreadList;
//  ctkErrorLogModel * ErrorLogModel;
//  int ThreadCount;
//  int MaxIteration;
//};

////-----------------------------------------------------------------------------
//template<class LogMessageThreadType>
//void SynchronousLogMessageStarterThread<LogMessageThreadType>::run()
//{
//  for(int i = 0; i < this->ThreadCount; ++i)
//    {
//    this->ThreadList << QSharedPointer<LogMessageThread>(new LogMessageThreadType(i, this->MaxIteration));
//    this->ThreadList.back()->start();
//    }
//  foreach(const QSharedPointer<LogMessageThread>& thread, this->ThreadList)
//    {
//    thread->wait();
//    }

//  int expectedMessageCount = this->ThreadCount * this->MaxIteration * 2;
//  QString errorMsg = checkRowCount(__LINE__, this->ErrorLogModel->rowCount(),
//                                   /* expected = */ expectedMessageCount);
//  if (!errorMsg.isEmpty())
//    {
//    this->ErrorLogModel->disableAllMsgHandler();
//    printErrorMessage(errorMsg);
//    printTextMessages(*this->ErrorLogModel);
//    QCoreApplication::exit(EXIT_FAILURE);
//    }
//}

//-----------------------------------------------------------------------------
QList<QSharedPointer<LogMessageThread> > ThreadList;

//-----------------------------------------------------------------------------
template<class LogMessageThreadType>
void startLogMessageThreads(int threadCount, int maxIteration)
{
  for(int i = 0; i < threadCount; ++i)
    {
    ThreadList << QSharedPointer<LogMessageThread>(new LogMessageThreadType(i, maxIteration));
    ThreadList.back()->start();
    }
}

} // end namespace
