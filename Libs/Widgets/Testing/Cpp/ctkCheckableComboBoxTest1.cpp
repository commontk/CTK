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
#include <QApplication>
#include <QTimer>

#if (QT_VERSION < 0x50000)
#include <QCleanlooksStyle>
#endif

// CTK includes
#include <ctkCheckableComboBox.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckableComboBoxTest1(int argc, char * argv [] )
{
  // QCleanlooksStyle is the only style that doesn't show the checkboxes by
  // default. Test it with it
#if (QT_VERSION < 0x50000)
  QApplication::setStyle(new QCleanlooksStyle);
#endif
  QApplication app(argc, argv);

  ctkCheckableComboBox comboBox;
  comboBox.addItem("toto");
  comboBox.addItem("tata");
  comboBox.addItem("titi");
  comboBox.addItem(comboBox.style()->standardIcon(QStyle::SP_FileIcon),"tutu");

  if (comboBox.checkedIndexes().count() != 0 ||
      comboBox.allChecked() ||
      !comboBox.noneChecked())
    {
    std::cerr << "ctkCheckableComboBox has wrong default values\n"
              << " count:" << comboBox.checkedIndexes().count()
              << " all:" << comboBox.allChecked()
              << " none:" << comboBox.noneChecked() << std::endl;
    return EXIT_FAILURE;
    }

  int row = 0;
  QModelIndex firstIndex = comboBox.model()->index(row,0);
  comboBox.setCheckState(firstIndex, Qt::Checked);

  if (comboBox.checkState(firstIndex) != Qt::Checked)
    {
    std::cerr << "ctkCheckableComboBox::setCheckedState failed\n"
              << static_cast<int>(comboBox.checkState(firstIndex))
              << std::endl;
    return EXIT_FAILURE;
    }

  if (comboBox.checkedIndexes().count() != 1 ||
      comboBox.allChecked() ||
      comboBox.noneChecked())
    {
    std::cerr << "ctkCheckableComboBox::setCheckedState(first) failed\n"
              << " count:" << comboBox.checkedIndexes().count()
              << " all:" << comboBox.allChecked()
              << " none:" << comboBox.noneChecked() << std::endl;
    return EXIT_FAILURE;
    }

  // Check all the items
  QModelIndex nextIndex = firstIndex;
  while( (nextIndex = nextIndex.sibling(++row, 0)).isValid())
    {
    comboBox.setCheckState(nextIndex, Qt::Checked);
    }

  if (comboBox.checkedIndexes().count() != 4 ||
      !comboBox.allChecked() ||
      comboBox.noneChecked())
    {
    std::cerr << "ctkCheckableComboBox::setCheckedState(all) failed\n"
              << " count:" << comboBox.checkedIndexes().count()
              << " all:" << comboBox.allChecked()
              << " none:" << comboBox.noneChecked() << std::endl;
    return EXIT_FAILURE;
    }

  comboBox.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
