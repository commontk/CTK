/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// Qt includes
#include <QSignalSpy>

// CTK includes
#include "ctkAddRemoveComboBox.h"
#include "ctkTestApplication.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
QCTK_DECLARE_TEST(ctkAddRemoveComboBoxTest1)
{
  ctkAddRemoveComboBox ctkObject;
  
  int currentCount = ctkObject.count();
  if (currentCount != 0)
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in count() - Expected: 0, current:" << currentCount << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  // Add items
  ctkObject.addItem("Item1");
  ctkObject.addItem("Item2");
  ctkObject.addItem("Item3");

  currentCount = ctkObject.count();
  if (currentCount != 3)
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in count() - Expected: 3, current:" << currentCount << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  if (ctkObject.itemText(0) != "Item1" ||
      ctkObject.itemText(1) != "Item2" ||
      ctkObject.itemText(2) != "Item3")
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in itemText()" << std::endl
              << " Expected items [Item1, Item2, Item3]" << std::endl
              << " Current items [" << qPrintable(ctkObject.itemText(0)) << ", "
                                    << qPrintable(ctkObject.itemText(1)) << ", "
                                    << qPrintable(ctkObject.itemText(2)) << "]" << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  ctkObject.removeItem(1);

  currentCount = ctkObject.count();
  if (currentCount != 2)
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in count() - Expected: 2, current:" << currentCount << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  if (ctkObject.itemText(0) != "Item1" ||
      ctkObject.itemText(1) != "Item3")
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in itemText()" << std::endl
              << " Expected items [Item1, Item3]" << std::endl
              << " Current items [" << qPrintable(ctkObject.itemText(0)) << ", "
                                    << qPrintable(ctkObject.itemText(1)) << "]" << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  ctkObject.addItem("Item4");

  currentCount = ctkObject.count();
  if (currentCount != 3)
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in count() - Expected: 3, current:" << currentCount << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  if (ctkObject.itemText(0) != "Item1" ||
      ctkObject.itemText(1) != "Item3" ||
      ctkObject.itemText(2) != "Item4")
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in itemText()" << std::endl
              << " Expected items [Item1, Item3, Item4]" << std::endl
              << " Current items [" << qPrintable(ctkObject.itemText(0)) << ", "
                                    << qPrintable(ctkObject.itemText(1)) << ", "
                                    << qPrintable(ctkObject.itemText(2)) << "]" << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  ctkObject.clear();

  currentCount = ctkObject.count();
  if (currentCount != 0)
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in count() - Expected: 0, current:" << currentCount << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  ctkObject.addItem("Item5");

  currentCount = ctkObject.count();
  if (currentCount != 1)
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in count() - Expected: 1, current:" << currentCount << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  if (ctkObject.itemText(0) != "Item5")
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in itemText()" << std::endl
              << " Expected items [Item5]" << std::endl
              << " Current items [" << qPrintable(ctkObject.itemText(0)) << "]" << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  ctkObject.clear();

  currentCount = ctkObject.count();
  if (currentCount != 0)
    {
    ctkObject.printAdditionalInfo();
    std::cerr << __LINE__ << " - Error in count() - Expected: 0, current:" << currentCount << std::endl;
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }
    
  QCTK_EXIT_TEST(EXIT_SUCCESS);
}

QCTK_RUN_TEST(ctkAddRemoveComboBoxTest1);
