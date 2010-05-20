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

