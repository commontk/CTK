/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

#ifndef __ctkDynamicSpacer_h
#define __ctkDynamicSpacer_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>
#include "CTKWidgetsExport.h"

class ctkDynamicSpacerPrivate;

/// Description
/// A Menu widget that show/hide its children depending on its checked/collapsed properties
class CTK_WIDGETS_EXPORT ctkDynamicSpacer : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QSizePolicy activeSizePolicy READ activeSizePolicy WRITE setActiveSizePolicy);
  Q_PROPERTY(QSizePolicy inactiveSizePolicy READ inactiveSizePolicy WRITE setInactiveSizePolicy);

public:
  ctkDynamicSpacer(QWidget *parent = 0);
  virtual ~ctkDynamicSpacer();

  QSizePolicy activeSizePolicy() const;
  void setActiveSizePolicy(QSizePolicy sizePolicy);
  inline void setActiveSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical);

  QSizePolicy inactiveSizePolicy() const;
  void setInactiveSizePolicy(QSizePolicy sizePolicy);
  inline void setInactiveSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical);

public slots:
  void activate(bool enable);

private:
  CTK_DECLARE_PRIVATE(ctkDynamicSpacer);
};

void ctkDynamicSpacer::setActiveSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical)
{
  this->setActiveSizePolicy(QSizePolicy(horizontal, vertical));
}

#endif
