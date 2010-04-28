/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
