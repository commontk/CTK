/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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
#include <QCoreApplication>

// CTK includes
#include <ctkLogger.h>

// STL includes
#include <cstdlib>
#include <iostream>

int ctkLoggerTest1(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  //--------------------------------------------------------------------
  ctkLogger logger("LoggerTest");
  std::cout<< "Logger default : " << logger.isOffEnabled() << std::endl;
  logger.setOff();
  if (!logger.isOffEnabled())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem vith Logger::setOff()" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout<< "Logger after setOff() : " << logger.isOffEnabled() << std::endl;

  //------------------------------------------------------------------
  std::cout<< "Logger Debug default : " << logger.isDebugEnabled() << std::endl;
  logger.setDebug();
  if (!logger.isDebugEnabled())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem vith Logger::setDebug()" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Logger Debug after setDebug() : "
            << logger.isDebugEnabled() << std::endl;

  //------------------------------------------------------------------
  logger.setInfo();
  if (!logger.isInfoEnabled())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem vith Logger::setDebug()" << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------
  logger.setTrace();
  if (!logger.isTraceEnabled())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem vith Logger::setTrace()" << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------
  logger.setError();
  if (!logger.isErrorEnabled())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem vith Logger::setError()" << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------
  logger.setWarn();
  if (!logger.isWarnEnabled())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem vith Logger::setWarn()" << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------
  logger.setFatal();
  if (!logger.isFatalEnabled())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem vith Logger::setFatal()" << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------
  logger.configure();
  QString defaultString;  
  logger.debug(defaultString);
  logger.info(defaultString);
  logger.trace(defaultString);
  logger.warn(defaultString);
  logger.warn(defaultString);
  logger.error(defaultString);
  logger.fatal(defaultString);

  return EXIT_SUCCESS;
}

