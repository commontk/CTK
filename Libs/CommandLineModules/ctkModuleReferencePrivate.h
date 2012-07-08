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

#ifndef CTKMODULEREFERENCEPRIVATE_H
#define CTKMODULEREFERENCEPRIVATE_H

#include <QByteArray>
#include <QObject>

class ctkModuleReferencePrivate : public QObject
{
  Q_OBJECT

public:

  ctkModuleReferencePrivate();

  ~ctkModuleReferencePrivate();

  void setGUI(QObject* gui);

  QByteArray xml;
  QString loc;
  QObject* objectRepresentation;

  QAtomicInt ref;

private:

  friend class ctkModuleReference;

  QObject* gui;

private Q_SLOTS:

  void guiDestroyed();
};

#endif // CTKMODULEREFERENCEPRIVATE_H
