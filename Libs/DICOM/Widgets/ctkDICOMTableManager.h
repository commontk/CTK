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

#ifndef CTKDICOMTABLEMANAGER_H
#define CTKDICOMTABLEMANAGER_H

#include "ctkDICOMWidgetsExport.h"
#include "ctkDICOMDatabase.h"

// Qt includes
#include <QWidget>
#include <QBoxLayout>
#include <QSharedPointer>

class ctkDICOMTableManagerPrivate;
class ctkDICOMDatabase;

class QItemSelection;

/// \ingroup DICOM_Widgets

class ctkDICOMTableManager : public QWidget
{
  Q_OBJECT

public:
  typedef QWidget Superclass;

  explicit ctkDICOMTableManager(QWidget* parent = 0);
  ctkDICOMTableManager(QSharedPointer<ctkDICOMDatabase> db, QWidget* parent = 0);
  virtual ~ctkDICOMTableManager();

  void setCTKDICOMDatabase(QSharedPointer<ctkDICOMDatabase> db);

Q_SIGNALS:
  void signalPatientsSelectionChanged(const QItemSelection&, const QItemSelection&);

protected:
  QScopedPointer<ctkDICOMTableManagerPrivate> d_ptr;

private Q_SLOTS:
  void onChangeLayoutPushed();

private:
  Q_DECLARE_PRIVATE(ctkDICOMTableManager)
  Q_DISABLE_COPY(ctkDICOMTableManager)
};

#endif // CTKDICOMTABLEMANAGER_H
