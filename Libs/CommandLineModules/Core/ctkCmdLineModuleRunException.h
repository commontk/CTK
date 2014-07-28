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

#ifndef CTKCMDLINEMODULERUNEXCEPTION_H
#define CTKCMDLINEMODULERUNEXCEPTION_H

#include "ctkCommandLineModulesCoreExport.h"

#include <ctkException.h>

#if (QT_VERSION < 0x50000)
#include <QtCore>
#else
#include <QtConcurrent>
#endif


/**
 * \class ctkCmdLineModuleRunException
 * \brief Exception class to describe problems with running the module.
 * \ingroup CommandLineModulesCore_API
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleRunException
    : public QtConcurrent::Exception, public ctkException
{
public:

  explicit ctkCmdLineModuleRunException(const QUrl& location, int errorCode,
                                        const QString& errorString);

  ctkCmdLineModuleRunException(const QUrl& location, int errorCode,
                               const QString& errorString, const ctkCmdLineModuleRunException& cause);

  ctkCmdLineModuleRunException(const ctkCmdLineModuleRunException& o);

  ctkCmdLineModuleRunException& operator=(const ctkCmdLineModuleRunException& o);

  ~ctkCmdLineModuleRunException() throw();

  QUrl location() const throw();
  int errorCode() const throw();
  QString errorString() const throw();

  virtual const char* name() const throw();
  virtual const char* className() const throw();
  virtual ctkCmdLineModuleRunException* clone() const;
  virtual void rethrow() const;

  virtual void raise() const;

private:

  QUrl Location;
  int ErrorCode;
  QString ErrorString;

};

#endif // CTKCMDLINEMODULERUNEXCEPTION_H
