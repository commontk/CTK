/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

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
#include <vector>

//-----------------------------------------------------------------------------
int ctkUtilsTest2(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);
  QString invalidNameFilter("Type {*.jpg} ");
  QString invalidNameFilter2("Type (*.jpg))");
  QString invalidNameFilter3("Type ()");
  QString simpleNameFilter("*.jpg");
  QString standardNameFilter("Images (*.jpg *.png)");
  QStringList standardNameFilterExtensions;
  standardNameFilterExtensions << "*.jpg" << "*.png";
  QString simpleStandardNameFilter("Text (*.txt)");
  QStringList simpleStandardNameFilterExtensions("*.txt");
  QStringList standardNameFilters;
  standardNameFilters << standardNameFilter << simpleStandardNameFilter;
  QStringList standardNameFiltersExtensions;
  standardNameFiltersExtensions << "*.jpg" << "*.png" << "*.txt";
  
  if (ctk::nameFilterToExtensions(invalidNameFilter).count())
    {
    qWarning() << "ctk::nameFilterToExtensions() failed: input "
               << invalidNameFilter << "output:"
               << ctk::nameFilterToExtensions(invalidNameFilter);
    return EXIT_FAILURE;
    }

  if (ctk::nameFilterToExtensions(invalidNameFilter2).count())
    {
    qWarning() << "ctk::nameFilterToExtensions() failed: input "
               << invalidNameFilter2 << "output:"
               << ctk::nameFilterToExtensions(invalidNameFilter2);
    return EXIT_FAILURE;
    }

  if (ctk::nameFilterToExtensions(invalidNameFilter3).count())
    {
    qWarning() << "ctk::nameFilterToExtensions() failed: input "
               << invalidNameFilter3 << "output:"
               << ctk::nameFilterToExtensions(invalidNameFilter3);
    return EXIT_FAILURE;
    }

  if (ctk::nameFilterToExtensions(simpleNameFilter) != QStringList(simpleNameFilter))
    {
    qWarning() << "ctk::nameFilterToExtensions() failed: input "
               << simpleNameFilter << "output:"
               << ctk::nameFilterToExtensions(simpleNameFilter);
    return EXIT_FAILURE;
    }

  if (ctk::nameFilterToExtensions(standardNameFilter) != standardNameFilterExtensions)
    {
    qWarning() << "ctk::nameFilterToExtensions() failed: input "
               << standardNameFilter << "output:"
               << ctk::nameFilterToExtensions(standardNameFilter);
    return EXIT_FAILURE;
    }

  if (ctk::nameFilterToExtensions(simpleStandardNameFilter) != simpleStandardNameFilterExtensions)
    {
    qWarning() << "ctk::nameFilterToExtensions() failed: input "
               << simpleStandardNameFilter << "output:"
               << ctk::nameFilterToExtensions(simpleStandardNameFilter);
    return EXIT_FAILURE;
    }

  if (ctk::nameFiltersToExtensions(standardNameFilters) != standardNameFiltersExtensions)
    {
    qWarning() << "ctk::nameFilterToExtensions() failed: input "
               << standardNameFilters << "output:"
               << ctk::nameFiltersToExtensions(standardNameFilters);
    return EXIT_FAILURE;
    }
  //QString extensionToRegExp(const QString& extension);
  //QRegExp nameFiltersToRegExp(const QStringList& nameFilters);
  return EXIT_SUCCESS;
}
