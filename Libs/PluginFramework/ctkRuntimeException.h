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


#ifndef CTKRUNTIMEEXCEPTION_H
#define CTKRUNTIMEEXCEPTION_H

#include <stdexcept>

#include <QString>

#include "ctkPluginFrameworkExport.h"

class Q_DECL_EXPORT ctkRuntimeException : public std::runtime_error
{
public:

  ctkRuntimeException(const QString& msg, const std::exception* cause = 0);
  ctkRuntimeException(const ctkRuntimeException& o);

  ctkRuntimeException& operator=(const ctkRuntimeException& o);

  ~ctkRuntimeException() throw() {}

  QString getCause() const;
  void setCause(const QString& cause) throw(std::logic_error);

  const char* what() const throw();

private:

  QString  cause;
};

#endif // CTKRUNTIMEEXCEPTION_H
