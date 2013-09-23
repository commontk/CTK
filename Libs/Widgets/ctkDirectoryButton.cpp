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

// Qt includes
#include <QDebug>
#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSortFilterProxyModel>
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
  void updateDisplayText();

  QDir         Directory;
  QPushButton* PushButton;
  QString      DialogCaption;
  QString      DisplayText;
#ifdef USE_QFILEDIALOG_OPTIONS
  QFileDialog::Options DialogOptions;
#else
  ctkDirectoryButton::Options DialogOptions;
#endif
  // TODO expose DisplayAbsolutePath into the API
  bool         DisplayAbsolutePath;
  QFileDialog::AcceptMode AcceptMode;
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
  this->AcceptMode = QFileDialog::AcceptOpen;
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
void ctkDirectoryButtonPrivate::updateDisplayText()
{
  QString buttonText = this->DisplayText;
  if (buttonText.isNull())
    {
    buttonText = this->DisplayAbsolutePath ?
      this->Directory.absolutePath() : this->Directory.path();
    }
  this->PushButton->setText(buttonText);
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
  d->updateDisplayText();

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
void ctkDirectoryButton::setText(const QString& text)
{
  Q_D(ctkDirectoryButton);
  d->DisplayText = text;
  d->updateDisplayText();
}

//-----------------------------------------------------------------------------
const QString& ctkDirectoryButton::text()const
{
  Q_D(const ctkDirectoryButton);
  return d->DisplayText;
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
QFileDialog::AcceptMode ctkDirectoryButton::acceptMode() const
{
  Q_D(const ctkDirectoryButton);
  return d->AcceptMode;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::setAcceptMode(QFileDialog::AcceptMode mode)
{
  Q_D(ctkDirectoryButton);
  d->AcceptMode = mode;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::browse()
{
  // See https://bugreports.qt-project.org/browse/QTBUG-10244
  class ExcludeReadOnlyFilterProxyModel : public QSortFilterProxyModel
  {
  public:
    ExcludeReadOnlyFilterProxyModel(QObject *parent):QSortFilterProxyModel(parent)
    {
    }
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
    {
      QString filePath =
          this->sourceModel()->data(sourceModel()->index(source_row, 0, source_parent),
          QFileSystemModel::FilePathRole).toString();
      return QFileInfo(filePath).isWritable();
    }
  };

  Q_D(ctkDirectoryButton);
  QScopedPointer<QFileDialog> fileDialog(
          new QFileDialog(this, d->DialogCaption.isEmpty() ? this->toolTip() :
          d->DialogCaption, d->Directory.path()));
  #ifdef USE_QFILEDIALOG_OPTIONS
    fileDialog->setOptions(d->DialogOptions);
  #else
    fileDialog->setOptions(QFlags<QFileDialog::Option>(int(d->DialogOptions)));
  #endif
    fileDialog->setAcceptMode(d->AcceptMode);
    fileDialog->setFileMode(QFileDialog::DirectoryOnly);

  if (d->AcceptMode == QFileDialog::AcceptSave)
    {
    // Ideally "Choose" button of QFileDialog should be disabled if a read-only folder
    // is selected and the acceptMode was AcceptSave.
    // This is captured in https://github.com/commontk/CTK/issues/365
    fileDialog->setProxyModel(new ExcludeReadOnlyFilterProxyModel(fileDialog.data()));
    }

  QString dir;
  if (fileDialog->exec())
    {
    dir = fileDialog->selectedFiles().at(0);
    }
  // An empty directory means either that the user cancelled the dialog or the selected directory is readonly
  if (dir.isEmpty())
    {
    return;
    }
  this->setDirectory(dir);
}
