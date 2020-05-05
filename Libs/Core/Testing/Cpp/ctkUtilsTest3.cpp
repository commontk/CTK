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
  QRegExp defaultDummyRegExp(".+");

  if (!ctk::extensionToRegExp(dummy).isEmpty())
    {
    qWarning() << "Line" << __LINE__ << "ctk::extensionToRegExp() failed: ";
    return EXIT_FAILURE;
    }
  QRegExp dummyRegExp = ctk::nameFiltersToRegExp(dummyList);
  if(dummyRegExp != defaultDummyRegExp )
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed: ";
    return EXIT_FAILURE;
    }
  //add test if it take all the extension, test with exemples
  if (!dummyRegExp.exactMatch("c:/foo.jpg"))
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed: ";
    return EXIT_FAILURE;
    }
  if (!dummyRegExp.exactMatch("c:/foo.jpga"))
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed: ";
    return EXIT_FAILURE;
    }
  if (!dummyRegExp.exactMatch("c:/foo.png"))
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed: ";
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
  QRegExp defaultRegExp(nameFiltersExtensions);

  if(ctk::nameFiltersToRegExp(standardNameFilters).isEmpty())
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed: input "
               << nameFiltersExtensions << "output:"
               << ctk::nameFiltersToRegExp(standardNameFilters).pattern();
    return EXIT_FAILURE;
    }

  QRegExp regExp = ctk::nameFiltersToRegExp(standardNameFilters);
  if (regExp != defaultRegExp)
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed: input "
               << nameFiltersExtensions << "output:"
               << regExp.pattern();
    return EXIT_FAILURE;
    }
  if (!regExp.exactMatch("c:/foo.jpg"))
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed:";
    return EXIT_FAILURE;
    }
  if (!regExp.exactMatch("c:/foo.txt"))
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed:";
    return EXIT_FAILURE;
    }
  if (regExp.exactMatch("c:/foo.txta"))
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed:";
    return EXIT_FAILURE;
    }
  if (regExp.exactMatch("c:/foo.jpga"))
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed:";
    return EXIT_FAILURE;
    }
  if (regExp.exactMatch("c:/foo.png"))
    {
    qWarning() << "Line" << __LINE__ << "ctk::nameFiltersToRegExp() failed:";
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
