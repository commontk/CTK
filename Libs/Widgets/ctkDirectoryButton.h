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

#ifndef __ctkDirectoryButton_h
#define __ctkDirectoryButton_h

// Qt includes
#include <QPushButton>
#include <QFileDialog>

// CTK includes
#include <ctkPimpl.h>
#include "CTKWidgetsExport.h"
class ctkDirectoryButtonPrivate;

class CTK_WIDGETS_EXPORT ctkDirectoryButton: public QPushButton
{
  Q_OBJECT
  Q_PROPERTY(QString caption READ caption WRITE setCaption)
  Q_PROPERTY(QFileDialog::Options options READ options WRITE setOptions)
public: 
  ctkDirectoryButton(QWidget * parent = 0);
  ctkDirectoryButton(const QString & text, QWidget * parent = 0);
  ctkDirectoryButton(const QIcon & icon, const QString & text, QWidget * parent = 0);

  void setCaption(const QString& caption);
  const QString& caption()const;
  
  void setOptions(const QFileDialog::Options& options);
  const QFileDialog::Options& options()const;

public slots:
  void browse();

signals:
  void directoryChanged(const QString&);

private:
  CTK_DECLARE_PRIVATE(ctkDirectoryButton);
};

#endif
