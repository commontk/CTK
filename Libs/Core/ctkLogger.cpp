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
#include <QDebug>

// CTK includes
#include <ctkLogger.h>

// Log4Qt includes
//#include <log4qt/log4qt.h>
//#include <log4qt/logger.h>
//#include <log4qt/basicconfigurator.h>

//-----------------------------------------------------------------------------
class ctkLoggerPrivate
{
public:
  //Log4Qt::Logger *Logger;
};

//-----------------------------------------------------------------------------
ctkLogger::ctkLogger(QString name, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkLoggerPrivate)
{
  Q_UNUSED(name);
  //Q_D(ctkLogger);
  //d->Logger = Log4Qt::Logger::logger( name.toStdString().c_str());
}

//-----------------------------------------------------------------------------
ctkLogger::~ctkLogger()
{
}

////-----------------------------------------------------------------------------
//void ctkLogger::configure()
//{
//  //Log4Qt::BasicConfigurator::configure();
//}

//-----------------------------------------------------------------------------
void ctkLogger::debug(const QString& s)
{
  //Q_D(ctkLogger);
  //d->Logger->debug(s);
  qDebug().nospace() << qUtf8Printable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::info(const QString& s)
{
  //Q_D(ctkLogger);
  //d->Logger->info(s);
  qDebug().nospace() << qUtf8Printable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::trace(const QString& s)
{
  //Q_D(ctkLogger);
  //d->Logger->trace(s);
  qDebug().nospace() << qUtf8Printable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::warn(const QString& s)
{
  //Q_D(ctkLogger);
  //d->Logger->warn(s);
  qWarning().nospace() << qUtf8Printable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::error(const QString& s)
{
  //Q_D(ctkLogger);
  //d->Logger->error(s);
  qCritical().nospace() << qUtf8Printable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::fatal(const QString& s)
{
  //Q_D(ctkLogger);
  //d->Logger->fatal(s);
  qCritical().nospace() << qUtf8Printable(s);
}

////-----------------------------------------------------------------------------
//void ctkLogger::setOff()
//{
//  //Q_D(ctkLogger);
//  //d->Logger->setLevel(Log4Qt::Level(Log4Qt::Level::OFF_INT));
//}

////-----------------------------------------------------------------------------
//void ctkLogger::setDebug()
//{
//  //Q_D(ctkLogger);
//  //d->Logger->setLevel(Log4Qt::Level(Log4Qt::Level::DEBUG_INT));
//}

////-----------------------------------------------------------------------------
//void ctkLogger::setInfo()
//{
//  //Q_D(ctkLogger);
//  //d->Logger->setLevel(Log4Qt::Level(Log4Qt::Level::INFO_INT));
//}

////-----------------------------------------------------------------------------
//void ctkLogger::setTrace()
//{
//  //Q_D(ctkLogger);
//  //d->Logger->setLevel(Log4Qt::Level(Log4Qt::Level::TRACE_INT));
//}

////-----------------------------------------------------------------------------
//void ctkLogger::setWarn()
//{
//  //Q_D(ctkLogger);
//  //d->Logger->setLevel(Log4Qt::Level(Log4Qt::Level::WARN_INT));
//}

////-----------------------------------------------------------------------------
//void ctkLogger::setError()
//{
//  //Q_D(ctkLogger);
//  //d->Logger->setLevel(Log4Qt::Level(Log4Qt::Level::ERROR_INT));
//}

////-----------------------------------------------------------------------------
//void ctkLogger::setFatal()
//{
//  //Q_D(ctkLogger);
//  //d->Logger->setLevel(Log4Qt::Level(Log4Qt::Level::FATAL_INT));
//}

////-----------------------------------------------------------------------------
//bool ctkLogger::isOffEnabled()
//{
//  //Q_D(ctkLogger); //Not sure
//  //return d->Logger->isEnabledFor(Log4Qt::Level(Log4Qt::Level::OFF_INT));
//}

////-----------------------------------------------------------------------------
//bool ctkLogger::isDebugEnabled()
//{
//  //Q_D(ctkLogger);
//  //return d->Logger->isDebugEnabled();
//}

////-----------------------------------------------------------------------------
//bool ctkLogger::isInfoEnabled()
//{
//  //Q_D(ctkLogger);
//  //return d->Logger->isInfoEnabled();
//}

////-----------------------------------------------------------------------------
//bool ctkLogger::isTraceEnabled()
//{
//  //Q_D(ctkLogger);
//  //return d->Logger->isTraceEnabled();
//}

////-----------------------------------------------------------------------------
//bool ctkLogger::isWarnEnabled()
//{
//  //Q_D(ctkLogger);
//  //return d->Logger->isWarnEnabled();
//}

////-----------------------------------------------------------------------------
//bool ctkLogger::isErrorEnabled()
//{
//  //Q_D(ctkLogger);
//  //return d->Logger->isErrorEnabled();
//}

////-----------------------------------------------------------------------------
//bool ctkLogger::isFatalEnabled()
//{
//  //Q_D(ctkLogger);
//  //return d->Logger->isFatalEnabled();
//}
