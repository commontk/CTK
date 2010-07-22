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
#include <QDebug>
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
#if QT_VERSION >= 0x040603
  QFileDialog::Options DialogOptions;
#else
  ctkDirectoryButton::Options DialogOptions;
#endif
  // TODO expose DisplayAbsolutePath into the API
  bool         DisplayAbsolutePath;
};

//-----------------------------------------------------------------------------
ctkDirectoryButtonPrivate::ctkDirectoryButtonPrivate()
{
#if QT_VERSION >= 0x040603
  this->DialogOptions = QFileDialog::ShowDirsOnly;
#else
  this->DialogOptions = ctkDirectoryButton::ShowDirsOnly;
#endif
  this->DisplayAbsolutePath = true;
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
  d->PushButton->setText(d->DisplayAbsolutePath ? d->Directory.absolutePath() : d->Directory.path());
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
  d->PushButton->setText(d->DisplayAbsolutePath ? d->Directory.absolutePath() : d->Directory.path());
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
  d->PushButton->setText(d->DisplayAbsolutePath ? d->Directory.absolutePath() : d->Directory.path());
  d->PushButton->setIcon(icon);
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::setDirectory(const QString& dir)
{
  CTK_D(ctkDirectoryButton);
  QDir newDirectory(dir);

  if (d->Directory == newDirectory)
    {
    emit directorySelected(d->DisplayAbsolutePath ?
                           newDirectory.absolutePath() :
                           newDirectory.path());
    return;
    }

  d->Directory = newDirectory;

  d->PushButton->setText(d->DisplayAbsolutePath ? d->Directory.absolutePath() : d->Directory.path());

  emit directorySelected(d->DisplayAbsolutePath ?
                         newDirectory.absolutePath() :
                         newDirectory.path());

  emit directoryChanged(d->DisplayAbsolutePath ? d->Directory.absolutePath() : d->Directory.path());
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
#if QT_VERSION >= 0x040603
void ctkDirectoryButton::setOptions(const QFileDialog::Options& dialogOptions)
#else
void ctkDirectoryButton::setOptions(const Options& dialogOptions)
#endif
{
  CTK_D(ctkDirectoryButton);
  d->DialogOptions = dialogOptions;
}

//-----------------------------------------------------------------------------
#if QT_VERSION >= 0x040603
const QFileDialog::Options& ctkDirectoryButton::options()const
#else
const ctkDirectoryButton::Options& ctkDirectoryButton::options()const
#endif
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
#if QT_VERSION >= 0x040603
      d->DialogOptions);
#else
      QFlags<QFileDialog::Option>(int(d->DialogOptions)));
#endif
  // An empty directory means that the user cancelled the dialog.
  if (dir.isEmpty())
    {
    return;
    }
  this->setDirectory(dir);
}
