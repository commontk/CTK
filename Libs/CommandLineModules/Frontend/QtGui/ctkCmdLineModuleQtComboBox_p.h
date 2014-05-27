/*=============================================================================

  Library: CTK

  Copyright (c) University College London

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#ifndef CTKCMDLINEMODULEQTCOMBOBOX_P_H
#define CTKCMDLINEMODULEQTCOMBOBOX_P_H

#include <QComboBox>


//-----------------------------------------------------------------------------
/**
 * \class ctkCmdLineModuleQtComboBox
 * \brief Private subclass of QComboBox, providing the currentEnumeration and setCurrentEnumeration methods.
 * \author m.clarkson@ucl.ac.uk
 * \ingroup CommandLineModulesFrontendQtGui
 */
class ctkCmdLineModuleQtComboBox : public QComboBox
{

  Q_OBJECT
  Q_PROPERTY(QString currentEnumeration READ currentEnumeration WRITE setCurrentEnumeration)

public:

  ctkCmdLineModuleQtComboBox(QWidget* parent = 0);

  void setCurrentEnumeration(const QString& text);

  QString currentEnumeration() const;

};


#endif // CTKCMDLINEMODULEQTCOMBOBOX_P_H
