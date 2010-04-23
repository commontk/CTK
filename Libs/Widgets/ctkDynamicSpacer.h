/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
