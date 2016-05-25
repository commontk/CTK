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
#include <QDebug>

// CTK includes
#include "ctkDICOMUtil.h"

// DCMTK includes
#include <dcmtk/oflog/oflog.h>

//------------------------------------------------------------------------------
void ctk::setDICOMLogLevel(ctkErrorLogLevel::LogLevel level)
{
  dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
  switch (level)
    {
    case ctkErrorLogLevel::Trace: log.setLogLevel(OFLogger::TRACE_LOG_LEVEL); break;
    case ctkErrorLogLevel::Debug: log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL); break;
    case ctkErrorLogLevel::Info: log.setLogLevel(OFLogger::INFO_LOG_LEVEL); break;
    case ctkErrorLogLevel::Warning: log.setLogLevel(OFLogger::WARN_LOG_LEVEL); break;
    case ctkErrorLogLevel::Error: log.setLogLevel(OFLogger::ERROR_LOG_LEVEL); break;
    case ctkErrorLogLevel::Fatal: log.setLogLevel(OFLogger::FATAL_LOG_LEVEL); break;
    default:
      qWarning() << "Failed to set DICOM log level - Supported levels are Trace, Debug, "
                    "Info, Warning, Error and Fatal !";
      break;
    }
}

//------------------------------------------------------------------------------
ctkErrorLogLevel::LogLevel ctk::dicomLogLevel()
{
  dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
  switch (log.getLogLevel())
    {
    case OFLogger::TRACE_LOG_LEVEL: return ctkErrorLogLevel::Trace;
    case OFLogger::DEBUG_LOG_LEVEL: return ctkErrorLogLevel::Debug;
    case OFLogger::INFO_LOG_LEVEL: return ctkErrorLogLevel::Info;
    case OFLogger::WARN_LOG_LEVEL: return ctkErrorLogLevel::Warning;
    case OFLogger::ERROR_LOG_LEVEL: return ctkErrorLogLevel::Error;
    case OFLogger::FATAL_LOG_LEVEL: return ctkErrorLogLevel::Fatal;
    default: return ctkErrorLogLevel::None;
    }
}

//------------------------------------------------------------------------------
QString ctk::dicomLogLevelAsString()
{
  return ctkErrorLogLevel::logLevelAsString(ctk::dicomLogLevel());
}
