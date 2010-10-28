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

// Qt includes
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>

// CTK includes
#include "ctkDirectoryButton.h"

//-----------------------------------------------------------------------------
class ctkDirectoryButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkDirectoryButton);

protected:
  ctkDirectoryButton* const q_ptr;

public:
  ctkDirectoryButtonPrivate(ctkDirectoryButton& object);
  void init();

  QDir         Directory;
  QPushButton* PushButton;
  QString      DialogCaption;
#ifdef USE_QFILEDIALOG_OPTIONS
  QFileDialog::Options DialogOptions;
#else
  ctkDirectoryButton::Options DialogOptions;
#endif
  // TODO expose DisplayAbsolutePath into the API
  bool         DisplayAbsolutePath;
};

//-----------------------------------------------------------------------------
ctkDirectoryButtonPrivate::ctkDirectoryButtonPrivate(ctkDirectoryButton& object)
  :q_ptr(&object)
{
#if USE_QFILEDIALOG_OPTIONS
  this->DialogOptions = QFileDialog::ShowDirsOnly;
#else
  this->DialogOptions = ctkDirectoryButton::ShowDirsOnly;
#endif
  this->DisplayAbsolutePath = true;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButtonPrivate::init()
{
  Q_Q(ctkDirectoryButton);
  this->PushButton = new QPushButton(q);
  QObject::connect(this->PushButton, SIGNAL(clicked()), q, SLOT(browse()));
  QHBoxLayout* l = new QHBoxLayout(q);
  l->addWidget(this->PushButton);
  l->setContentsMargins(0,0,0,0);
  q->setLayout(l);
  q->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::ButtonBox));
}

//-----------------------------------------------------------------------------
ctkDirectoryButton::ctkDirectoryButton(QWidget * parentWidget)
  :QWidget(parentWidget)
  , d_ptr(new ctkDirectoryButtonPrivate(*this))
{
  Q_D(ctkDirectoryButton);
  d->init();
  d->PushButton->setText(d->DisplayAbsolutePath ? d->Directory.absolutePath() : d->Directory.path());
  d->PushButton->setIcon(this->style()->standardIcon(QStyle::SP_DirIcon));
}
    
//-----------------------------------------------------------------------------
ctkDirectoryButton::ctkDirectoryButton(const QString& dir, 
                                       QWidget * parentWidget)
  :QWidget(parentWidget)
  , d_ptr(new ctkDirectoryButtonPrivate(*this))
{
  Q_D(ctkDirectoryButton);
  d->init();
  d->Directory = QDir(dir);
  d->PushButton->setText(d->DisplayAbsolutePath ? d->Directory.absolutePath() : d->Directory.path());
  d->PushButton->setIcon(this->style()->standardIcon(QStyle::SP_DirIcon));
}

//-----------------------------------------------------------------------------
ctkDirectoryButton::ctkDirectoryButton(
  const QIcon & icon, const QString& dir, QWidget * parentWidget)
  :QWidget(parentWidget)
  , d_ptr(new ctkDirectoryButtonPrivate(*this))
{
  Q_D(ctkDirectoryButton);
  d->init();
  d->Directory = QDir(dir);
  d->PushButton->setText(d->DisplayAbsolutePath ? d->Directory.absolutePath() : d->Directory.path());
  d->PushButton->setIcon(icon);
}

//-----------------------------------------------------------------------------
ctkDirectoryButton::~ctkDirectoryButton()
{
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::setDirectory(const QString& dir)
{
  Q_D(ctkDirectoryButton);
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
  Q_D(const ctkDirectoryButton);
  return d->Directory.path();
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::setCaption(const QString& caption)
{
  Q_D(ctkDirectoryButton);
  d->DialogCaption = caption;
}

//-----------------------------------------------------------------------------
const QString& ctkDirectoryButton::caption()const
{
  Q_D(const ctkDirectoryButton);
  return d->DialogCaption;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::setIcon(const QIcon& newIcon)
{
  Q_D(const ctkDirectoryButton);
  return d->PushButton->setIcon(newIcon);
}


//-----------------------------------------------------------------------------
QIcon ctkDirectoryButton::icon()const
{
  Q_D(const ctkDirectoryButton);
  return d->PushButton->icon();
}

//-----------------------------------------------------------------------------
#ifdef USE_QFILEDIALOG_OPTIONS
void ctkDirectoryButton::setOptions(const QFileDialog::Options& dialogOptions)
#else
void ctkDirectoryButton::setOptions(const Options& dialogOptions)
#endif
{
  Q_D(ctkDirectoryButton);
  d->DialogOptions = dialogOptions;
}

//-----------------------------------------------------------------------------
#ifdef USE_QFILEDIALOG_OPTIONS
const QFileDialog::Options& ctkDirectoryButton::options()const
#else
const ctkDirectoryButton::Options& ctkDirectoryButton::options()const
#endif
{
  Q_D(const ctkDirectoryButton);
  return d->DialogOptions;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::browse()
{
  Q_D(ctkDirectoryButton);
  QString dir =
    QFileDialog::getExistingDirectory(
      this,
      d->DialogCaption.isEmpty() ? this->toolTip() : d->DialogCaption,
      d->Directory.path(),
#ifdef USE_QFILEDIALOG_OPTIONS
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
