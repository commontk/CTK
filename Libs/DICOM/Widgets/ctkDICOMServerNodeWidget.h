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

#ifndef __ctkDICOMServerNodeWidget_h
#define __ctkDICOMServerNodeWidget_h

// Qt includes 
#include <QWidget>
#include <QString>
#include <QList>
#include <QMap>

#include "ctkDICOMWidgetsExport.h"

class QTableWidgetItem;
class ctkDICOMServerNodeWidgetPrivate;

/// \ingroup DICOM_Widgets
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMServerNodeWidget : public QWidget
{
Q_OBJECT;
public:
  typedef QWidget Superclass;
  explicit ctkDICOMServerNodeWidget(QWidget* parent=0);
  virtual ~ctkDICOMServerNodeWidget();

  /// "FINDSCU" by default
  QString                callingAETitle()const;
  /// "CTKSTORE" by default
  QString                storageAETitle()const;
  /// 11112 by default
  int                    storagePort()const;
  /// Utility function that returns the callingAETitle, storageAETitle and
  /// storagePort in a map
  QMap<QString,QVariant> parameters()const;

  /// Return the list of server names
  QStringList            serverNodes()const;
  /// Return the list of selected(checked) server names 
  QStringList            selectedServerNodes()const;
  /// Return all the information associated to a server defined by its name
  QMap<QString,QVariant> serverNodeParameters(const QString &serverNode)const;
  QMap<QString,QVariant> serverNodeParameters(int row)const;
  
  /// Add a server node with the given parameters
  /// Return the row index added into the table
  int addServerNode(const QMap<QString, QVariant>& parameters);

public Q_SLOTS:
  /// Add an empty server node and make it current
  /// Return the row index added into the table
  int addServerNode();
  /// Remove the current row (different from the checked rows)
  void removeCurrentServerNode();

  void readSettings();
  void saveSettings();

protected Q_SLOTS:
  void updateRemoveButtonEnableState();

protected:
  QScopedPointer<ctkDICOMServerNodeWidgetPrivate> d_ptr;
  enum ServerColumns{
    NameColumn = 0,
    AETitleColumn,
    AddressColumn,
    PortColumn,
    CGETColumn
  };
private:
  Q_DECLARE_PRIVATE(ctkDICOMServerNodeWidget);
  Q_DISABLE_COPY(ctkDICOMServerNodeWidget);
};

#endif
