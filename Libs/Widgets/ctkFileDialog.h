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

#ifndef __ctkFileDialog_h
#define __ctkFileDialog_h

// Qt includes
#include <QFileDialog>

// CTK includes
#include <ctkPimpl.h>
#include "CTKWidgetsExport.h"

class ctkFileDialogPrivate;

class CTK_WIDGETS_EXPORT ctkFileDialog : public QFileDialog
{
  Q_OBJECT

public:
  // Superclass typedef
  typedef QFileDialog Superclass;
  // Constructors
  explicit ctkFileDialog(QWidget *parent = 0,
              const QString &caption = QString(),
              const QString &directory = QString(),
              const QString &filter = QString());
  virtual ~ctkFileDialog();

  void setBottomWidget(QWidget* widget, const QString& label=QString());
  QWidget* bottomWidget()const;

  bool eventFilter(QObject *obj, QEvent *event);
public slots:
  void setAcceptButtonEnable(bool enable);

private:
  CTK_DECLARE_PRIVATE(ctkFileDialog);
};

#endif
