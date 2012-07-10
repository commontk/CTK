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

#ifndef CTKCMDLINEMODULEPROCESSEXCEPTION_H
#define CTKCMDLINEMODULEPROCESSEXCEPTION_H

#include <qtconcurrentexception.h>

#include <QProcess>

class ctkCmdLineModuleProcessException : public QtConcurrent::Exception
{
public:

  ctkCmdLineModuleProcessException(const QString& msg, int code = 0,
                            QProcess::ExitStatus status = QProcess::NormalExit);

  ~ctkCmdLineModuleProcessException() throw() {}

  int exitCode() const;

  QProcess::ExitStatus exitStatus() const;

  QString message() const;

  const char* what() const throw();

  void raise() const;
  ctkCmdLineModuleProcessException* clone() const;

private:

  QString msg;
  int code;
  QProcess::ExitStatus status;
};

#endif // CTKCMDLINEMODULEPROCESSEXCEPTION_H
