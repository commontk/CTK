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

// Qt includes
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>

// CTK includes
#include "ctkDirectoryButton.h"

//-----------------------------------------------------------------------------
class ctkDirectoryButtonPrivate: public ctkPrivate<ctkDirectoryButton>
{
public:
  ctkDirectoryButtonPrivate();
  void init();

  QDir         Directory;
  QPushButton* PushButton;
  QString      DialogCaption;
  ctkDirectoryButton::Options DialogOptions;
};

//-----------------------------------------------------------------------------
ctkDirectoryButtonPrivate::ctkDirectoryButtonPrivate()
{
  this->DialogOptions = ctkDirectoryButton::ShowDirsOnly;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButtonPrivate::init()
{
  CTK_P(ctkDirectoryButton);
  this->PushButton = new QPushButton(p);
  QObject::connect(this->PushButton, SIGNAL(clicked()), p, SLOT(browse()));
  QHBoxLayout* l = new QHBoxLayout(p);
  l->addWidget(this->PushButton);
  l->setContentsMargins(0,0,0,0);
  p->setLayout(l);
}

//-----------------------------------------------------------------------------
ctkDirectoryButton::ctkDirectoryButton(QWidget * parentWidget)
  :QWidget(parentWidget)
{
  CTK_INIT_PRIVATE(ctkDirectoryButton);
  CTK_D(ctkDirectoryButton);
  d->init();
  d->PushButton->setIcon(this->style()->standardIcon(QStyle::SP_DirIcon));
}
    
//-----------------------------------------------------------------------------
ctkDirectoryButton::ctkDirectoryButton(const QString& dir, 
                                       QWidget * parentWidget)
  :QWidget(parentWidget)
{
  CTK_INIT_PRIVATE(ctkDirectoryButton);
  CTK_D(ctkDirectoryButton);
  d->init();
  d->Directory = QDir(dir);
  d->PushButton->setText(d->Directory.path());
  d->PushButton->setIcon(this->style()->standardIcon(QStyle::SP_DirIcon));
}

//-----------------------------------------------------------------------------
ctkDirectoryButton::ctkDirectoryButton(
  const QIcon & icon, const QString& dir, QWidget * parentWidget)
  :QWidget(parentWidget)
{
  CTK_INIT_PRIVATE(ctkDirectoryButton);
  CTK_D(ctkDirectoryButton);
  d->init();
  d->Directory = QDir(dir);
  d->PushButton->setText(d->Directory.path());
  d->PushButton->setIcon(icon);
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::setDirectory(const QString& dir)
{
  CTK_D(ctkDirectoryButton);
  QDir newDirectory(dir);
  if (d->Directory == newDirectory )
    {
    return;
    }
  d->Directory = newDirectory;
  d->PushButton->setText(d->Directory.path());
  emit directoryChanged(d->Directory.path());
}

//-----------------------------------------------------------------------------
QString ctkDirectoryButton::directory()const
{
  CTK_D(const ctkDirectoryButton);
  return d->Directory.path();
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::setCaption(const QString& caption)
{
  CTK_D(ctkDirectoryButton);
  d->DialogCaption = caption;
}

//-----------------------------------------------------------------------------
const QString& ctkDirectoryButton::caption()const
{
  CTK_D(const ctkDirectoryButton);
  return d->DialogCaption;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::setOptions(const Options& dialogOptions)
{
  CTK_D(ctkDirectoryButton);
  d->DialogOptions = dialogOptions;
}

//-----------------------------------------------------------------------------
const ctkDirectoryButton::Options& ctkDirectoryButton::options()const
{
  CTK_D(const ctkDirectoryButton);
  return d->DialogOptions;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::browse()
{
  CTK_D(ctkDirectoryButton);
  QString dir = 
    QFileDialog::getExistingDirectory(
      this, 
      d->DialogCaption.isEmpty() ? this->toolTip() : d->DialogCaption, 
      d->Directory.path(), 
      QFlags<QFileDialog::Option>(int(d->DialogOptions)));
  // An empty directory means that the user cancelled the dialog.
  if (dir.isEmpty())
    {
    return;
    }
  this->setDirectory(dir);
}
