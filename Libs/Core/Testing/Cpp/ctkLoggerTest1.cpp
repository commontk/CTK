/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

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
#include <ctkUtils.h>

// STL includes
#include <cstdlib>
#include <iostream>

int ctkLoggerTest1(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  //--------------------------------------------------------------------
  ctkLogger logger("LoggerTest");

  logger.debug("logger.debug");
  logger.info("logger.info");
  logger.trace("logger.trace");
  logger.warn("logger.warn");
  logger.error("logger.error");
  logger.fatal("logger.fatal");

  // This should log a warning "Assertion `5 == 6` failed ..."
  CTK_SOFT_ASSERT(5 == 6);

  // This should not log anything
  CTK_SOFT_ASSERT(8 == 8);

  return EXIT_SUCCESS;
}

