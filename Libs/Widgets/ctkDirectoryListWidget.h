/*=========================================================================

  Library:   CTK

  Copyright (c) University College London.

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

#ifndef __ctkDirectoryListWidget_h
#define __ctkDirectoryListWidget_h

// Qt includes
#include <QWidget>
#include <QStringList>

// QtGUI includes
#include "ctkWidgetsExport.h"

class ctkDirectoryListWidgetPrivate;

/**
 * \class ctkDirectoryListWidget
 * \brief A widget to maintain a list of directories, with add and remove buttons,
 * such as might be used in a settings panel to select a series of directories to search.
 *
 * \author m.clarkson@ucl.ac.uk
 */
class CTK_WIDGETS_EXPORT ctkDirectoryListWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QStringList directoryList READ directoryList WRITE setDirectoryList)

public:
  /// Superclass typedef
  typedef QWidget Superclass;
  ctkDirectoryListWidget(QWidget* parent = 0);
  virtual ~ctkDirectoryListWidget();

  /// Set the directory list, which will overwrite any existing list.
  void setDirectoryList(const QStringList& list);
  QStringList directoryList() const;

public Q_SLOTS:

Q_SIGNALS:
  /// directoryListChanged emmitted whenever the list of directories is changed.
  void directoryListChanged(const QStringList& directories);

protected:
  QScopedPointer<ctkDirectoryListWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDirectoryListWidget);
  Q_DISABLE_COPY(ctkDirectoryListWidget);
};

#endif
