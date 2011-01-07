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

#include <ctkServiceReference.h>
#include <QTextStream>

class ctkLogService;

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

class CTK_PLUGINFW_EXPORT ctkLogStreamWithServiceRef : public ctkLogStream
{
public:

  ctkLogStreamWithServiceRef(ctkLogService* logService, const ctkServiceReference& sr,
                             int level, const std::exception* exc = 0,
                             const char* file = 0, const char* function = 0, int line = -1);
  ctkLogStreamWithServiceRef(const ctkLogStreamWithServiceRef& logStreamWithRef);

  ~ctkLogStreamWithServiceRef();

protected:

  ctkServiceReference sr;
};

class CTK_PLUGINFW_EXPORT ctkNullLogStream : public ctkLogStream
{
public:

  ctkNullLogStream();
  ~ctkNullLogStream();

};


#define CTK_DEBUG(logService) ((logService && logService->getLogLevel() >= ctkLogService::LOG_DEBUG) ? ctkLogStream(logService, ctkLogService::LOG_DEBUG, 0, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream())
#define CTK_DEBUG_EXC(logService, exc) (logService && logService->getLogLevel() >= ctkLogService::LOG_DEBUG) ? ctkLogStream(logService, ctkLogService::LOG_DEBUG, exc, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_DEBUG_SR(logService, serviceRef) (logService && logService->getLogLevel() >= ctkLogService::LOG_DEBUG) ? ctkLogStreamWithServiceRef(logService, serviceRef, ctkLogService::LOG_DEBUG, 0, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_DEBUG_SR_EXC(logService, serviceRef, exc) (logService && logService->getLogLevel() >= ctkLogService::LOG_DEBUG) ? ctkLogStreamWithServiceRef(logService, serviceRef, ctkLogService::LOG_DEBUG, exc, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()

#define CTK_INFO(logService) (logService && logService->getLogLevel() >= ctkLogService::LOG_INFO) ? ctkLogStream(logService, ctkLogService::LOG_INFO, 0, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_INFO_EXC(logService, exc) (logService && logService->getLogLevel() >= ctkLogService::LOG_INFO) ? ctkLogStream(logService, ctkLogService::LOG_INFO, exc, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_INFO_SR(logService, serviceRef) (logService && logService->getLogLevel() >= ctkLogService::LOG_INFO) ? ctkLogStreamWithServiceRef(logService, serviceRef, ctkLogService::LOG_INFO, 0, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_INFO_SR_EXC(logService, serviceRef, exc) (logService && logService->getLogLevel() >= ctkLogService::LOG_INFO) ? ctkLogStreamWithServiceRef(logService, serviceRef, ctkLogService::LOG_INFO, exc, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()

#define CTK_WARN(logService) (logService && logService->getLogLevel() >= ctkLogService::LOG_WARNING) ? ctkLogStream(logService, ctkLogService::LOG_WARNING, 0, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_WARN_EXC(logService, exc) (logService && logService->getLogLevel() >= ctkLogService::LOG_WARNING) ? ctkLogStream(logService, ctkLogService::LOG_WARNING, exc, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_WARN_SR(logService, serviceRef) (logService && logService->getLogLevel() >= ctkLogService::LOG_WARNING) ? ctkLogStreamWithServiceRef(logService, serviceRef, ctkLogService::LOG_WARNING, 0, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_WARN_SR_EXC(logService, serviceRef, exc) (logService && logService->getLogLevel() >= ctkLogService::LOG_WARNING) ? ctkLogStreamWithServiceRef(logService, serviceRef, ctkLogService::LOG_WARNING, exc, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()

#define CTK_ERROR(logService) (logService && logService->getLogLevel() >= ctkLogService::LOG_ERROR) ? ctkLogStream(logService, ctkLogService::LOG_ERROR, 0, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_ERROR_EXC(logService, exc) (logService && logService->getLogLevel() >= ctkLogService::LOG_ERROR) ? ctkLogStream(logService, ctkLogService::LOG_ERROR, exc, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_ERROR_SR(logService, serviceRef) (logService && logService->getLogLevel() >= ctkLogService::LOG_ERRO) ? ctkLogStreamWithServiceRef(logService, serviceRef, ctkLogService::LOG_ERROR, 0, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()
#define CTK_ERROR_SR_EXC(logService, serviceRef, exc) (logService && logService->getLogLevel() >= ctkLogService::LOG_ERROR) ? ctkLogStreamWithServiceRef(logService, serviceRef, ctkLogService::LOG_ERROR, exc, __FILE__, __FUNCTION__, __LINE__) : ctkNullLogStream()


#endif // CTKLOGSTREAM_H
