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
#include <QDebug>
#include <QRegularExpression>
#include <QStringList>

// CTK includes
#include "ctkUtils.h"

// STD includes
#include <cstdlib>
#include <iostream>
#include <string>


//-----------------------------------------------------------------------------
int ctkUtilsTest3(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  QString dummy;
  QStringList dummyList;
  QString defaultDummyPattern(".+");

  if (!ctk::extensionToRegExp(dummy).isEmpty())
  {
    qWarning() << "Line" << __LINE__ << "ctk::extensionToRegExp() failed: ";
    return EXIT_FAILURE;
  }
  QRegularExpression dummyRegExp = ctk::nameFiltersToRegularExpression(dummyList);
  if(dummyRegExp.pattern() != defaultDummyPattern )
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed: ";
    return EXIT_FAILURE;
  }
  //add test if it take all the extension, test with examples
  if (!dummyRegExp.match("c:/foo.jpg").hasMatch())
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed: ";
    return EXIT_FAILURE;
  }
  if (!dummyRegExp.match("c:/foo.jpga").hasMatch())
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed: ";
    return EXIT_FAILURE;
  }
  if (!dummyRegExp.match("c:/foo.png").hasMatch())
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed: ";
    return EXIT_FAILURE;
  }

  //-------Test Function extensionToRegExp(const QString& extension)
  QString simpleExtension("*.jpg");
  QString standardSimpleExtension(".*\\.jpg?$");

  if (ctk::extensionToRegExp(simpleExtension).isEmpty())
  {
    qWarning() << "Line" << __LINE__ << "ctk::extensionToRegExp() failed: input "
               << simpleExtension << "output:"
               << ctk::extensionToRegExp(simpleExtension);
    return EXIT_FAILURE;
  }
  if (ctk::extensionToRegExp(simpleExtension) != standardSimpleExtension)
  {
    qWarning() << "Line" << __LINE__<< "ctk::extensionToRegExp() failed: input "
               << standardSimpleExtension << "output:"
               << ctk::extensionToRegExp(simpleExtension);
    return EXIT_FAILURE;
  }

  QString standardNameFilter("Images (*.jpg)");
  QString simpleStandardNameFilter("Text (*.txt)");
  QStringList standardNameFilters;
  standardNameFilters << standardNameFilter << simpleStandardNameFilter;

  QString nameFiltersExtensions("(.*\\.jpg?$|.*\\.txt?$)");

  QRegularExpression regExp = ctk::nameFiltersToRegularExpression(standardNameFilters);
  if(regExp.pattern().isEmpty())
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed: input "
               << nameFiltersExtensions << "output:"
               << regExp.pattern();
    return EXIT_FAILURE;
  }

  if (regExp.pattern() != nameFiltersExtensions)
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed: input "
               << nameFiltersExtensions << "output:"
               << regExp.pattern();
    return EXIT_FAILURE;
  }
  if (!regExp.match("c:/foo.jpg").hasMatch())
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed:";
    return EXIT_FAILURE;
  }
  if (!regExp.match("c:/foo.txt").hasMatch())
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed:";
    return EXIT_FAILURE;
  }
  if (regExp.match("c:/foo.txta").hasMatch())
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed:";
    return EXIT_FAILURE;
  }
  if (regExp.match("c:/foo.jpga").hasMatch())
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed:";
    return EXIT_FAILURE;
  }
  if (regExp.match("c:/foo.png").hasMatch())
  {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegularExpression() failed:";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
