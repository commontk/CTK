/*=========================================================================

  Library:   CTK

  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkComboBox_h
#define __ctkComboBox_h

// Qt includes
#include <QComboBox>

// CTK includes
#include "ctkPimpl.h"
#include "CTKWidgetsExport.h"
class ctkComboBoxPrivate;

class CTK_WIDGETS_EXPORT ctkComboBox : public QComboBox
{
  Q_OBJECT
  Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
  Q_PROPERTY(QIcon defaultIcon READ defaultIcon WRITE setDefaultIcon)
  Q_PROPERTY(bool forceDefault READ isDefaultForced WRITE forceDefault)
  Q_PROPERTY(Qt::TextElideMode elideMode READ elideMode WRITE setElideMode)

public:
  explicit ctkComboBox(QWidget* parent = 0);
  virtual ~ctkComboBox();

  /// Empty by default (same behavior as QComboBox)
  void setDefaultText(const QString&);
  QString defaultText()const;

  /// Empty by default (same behavior as QComboBox)
  void setDefaultIcon(const QIcon&);
  QIcon defaultIcon()const;

  void forceDefault(bool forceDefault);
  bool isDefaultForced()const;

  /// Qt::ElideNone by default (same behavior as QComboBox)
  void setElideMode(const Qt::TextElideMode& newMode);
  Qt::TextElideMode elideMode()const;

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;

protected:
  virtual void paintEvent(QPaintEvent*);

private:
  CTK_DECLARE_PRIVATE(ctkComboBox);
};

#endif
