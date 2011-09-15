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
#include <QCoreApplication>
#include <QDebug>
#include <QFocusEvent>
#include <QScopedPointer>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>

// CTK includes
#include "ctkCheckableModelHelper.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckableModelHelperTest1(int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);

  QStandardItemModel model;
  QList<QStandardItem*> row0;
  row0 << new QStandardItem << new QStandardItem << new QStandardItem;
  row0[0]->setText("not user checkable");
  model.appendRow(row0);
  QList<QStandardItem*> row1;
  row1 << new QStandardItem << new QStandardItem << new QStandardItem;
  row1[0]->setCheckable(true);
  row1[0]->setText("checkable");
  model.appendRow(row1);
  QList<QStandardItem*> row2;
  row2 << new QStandardItem << new QStandardItem << new QStandardItem;
  row2[0]->setCheckable(true);
  row2[0]->setText("checkable");
  model.appendRow(row2);

  // items are unchecked by default 
  if (row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "QStandardItem default failed: "
              << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  // CheckForce & Default model
  QStandardItemModel modelForce;
  modelForce.appendRow(row0);

  {
    QModelIndex modelIndex;
    QScopedPointer<ctkCheckableModelHelper> modelHelperCF(new ctkCheckableModelHelper(Qt::Horizontal));

    modelHelperCF->setForceCheckability(true);
    if (!modelHelperCF->forceCheckability())
      {
      std::cerr << "Line " << __LINE__
                << " - ctkCheckableModelHelper::setForceCheckability() failed: "
                << static_cast<int>(modelHelperCF->forceCheckability()) << std::endl;
      return EXIT_FAILURE;
      }

    modelHelperCF->isCheckable(modelIndex);
    modelHelperCF->toggleCheckState(modelIndex);

    modelHelperCF->setForceCheckability(false);
    if (modelHelperCF->forceCheckability())
      {
      std::cerr << "Line " << __LINE__
                << " - ctkCheckableModelHelper::setForceCheckability() failed: "
                << static_cast<int>(modelHelperCF->forceCheckability()) << std::endl;
      return EXIT_FAILURE;
      }

    modelHelperCF->setCheckState(modelIndex, Qt::Unchecked);

    modelHelperCF->setDefaultCheckState(Qt::Checked);
    if (modelHelperCF->defaultCheckState() != Qt::Checked)
      {
      std::cerr << "Line " << __LINE__
                << " - ctkCheckableModelHelper::setDefaultCheckState() failed: "
                << static_cast<int>(modelHelperCF->defaultCheckState()) << std::endl;
      return EXIT_FAILURE;
      }

    modelHelperCF->setRootIndex(modelIndex);
    modelHelperCF->setForceCheckability(true);
    modelHelperCF->setCheckState(modelIndex, Qt::Checked);
    Qt::CheckState statutCheck = Qt::Checked;
    if (modelHelperCF->checkState(modelIndex, statutCheck))
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::setCheckState() failed: "
                << static_cast<int>(modelHelperCF->checkState(modelIndex, statutCheck))
                << std::endl;
      return EXIT_FAILURE;
      }

    modelHelperCF->checkState(modelIndex);

    modelHelperCF->setModel(&modelForce);
    if (!modelHelperCF->model())
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::setModel() failed: "
                << "is null" << std::endl;
      return EXIT_FAILURE;
      }

    modelHelperCF->setCheckState(modelIndex, Qt::Checked);
    if (modelHelperCF->checkState(modelIndex, statutCheck))
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::setCheckState() failed: "
                << static_cast<int>(modelHelperCF->checkState(modelIndex, statutCheck))
                << std::endl;
      return EXIT_FAILURE;
      }
  } // end of local scope

  {
    // Row & Column dummy insert
    QList<QStandardItem*> col0;
    col0 << new QStandardItem << new QStandardItem << new QStandardItem;
    col0[0]->setText("not user checkable");
    modelForce.appendColumn(col0);
    modelForce.appendRow(row1);

    QScopedPointer<ctkCheckableModelHelper> modelHelperRC(new ctkCheckableModelHelper(Qt::Vertical));
    modelHelperRC->setForceCheckability(true);
    modelHelperRC->setModel(&modelForce);
    modelForce.appendColumn(col0);
    modelForce.appendRow(row1);

  } // end of local scope

  {
    // Header is checked by default
    model.setHeaderData(0, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole);

    QScopedPointer<ctkCheckableModelHelper> modelHelper(new ctkCheckableModelHelper(Qt::Horizontal));
    modelHelper->setModel(&model);

    // propagatetoitems is true by default
    //modelHelper->setPropagateToItems(true);
    modelHelper->toggleHeaderCheckState(-1);

    // As propagateToItems is true, once the model is set to the modelHelper,
    // the checkable header is updated from the check state of all the items
    // all the items are unchecked by default, so the header becomes unchecked
    if (modelHelper->headerCheckState(0) != Qt::Unchecked ||
        row0[0]->checkState() != Qt::Unchecked ||
        row1[0]->checkState() != Qt::Unchecked ||
        row2[0]->checkState() != Qt::Unchecked)
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::checkstate() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0)) << " "
                << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }
    // Retrieve checkstate of the header
    Qt::CheckState checkstate;
    if (!modelHelper->headerCheckState(0, checkstate))
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::checkstate() failed: "
                << static_cast<int>(checkstate) << std::endl;
      return EXIT_FAILURE;
      }

    if (modelHelper->propagateDepth() == 0)
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::propagateDepth() failed: "
                << modelHelper->propagateDepth() << std::endl;
      return EXIT_FAILURE;
      }
    modelHelper->setPropagateDepth(0);
    if (modelHelper->propagateDepth() != 0)
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::propagateDepth() failed: "
                << modelHelper->propagateDepth() << std::endl;
      return EXIT_FAILURE;
      }
    if (modelHelper->headerCheckState(0) != Qt::Unchecked ||
        row0[0]->checkState() != Qt::Unchecked ||
        row1[0]->checkState() != Qt::Unchecked ||
        row2[0]->checkState() != Qt::Unchecked)
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::propagateToItems() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0)) << " "
                << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }

    // check the header
    modelHelper->setHeaderCheckState(0, Qt::Checked);

    // make sure it didn't uncheck the checkable items
    if (modelHelper->headerCheckState(0) != Qt::Checked ||
        row0[0]->checkState() != Qt::Unchecked ||
        row1[0]->checkState() != Qt::Unchecked ||
        row2[0]->checkState() != Qt::Unchecked)
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::toggleCheckState() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0))
                << " "        << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }

    row0[0]->setCheckState(Qt::Checked);
    // make sure it didn't uncheck the checkable items
    if (modelHelper->headerCheckState(0) != Qt::Checked ||
        row0[0]->checkState() != Qt::Checked ||
        row1[0]->checkState() != Qt::Unchecked ||
        row2[0]->checkState() != Qt::Unchecked)
      {
      std::cerr << "Line " << __LINE__ << " - QStandardItem::setCheckState() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0)) << " "
                << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }

    // The checkable header gets updated with the item check states
    modelHelper->setPropagateDepth(-1);

    if (modelHelper->propagateDepth() == 0 ||
        modelHelper->headerCheckState(0) != Qt::PartiallyChecked ||
        row0[0]->checkState() != Qt::Checked ||
        row1[0]->checkState() != Qt::Unchecked ||
        row2[0]->checkState() != Qt::Unchecked)
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::setPropagateToItems() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0)) << " "
                << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }

    row0[0]->setCheckState(Qt::Unchecked);

    if (modelHelper->headerCheckState(0) != Qt::Unchecked ||
        row0[0]->checkState() != Qt::Unchecked ||
        row1[0]->checkState() != Qt::Unchecked ||
        row2[0]->checkState() != Qt::Unchecked)
      {
      std::cerr << "Line " << __LINE__ << " - QStandardItem::setCheckState() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0)) << " "
                << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }

    row1[0]->setCheckState(Qt::Checked);

      // make sure it didn't uncheck the checkable items
    if (modelHelper->headerCheckState(0) != Qt::PartiallyChecked ||
        row0[0]->checkState() != Qt::Unchecked ||
        row1[0]->checkState() != Qt::Checked ||
        row2[0]->checkState() != Qt::Unchecked)
      {
      std::cerr << "Line " << __LINE__ << " - QStandardItem::setCheckState() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0)) << " "
                << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }

    row1[0]->setCheckState(Qt::Checked);

    // make sure it didn't check the checkable items
    if (modelHelper->headerCheckState(0) != Qt::PartiallyChecked ||
        row0[0]->checkState() != Qt::Unchecked ||
        row1[0]->checkState() != Qt::Checked ||
        row2[0]->checkState() != Qt::Unchecked)
      {
      std::cerr << "Line " << __LINE__ << " - QStandardItem::setCheckState() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0)) << " "
                << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }

    row0[0]->setCheckState(Qt::Checked);
    row2[0]->setCheckState(Qt::Checked);

    // make sure the header is now checked
    if (modelHelper->headerCheckState(0) != Qt::Checked ||
        row0[0]->checkState() != Qt::Checked ||
        row1[0]->checkState() != Qt::Checked ||
        row2[0]->checkState() != Qt::Checked)
      {
      std::cerr << "Line " << __LINE__ << " - QStandardItem::setCheckState() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0)) << " "
                << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }

    modelHelper->setHeaderCheckState(0, Qt::Unchecked);

    if (modelHelper->headerCheckState(0) != Qt::Unchecked ||
        row0[0]->checkState() != Qt::Unchecked ||
        row1[0]->checkState() != Qt::Unchecked ||
        row2[0]->checkState() != Qt::Unchecked)
      {
      std::cerr << "Line " << __LINE__ << " - ctkCheckableModelHelper::setCheckState() failed: "
                << static_cast<int>(modelHelper->headerCheckState(0)) << " "
                << static_cast<int>(row0[0]->checkState()) << " "
                << static_cast<int>(row1[0]->checkState()) << " "
                << static_cast<int>(row2[0]->checkState()) << std::endl;
      return EXIT_FAILURE;
      }
  } // end of local scope
  return EXIT_SUCCESS;
}
