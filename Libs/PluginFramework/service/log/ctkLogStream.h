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


#ifndef CTKLOGSTREAM_H
#define CTKLOGSTREAM_H

#include <ctkPluginFrameworkExport.h>

#include <QString>
#include <QTextStream>

struct ctkLogService;

/**
 * \ingroup LogService
 */
class CTK_PLUGINFW_EXPORT ctkLogStream
{
public:

  ctkLogStream(ctkLogService* logService, int level, const std::exception* exc = 0,
               const char* file = 0, const char* function = 0, int line = -1);
  ctkLogStream(const ctkLogStream& logStream);

  virtual ~ctkLogStream();

  template<class T>
  ctkLogStream& operator <<(const T& t)
  {
    ts << t;
    return *this;
  }

  ctkLogStream& operator <<(const char* c)
  {
    ts << c;
    return *this;
  }

  ctkLogStream& operator <<(bool b)
  {
    ts << (b ? "true" : "false");
    return *this;
  }

protected:

  QString msg;
  QTextStream ts;
  bool logged;

  ctkLogService* logService;
  int level;
  const std::exception* exc;

  const char* file;
  const char* function;
  const int line;
};

#endif // CTKLOGSTREAM_H
