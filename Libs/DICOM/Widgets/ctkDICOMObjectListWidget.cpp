/*=========================================================================

  Library:   CTK

  Copyright (c) Brigham and Women's Hospital (BWH).

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

// ctkDICOMWidgets includes
#include "ctkDICOMObjectListWidget.h"
#include "ui_ctkDICOMObjectListWidget.h"

// Qt includes
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QSortFilterProxyModel>
#include <QString>
#include <QStringList>
#include <QUrl>

//CTK includes
#include <ctkDICOMObjectModel.h>
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMObjectListWidget");

class qRecursiveTreeProxyFilter : public QSortFilterProxyModel
{
public:
  qRecursiveTreeProxyFilter(QObject *parent = NULL):
    QSortFilterProxyModel(parent)
  {
  }

  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
  {
    if (filterRegExp().isEmpty())
      {
      return true;
      }
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    return filterAcceptsIndex(index);
  }

private:
  bool filterAcceptsIndex(const QModelIndex index) const
  {
    // Accept item if its tag, attribute, or value text matches
    if ((sourceModel()->data(sourceModel()->index(index.row(), ctkDICOMObjectModel::TagColumn,
      index.parent()), Qt::DisplayRole).toString().contains(filterRegExp()))
      || (sourceModel()->data(sourceModel()->index(index.row(), ctkDICOMObjectModel::AttributeColumn,
      index.parent()), Qt::DisplayRole).toString().contains(filterRegExp()))
      || (sourceModel()->data(sourceModel()->index(index.row(), ctkDICOMObjectModel::ValueColumn,
      index.parent()), Qt::DisplayRole).toString().contains(filterRegExp())))
      {
      return true;
      }
    // Accept item if any child matches
    for (int row = 0; row < sourceModel()->rowCount(index); row++)
      {
      QModelIndex childIndex = sourceModel()->index(row, 0, index);
      if (!childIndex.isValid())
        {
        break;
        }
      if (filterAcceptsIndex(childIndex))
        {
        return true;
        }
      }
    return false;
  }
};

//----------------------------------------------------------------------------
class ctkDICOMObjectListWidgetPrivate: public Ui_ctkDICOMObjectListWidget
{
public:
  ctkDICOMObjectListWidgetPrivate();
  ~ctkDICOMObjectListWidgetPrivate();
  void populateDICOMObjectTreeView(const QString& fileName);
  void setPathLabel(const QString& currentFile);
  QString dicomObjectModelAsString(QAbstractItemModel* dicomObjectModel, QModelIndex parent = QModelIndex(), int indent = 0, QString rowPrefix = QString());
  void setFilterExpressionInModel(qRecursiveTreeProxyFilter* filterModel, const QString& expr);

  QString endOfLine;
  QString currentFile;
  QStringList fileList;
  ctkDICOMObjectModel* dicomObjectModel;
  qRecursiveTreeProxyFilter* filterModel;
  QString filterExpression;
};

//----------------------------------------------------------------------------
// ctkDICOMObjectListWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMObjectListWidgetPrivate::ctkDICOMObjectListWidgetPrivate()
{
#ifdef WIN32
  this->endOfLine = "\r\n";
#else
  this->endOfLine = "\n";
#endif
  this->dicomObjectModel = 0;
  this->filterModel = 0;
}

//----------------------------------------------------------------------------
ctkDICOMObjectListWidgetPrivate::~ctkDICOMObjectListWidgetPrivate()
{
}

//----------------------------------------------------------------------------
void ctkDICOMObjectListWidgetPrivate::setFilterExpressionInModel(qRecursiveTreeProxyFilter* filterModel, const QString& expr)
{
  const QString regexpPrefix("regexp:");
  if (expr.startsWith(regexpPrefix))
    {
    filterModel->setFilterCaseSensitivity(Qt::CaseSensitive);
    filterModel->setFilterRegExp(expr.right(expr.length() - regexpPrefix.length()));
    }
  else
    {
    filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    filterModel->setFilterWildcard(expr);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMObjectListWidgetPrivate::populateDICOMObjectTreeView(const QString& fileName)
{
  this->dicomObjectModel->setFile(fileName);
  this->filterModel->invalidate();
  this->dcmObjectTreeView->setModel(this->filterModel);
  this->dcmObjectTreeView->expandAll();
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidgetPrivate::setPathLabel(const QString& currentFile)
{
  currentPathLabel->setText(currentFile);
}

// --------------------------------------------------------------------------
QString ctkDICOMObjectListWidgetPrivate::dicomObjectModelAsString(QAbstractItemModel* aDicomObjectModel, QModelIndex parent /*=QModelIndex()*/, int indent /*=0*/, QString rowPrefix /*=QString()*/)
{
  QString dump;
  QString indentString(indent, '\t'); // add tab characters, (indent) number of times
  for (int r = 0; r < aDicomObjectModel->rowCount(parent); ++r)
    {
    for (int c = 0; c < aDicomObjectModel->columnCount(); ++c)
      {
      QModelIndex index = aDicomObjectModel->index(r, c, parent);
      QString name = aDicomObjectModel->data(index).toString();
      if (c == 0)
        {
        // Replace round brackets by square brackets.
        // If the text is copied into Excel, Excel would recognize tag (0008,0012)
        // as a negative number (-80,012). Instead, [0008,0012] is displayed fine.
        name.replace('(', '[');
        name.replace(')', ']');
        dump += rowPrefix + indentString + name;
        }
      else
        {
        dump += "\t" + name;
        }
      }
    dump += endOfLine;
    // Print children
    QModelIndex index0 = aDicomObjectModel->index(r, 0, parent);
    if (aDicomObjectModel->hasChildren(index0))
      {
      dump += dicomObjectModelAsString(aDicomObjectModel, index0, indent + 1, rowPrefix);
      }
    }
  return dump;
}

//----------------------------------------------------------------------------
// ctkDICOMObjectListWidget methods

//----------------------------------------------------------------------------
ctkDICOMObjectListWidget::ctkDICOMObjectListWidget(QWidget* _parent):Superclass(_parent),
  d_ptr(new ctkDICOMObjectListWidgetPrivate)
{
  Q_D(ctkDICOMObjectListWidget);

  d->setupUi(this);

  d->metadataSearchBox->setAlwaysShowClearIcon(true);
  d->metadataSearchBox->setShowSearchIcon(true);

  d->dicomObjectModel = new ctkDICOMObjectModel(this);
  d->filterModel = new qRecursiveTreeProxyFilter(this);
  d->filterModel->setSourceModel(d->dicomObjectModel);

  d->fileSliderWidget->setMaximum(1);
  d->fileSliderWidget->setMinimum(1);
  d->fileSliderWidget->setPageStep(1);

  d->currentPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  connect(d->fileSliderWidget, SIGNAL(valueChanged(double)), this, SLOT(updateWidget()));
  connect(d->dcmObjectTreeView, SIGNAL(doubleClicked(const QModelIndex&)),
    this, SLOT(itemDoubleClicked(const QModelIndex&)));
  connect(d->copyPathPushButton , SIGNAL(clicked(bool)),this, SLOT(copyPath()));

  connect(d->expandAllPushButton, SIGNAL(clicked(bool)), d->dcmObjectTreeView, SLOT(expandAll()));
  connect(d->collapseAllPushButton, SIGNAL(clicked(bool)), d->dcmObjectTreeView, SLOT(collapseAll()));
  connect(d->copyMetadataPushButton, SIGNAL(clicked(bool)), this, SLOT(copyMetadata()));
  connect(d->copyAllFilesMetadataPushButton, SIGNAL(clicked(bool)), this, SLOT(copyAllFilesMetadata()));

  QObject::connect(d->metadataSearchBox, SIGNAL(textChanged(QString)), this, SLOT(setFilterExpression(QString)));
  QObject::connect(d->metadataSearchBox, SIGNAL(textChanged(QString)), this, SLOT(onFilterChanged()));
}

//----------------------------------------------------------------------------
ctkDICOMObjectListWidget::~ctkDICOMObjectListWidget()
{
  Q_D(ctkDICOMObjectListWidget);
  d->dicomObjectModel->deleteLater();
  d->filterModel->deleteLater();
}

//----------------------------------------------------------------------------
void ctkDICOMObjectListWidget::setCurrentFile(const QString& newFileName)
{
  Q_D(ctkDICOMObjectListWidget);
  d->setPathLabel(newFileName);
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::setFileList(const QStringList& fileList)
{
  Q_D(ctkDICOMObjectListWidget);
  d->fileList = fileList;
  if (d->fileList.size() > 0)
    {
    d->currentFile = d->fileList[0];
    
    d->populateDICOMObjectTreeView(d->currentFile);
    d->fileSliderWidget->setMaximum(fileList.size());
    d->fileSliderWidget->setSuffix(QString(" / %1").arg(fileList.size()));
    for (int columnIndex = 0; columnIndex < d->dicomObjectModel->columnCount(); ++columnIndex)
      {
      d->dcmObjectTreeView->resizeColumnToContents(columnIndex);
      }
    }
  else
    {
    d->currentFile.clear();
    d->dicomObjectModel->clear();
    }

  d->setPathLabel(d->currentFile);
  d->fileSliderWidget->setVisible(d->fileList.size() > 1);
}

// --------------------------------------------------------------------------
QString ctkDICOMObjectListWidget::currentFile()
{
  Q_D(ctkDICOMObjectListWidget);
  return d->currentFile;
}

// --------------------------------------------------------------------------
QStringList ctkDICOMObjectListWidget::fileList()
{
  Q_D(ctkDICOMObjectListWidget);
  return d->fileList;
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::openLookupUrl(QString tag)
{
  QString lookupUrl = "http://dicomlookup.com/lookup.asp?sw=Tnumber&q=" + tag;
  QUrl url(lookupUrl);
  QDesktopServices::openUrl(url);
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::itemDoubleClicked(const QModelIndex& index)
{
  Q_D(ctkDICOMObjectListWidget);
  QModelIndex tagIndex = d->filterModel->index(index.row(), 0, index.parent());
  QString tag = d->filterModel->data(tagIndex).toString();
  openLookupUrl(tag);
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::updateWidget()
{
  Q_D(ctkDICOMObjectListWidget);
  d->currentFile = d->fileList[static_cast<int>(d->fileSliderWidget->value())-1];
  d->setPathLabel(d->currentFile);
  d->populateDICOMObjectTreeView(d->currentFile);
 }

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::copyPath()
{
  Q_D(ctkDICOMObjectListWidget);
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(d->currentFile);
}

// --------------------------------------------------------------------------
QString ctkDICOMObjectListWidget::metadataAsText(bool allFiles /*=false*/)
{
  Q_D(ctkDICOMObjectListWidget);
  QString metadata;
  if (allFiles)
    {
    foreach(QString fileName, d->fileList)
      {
      // copy metadata of all files

      ctkDICOMObjectModel* aDicomObjectModel = new ctkDICOMObjectModel();
      aDicomObjectModel->setFile(fileName);

      qRecursiveTreeProxyFilter* afilterModel = new qRecursiveTreeProxyFilter();
      afilterModel->setSourceModel(aDicomObjectModel);
      d->setFilterExpressionInModel(afilterModel, d->filterExpression);

      QString thisFileMetadata = d->dicomObjectModelAsString(afilterModel, QModelIndex(), 0, fileName + "\t");

      if (!thisFileMetadata.isEmpty())
        {
        metadata += thisFileMetadata;
        }
      else
        {
        metadata += fileName + "\t(none)" + d->endOfLine;
        }

      delete afilterModel;
      delete aDicomObjectModel;
      }
    }
  else
    {
    // single file
    metadata = d->dicomObjectModelAsString(d->filterModel);
    }
  return metadata;
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::copyMetadata()
{
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(metadataAsText());
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::copyAllFilesMetadata()
{
  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(metadataAsText(true));
  QApplication::restoreOverrideCursor();
}

//------------------------------------------------------------------------------
void ctkDICOMObjectListWidget::onFilterChanged()
{
  Q_D(ctkDICOMObjectListWidget);

  // Change the searchbox background to yellow
  // if there are no matches
  bool showWarning = (d->filterModel->rowCount() == 0 &&
    d->dicomObjectModel->rowCount() != 0);
  QPalette palette;
  if (showWarning)
    {
    palette.setColor(QPalette::Base, Qt::yellow);
    palette.setColor(QPalette::Text, Qt::black);
    }
  d->metadataSearchBox->setPalette(palette);
}

//------------------------------------------------------------------------------
void ctkDICOMObjectListWidget::setFilterExpression(const QString& expr)
{
  Q_D(ctkDICOMObjectListWidget);
  d->filterExpression = expr;
  d->setFilterExpressionInModel(d->filterModel, expr);
}

//------------------------------------------------------------------------------
QString ctkDICOMObjectListWidget::filterExpression()
{
  Q_D(ctkDICOMObjectListWidget);
  return d->filterExpression;
}
