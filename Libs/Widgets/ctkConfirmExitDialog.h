/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

#ifndef __ctkConfirmExitDialog_h
#define __ctkConfirmExitDialog_h

// Qt includes
#include <QDialog>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkConfirmExitDialogPrivate;

///
class CTK_WIDGETS_EXPORT ctkConfirmExitDialog : public QDialog
{
  Q_OBJECT
public:
  typedef QDialog Superclass;
  ctkConfirmExitDialog(QWidget* newParent = 0);
  virtual ~ctkConfirmExitDialog();

  void setPixmap(const QPixmap& pixmap);
  void setText(const QString& text);
  
  void setDontShowAnymoreSettingsKey(const QString& key);
  QString dontShowAnymoreSettingsKey()const;
  
  bool dontShowAnymore()const;

  virtual void setVisible(bool visible);
public slots:
  virtual void accept();
  
  void setDontShowAnymore(bool dontShow);

protected:
  QScopedPointer<ctkConfirmExitDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkConfirmExitDialog);
  Q_DISABLE_COPY(ctkConfirmExitDialog);
};

#endif
