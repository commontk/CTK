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

#ifndef __ctkDynamicSpacer_h
#define __ctkDynamicSpacer_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>
#include "ctkWidgetsExport.h"

class ctkDynamicSpacerPrivate;

/// \ingroup Widgets
/// Description
/// A spacer widget that has a dynamic size policy controllable via its slot
/// activate(bool). It can be usefully when you don't want a rigid layout. 
class CTK_WIDGETS_EXPORT ctkDynamicSpacer : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QSizePolicy activeSizePolicy READ activeSizePolicy WRITE setActiveSizePolicy);
  Q_PROPERTY(QSizePolicy inactiveSizePolicy READ inactiveSizePolicy WRITE setInactiveSizePolicy);
  Q_PROPERTY(bool active READ isActive WRITE setActive);
public:
  /// Constructor, builds a ctkDynamicSpacer, inactive by default
  ctkDynamicSpacer(QWidget *parent = 0);
  virtual ~ctkDynamicSpacer();

  /// The active size policy of the spacer. By default the same as QWidget
  QSizePolicy activeSizePolicy() const;
  void setActiveSizePolicy(QSizePolicy sizePolicy);
  inline void setActiveSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical);

  /// The inactive size policy of the spacer. By default the same as QWidget. 
  QSizePolicy inactiveSizePolicy() const;
  void setInactiveSizePolicy(QSizePolicy sizePolicy);
  inline void setInactiveSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical);

  /// Return the what size policy is current:
  /// false is inactive Size Policy that is current
  /// true is active SizePolicy that is current
  bool isActive()const;
  inline bool isInactive()const;

public Q_SLOTS:
  /// Change the size policy. If enable is true, activeSizePolicy is used,
  /// inactiveSizePolicy otherwise
  void setActive(bool enable);
  inline void setInactive(bool disable);

protected:
  QScopedPointer<ctkDynamicSpacerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDynamicSpacer);
  Q_DISABLE_COPY(ctkDynamicSpacer);
};

void ctkDynamicSpacer::setActiveSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical)
{
  this->setActiveSizePolicy(QSizePolicy(horizontal, vertical));
}

void ctkDynamicSpacer::setInactiveSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical)
{
  this->setInactiveSizePolicy(QSizePolicy(horizontal, vertical));
}

bool ctkDynamicSpacer::isInactive()const
{
  return !this->isActive();
}

void ctkDynamicSpacer::setInactive(bool disable)
{
  this->setActive(!disable);
}

#endif
