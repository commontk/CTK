/*=========================================================================

  Library:   CTK

  Copyright (c) Brigham & Women's Hospital

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
#include <QStringList>
#include <QFlags>

// ctkDICOMCore includes
#include "ctkDICOMUtil.h"
#include "ctkLogger.h"

// DCMTK includes
#include <dcmtk/dcmnet/diutil.h>

static ctkLogger logger ( "org.commontk.dicom.DICOMUtil" );

//------------------------------------------------------------------------------
class ctkDICOMUtilPrivate
{
public:
  ctkDICOMUtilPrivate() {};
  ~ctkDICOMUtilPrivate() {};
};

//------------------------------------------------------------------------------
// ctkDICOMUtil methods

//------------------------------------------------------------------------------
ctkDICOMUtil::ctkDICOMUtil(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkDICOMUtilPrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMUtil::~ctkDICOMUtil()
{
}

//------------------------------------------------------------------------------
void ctkDICOMUtil::setDICOMLogLevel(ctkDICOMUtil::LogLevel level)
{
  dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
  
  switch (level)
    {
    case ctkDICOMUtil::Trace: log.setLogLevel(OFLogger::TRACE_LOG_LEVEL); break;
    case ctkDICOMUtil::Debug: log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL); break;
    case ctkDICOMUtil::Info: log.setLogLevel(OFLogger::INFO_LOG_LEVEL); break;
    case ctkDICOMUtil::Warning: log.setLogLevel(OFLogger::WARN_LOG_LEVEL); break;
    case ctkDICOMUtil::Error: log.setLogLevel(OFLogger::ERROR_LOG_LEVEL); break;
    case ctkDICOMUtil::Fatal: log.setLogLevel(OFLogger::FATAL_LOG_LEVEL); break;
    default:
      logger.info("ctkDICOMUtil::setDICOMLogLevel: Unsupported DICOM log level specified");
      break;
    }
}

ctkDICOMUtil::LogLevel ctkDICOMUtil::getDICOMLogLevel() const
{
  dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
  
  switch (log.getLogLevel())
    {
    case OFLogger::TRACE_LOG_LEVEL: return ctkDICOMUtil::Trace;
    case OFLogger::DEBUG_LOG_LEVEL: return ctkDICOMUtil::Debug;
    case OFLogger::INFO_LOG_LEVEL: return ctkDICOMUtil::Info;
    case OFLogger::WARN_LOG_LEVEL: return ctkDICOMUtil::Warning;
    case OFLogger::ERROR_LOG_LEVEL: return ctkDICOMUtil::Error;
    case OFLogger::FATAL_LOG_LEVEL: return ctkDICOMUtil::Fatal;
    default:
      return ctkDICOMUtil::None;
    }
}

// --------------------------------------------------------------------------
QString ctkDICOMUtil::DICOMLogLevel()const
{
  ctkDICOMUtil::LogLevel logLevel = getDICOMLogLevel();
  QMetaEnum logLevelEnum = this->metaObject()->enumerator(0);
  Q_ASSERT(QString("LogLevel").compare(logLevelEnum.name()) == 0);
  return QLatin1String(logLevelEnum.valueToKey(logLevel));
}
