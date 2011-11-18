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

#ifndef __ctkSettingsPanelTest2Helper_h
#define __ctkSettingsPanelTest2Helper_h

#include <QObject>
#include <QStringList>

// --------------------------------------------------------------------------
class ctkSettingsPanelTest2Helper : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QStringList list READ list WRITE setList)
public:
  ctkSettingsPanelTest2Helper(QObject * parent = 0) : QObject(parent){}
  QStringList list()const;
public Q_SLOTS:
  void setList(const QStringList& alist);
Q_SIGNALS:
  void listChanged();
private:
  QStringList List;
};
    
#endif

