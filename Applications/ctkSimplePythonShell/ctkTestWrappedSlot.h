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

#ifndef __ctkTestWrappedSlot_h
#define __ctkTestWrappedSlot_h

// Qt includes
#include <QObject>

class ctkTestWrappedSlot : public QObject
{
  Q_OBJECT

public:

  ctkTestWrappedSlot(QObject * newParent = 0) : QObject(newParent)
    {
    this->Value = 0;
    }

public Q_SLOTS:

  int value() const { return this->Value; }
  void setValue(int newValue){ this->Value = newValue; }

private:
  int        Value;
};

#endif
