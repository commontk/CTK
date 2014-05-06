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

#ifndef __ctkDICOMTableManager_h
#define __ctkDICOMTableManager_h

#include "ctkDICOMWidgetsExport.h"
#include "ctkDICOMDatabase.h"

// Qt includes
#include <QWidget>
#include <QSharedPointer>

class ctkDICOMTableManagerPrivate;
class ctkDICOMDatabase;

class QItemSelection;
class QModelIndex;

/// \ingroup DICOM_Widgets

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMTableManager : public QWidget
{
  Q_OBJECT

  /**
    * Property for setting the table layout either to vertical or horizontal alignment
    */
  Q_PROPERTY(Qt::Orientation tableOrientation READ tableOrientation WRITE setTableOrientation)
  /**
    * Property for setting a dynamic table layout which switches according to the current
    * window size between vertical and horizontal layout
    */
  Q_PROPERTY(bool dynamicTableLayout READ dynamicTableLayout WRITE setDynamicTableLayout)

public:
  typedef QWidget Superclass;

  explicit ctkDICOMTableManager(QWidget* parent = 0);
  ctkDICOMTableManager(ctkDICOMDatabase* db, QWidget* parent = 0);
  virtual ~ctkDICOMTableManager();

  /**
   * @brief Set the ctkDICOMDatabase
   * @param db the dicom database which should be used
   */
  void setDICOMDatabase(ctkDICOMDatabase* db);

  void setTableOrientation(const Qt::Orientation&) const;
  Qt::Orientation tableOrientation();

  /**
   * @brief Get the current selection of the dicomTableViews
   * @return a list with the uids of the selected items
   */
  QStringList currentPatientsSelection();
  QStringList currentStudiesSelection();
  QStringList currentSeriesSelection();

  void setDynamicTableLayout(bool);
  bool dynamicTableLayout() const;

  void updateTableViews();

public Q_SLOTS:

  void onPatientsQueryChanged(const QStringList&);
  void onStudiesQueryChanged(const QStringList&);
  void onPatientsSelectionChanged(const QStringList&);
  void onStudiesSelectionChanged(const QStringList&);

Q_SIGNALS:
  /// Signals for propagating selection changes of the different tables
  void patientsSelectionChanged(const QItemSelection&, const QItemSelection&);
  void studiesSelectionChanged(const QItemSelection&, const QItemSelection&);
  void seriesSelectionChanged(const QItemSelection&, const QItemSelection&);

  void patientsSelectionChanged(const QStringList &uids);
  void studiesSelectionChanged(const QStringList &uids);
  void seriesSelectionChanged(const QStringList &uids);

  void seriesDoubleClicked(const QModelIndex&);

protected:

  virtual void resizeEvent(QResizeEvent *);

  QScopedPointer<ctkDICOMTableManagerPrivate> d_ptr;

private:

  Q_DECLARE_PRIVATE(ctkDICOMTableManager)
  Q_DISABLE_COPY(ctkDICOMTableManager)
};

#endif // __ctkDICOMTableManager_h
