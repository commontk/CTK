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
#include <QAbstractItemView>
#include <QApplication>
#include <QComboBox>
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSettings>
#include <QStyleOptionComboBox>
#include <QToolButton>

// CTK includes
#include "ctkPathLineEdit.h"
#include "ctkUtils.h"

//-----------------------------------------------------------------------------
class ctkPathLineEditPrivate
{
  Q_DECLARE_PUBLIC(ctkPathLineEdit);

protected:
  ctkPathLineEdit* const q_ptr;

public:
  ctkPathLineEditPrivate(ctkPathLineEdit& object);
  void init();
  QSize recomputeSizeHint(QSize& sh)const;
  void updateFilter();

  void adjustPathLineEditSize();

  void _q_recomputeCompleterPopupSize();

  void createPathLineEditWidget(bool useComboBox);
  QString settingKey()const;

  QLineEdit*            LineEdit;
  QComboBox*            ComboBox;
  QToolButton*          BrowseButton;       //!< "..." button

  int                   MinimumContentsLength;
  ctkPathLineEdit::SizeAdjustPolicy SizeAdjustPolicy;

  QString               Label;              //!< used in file dialogs
  QStringList           NameFilters;        //!< Regular expression (in wildcard mode) used to help the user to complete the line
  QDir::Filters         Filters;            //!< Type of path (file, dir...)
#ifdef USE_QFILEDIALOG_OPTIONS
  QFileDialog::Options DialogOptions;
#else
  ctkPathLineEdit::Options DialogOptions;
#endif

  bool                  HasValidInput;      //!< boolean that stores the old state of valid input
  QString               SettingKey;

  static QString        sCurrentDirectory;   //!< Content the last value of the current directory
  static int            sMaxHistory;     //!< Size of the history, if the history is full and a new value is added, the oldest value is dropped

  mutable QSize SizeHint;
  mutable QSize MinimumSizeHint;
};

QString ctkPathLineEditPrivate::sCurrentDirectory = "";
int ctkPathLineEditPrivate::sMaxHistory = 5;

//-----------------------------------------------------------------------------
ctkPathLineEditPrivate::ctkPathLineEditPrivate(ctkPathLineEdit& object)
  : q_ptr(&object)
  , LineEdit(0)
  , ComboBox(0)
  , BrowseButton(0)
  , MinimumContentsLength(0)
  , SizeAdjustPolicy(ctkPathLineEdit::AdjustToContentsOnFirstShow)
  , Filters(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Readable)
  , HasValidInput(false)
{
}

//-----------------------------------------------------------------------------
void ctkPathLineEditPrivate::init()
{
  Q_Q(ctkPathLineEdit);

  QHBoxLayout* layout = new QHBoxLayout(q);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0); // no space between the combobx and button

  this->createPathLineEditWidget(true);

  this->BrowseButton = new QToolButton(q);
  this->BrowseButton->setText("...");
  // Don't vertically stretch the path line edit unnecessary
  this->BrowseButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored));
  this->BrowseButton->setToolTip(ctkPathLineEdit::tr("Open a dialog"));

  QObject::connect(this->BrowseButton,SIGNAL(clicked()),
                   q, SLOT(browse()));

  layout->addWidget(this->BrowseButton);

  q->setSizePolicy(QSizePolicy(
                     QSizePolicy::Expanding, QSizePolicy::Fixed,
                     QSizePolicy::LineEdit));
}

//------------------------------------------------------------------------------
void ctkPathLineEditPrivate::createPathLineEditWidget(bool useComboBox)
{
  Q_Q(ctkPathLineEdit);

  QString path = q->currentPath();

  if (useComboBox)
    {
    this->ComboBox = new QComboBox(q);
    this->ComboBox->setEditable(true);
    this->ComboBox->setInsertPolicy(QComboBox::NoInsert);
    this->LineEdit = this->ComboBox->lineEdit();
    }
  else
    {
    this->ComboBox = 0;
    this->LineEdit = new QLineEdit(q);
    }

  if (q->layout() && q->layout()->itemAt(0))
    {
    delete q->layout()->itemAt(0)->widget();
    }
  qobject_cast<QHBoxLayout*>(q->layout())->insertWidget(
    0,
    this->ComboBox ? qobject_cast<QWidget*>(this->ComboBox) :
    qobject_cast<QWidget*>(this->LineEdit));

  this->updateFilter();
  q->retrieveHistory();
  q->setCurrentPath(path);

  QObject::connect(this->LineEdit, SIGNAL(textChanged(QString)),
                   q, SLOT(setCurrentDirectory(QString)));
  QObject::connect(this->LineEdit, SIGNAL(textChanged(QString)),
                   q, SLOT(updateHasValidInput()));
  q->updateGeometry();
}

//------------------------------------------------------------------------------
QSize ctkPathLineEditPrivate::recomputeSizeHint(QSize& sh)const
{
  Q_Q(const ctkPathLineEdit);
  if (!sh.isValid())
    {
    int frame = 0;
    if (this->ComboBox)
      {
      QStyleOptionComboBox option;
      int arrowWidth = this->ComboBox->style()->subControlRect(
            QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxArrow, this->ComboBox).width()
          + (this->ComboBox->hasFrame() ? 2 : 0);
      frame = 2 * (this->ComboBox->hasFrame() ? 3 : 0)
          + arrowWidth
          + 1; // for mac style, not sure why
      }
    else
      {
      QStyleOptionFrame option;
      int frameWidth = this->LineEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, q);
      int horizontalMargin = 2; // QLineEditPrivate::horizontalMargin
      // See QLineEdit::sizeHint
      frame = 2 * frameWidth
          + this->LineEdit->textMargins().left()
          + this->LineEdit->textMargins().right()
          + this->LineEdit->contentsMargins().left()
          + this->LineEdit->contentsMargins().right()
          + 2 * horizontalMargin;
      }
    int browseWidth = 0;
    if (q->showBrowseButton())
      {
      browseWidth = this->BrowseButton->minimumSizeHint().width();
      }

    // text width
    int textWidth = 0;
    if (&sh == &this->SizeHint || this->MinimumContentsLength == 0)
      {
      switch (SizeAdjustPolicy)
        {
        case ctkPathLineEdit::AdjustToContents:
        case ctkPathLineEdit::AdjustToContentsOnFirstShow:
          if (this->LineEdit->text().isEmpty())
            {
            textWidth = 7 * this->LineEdit->fontMetrics().width(QLatin1Char('x'));
            }
          else
            {
            textWidth = this->LineEdit->fontMetrics().boundingRect(this->LineEdit->text()).width() + 8;
            }
          break;
        case QComboBox::AdjustToMinimumContentsLength:
        default:
          ;
        }
      }

    if (this->MinimumContentsLength > 0)
      {
      textWidth = qMax(textWidth, this->MinimumContentsLength * this->LineEdit->fontMetrics().width(QLatin1Char('X')));
      }

    int height = (this->ComboBox ? this->ComboBox->minimumSizeHint() :
                                   this->LineEdit->minimumSizeHint()).height();
    sh.rwidth() = frame + textWidth + browseWidth;
    sh.rheight() = height;
  }
  return sh.expandedTo(QApplication::globalStrut());
}

//-----------------------------------------------------------------------------
void ctkPathLineEditPrivate::updateFilter()
{
  Q_Q(ctkPathLineEdit);
  // help completion for the QComboBox::QLineEdit
  QCompleter *newCompleter = new QCompleter(q);
  newCompleter->setModel(new QDirModel(
                           ctk::nameFiltersToExtensions(this->NameFilters),
                           this->Filters | QDir::NoDotAndDotDot | QDir::AllDirs,
                           QDir::Name|QDir::DirsLast, newCompleter));
  this->LineEdit->setCompleter(newCompleter);

  QObject::connect(this->LineEdit->completer()->completionModel(), SIGNAL(layoutChanged()),
                   q, SLOT(_q_recomputeCompleterPopupSize()));

  // don't accept invalid path
  QRegExpValidator* validator = new QRegExpValidator(
    ctk::nameFiltersToRegExp(this->NameFilters), q);
  this->LineEdit->setValidator(validator);
}

//-----------------------------------------------------------------------------
void ctkPathLineEditPrivate::adjustPathLineEditSize()
{
  Q_Q(ctkPathLineEdit);
  if (q->sizeAdjustPolicy() == ctkPathLineEdit::AdjustToContents)
    {
    q->updateGeometry();
    q->adjustSize();
    q->update();
    }
}

//-----------------------------------------------------------------------------
void ctkPathLineEditPrivate::_q_recomputeCompleterPopupSize()
{
  QSize lineEditSize = this->LineEdit->size();

  QAbstractItemView* view = this->LineEdit->completer()->popup();
  const QFontMetrics& fm = view->fontMetrics();

  int iconWidth = 0;
  int textWidth = 0;

  QStyleOptionFrame option;
  int frameWidth = view->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, view);
  int frame = 2 * frameWidth
      + view->contentsMargins().left()
      + view->contentsMargins().right();

  QAbstractItemModel* model = this->LineEdit->completer()->completionModel();
  for (int i = 0; i < model->rowCount(); ++i)
    {
    QVariant icon = model->data(model->index(i, 0), Qt::DecorationRole);
    if (icon.isValid() && icon.canConvert<QIcon>())
      {
      iconWidth = qMax(iconWidth, icon.value<QIcon>().availableSizes().front().width() + 4);
      }
    textWidth = qMax(textWidth, fm.boundingRect(model->data(model->index(i, 0)).toString()).width());
    }

  view->setMinimumWidth(qMax(frame + iconWidth + textWidth, lineEditSize.width()));
}

//-----------------------------------------------------------------------------
QString ctkPathLineEditPrivate::settingKey()const
{
  Q_Q(const ctkPathLineEdit);
  return QString("ctkPathLineEdit/") +
    (this->SettingKey.isEmpty() ? q->objectName() : this->SettingKey);
}

//-----------------------------------------------------------------------------
ctkPathLineEdit::ctkPathLineEdit(QWidget *parentWidget)
  : QWidget(parentWidget)
  , d_ptr(new ctkPathLineEditPrivate(*this))
{
  Q_D(ctkPathLineEdit);
  d->init();

  this->setNameFilters(nameFilters());
  this->setFilters(filters());
}

//-----------------------------------------------------------------------------
ctkPathLineEdit::ctkPathLineEdit(const QString& label,
                                 const QStringList& nameFilters,
                                 Filters filters,
                                 QWidget *parentWidget)
  : QWidget(parentWidget)
  , d_ptr(new ctkPathLineEditPrivate(*this))
{
  Q_D(ctkPathLineEdit);
  d->init();

  this->setLabel(label);
  this->setNameFilters(nameFilters);
  this->setFilters(filters);
}

//-----------------------------------------------------------------------------
ctkPathLineEdit::~ctkPathLineEdit()
{
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::setLabel(const QString &label)
{
  Q_D(ctkPathLineEdit);
  d->Label = label;
}

//-----------------------------------------------------------------------------
const QString& ctkPathLineEdit::label()const
{
  Q_D(const ctkPathLineEdit);
  return d->Label;
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::setNameFilters(const QStringList &nameFilters)
{
  Q_D(ctkPathLineEdit);
  d->NameFilters = nameFilters;
  d->updateFilter();
}

//-----------------------------------------------------------------------------
const QStringList& ctkPathLineEdit::nameFilters()const
{
  Q_D(const ctkPathLineEdit);
  return d->NameFilters;
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::setFilters(const Filters &filters)
{
  Q_D(ctkPathLineEdit);
  d->Filters = QFlags<QDir::Filter>(static_cast<int>(filters));
  d->updateFilter();
}

//-----------------------------------------------------------------------------
ctkPathLineEdit::Filters ctkPathLineEdit::filters()const
{
  Q_D(const ctkPathLineEdit);
  return QFlags<ctkPathLineEdit::Filter>(static_cast<int>(d->Filters));
}

//-----------------------------------------------------------------------------
#ifdef USE_QFILEDIALOG_OPTIONS
void ctkPathLineEdit::setOptions(const QFileDialog::Options& dialogOptions)
#else
void ctkPathLineEdit::setOptions(const Options& dialogOptions)
#endif
{
  Q_D(ctkPathLineEdit);
  d->DialogOptions = dialogOptions;
}

//-----------------------------------------------------------------------------
#ifdef USE_QFILEDIALOG_OPTIONS
const QFileDialog::Options& ctkPathLineEdit::options()const
#else
const ctkPathLineEdit::Options& ctkPathLineEdit::options()const
#endif
{
  Q_D(const ctkPathLineEdit);
  return d->DialogOptions;
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::browse()
{
  Q_D(ctkPathLineEdit);
  QString path = "";
  if ( d->Filters & QDir::Files ) //file
    {
    if ( d->Filters & QDir::Writable) // load or save
      {
      path = QFileDialog::getSaveFileName(
	this,
        tr("Select a file to save "),
        this->currentPath().isEmpty() ? ctkPathLineEditPrivate::sCurrentDirectory :
	                                this->currentPath(),
	d->NameFilters.join(";;"),
	0,
#ifdef USE_QFILEDIALOG_OPTIONS
      d->DialogOptions);
#else
      QFlags<QFileDialog::Option>(int(d->DialogOptions)));
#endif
      }
    else
      {
      path = QFileDialog::getOpenFileName(
        this,
        QString("Open a file"),
        this->currentPath().isEmpty()? ctkPathLineEditPrivate::sCurrentDirectory :
	                               this->currentPath(),
        d->NameFilters.join(";;"),
	0,
#ifdef USE_QFILEDIALOG_OPTIONS
      d->DialogOptions);
#else
      QFlags<QFileDialog::Option>(int(d->DialogOptions)));
#endif
      }
    }
  else //directory
    {
    path = QFileDialog::getExistingDirectory(
      this,
      QString("Select a directory..."),
      this->currentPath().isEmpty() ? ctkPathLineEditPrivate::sCurrentDirectory :
                                      this->currentPath(),
#ifdef USE_QFILEDIALOG_OPTIONS
      d->DialogOptions);
#else
      QFlags<QFileDialog::Option>(int(d->DialogOptions)));
#endif
    }
  if (path.isEmpty())
    {
    return;
    }
  this->setCurrentPath(path);
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::retrieveHistory()
{
  Q_D(ctkPathLineEdit);
  if (d->ComboBox == 0)
    {
    return;
    }
  QString path = this->currentPath();
  bool wasBlocking = this->blockSignals(true);
  d->ComboBox->clear();
  // fill the combobox using the QSettings
  QSettings settings;
  QString key = d->settingKey();
  const QStringList history = settings.value(key).toStringList();
  foreach(const QString& path, history)
    {
    d->ComboBox->addItem(path);
    if (d->ComboBox->count() >= ctkPathLineEditPrivate::sMaxHistory)
      {
      break;
      }
    }
  // Restore path or select the most recent file location if none set.
  if (path.isEmpty())
    {
    this->blockSignals(wasBlocking);
    d->ComboBox->setCurrentIndex(0);
    }
  else
    {
    this->setCurrentPath(path);
    this->blockSignals(wasBlocking);
    }
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::addCurrentPathToHistory()
{
  Q_D(ctkPathLineEdit);
  if (d->ComboBox == 0 ||
      this->currentPath().isEmpty())
    {
    return;
    }
  QSettings settings;
  //keep the same values, add the current value
  //if more than m_MaxHistory entrees, drop the oldest.
  QString key = d->settingKey();
  QStringList history = settings.value(key).toStringList();
  QString pathToAdd = this->currentPath();
  if (history.contains(pathToAdd))
    {
    history.removeAll(pathToAdd);
    }
  history.push_front(pathToAdd);
  settings.setValue(key, history);
  // don't fire intermediate events.
  bool wasBlocking = d->ComboBox->blockSignals(false);
  int index = d->ComboBox->findText(this->currentPath());
  if (index >= 0)
    {
    d->ComboBox->removeItem(index);
    }
  while (d->ComboBox->count() >= ctkPathLineEditPrivate::sMaxHistory)
    {
    d->ComboBox->removeItem(d->ComboBox->count() - 1);
    }
  d->ComboBox->insertItem(0, pathToAdd);
  d->ComboBox->setCurrentIndex(0);
  d->ComboBox->blockSignals(wasBlocking);
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setCurrentFileExtension(const QString& extension)
{
  QString filename = this->currentPath();
  QFileInfo fileInfo(filename);

  if (!fileInfo.suffix().isEmpty())
    {
    filename.replace(fileInfo.suffix(), extension);
    }
  else
    {
    filename.append(QString(".") + extension);
    }
  this->setCurrentPath(filename);
}

//------------------------------------------------------------------------------
QComboBox* ctkPathLineEdit::comboBox() const
{
  Q_D(const ctkPathLineEdit);
  return d->ComboBox;
}

//------------------------------------------------------------------------------
QString ctkPathLineEdit::currentPath()const
{
  Q_D(const ctkPathLineEdit);
  return d->LineEdit ? d->LineEdit->text() : QString();
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setCurrentPath(const QString& path)
{
  Q_D(ctkPathLineEdit);
  d->LineEdit->setText(path);
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setCurrentDirectory(const QString& directory)
{
  ctkPathLineEditPrivate::sCurrentDirectory = directory;
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::updateHasValidInput()
{
  Q_D(ctkPathLineEdit);

  bool oldHasValidInput = d->HasValidInput;
  d->HasValidInput = d->LineEdit->hasAcceptableInput();
  if (d->HasValidInput)
    {
    QFileInfo fileInfo(this->currentPath());
    ctkPathLineEditPrivate::sCurrentDirectory =
      fileInfo.isFile() ? fileInfo.absolutePath() : fileInfo.absoluteFilePath();
    emit currentPathChanged(this->currentPath());
    }
  if (d->HasValidInput != oldHasValidInput)
    {
    emit validInputChanged(d->HasValidInput);
    }

  if (d->SizeAdjustPolicy == AdjustToContents)
    {
    d->SizeHint = QSize();
    d->adjustPathLineEditSize();
    this->updateGeometry();
    }
}

//------------------------------------------------------------------------------
QString ctkPathLineEdit::settingKey()const
{
  Q_D(const ctkPathLineEdit);
  return d->SettingKey;
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setSettingKey(const QString& key)
{
  Q_D(ctkPathLineEdit);
  d->SettingKey = key;
  this->retrieveHistory();
}

//------------------------------------------------------------------------------
bool ctkPathLineEdit::showBrowseButton()const
{
  Q_D(const ctkPathLineEdit);
  return d->BrowseButton->isVisibleTo(const_cast<ctkPathLineEdit*>(this));
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setShowBrowseButton(bool visible)
{
  Q_D(ctkPathLineEdit);
  d->BrowseButton->setVisible(visible);
}

//------------------------------------------------------------------------------
bool ctkPathLineEdit::showHistoryButton()const
{
  Q_D(const ctkPathLineEdit);
  return d->ComboBox ? true: false;
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setShowHistoryButton(bool visible)
{
  Q_D(ctkPathLineEdit);
  d->createPathLineEditWidget(visible);
}

//------------------------------------------------------------------------------
ctkPathLineEdit::SizeAdjustPolicy ctkPathLineEdit::sizeAdjustPolicy() const
{
  Q_D(const ctkPathLineEdit);
  return d->SizeAdjustPolicy;
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setSizeAdjustPolicy(ctkPathLineEdit::SizeAdjustPolicy policy)
{
  Q_D(ctkPathLineEdit);
  if (policy == d->SizeAdjustPolicy)
    return;

  d->SizeAdjustPolicy = policy;
  d->SizeHint = QSize();
  d->adjustPathLineEditSize();
  this->updateGeometry();
}

//------------------------------------------------------------------------------
int ctkPathLineEdit::minimumContentsLength()const
{
  Q_D(const ctkPathLineEdit);
  return d->MinimumContentsLength;
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setMinimumContentsLength(int length)
{
  Q_D(ctkPathLineEdit);
  if (d->MinimumContentsLength == length || length < 0) return;

  d->MinimumContentsLength = length;

  if (d->SizeAdjustPolicy == AdjustToContents ||
      d->SizeAdjustPolicy == AdjustToMinimumContentsLength)
    {
    d->SizeHint = QSize();
    d->adjustPathLineEditSize();
    this->updateGeometry();
    }
}

//------------------------------------------------------------------------------
QSize ctkPathLineEdit::minimumSizeHint()const
{
  Q_D(const ctkPathLineEdit);
  return d->recomputeSizeHint(d->MinimumSizeHint);
}

//------------------------------------------------------------------------------
QSize ctkPathLineEdit::sizeHint()const
{
  Q_D(const ctkPathLineEdit);
  return d->recomputeSizeHint(d->SizeHint);
}

#include "moc_ctkPathLineEdit.cpp"
