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

// CTK includes
#include "ctkPushButton.h"

//-----------------------------------------------------------------------------
class ctkPushButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkPushButton);
protected:
  ctkPushButton* const q_ptr;
public:

  virtual ~ctkPushButtonPrivate();

  ctkPushButtonPrivate(ctkPushButton& object);
  void init();

  virtual QRect iconRect() const;
  virtual QSize buttonSizeHint()const;
  virtual QStyleOptionButton drawIcon(QPainter* p);

  // Tuning of the button look&feel
  Qt::Alignment ButtonTextAlignment;
  Qt::Alignment IconAlignment;
  int IconSpacing;
};
