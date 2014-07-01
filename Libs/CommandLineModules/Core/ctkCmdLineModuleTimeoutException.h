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

#ifndef CTKCMDLINEMODULETIMEOUTEXCEPTION_H
#define CTKCMDLINEMODULETIMEOUTEXCEPTION_H

#include <ctkCmdLineModuleRunException.h>
#include "ctkCommandLineModulesCoreExport.h"

/**
 * \class ctkCmdLineModuleTimeoutException
 * \brief Exception class to describe problems with timeouts when running a module.
 * \ingroup CommandLineModulesCore_API
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleTimeoutException
    : public ctkCmdLineModuleRunException
{
public:

  explicit ctkCmdLineModuleTimeoutException(const QUrl& location, const QString& errorString);

  ctkCmdLineModuleTimeoutException(const QUrl& location, const QString& errorString,
                                   const ctkCmdLineModuleRunException& cause);

  ~ctkCmdLineModuleTimeoutException() throw();

  virtual const char* name() const throw();
  virtual const char* className() const throw();
  virtual ctkCmdLineModuleTimeoutException* clone() const;
  virtual void rethrow() const;

  virtual void raise() const;

};

#endif // CTKCMDLINEMODULETIMEOUTEXCEPTION_H
