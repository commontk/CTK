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
#include "ctkFileDialog.h"

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
    ExcludeReadOnlyFilterProxyModel(QPalette palette, QObject *parent)
      : QSortFilterProxyModel(parent)
      , Palette(palette)
    {
    }
    virtual Qt::ItemFlags flags(const QModelIndex& index)const
    {
      QString filePath =
        this->sourceModel()->data(this->mapToSource(index),
                                  QFileSystemModel::FilePathRole).toString();
      if (!QFileInfo(filePath).isWritable())
        {
        // Double clickable (to open) but can't be "choosen".
        return Qt::ItemIsSelectable;
        }
      return this->QSortFilterProxyModel::flags(index);
    }
    QPalette Palette;
  };

  Q_D(ctkDirectoryButton);
  // Use a ctkFileDialog (vs QFileDialog) for the AcceptSave mode so it does not
  // select non writable folders.
  QScopedPointer<ctkFileDialog> fileDialog(
    new ctkFileDialog(this, d->DialogCaption.isEmpty() ? this->toolTip() :
                      d->DialogCaption, d->Directory.path()));
  #ifdef USE_QFILEDIALOG_OPTIONS
    fileDialog->setOptions(d->DialogOptions);
  #else
    fileDialog->setOptions(QFlags<QFileDialog::Option>(int(d->DialogOptions)));
  #endif
    fileDialog->setAcceptMode(d->AcceptMode);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->setOption(QFileDialog::ShowDirsOnly, true);

  if (d->AcceptMode == QFileDialog::AcceptSave)
    {
    // Gray out the non-writable folders. They are still openable with double click,
    // but they can't be selected because they don't have the ItemIsEnabled
    // flag and because ctkFileDialog would not let it to be selected.
    fileDialog->setProxyModel(
      new ExcludeReadOnlyFilterProxyModel(this->palette(), fileDialog.data()));
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
