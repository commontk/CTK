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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// Qt includes
#include <QComboBox>
#include <QDebug>
#include <QIntValidator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QSettings>
#include <QSpinBox>
#include <QStyledItemDelegate>
#include <QTableWidgetItem>
#include <QVariant>

// ctkCore includes
#include <ctkCheckableHeaderView.h>
#include <ctkCheckableModelHelper.h>
#include <ctkLogger.h>

// ctkDICOMCore includes
#include <ctkDICOMEcho.h>
#include <ctkDICOMJob.h>
#include <ctkDICOMScheduler.h>
#include <ctkDICOMServer.h>

// ctkDICOMWidgets includes
#include "ctkDICOMServerNodeWidget2.h"
#include "ui_ctkDICOMServerNodeWidget2.h"

static ctkLogger logger("org.commontk.DICOM.Widgets.DICOMServerNodeWidget2");
QColor ctkDICOMServerNodeWidget2DefaultColor(Qt::white);
QColor ctkDICOMServerNodeWidget2DarkModeDefaultColor(50, 50, 50);
QColor ctkDICOMServerNodeWidget2ModifiedColor(Qt::darkYellow);
QColor ctkDICOMServerNodeWidget2ServerSuccesColor(Qt::darkGreen);
QColor ctkDICOMServerNodeWidget2ServerProgressColor(Qt::darkCyan);
QColor ctkDICOMServerNodeWidget2ServerFailedColor(Qt::darkRed);

class QSelectionColorStyledItemDelegate : public QStyledItemDelegate
{
public:
  using QStyledItemDelegate::QStyledItemDelegate;
  void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
  {
    QStyledItemDelegate::initStyleOption(option, index);

    const QWidget* widget = option->widget;
    const QTableWidget* table = qobject_cast<const QTableWidget*>(widget);
    if (!table)
    {
      return;
    }

    QTableWidgetItem* item = table->item(index.row(), index.column());
    if (!item || item->background() == ctkDICOMServerNodeWidget2DefaultColor || item->background() == ctkDICOMServerNodeWidget2DarkModeDefaultColor)
    {
      return;
    }

    option->backgroundBrush = item->background();
    option->state &= ~QStyle::State_Selected;
  }
};

class QCheckStateStyledItemDelegate : public QStyledItemDelegate
{
public:
  using QStyledItemDelegate::QStyledItemDelegate;
  void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
  {
    QStyledItemDelegate::initStyleOption(option, index);

    const QWidget* widget = option->widget;
    const QTableWidget* table = qobject_cast<const QTableWidget*>(widget);
    if (!table)
    {
      return;
    }

    QTableWidgetItem* item = table->item(index.row(), index.column());
    if (!item || item->background() == ctkDICOMServerNodeWidget2DefaultColor || item->background() == ctkDICOMServerNodeWidget2DarkModeDefaultColor)
    {
      return;
    }

    option->backgroundBrush = item->background();
    option->state &= ~QStyle::State_Selected;
  }

  void paint(QPainter* painter,
             const QStyleOptionViewItem& option,
             const QModelIndex& index) const override
  {
    QStyleOptionViewItem opt = option;
    const QWidget* widget = opt.widget;
    initStyleOption(&opt, index);
    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

    if (opt.features & QStyleOptionViewItem::HasCheckIndicator)
    {
      switch (opt.checkState)
      {
        case Qt::Unchecked:
          opt.state |= QStyle::State_Off;
          break;
        case Qt::PartiallyChecked:
          opt.state |= QStyle::State_NoChange;
          break;
        case Qt::Checked:
          opt.state |= QStyle::State_On;
          break;
      }
      auto rect = style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt, widget);
      opt.rect = QStyle::alignedRect(opt.direction, Qt::AlignCenter, rect.size(), opt.rect);
      opt.state = opt.state & ~QStyle::State_HasFocus;
      style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &opt, painter, widget);
    }
    else if (!opt.icon.isNull())
    {
      // draw the icon
      QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
      iconRect = QStyle::alignedRect(opt.direction, Qt::AlignCenter, iconRect.size(), opt.rect);
      QIcon::Mode mode = QIcon::Normal;
      if (!(opt.state & QStyle::State_Enabled))
      {
        mode = QIcon::Disabled;
      }
      else if (opt.state & QStyle::State_Selected)
      {
        mode = QIcon::Selected;
      }
      QIcon::State state = opt.state & QStyle::State_Open ? QIcon::On : QIcon::Off;
      opt.icon.paint(painter, iconRect, opt.decorationAlignment, mode, state);
    }
    else
    {
      QStyledItemDelegate::paint(painter, opt, index);
    }
  }
protected:
  bool editorEvent(QEvent* event,
                   QAbstractItemModel* model,
                   const QStyleOptionViewItem& option,
                   const QModelIndex& index) override
  {
    Q_ASSERT(event);
    Q_ASSERT(model);
    // make sure that the item is checkable
    Qt::ItemFlags flags = model->flags(index);
    if (!(flags & Qt::ItemIsUserCheckable) || !(option.state & QStyle::State_Enabled) ||
        !(flags & Qt::ItemIsEnabled))
    {
      return false;
    }
    // make sure that we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
    {
      return false;
    }
    const QWidget* widget = option.widget;
    QStyle* style = option.widget ? widget->style() : QApplication::style();
    // make sure that we have the right event type
    if ((event->type() == QEvent::MouseButtonRelease) || (event->type() == QEvent::MouseButtonDblClick) ||
        (event->type() == QEvent::MouseButtonPress))
    {
      QStyleOptionViewItem viewOpt(option);
      initStyleOption(&viewOpt, index);
      QRect checkRect = style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &viewOpt, widget);
      checkRect = QStyle::alignedRect(viewOpt.direction, Qt::AlignCenter, checkRect.size(), viewOpt.rect);
      QMouseEvent* me = static_cast<QMouseEvent*>(event);
      if (me->button() != Qt::LeftButton || !checkRect.contains(me->pos()))
      {
        return false;
      }
      if ((event->type() == QEvent::MouseButtonPress) || (event->type() == QEvent::MouseButtonDblClick))
      {
        return true;
      }
    }
    else if (event->type() == QEvent::KeyPress)
    {
      if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
          static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
      {
        return false;
      }
    }
    else
    {
      return false;
    }
    Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
    if (flags & Qt::ItemIsUserTristate)
    {
      state = (static_cast<Qt::CheckState>((state + 1) % 3));
    }
    else
    {
      state = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
    }
    return model->setData(index, static_cast<int>(state), Qt::CheckStateRole);
  }
};

//----------------------------------------------------------------------------
class ctkDICOMServerNodeWidget2Private : public Ui_ctkDICOMServerNodeWidget2
{
  Q_DECLARE_PUBLIC(ctkDICOMServerNodeWidget2);

protected:
  ctkDICOMServerNodeWidget2* const q_ptr;

public:
  ctkDICOMServerNodeWidget2Private(ctkDICOMServerNodeWidget2& obj);
  ~ctkDICOMServerNodeWidget2Private();

  void init();
  void disconnectScheduler();
  void connectScheduler();
  /// Utility function that returns the storageAETitle and
  /// storagePort in a map
  QMap<QString, QVariant> parameters() const;

  /// Return the list of server names
  QStringList serverNodes() const;
  /// Return all the information associated to a server defined by its name
  QMap<QString, QVariant> serverNodeParameters(const QString& connectionName) const;
  QMap<QString, QVariant> serverNodeParameters(int row) const;
  QStringList getAllNodesName() const;
  int getServerNodeRowFromConnectionName(const QString& connectionName) const;
  QString getServerNodeConnectionNameFromRow(int row) const;

  /// Add a server node with the given parameters
  /// Return the row index added into the table
  int addServerNode(const QMap<QString, QVariant>& parameters);
  int addServerNode(ctkDICOMServer* server);
  QSharedPointer<ctkDICOMServer> createServerFromServerNode(const QMap<QString, QVariant>& node);
  void updateProxyComboBoxes() const;
  QStringList getAllServerNames();

  void updateServerVerification(const ctkDICOMJobDetail& td,
                                const QString& status);

  void settingsModified();
  void restoreFocus(QModelIndexList selectedIndexes,
                    int horizontalScrollBarValue,
                    int verticalScrollBarValue,
                    bool resetServerStatus = true);
  void setDynamicPaletteColorToWidget(QPalette* palette, QWidget* widget);

  bool SettingsModified;
  QSharedPointer<ctkDICOMScheduler> Scheduler;
  QPushButton* SaveButton;
  QPushButton* CancelButton;
  QPalette DefaultPalette;
  QPalette ModifiedPalette;
  QPalette ServerSuccesPalette;
  QPalette ServerProgresPalette;
  QPalette ServerFailedPalette;
};

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget2Private::ctkDICOMServerNodeWidget2Private(ctkDICOMServerNodeWidget2& obj)
  : q_ptr(&obj)
{
  this->SettingsModified = false;
  this->Scheduler = nullptr;
  this->CancelButton = nullptr;
  this->SaveButton = nullptr;
  this->DefaultPalette.setColor(QPalette::Button, ctkDICOMServerNodeWidget2DefaultColor);
  this->DefaultPalette.setColor(QPalette::Base, ctkDICOMServerNodeWidget2DefaultColor);
  this->ModifiedPalette.setColor(QPalette::Button, ctkDICOMServerNodeWidget2ModifiedColor);
  this->ModifiedPalette.setColor(QPalette::Base, ctkDICOMServerNodeWidget2ModifiedColor);
  this->ServerSuccesPalette.setColor(QPalette::Button, ctkDICOMServerNodeWidget2ServerSuccesColor);
  this->ServerSuccesPalette.setColor(QPalette::Base, ctkDICOMServerNodeWidget2ServerSuccesColor);
  this->ServerProgresPalette.setColor(QPalette::Button, ctkDICOMServerNodeWidget2ServerProgressColor);
  this->ServerProgresPalette.setColor(QPalette::Base, ctkDICOMServerNodeWidget2ServerProgressColor);
  this->ServerFailedPalette.setColor(QPalette::Button, ctkDICOMServerNodeWidget2ServerFailedColor);
  this->ServerFailedPalette.setColor(QPalette::Base, ctkDICOMServerNodeWidget2ServerFailedColor);
}

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget2Private::~ctkDICOMServerNodeWidget2Private()
{
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2Private::init()
{
  Q_Q(ctkDICOMServerNodeWidget2);

  this->setupUi(q);

  // checkable headers.
  QHeaderView* previousHeaderView = this->NodeTable->horizontalHeader();
  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, this->NodeTable);
  headerView->setSectionsClickable(false);
  headerView->setSectionsMovable(false);
  headerView->setHighlightSections(false);
  headerView->checkableModelHelper()->setPropagateDepth(-1);
  headerView->setStretchLastSection(previousHeaderView->stretchLastSection());
  headerView->setMinimumSectionSize(previousHeaderView->minimumSectionSize());
  headerView->setDefaultSectionSize(previousHeaderView->defaultSectionSize());
  this->NodeTable->setHorizontalHeader(headerView);

  this->VerifyButton->setEnabled(false);
  this->RemoveButton->setEnabled(false);

  this->NodeTable->setItemDelegateForColumn(ctkDICOMServerNodeWidget2::NameColumn,
                                            new QSelectionColorStyledItemDelegate());
  this->NodeTable->setItemDelegateForColumn(ctkDICOMServerNodeWidget2::QueryRetrieveColumn,
                                            new QCheckStateStyledItemDelegate());
  this->NodeTable->setItemDelegateForColumn(ctkDICOMServerNodeWidget2::StorageColumn,
                                            new QCheckStateStyledItemDelegate());
  this->NodeTable->setItemDelegateForColumn(ctkDICOMServerNodeWidget2::TrustedColumn,
                                            new QCheckStateStyledItemDelegate());
  this->NodeTable->setItemDelegateForColumn(ctkDICOMServerNodeWidget2::CallingAETitleColumn,
                                            new QSelectionColorStyledItemDelegate());
  this->NodeTable->setItemDelegateForColumn(ctkDICOMServerNodeWidget2::CalledAETitleColumn,
                                            new QSelectionColorStyledItemDelegate());
  this->NodeTable->setItemDelegateForColumn(ctkDICOMServerNodeWidget2::AddressColumn,
                                            new QSelectionColorStyledItemDelegate());

  QIntValidator* validator = new QIntValidator(0, INT_MAX);
  this->StoragePort->setValidator(validator);

  q->readSettings();

  QObject::connect(this->StorageEnabledCheckBox, SIGNAL(stateChanged(int)),
                   q, SLOT(onSettingsModified()));
  QObject::connect(this->StorageAETitle, SIGNAL(textChanged(QString)),
                   q, SLOT(onSettingsModified()));
  QObject::connect(this->StoragePort, SIGNAL(textChanged(QString)),
                   q, SLOT(onSettingsModified()));

  QObject::connect(this->NodeTable, SIGNAL(cellChanged(int,int)),
                   q, SLOT(onCellSettingsModified(int,int)));
  QObject::connect(this->NodeTable, SIGNAL(itemSelectionChanged()),
                   q, SLOT(onItemSelectionChanged()));

  QObject::connect(this->AddButton, SIGNAL(clicked()),
                   q, SLOT(onAddServerNode()));
  QObject::connect(this->VerifyButton, SIGNAL(clicked()),
                   q, SLOT(onVerifyCurrentServerNode()));
  QObject::connect(this->RemoveButton, SIGNAL(clicked()),
                   q, SLOT(onRemoveCurrentServerNode()));
  QObject::connect(this->RestoreDefaultPushButton, SIGNAL(clicked()),
                   q, SLOT(onRestoreDefaultServers()));
  this->SaveButton = this->ActionsButtonBox->button(QDialogButtonBox::StandardButton::Save);
  this->SaveButton->setText(ctkDICOMServerNodeWidget2::tr("Apply changes"));
  this->SaveButton->setIcon(QIcon(":/Icons/save.svg"));
  this->CancelButton = this->ActionsButtonBox->button(QDialogButtonBox::StandardButton::Discard);
  this->CancelButton->setText(ctkDICOMServerNodeWidget2::tr("Discard changes"));
  this->CancelButton->setIcon(QIcon(":/Icons/cancel.svg"));
  QObject::connect(this->CancelButton, SIGNAL(clicked()),
                   q, SLOT(readSettings()));
  QObject::connect(this->SaveButton, SIGNAL(clicked()),
                   q, SLOT(saveSettings()));

  this->RestoreDefaultPushButton->hide();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2Private::disconnectScheduler()
{
  Q_Q(ctkDICOMServerNodeWidget2);
  if (!this->Scheduler)
  {
    return;
  }

  ctkDICOMServerNodeWidget2::disconnect(this->Scheduler.data(), SIGNAL(jobStarted(QVariant)),
                                        q, SLOT(onJobStarted(QVariant)));
  ctkDICOMServerNodeWidget2::disconnect(this->Scheduler.data(), SIGNAL(jobCanceled(QVariant)),
                                        q, SLOT(onJobCanceled(QVariant)));
  ctkDICOMServerNodeWidget2::disconnect(this->Scheduler.data(), SIGNAL(jobFinished(QVariant)),
                                        q, SLOT(onJobFinished(QVariant)));
  ctkDICOMServerNodeWidget2::disconnect(this->Scheduler.data(), SIGNAL(jobFailed(QVariant)),
                                        q, SLOT(onJobFailed(QVariant)));
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2Private::connectScheduler()
{
  Q_Q(ctkDICOMServerNodeWidget2);
  if (!this->Scheduler)
  {
    return;
  }

  ctkDICOMServerNodeWidget2::connect(this->Scheduler.data(), SIGNAL(jobStarted(QVariant)),
                                     q, SLOT(onJobStarted(QVariant)));
  ctkDICOMServerNodeWidget2::connect(this->Scheduler.data(), SIGNAL(jobCanceled(QVariant)),
                                     q, SLOT(onJobCanceled(QVariant)));
  ctkDICOMServerNodeWidget2::connect(this->Scheduler.data(), SIGNAL(jobFinished(QVariant)),
                                     q, SLOT(onJobFinished(QVariant)));
  ctkDICOMServerNodeWidget2::connect(this->Scheduler.data(), SIGNAL(jobFailed(QVariant)),
                                     q, SLOT(onJobFailed(QVariant)));
}

//----------------------------------------------------------------------------
QMap<QString, QVariant> ctkDICOMServerNodeWidget2Private::parameters() const
{
  Q_Q(const ctkDICOMServerNodeWidget2);
  QMap<QString, QVariant> parameters;

  parameters["StorageAETitle"] = this->StorageAETitle->text();
  parameters["StoragePort"] = q->storagePort();

  return parameters;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMServerNodeWidget2Private::serverNodes() const
{
  QStringList nodes;
  int count = this->NodeTable->rowCount();
  for (int row = 0; row < count; ++row)
  {
    QTableWidgetItem* item = this->NodeTable->item(row, ctkDICOMServerNodeWidget2::NameColumn);
    nodes << (item ? item->text() : QString(""));
  }
  // If there are duplicates, serverNodeParameters(QString) will behave
  // strangely
  Q_ASSERT(nodes.removeDuplicates() == 0);
  return nodes;
}

//----------------------------------------------------------------------------
QMap<QString, QVariant> ctkDICOMServerNodeWidget2Private::serverNodeParameters(const QString& connectionName) const
{
  QMap<QString, QVariant> parameters;
  int count = this->NodeTable->rowCount();
  for (int row = 0; row < count; ++row)
  {
    if (this->NodeTable->item(row, 0)->text() == connectionName)
    {
      return this->serverNodeParameters(row);
    }
  }

  return parameters;
}

//----------------------------------------------------------------------------
QMap<QString, QVariant> ctkDICOMServerNodeWidget2Private::serverNodeParameters(int row) const
{
  QMap<QString, QVariant> node;
  if (row < 0 || row >= this->NodeTable->rowCount())
  {
    return node;
  }
  int columnCount = this->NodeTable->columnCount();
  for (int column = 0; column < columnCount; ++column)
  {
    if (!this->NodeTable->item(row, column))
    {
      continue;
    }
    QString label = this->NodeTable->horizontalHeaderItem(column)->text();
    node[label] = this->NodeTable->item(row, column)->data(Qt::DisplayRole);
  }

  node["QueryRetrieveCheckState"] = this->NodeTable->item(row, ctkDICOMServerNodeWidget2::QueryRetrieveColumn) ?
    this->NodeTable->item(row, ctkDICOMServerNodeWidget2::QueryRetrieveColumn)->checkState() :
    static_cast<int>(Qt::Unchecked);
  node["StorageCheckState"] = this->NodeTable->item(row, ctkDICOMServerNodeWidget2::StorageColumn) ?
    this->NodeTable->item(row, ctkDICOMServerNodeWidget2::StorageColumn)->checkState() :
    static_cast<int>(Qt::Unchecked);
  node["TrustedCheckState"] = this->NodeTable->item(row, ctkDICOMServerNodeWidget2::TrustedColumn) ?
    this->NodeTable->item(row, ctkDICOMServerNodeWidget2::TrustedColumn)->checkState() :
    static_cast<int>(Qt::Unchecked);

  QLineEdit* portLineEdit = qobject_cast<QLineEdit*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::PortColumn));
  if (portLineEdit)
  {
    node["Port"] = portLineEdit->text();
  }
  QSpinBox* timeoutSpinBox = qobject_cast<QSpinBox*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::TimeoutColumn));
  if (timeoutSpinBox)
  {
    node["Timeout"] = timeoutSpinBox->value();
  }
  QComboBox* protocolComboBox = qobject_cast<QComboBox*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::ProtocolColumn));
  if (protocolComboBox)
  {
    node["Retrieve Protocol"] = protocolComboBox->currentText();
  }
  QComboBox* proxyComboBox = qobject_cast<QComboBox*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::ProxyColumn));
  if (proxyComboBox)
  {
    node["Retrieve Proxy"] = proxyComboBox->currentText();
  }

  return node;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMServerNodeWidget2Private::getAllNodesName() const
{
  QStringList nodesNames;
  int count = this->NodeTable->rowCount();
  for (int row = 0; row < count; ++row)
  {
    nodesNames.append(this->NodeTable->item(row, ctkDICOMServerNodeWidget2::NameColumn)->data(Qt::DisplayRole).toString());
  }

  return nodesNames;
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget2Private::getServerNodeRowFromConnectionName(const QString& connectionName) const
{
  QMap<QString, QVariant> parameters;
  int count = this->NodeTable->rowCount();
  for (int row = 0; row < count; ++row)
  {
    if (this->NodeTable->item(row, 0)->text() == connectionName)
    {
      return row;
    }
  }

  return -1;
}

//----------------------------------------------------------------------------
QString ctkDICOMServerNodeWidget2Private::getServerNodeConnectionNameFromRow(int row) const
{
  if (row < 0 || row >= this->NodeTable->rowCount())
  {
    return "";
  }

  return this->NodeTable->item(row, 0)->text();
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget2Private::addServerNode(const QMap<QString, QVariant>& node)
{
  Q_Q(ctkDICOMServerNodeWidget2);

  if (this->getServerNodeRowFromConnectionName(node["Name"].toString()) != -1)
  {
    logger.warn("addServerNode failed: the server has a duplicate. The connection name has to be unique \n");
    return -1;
  }

  int rowCount = this->NodeTable->rowCount();
  this->NodeTable->setRowCount(rowCount + 1);

  QTableWidgetItem* newItem;

  QString serverName = node["Name"].toString();
  newItem = new QTableWidgetItem(serverName);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::NameColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QLineEdit* verificationLineEdit = new QLineEdit();
  verificationLineEdit->setObjectName("verificationLineEdit");
  verificationLineEdit->setText(ctkDICOMServerNodeWidget2::tr("unknown"));
  verificationLineEdit->setReadOnly(true);
  verificationLineEdit->setAlignment(Qt::AlignLeft);
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::VerificationColumn, verificationLineEdit);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::VerificationColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::CheckState(node["QueryRetrieveCheckState"].toInt()));
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::QueryRetrieveColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::CheckState(node["StorageCheckState"].toInt()));
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::StorageColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::CheckState(node["TrustedCheckState"].toInt()));
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::TrustedColumn, newItem);

  newItem = new QTableWidgetItem(node["Calling AETitle"].toString());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CallingAETitleColumn, newItem);

  newItem = new QTableWidgetItem(node["Called AETitle"].toString());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CalledAETitleColumn, newItem);

  newItem = new QTableWidgetItem(node["Address"].toString());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::AddressColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QLineEdit* portLineEdit = new QLineEdit();
  QIntValidator* validator = new QIntValidator(0, INT_MAX);
  portLineEdit->setValidator(validator);

  portLineEdit->setObjectName("portLineEdit");
  portLineEdit->setText(node["Port"].toString());
  portLineEdit->setAlignment(Qt::AlignLeft);
  QObject::connect(portLineEdit, SIGNAL(textChanged(QString)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::PortColumn, portLineEdit);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::PortColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox* protocolComboBox = new QComboBox();
  protocolComboBox->setObjectName("protocolComboBox");
  protocolComboBox->addItem("CGET");
  protocolComboBox->addItem("CMOVE");
  // To Do: protocolComboBox->addItem("WADO");
  protocolComboBox->setCurrentIndex(protocolComboBox->findText(node["Retrieve Protocol"].toString()));
  QObject::connect(protocolComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, newItem);
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, protocolComboBox);

  newItem = new QTableWidgetItem(QString(""));
  QSpinBox* timeoutSpinBox = new QSpinBox();
  timeoutSpinBox->setObjectName("timeoutSpinBox");
  timeoutSpinBox->setValue(node["Timeout"].toInt());
  timeoutSpinBox->setMinimum(1);
  timeoutSpinBox->setMaximum(INT_MAX);
  timeoutSpinBox->setSingleStep(1);
  timeoutSpinBox->setSuffix(" s");
  timeoutSpinBox->setAlignment(Qt::AlignLeft);
  QObject::connect(timeoutSpinBox, SIGNAL(valueChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, timeoutSpinBox);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox* proxyComboBox = new QComboBox();
  proxyComboBox->setObjectName("proxyComboBox");
  QStringListModel* cbModel = new QStringListModel();
  proxyComboBox->setModel(cbModel);

  proxyComboBox->addItem("");
  QStringList nodesNames = this->getAllNodesName();
  nodesNames.removeOne(serverName);
  QString proxyName = node["Retrieve Proxy"].toString();
  if (!nodesNames.contains(proxyName) && !proxyName.isEmpty())
  {
    nodesNames.append(proxyName);
  }
  proxyComboBox->addItems(nodesNames);
  proxyComboBox->setCurrentIndex(proxyComboBox->findText(node["Retrieve Proxy"].toString()));
  QObject::connect(proxyComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::ProxyColumn, proxyComboBox);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::ProxyColumn, newItem);

  this->settingsModified();

  return rowCount;
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget2Private::addServerNode(ctkDICOMServer* server)
{
  Q_Q(ctkDICOMServerNodeWidget2);

  if (!server)
  {
    return -1;
  }

  if (this->getServerNodeRowFromConnectionName(server->connectionName()) != -1)
  {
    logger.debug("addServerNode failed: the server has a duplicate. The connection name has to be unique \n");
    return -1;
  }

  if (server->proxyServer())
    {
    this->addServerNode(server->proxyServer());
    }

  int rowCount = this->NodeTable->rowCount();
  this->NodeTable->setRowCount(rowCount + 1);

  QTableWidgetItem* newItem;
  newItem = new QTableWidgetItem(server->connectionName());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::NameColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QLineEdit* verificationLineEdit = new QLineEdit();
  verificationLineEdit->setObjectName("verificationLineEdit");
  verificationLineEdit->setText(ctkDICOMServerNodeWidget2::tr("unknown"));
  verificationLineEdit->setReadOnly(true);
  verificationLineEdit->setAlignment(Qt::AlignLeft);
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::VerificationColumn, verificationLineEdit);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::VerificationColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(server->queryRetrieveEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::QueryRetrieveColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(server->storageEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::StorageColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(server->storageEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::TrustedColumn, newItem);

  newItem = new QTableWidgetItem(server->callingAETitle());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CallingAETitleColumn, newItem);

  newItem = new QTableWidgetItem(server->calledAETitle());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CalledAETitleColumn, newItem);

  newItem = new QTableWidgetItem(server->host());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::AddressColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QLineEdit* portLineEdit = new QLineEdit();
  QIntValidator* validator = new QIntValidator(0, INT_MAX);
  portLineEdit->setValidator(validator);

  portLineEdit->setObjectName("portLineEdit");
  portLineEdit->setText(QString::number(server->port()));
  portLineEdit->setAlignment(Qt::AlignLeft);
  QObject::connect(portLineEdit, SIGNAL(textChanged(QString)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::PortColumn, portLineEdit);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::PortColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox* protocolComboBox = new QComboBox();
  protocolComboBox->addItem("CGET");
  protocolComboBox->addItem("CMOVE");
  protocolComboBox->setCurrentIndex(protocolComboBox->findText(server->retrieveProtocolAsString()));
  // To Do: protocolComboBox->addItem("WADO");
  QObject::connect(protocolComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, newItem);
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, protocolComboBox);

  newItem = new QTableWidgetItem(QString(""));
  QSpinBox* timeoutSpinBox = new QSpinBox();
  timeoutSpinBox->setObjectName("timeoutSpinBox");
  timeoutSpinBox->setValue(server->connectionTimeout());
  timeoutSpinBox->setMinimum(1);
  timeoutSpinBox->setMaximum(INT_MAX);
  timeoutSpinBox->setSingleStep(1);
  timeoutSpinBox->setSuffix(" s");
  timeoutSpinBox->setAlignment(Qt::AlignLeft);
  QObject::connect(timeoutSpinBox, SIGNAL(valueChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, timeoutSpinBox);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, newItem);


  newItem = new QTableWidgetItem(QString(""));
  QComboBox* proxyComboBox = new QComboBox();
  QStringListModel* cbModel = new QStringListModel();
  proxyComboBox->setModel(cbModel);

  proxyComboBox->addItem("");
  QStringList nodesNames = this->getAllNodesName();
  nodesNames.removeOne(server->connectionName());

  if (server->proxyServer())
  {
    QString proxyName = server->proxyServer()->connectionName();
    if (!nodesNames.contains(proxyName))
    {
      nodesNames.append(proxyName);
    }
  }
  proxyComboBox->addItems(nodesNames);
  if (server->proxyServer())
  {
    QString proxyName = server->proxyServer()->connectionName();
    proxyComboBox->setCurrentIndex(proxyComboBox->findText(proxyName));
  }

  QObject::connect(proxyComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::ProxyColumn, proxyComboBox);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::ProxyColumn, newItem);

  this->settingsModified();

  return rowCount;
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMServer> ctkDICOMServerNodeWidget2Private::createServerFromServerNode(const QMap<QString, QVariant>& node)
{
  QSharedPointer<ctkDICOMServer> server =
    QSharedPointer<ctkDICOMServer>(new ctkDICOMServer);
  server->setConnectionName(node["Name"].toString());
  server->setQueryRetrieveEnabled(node["QueryRetrieveCheckState"].toInt() == 0 ? false : true);
  server->setStorageEnabled(node["StorageCheckState"].toInt() == 0 ? false : true);
  server->setTrustedEnabled(node["TrustedCheckState"].toInt() == 0 ? false : true);
  server->setCallingAETitle(node["Calling AETitle"].toString());
  server->setCalledAETitle(node["Called AETitle"].toString());
  server->setHost(node["Address"].toString());
  server->setPort(node["Port"].toInt());
  server->setRetrieveProtocolAsString(node["Retrieve Protocol"].toString());
  server->setConnectionTimeout(node["Timeout"].toInt());
  server->setMoveDestinationAETitle(this->StorageAETitle->text());

  return server;
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2Private::updateProxyComboBoxes() const
{
  int rowCount = this->NodeTable->rowCount();
  QStringList serverNames = this->getAllNodesName();
  for (int row = 0; row < rowCount; ++row)
  {
    QComboBox* proxyComboBox = qobject_cast<QComboBox*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::ProxyColumn));
    if (proxyComboBox)
    {
      bool wasBlocking = proxyComboBox->blockSignals(true);
      QStringList rowServerNames(serverNames);
      QString serverName = this->NodeTable->item(row, ctkDICOMServerNodeWidget2::NameColumn)->data(Qt::DisplayRole).toString();
      rowServerNames.removeOne(serverName);
      QString currentServer = proxyComboBox->currentText();
      proxyComboBox->clear();
      proxyComboBox->addItem("");
      proxyComboBox->addItems(rowServerNames);
      proxyComboBox->setCurrentText(currentServer);
      proxyComboBox->blockSignals(wasBlocking);
    }
  }
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2Private::updateServerVerification(const ctkDICOMJobDetail& td,
                                                                const QString &status)
{
  if (td.JobClass.isEmpty())
  {
    return;
  }

  if (td.JobClass != "ctkDICOMEchoJob")
  {
    return;
  }

  int row = this->getServerNodeRowFromConnectionName(td.ConnectionName);
  if (row == -1)
  {
    return;
  }

  QMap<QString, QVariant> node = this->serverNodeParameters(row);
  QString verificationQDateTimeString = node["Verification"].toString();
  QString verificationJobCreationQDateTimeString = td.CreationDateTime;
  QDateTime verificationQDateTime =
    QDateTime::fromString(verificationQDateTimeString, "HH:mm:ss.zzz ddd dd MMM yyyy");
  QDateTime verificationJobCreationQDateTime =
    QDateTime::fromString(verificationJobCreationQDateTimeString, "HH:mm:ss.zzz ddd dd MMM yyyy");;

  if (verificationJobCreationQDateTime < verificationQDateTime)
  {
    return;
  }

  QLineEdit* lineEdit = qobject_cast<QLineEdit*>
    (this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::VerificationColumn));
  if (!lineEdit)
  {
    return;
  }

  lineEdit->setReadOnly(false);
  lineEdit->setText(status);
  lineEdit->setReadOnly(true);

  QPalette palette;
  if (status == ctkDICOMServerNodeWidget2::tr("success"))
  {
    palette = this->ServerSuccesPalette;
  }
  else if (status == ctkDICOMServerNodeWidget2::tr("in progress"))
  {
    palette = this->ServerProgresPalette;
  }
  else
  {
    palette = this->ServerFailedPalette;
  }

  this->setDynamicPaletteColorToWidget(&palette, lineEdit);
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2Private::settingsModified()
{
  Q_Q(ctkDICOMServerNodeWidget2);

  this->SettingsModified = true;
  q->updateGUIState();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2Private::restoreFocus(QModelIndexList selectedIndexes,
                                                    int horizontalScrollBarValue,
                                                    int verticalScrollBarValue,
                                                    bool resetServerStatus)
{
  Q_Q(ctkDICOMServerNodeWidget2);

  bool wasBlocked = this->NodeTable->blockSignals(true);
  int rowCount = this->NodeTable->rowCount();
  int columnCount = this->NodeTable->columnCount();

  for (int row = 0; row < rowCount; ++row)
  {
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::VerificationColumn));
    if (!lineEdit)
    {
      continue;
    }

    if (resetServerStatus)
    {
      lineEdit->setReadOnly(false);
      lineEdit->setText(ctkDICOMServerNodeWidget2::tr("unknown"));
      lineEdit->setReadOnly(true);
    }

    lineEdit->deselect();
    lineEdit->clearFocus();
  }

  QColor serverNodeWidgetColor = q->palette().color(QPalette::Normal, q->backgroundRole());
  if (serverNodeWidgetColor.lightnessF() < 0.5)
  {
    this->DefaultPalette.setColor(QPalette::Button, ctkDICOMServerNodeWidget2DarkModeDefaultColor);
    this->DefaultPalette.setColor(QPalette::Base, ctkDICOMServerNodeWidget2DarkModeDefaultColor);
  }
  else
  {
    this->DefaultPalette.setColor(QPalette::Button, ctkDICOMServerNodeWidget2DefaultColor);
    this->DefaultPalette.setColor(QPalette::Base, ctkDICOMServerNodeWidget2DefaultColor);
  }

  for (int row = 0; row < rowCount; ++row)
  {
    for (int column = 0; column < columnCount; ++column)
    {
      QTableWidgetItem *item = this->NodeTable->item(row, column);
      if (item)
      {
        if (serverNodeWidgetColor.lightnessF() < 0.5)
        {
          item->setBackground(ctkDICOMServerNodeWidget2DarkModeDefaultColor);
        }
        else
        {
          item->setBackground(ctkDICOMServerNodeWidget2DefaultColor);
        }
      }

      if (column == ctkDICOMServerNodeWidget2::VerificationColumn && !resetServerStatus)
      {
        continue;
      }

      QWidget *widget = this->NodeTable->cellWidget(row, column);
      this->setDynamicPaletteColorToWidget(&this->DefaultPalette, widget);
    }
  }

  this->setDynamicPaletteColorToWidget(&this->DefaultPalette, this->StorageEnabledCheckBox);
  this->setDynamicPaletteColorToWidget(&this->DefaultPalette, this->StorageAETitle);
  this->setDynamicPaletteColorToWidget(&this->DefaultPalette, this->StoragePort);

  this->NodeTable->horizontalScrollBar()->setValue(horizontalScrollBarValue);
  this->NodeTable->verticalScrollBar()->setValue(verticalScrollBarValue);

  this->NodeTable->blockSignals(wasBlocked);
  if (selectedIndexes.count() == 0)
  {
    return;
  }

  QModelIndex index = selectedIndexes.at(0);
  this->NodeTable->selectRow(index.row());
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2Private::setDynamicPaletteColorToWidget(QPalette *palette, QWidget *widget)
{
  if (!palette || !widget)
  {
    return;
  }

  QPalette widgetPalette = widget->palette();
  widgetPalette.setColor(QPalette::Base, palette->color(QPalette::Base));
  widgetPalette.setColor(QPalette::Button, palette->color(QPalette::Button));
  widget->setPalette(widgetPalette);
}

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget2::ctkDICOMServerNodeWidget2(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkDICOMServerNodeWidget2Private(*this))
{
  Q_D(ctkDICOMServerNodeWidget2);

  d->init();
}


//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget2::~ctkDICOMServerNodeWidget2()
{
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget2::onAddServerNode()
{
  Q_D(ctkDICOMServerNodeWidget2);
  int rowCount = d->NodeTable->rowCount();
  d->NodeTable->setRowCount(rowCount + 1);

  QString serverName = "server";
  QTableWidgetItem* newItem = new QTableWidgetItem(serverName);
  d->NodeTable->setItem(rowCount, NameColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QLineEdit* verificationLineEdit = new QLineEdit();
  verificationLineEdit->setObjectName("verificationLineEdit");
  verificationLineEdit->setText(tr("unknown"));
  verificationLineEdit->setReadOnly(true);
  verificationLineEdit->setAlignment(Qt::AlignLeft);
  d->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::VerificationColumn, verificationLineEdit);
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::VerificationColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::Unchecked);
  d->NodeTable->setItem(rowCount, QueryRetrieveColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::Unchecked);
  d->NodeTable->setItem(rowCount, StorageColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::Unchecked);
  d->NodeTable->setItem(rowCount, TrustedColumn, newItem);

  newItem = new QTableWidgetItem();
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CallingAETitleColumn, newItem);

  newItem = new QTableWidgetItem();
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CalledAETitleColumn, newItem);

  newItem = new QTableWidgetItem();
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::AddressColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QLineEdit* portLineEdit = new QLineEdit();
  QIntValidator* validator = new QIntValidator(0, INT_MAX);
  portLineEdit->setValidator(validator);

  portLineEdit->setObjectName("portLineEdit");
  portLineEdit->setText("80");
  portLineEdit->setAlignment(Qt::AlignLeft);
  QObject::connect(portLineEdit, SIGNAL(textChanged(QString)),
                   this, SLOT(onSettingsModified()));
  d->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::PortColumn, portLineEdit);
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::PortColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox* protocolComboBox = new QComboBox();
  protocolComboBox->setObjectName("protocolComboBox");
  protocolComboBox->addItem("CGET");
  protocolComboBox->addItem("CMOVE");
  // To Do: protocolComboBox->addItem("WADO");
  protocolComboBox->setCurrentIndex(protocolComboBox->findText("CGET"));
  QObject::connect(protocolComboBox, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(onSettingsModified()));
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, newItem);
  d->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, protocolComboBox);

  newItem = new QTableWidgetItem(QString(""));
  QSpinBox* timeoutSpinBox = new QSpinBox();
  timeoutSpinBox->setObjectName("timeoutSpinBox");
  timeoutSpinBox->setValue(10);
  timeoutSpinBox->setMinimum(1);
  timeoutSpinBox->setMaximum(INT_MAX);
  timeoutSpinBox->setSingleStep(1);
  timeoutSpinBox->setSuffix(" s");
  timeoutSpinBox->setAlignment(Qt::AlignLeft);
  QObject::connect(timeoutSpinBox, SIGNAL(valueChanged(int)),
                   this, SLOT(onSettingsModified()));
  d->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, timeoutSpinBox);
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox* proxyComboBox = new QComboBox();
  proxyComboBox->setObjectName("proxyComboBox");
  QStringListModel* cbModel = new QStringListModel();
  proxyComboBox->setModel(cbModel);

  proxyComboBox->addItem("");
  QStringList nodesNames = d->getAllNodesName();
  nodesNames.removeOne(serverName);
  proxyComboBox->addItems(nodesNames);
  proxyComboBox->setCurrentIndex(-1);
  QObject::connect(proxyComboBox, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(onSettingsModified()));
  d->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::ProxyColumn, proxyComboBox);
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::ProxyColumn, newItem);

  d->NodeTable->setCurrentCell(rowCount, NameColumn);

  d->settingsModified();

  return rowCount;
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onRemoveCurrentServerNode()
{
  Q_D(ctkDICOMServerNodeWidget2);

  QModelIndexList selectedRows = d->NodeTable->selectionModel()->selectedIndexes();
  if (selectedRows.count() == 0)
  {
    return;
  }

  QModelIndex index = selectedRows.at(0);
  int row = index.row();
  d->NodeTable->removeRow(row);
  d->NodeTable->clearSelection();
  d->settingsModified();
}


//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onRestoreDefaultServers()
{
  Q_D(ctkDICOMServerNodeWidget2);

  d->NodeTable->clearContents();
  d->NodeTable->setRowCount(0);
  QSettings settings;
  settings.remove("DICOM/ServerNodes");
  settings.setValue("DICOM/ServerNodeCount", 0);
  this->stopAllJobs();
  this->removeAllServers();
  this->readSettings();
  this->saveSettings();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onVerifyCurrentServerNode()
{
  Q_D(ctkDICOMServerNodeWidget2);

  QModelIndexList selection = d->NodeTable->selectionModel()->selectedIndexes();
  if (selection.count() == 0)
  {
    return;
  }

  QModelIndex index = selection.at(0);
  int row = index.row();
  QDateTime verificationQDateTime = QDateTime::currentDateTime();
  QString verificationQDateString = verificationQDateTime.toString("HH:mm:ss.zzz ddd dd MMM yyyy");
  bool wasBlocked = d->NodeTable->blockSignals(true);
  d->NodeTable->item(row, ctkDICOMServerNodeWidget2::VerificationColumn)->
    setData(Qt::DisplayRole, verificationQDateString);
  d->NodeTable->blockSignals(wasBlocked);
  QMap<QString, QVariant> node = d->serverNodeParameters(row);
  QSharedPointer<ctkDICOMServer> server = d->createServerFromServerNode(node);
  d->Scheduler->echo(*server.data());
}


//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onJobStarted(QVariant data)
{
  Q_D(ctkDICOMServerNodeWidget2);
  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  d->updateServerVerification(td, QString(tr("in progress")));
  this->updateGUIState();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onJobCanceled(QVariant data)
{
  Q_D(ctkDICOMServerNodeWidget2);
  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  d->updateServerVerification(td, QString(tr("canceled")));
  this->updateGUIState();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onJobFailed(QVariant data)
{
  Q_D(ctkDICOMServerNodeWidget2);
  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  d->updateServerVerification(td, QString(tr("failed")));
  this->updateGUIState();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onJobFinished(QVariant data)
{
  Q_D(ctkDICOMServerNodeWidget2);
  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  d->updateServerVerification(td, QString(tr("success")));
  this->updateGUIState();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::updateGUIState()
{
  Q_D(ctkDICOMServerNodeWidget2);
  QList<QTableWidgetItem*> selectedItems = d->NodeTable->selectedItems();
  d->RemoveButton->setEnabled(selectedItems.count() > 0);
  d->VerifyButton->setEnabled(selectedItems.count() > 0);

  if (d->CancelButton && d->SaveButton)
  {
    d->CancelButton->setEnabled(d->SettingsModified);
    d->SaveButton->setEnabled(d->SettingsModified);
  }

  if (d->Scheduler && d->Scheduler->isStorageListenerActive())
  {
    d->StorageStatusValueLabel->setText(ctkDICOMServerNodeWidget2::tr("Active"));
  }
  else
  {
    d->StorageStatusValueLabel->setText(ctkDICOMServerNodeWidget2::tr("Inactive"));
  }

  d->updateProxyComboBoxes();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onItemSelectionChanged()
{
  this->updateGUIState();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onSettingsModified()
{
  Q_D(ctkDICOMServerNodeWidget2);
  QObject *senderObj = sender();
  if (senderObj)
  {
    QWidget *widget = qobject_cast<QWidget*>(senderObj);
    d->setDynamicPaletteColorToWidget(&d->ModifiedPalette, widget);
  }

  d->settingsModified();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onCellSettingsModified(int row, int column)
{
  Q_D(ctkDICOMServerNodeWidget2);
  QTableWidgetItem *item = d->NodeTable->item(row, column);
  if (item)
  {
    item->setBackground(ctkDICOMServerNodeWidget2ModifiedColor);
  }

  QLineEdit* lineEdit = qobject_cast<QLineEdit*>(d->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::VerificationColumn));
  if (lineEdit)
  {
    lineEdit->setReadOnly(false);
    lineEdit->setText(ctkDICOMServerNodeWidget2::tr("unknown"));
    lineEdit->setReadOnly(true);

    QColor serverNodeWidgetColor = this->palette().color(QPalette::Normal, this->backgroundRole());
    if (serverNodeWidgetColor.lightnessF() < 0.5)
    {
      d->DefaultPalette.setColor(QPalette::Button, ctkDICOMServerNodeWidget2DarkModeDefaultColor);
      d->DefaultPalette.setColor(QPalette::Base, ctkDICOMServerNodeWidget2DarkModeDefaultColor);
    }
    else
    {
      d->DefaultPalette.setColor(QPalette::Button, ctkDICOMServerNodeWidget2DefaultColor);
      d->DefaultPalette.setColor(QPalette::Base, ctkDICOMServerNodeWidget2DefaultColor);
    }

    d->setDynamicPaletteColorToWidget(&d->DefaultPalette, lineEdit);
  }

  d->SettingsModified = true;
  this->updateGUIState();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::saveSettings()
{
  Q_D(ctkDICOMServerNodeWidget2);

  if (!d->Scheduler)
  {
    return;
  }

  QSettings settings;
  QModelIndexList selectedIndexes = d->NodeTable->selectionModel()->selectedIndexes();
  int horizontalScrollBarValue = d->NodeTable->horizontalScrollBar()->value();
  int verticalScrollBarValue = d->NodeTable->verticalScrollBar()->value();
  int rowCount = d->NodeTable->rowCount();

  settings.remove("DICOM/ServerNodes");
  this->stopAllJobs();
  this->removeAllServers();

  settings.setValue("DICOM/ServerNodeCount", rowCount);

  QStringList proxyServers;
  for (int row = 0; row < rowCount; ++row)
  {
    QMap<QString, QVariant> node = d->serverNodeParameters(row);
    QString proxyName = node["Retrieve Proxy"].toString();
    if (!proxyName.isEmpty() && node["QueryRetrieveCheckState"].toInt() > 0)
    {
      proxyServers.append(proxyName);
    }

    // Convert QMap to QJsonObject
    QJsonObject jsonObject;
    for (auto it = node.constBegin(); it != node.constEnd(); ++it)
    {
      jsonObject[it.key()] = QJsonValue::fromVariant(it.value());
    }

    // Convert QJsonObject to QJsonDocument
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonData = jsonDocument.toJson(QJsonDocument::Indented);

    settings.setValue(QString("DICOM/ServerNodes/%1").arg(row), jsonData);
  }

  for (int row = 0; row < rowCount; ++row)
  {
    QMap<QString, QVariant> node = d->serverNodeParameters(row);
    QString serverName = node["Name"].toString();
    if (proxyServers.contains(serverName))
    {
      continue;
    }

    QSharedPointer<ctkDICOMServer> server = d->createServerFromServerNode(node);
    d->Scheduler->addServer(server);
  }

  for (int ii = 0; ii < rowCount; ++ii)
  {
    QMap<QString, QVariant> node = d->serverNodeParameters(ii);
    QString serverName = node["Name"].toString();
    if (!proxyServers.contains(serverName))
    {
      continue;
    }

    QSharedPointer<ctkDICOMServer> proxyServer = d->createServerFromServerNode(node);
    for (int jj = 0; jj < rowCount; ++jj)
    {
      QMap<QString, QVariant> tmpNode = d->serverNodeParameters(jj);
      QString tmpServerName = tmpNode["Name"].toString();
      if (serverName == tmpServerName)
      {
        continue;
      }
      QString tmpProxyName = tmpNode["Retrieve Proxy"].toString();
      if (serverName == tmpProxyName)
      {
        ctkDICOMServer* server = this->getServer(tmpServerName.toStdString().c_str());
        if (server)
        {
          server->setProxyServer(*proxyServer);
          server->setMoveDestinationAETitle(proxyServer->calledAETitle());
          break;
        }
      }
    }
  }

  settings.setValue("DICOM/StorageEnabled", this->storageListenerEnabled());
  settings.setValue("DICOM/StorageAETitle", this->storageAETitle());
  settings.setValue("DICOM/StoragePort", this->storagePort());
  settings.sync();

  if (d->StorageEnabledCheckBox->isChecked() && !d->Scheduler->isStorageListenerActive())
  {
    d->Scheduler->startListener(this->storagePort(),
                                this->storageAETitle(),
                                QThread::Priority::NormalPriority);
  }

  d->SettingsModified = false;
  this->updateGUIState();
  d->restoreFocus(selectedIndexes, horizontalScrollBarValue, verticalScrollBarValue, false);

  emit serversSettingsChanged();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::readSettings()
{
  Q_D(ctkDICOMServerNodeWidget2);

  QModelIndexList selectedIndexes = d->NodeTable->selectionModel()->selectedIndexes();
  int horizontalScrollBarValue = d->NodeTable->horizontalScrollBar()->value();
  int verticalScrollBarValue = d->NodeTable->verticalScrollBar()->value();
  d->NodeTable->setRowCount(0);

  QSettings settings;

  QMap<QString, QVariant> node;
  if (settings.status() == QSettings::AccessError ||
      settings.value("DICOM/ServerNodeCount").toInt() == 0)
  {
    d->StorageAETitle->setText("CTKSTORE");
    d->StoragePort->setText("11112");
    d->StorageEnabledCheckBox->setChecked(false);

    // a dummy example
    QMap<QString, QVariant> defaultServerNode;
    defaultServerNode["Name"] = QString("ExampleHost");
    defaultServerNode["Verification"] = tr("unknown");
    defaultServerNode["QueryRetrieveCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["StorageCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["TrustedCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["Calling AETitle"] = QString("CTK");
    defaultServerNode["Called AETitle"] = QString("AETITLE");
    defaultServerNode["Address"] = QString("dicom.example.com");
    defaultServerNode["Port"] = QString("11112");
    defaultServerNode["Retrieve Protocol"] = QString("CGET");
    defaultServerNode["Timeout"] = QString("30");
    defaultServerNode["Retrieve Proxy"] = QString("");
    d->addServerNode(defaultServerNode);

    // the uk example - see http://www.dicomserver.co.uk/
    // and http://www.medicalconnections.co.uk/
    defaultServerNode["Name"] = QString("MedicalConnections");
    defaultServerNode["Verification"] = tr("unknown");
    defaultServerNode["QueryRetrieveCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["StorageCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["TrustedCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["Calling AETitle"] = QString("CTK");
    defaultServerNode["Called AETitle"] = QString("ANYAE");
    defaultServerNode["Address"] = QString("dicomserver.co.uk");
    defaultServerNode["Port"] = QString("104");
    defaultServerNode["Retrieve Protocol"] = QString("CGET");
    defaultServerNode["Timeout"] = QString("30");
    defaultServerNode["Retrieve Proxy"] = QString("");
    d->addServerNode(defaultServerNode);

    d->SettingsModified = false;
    this->updateGUIState();
    d->NodeTable->selectionModel()->clearSelection();
    return;
  }

  d->StorageEnabledCheckBox->setChecked(settings.value("DICOM/StorageEnabled").toBool());
  d->StorageAETitle->setText(settings.value("DICOM/StorageAETitle").toString());
  d->StoragePort->setText(settings.value("DICOM/StoragePort").toString());

  int count = settings.value("DICOM/ServerNodeCount").toInt();
  for (int row = 0; row < count; ++row)
  {
    QString jsonDataString = settings.value(QString("DICOM/ServerNodes/%1").arg(row)).toString();

    // Convert JSON data back to QJsonDocument
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonDataString.toUtf8());

    // Convert QJsonDocument to QJsonObject
    QJsonObject jsonObject = jsonDocument.object();

    // Convert QJsonObject back to QMap<QString, QVariant>
    QMap<QString, QVariant> node;
    for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it)
    {
      node.insert(it.key(), it.value().toVariant());
    }

    d->addServerNode(node);
  }

  d->SettingsModified = false;
  this->updateGUIState();

  d->NodeTable->selectionModel()->clearSelection();
  d->restoreFocus(selectedIndexes, horizontalScrollBarValue, verticalScrollBarValue);
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::setStorageListenerEnabled(const bool enabled)
{
  Q_D(const ctkDICOMServerNodeWidget2);
  d->StorageEnabledCheckBox->setChecked(enabled);
  this->onSettingsModified();
}

//----------------------------------------------------------------------------
bool ctkDICOMServerNodeWidget2::storageListenerEnabled() const
{
  Q_D(const ctkDICOMServerNodeWidget2);
  return d->StorageEnabledCheckBox->isChecked();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::setStorageAETitle(const QString& storageAETitle)
{
  Q_D(const ctkDICOMServerNodeWidget2);
  d->StorageAETitle->setText(storageAETitle);
  this->onSettingsModified();
}

//----------------------------------------------------------------------------
QString ctkDICOMServerNodeWidget2::storageAETitle() const
{
  Q_D(const ctkDICOMServerNodeWidget2);
  return d->StorageAETitle->text();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::setStoragePort(int storagePort)
{
  Q_D(const ctkDICOMServerNodeWidget2);
  d->StoragePort->setText(QString::number(storagePort));
  this->onSettingsModified();
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget2::storagePort() const
{
  Q_D(const ctkDICOMServerNodeWidget2);
  bool ok = false;
  int port = d->StoragePort->text().toInt(&ok);
  Q_ASSERT(ok);
  return port;
}

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMServerNodeWidget2::scheduler() const
{
  Q_D(const ctkDICOMServerNodeWidget2);
  return d->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMServerNodeWidget2::schedulerShared() const
{
  Q_D(const ctkDICOMServerNodeWidget2);
  return d->Scheduler;
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::setScheduler(ctkDICOMScheduler& scheduler)
{
  Q_D(ctkDICOMServerNodeWidget2);
  d->disconnectScheduler();
  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);
  d->connectScheduler();
  this->saveSettings();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMServerNodeWidget2);
  d->disconnectScheduler();
  d->Scheduler = scheduler;
  d->connectScheduler();
  this->saveSettings();
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget2::serversCount()
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    logger.error("serversCount failed, no task pool has been set. \n");
    return -1;
  }

  return d->Scheduler->serversCount();
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMServerNodeWidget2::getNthServer(int id)
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    logger.error("getNthServer failed, no task pool has been set. \n");
    return nullptr;
  }

  return d->Scheduler->getNthServer(id);
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMServerNodeWidget2::getServer(const QString& connectionName)
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    logger.error("getServer failed, no task pool has been set. \n");
    return nullptr;
  }

  return d->Scheduler->getServer(connectionName);
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::addServer(ctkDICOMServer* server)
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    logger.error("addServer failed, no task pool has been set. \n");
    return;
  }

  d->addServerNode(server);
  this->saveSettings();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::removeServer(const QString& connectionName)
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    logger.error("removeServer failed, no task pool has been set. \n");
    return;
  }

  this->removeNthServer(this->getServerIndexFromName(connectionName));
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::removeNthServer(int id)
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    logger.error("removeNthServer failed, no task pool has been set. \n");
    return;
  }

  QString connectionName = this->getServerNameFromIndex(id);
  int row = d->getServerNodeRowFromConnectionName(connectionName);
  d->NodeTable->removeRow(row);
  this->saveSettings();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::removeAllServers()
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    logger.error("removeAllServers failed, no task pool has been set. \n");
    return;
  }

  d->Scheduler->removeAllServers();
}

//----------------------------------------------------------------------------
QString ctkDICOMServerNodeWidget2::getServerNameFromIndex(int id)
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    logger.error("getServerNameFromIndex failed, no task pool has been set. \n");
    return "";
  }

  return d->Scheduler->getServerNameFromIndex(id);
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget2::getServerIndexFromName(const QString& connectionName)
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    logger.error("getServerIndexFromName failed, no task pool has been set. \n");
    return -1;
  }

  return d->Scheduler->getServerIndexFromName(connectionName);
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::stopAllJobs()
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
  {
    return;
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  d->Scheduler->stopAllJobs(true);
  QApplication::restoreOverrideCursor();
}
