/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

// Qt includes
#include <QDebug>
#include <QSqlDatabase>

#include "ctkLogger.h"

// log4cpp
#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>


class ctkLogger::ctkInternal {
public:
  ctkInternal( QString name ) : Logger ( ::log4cpp::Category::getInstance ( name.toStdString().c_str() ) ) {
  }
  log4cpp::Category& Logger;
};

ctkLogger::ctkLogger(QString name, QObject* _parent): Superclass(_parent)
{
  this->Internal = new ctkInternal ( name );
}

ctkLogger::~ctkLogger()
{
  delete this->Internal;
}



void ctkLogger::debug ( QString s ) 
{ 
  this->Internal->Logger.debug ( s.toStdString() );
}
void ctkLogger::info ( QString s ) 
{ 
  this->Internal->Logger.info ( s.toStdString() );
}
void ctkLogger::notice ( QString s ) 
{ 
  this->Internal->Logger.notice ( s.toStdString() );
}
void ctkLogger::warn ( QString s ) 
{ 
  this->Internal->Logger.warn ( s.toStdString() );
}
void ctkLogger::warning ( QString s ) 
{ 
  this->Internal->Logger.warn ( s.toStdString() );
}
void ctkLogger::error ( QString s ) 
{ 
  this->Internal->Logger.error ( s.toStdString() );
}
void ctkLogger::crit ( QString s ) 
{ 
  this->Internal->Logger.crit ( s.toStdString() );
}
void ctkLogger::critical ( QString s ) 
{ 
  this->Internal->Logger.crit ( s.toStdString() );
}
void ctkLogger::alert ( QString s ) 
{ 
  this->Internal->Logger.alert ( s.toStdString() );
}
void ctkLogger::emerg ( QString s ) 
{ 
  this->Internal->Logger.emerg ( s.toStdString() );
}
void ctkLogger::emergercy ( QString s ) 
{ 
  this->Internal->Logger.emerg ( s.toStdString() );
}
void ctkLogger::fatal ( QString s ) 
{ 
  this->Internal->Logger.fatal ( s.toStdString() );
}

void ctkLogger::setDebug() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::DEBUG ); 
}
void ctkLogger::setInfo() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::INFO ); 
}
void ctkLogger::setNotice() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::NOTICE ); 
}
void ctkLogger::setWarn() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::WARN ); 
}
void ctkLogger::setWarning() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::WARN ); 
}
void ctkLogger::setError() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::ERROR ); 
}
void ctkLogger::setCrit() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::CRIT ); 
}
void ctkLogger::setCritical() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::CRIT ); 
}
void ctkLogger::setAlert() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::ALERT ); 
}
void ctkLogger::setEmerg() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::EMERG ); 
}
void ctkLogger::setEmergercy() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::EMERG ); 
}
void ctkLogger::setFatal() 
{ 
  this->Internal->Logger.setPriority ( log4cpp::Priority::FATAL ); 
}

bool ctkLogger::isDebugEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::DEBUG ); 
}
bool ctkLogger::isInfoEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::INFO ); 
}
bool ctkLogger::isNoticeEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::NOTICE ); 
}
bool ctkLogger::isWarnEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::WARN ); 
}
bool ctkLogger::isWarningEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::WARN ); 
}
bool ctkLogger::isErrorEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::ERROR ); 
}
bool ctkLogger::isCritEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::CRIT ); 
}
bool ctkLogger::isCriticalEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::CRIT ); 
}
bool ctkLogger::isAlertEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::ALERT ); 
}
bool ctkLogger::isEmergEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::EMERG ); 
}
bool ctkLogger::isEmergercyEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::EMERG ); 
}
bool ctkLogger::isFatalEnabled() 
{ 
  return this->Internal->Logger.isPriorityEnabled ( log4cpp::Priority::FATAL ); 
}


