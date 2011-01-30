/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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


#ifndef CTKMTICON_P_H
#define CTKMTICON_P_H

#include <QString>
#include <QSharedPointer>

class ctkPlugin;

/**
 * Represents an Icon with a name and a size
 */
class ctkMTIcon
{

private:

  QString _fileName;
  int _size;
  QSharedPointer<ctkPlugin> _plugin;

public:

  ctkMTIcon();

  /**
   * Constructor of class ctkMTIcon.
   */
  ctkMTIcon(const QString& fileName, int size, const QSharedPointer<ctkPlugin>& plugin);

  /**
   * Constructor of class ctkMTIcon.
   */
  ctkMTIcon(const QString& fileName, const QSharedPointer<ctkPlugin>& plugin);

  /**
   * Method to get the icon's file name.
   */
  QString getIconName() const;

  /**
   * returns the size specified when the icon was created
   *
   * @return size or -1 if no size was specified
   */
  int getIconSize() const;

  /**
   * Method to get the plugin having this Icon.
   */
  QSharedPointer<ctkPlugin> getIconPlugin() const;

  operator bool () const;
};

#endif // CTKMTICON_P_H
