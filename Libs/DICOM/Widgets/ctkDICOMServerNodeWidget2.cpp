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
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QMouseEvent>
#include <QSettings>
#include <QSpinBox>
#include <QStyledItemDelegate>
#include <QTableWidgetItem>
#include <QVariant>

// ctkCore includes
#include <ctkCheckableHeaderView.h>
#include <ctkCheckableModelHelper.h>
#include <ctkLogger.h>
#include <ctkMessageBox.h>

// ctkDICOMCore includes
#include <ctkDICOMEcho.h>
#include <ctkDICOMScheduler.h>
#include <ctkDICOMServer.h>

// ctkDICOMWidgets includes
#include "ctkDICOMServerNodeWidget2.h"
#include "ui_ctkDICOMServerNodeWidget2.h"

static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMServerNodeWidget2");

class QCenteredStyledItemDelegate : public QStyledItemDelegate
{
public:
  using QStyledItemDelegate::QStyledItemDelegate;
  void paint(QPainter *painter,
             const QStyleOptionViewItem &option,
             const QModelIndex &index) const override
    {
    QStyleOptionViewItem opt = option;
    const QWidget *widget = option.widget;
    initStyleOption(&opt, index);
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
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
      QStyledItemDelegate::paint(painter, option, index);
      }
    }
protected:
  bool editorEvent(QEvent *event,
                   QAbstractItemModel *model,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) override
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
    const QWidget *widget = option.widget;
    QStyle *style = option.widget ? widget->style() : QApplication::style();
    // make sure that we have the right event type
    if ((event->type() == QEvent::MouseButtonRelease) || (event->type() == QEvent::MouseButtonDblClick) ||
        (event->type() == QEvent::MouseButtonPress))
      {
      QStyleOptionViewItem viewOpt(option);
      initStyleOption(&viewOpt, index);
      QRect checkRect = style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &viewOpt, widget);
      checkRect = QStyle::alignedRect(viewOpt.direction, Qt::AlignCenter, checkRect.size(), viewOpt.rect);
      QMouseEvent *me = static_cast<QMouseEvent *>(event);
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
      if (static_cast<QKeyEvent *>(event)->key() != Qt::Key_Space &&
          static_cast<QKeyEvent *>(event)->key() != Qt::Key_Select)
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
class ctkDICOMServerNodeWidget2Private: public Ui_ctkDICOMServerNodeWidget2
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
  QMap<QString,QVariant> parameters()const;

  /// Return the list of server names
  QStringList serverNodes()const;
  /// Return all the information associated to a server defined by its name
  QMap<QString,QVariant> serverNodeParameters(const QString &connectionName) const;
  QMap<QString,QVariant> serverNodeParameters(int row) const;
  QStringList getAllNodesName() const;
  int getServerNodeRowFromConnectionName(const QString &connectionName) const;
  QString getServerNodeConnectionNameFromRow(int row) const;

  /// Add a server node with the given parameters
  /// Return the row index added into the table
  int addServerNode(const QMap<QString, QVariant>& parameters);
  int addServerNode(ctkDICOMServer* server);
  QSharedPointer<ctkDICOMServer> createServerFromServerNode(const QMap<QString, QVariant>& node);
  void updateProxyComboBoxes(const QString &connectionName, int rowCount) const;

  bool SettingsModified;
  QSharedPointer<ctkDICOMScheduler> Scheduler;
  QPushButton *SaveButton;
  QPushButton *RestoreButton;
};

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget2Private::ctkDICOMServerNodeWidget2Private(ctkDICOMServerNodeWidget2& obj)
  : q_ptr(&obj)
{
  this->SettingsModified = false;
  this->Scheduler = nullptr;
  this->RestoreButton = nullptr;
  this->SaveButton = nullptr;
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

  this->TestButton->setEnabled(false);
  this->RemoveButton->setEnabled(false);

  this->NodeTable->setItemDelegateForColumn(ctkDICOMServerNodeWidget2::QueryRetrieveColumn,
                                            new QCenteredStyledItemDelegate());
  this->NodeTable->setItemDelegateForColumn(ctkDICOMServerNodeWidget2::StorageColumn,
                                            new QCenteredStyledItemDelegate());

  QIntValidator *validator = new QIntValidator(0, INT_MAX);
  this->StoragePort->setValidator(validator);

  q->readSettings();

  QObject::connect(this->StorageEnabledCheckBox, SIGNAL(stateChanged(int)),
                   q, SLOT(onSettingsModified()));
  QObject::connect(this->StorageAETitle, SIGNAL(textChanged(QString)),
                   q, SLOT(onSettingsModified()));
  QObject::connect(this->StoragePort, SIGNAL(textChanged(QString)),
                   q, SLOT(onSettingsModified()));

  QObject::connect(this->NodeTable, SIGNAL(cellChanged(int,int)),
                   q, SLOT(onSettingsModified()));
  QObject::connect(this->NodeTable, SIGNAL(itemSelectionChanged()),
                   q, SLOT(updateGUIState()));

  QObject::connect(this->AddButton, SIGNAL(clicked()),
                   q, SLOT(onAddServerNode()));
  QObject::connect(this->TestButton, SIGNAL(clicked()),
                   q, SLOT(onTestCurrentServerNode()));
  QObject::connect(this->RemoveButton, SIGNAL(clicked()),
                   q, SLOT(onRemoveCurrentServerNode()));
  this->SaveButton = this->ActionsButtonBox->button(QDialogButtonBox::StandardButton::Save);
  this->SaveButton->setText(QObject::tr("Apply changes"));
  this->SaveButton->setIcon(QIcon(":/Icons/save.svg"));
  this->RestoreButton = this->ActionsButtonBox->button(QDialogButtonBox::StandardButton::Discard);
  this->RestoreButton->setText(QObject::tr("Discard changes"));
  this->RestoreButton->setIcon(QIcon(":/Icons/cancel.svg"));
  QObject::connect(this->RestoreButton, SIGNAL(clicked()),
                   q, SLOT(readSettings()));
  QObject::connect(this->SaveButton, SIGNAL(clicked()),
                   q, SLOT(saveSettings()));
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
                                        q, SLOT(updateGUIState()));
  ctkDICOMServerNodeWidget2::disconnect(this->Scheduler.data(), SIGNAL(jobFinished(QVariant)),
                                        q, SLOT(updateGUIState()));
  ctkDICOMServerNodeWidget2::disconnect(this->Scheduler.data(), SIGNAL(jobFailed(QVariant)),
                                        q, SLOT(updateGUIState()));
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
                                     q, SLOT(updateGUIState()));
  ctkDICOMServerNodeWidget2::connect(this->Scheduler.data(), SIGNAL(jobFinished(QVariant)),
                                     q, SLOT(updateGUIState()));
  ctkDICOMServerNodeWidget2::connect(this->Scheduler.data(), SIGNAL(jobFailed(QVariant)),
                                     q, SLOT(updateGUIState()));
}

//----------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMServerNodeWidget2Private::parameters()const
{
  Q_Q(const ctkDICOMServerNodeWidget2);
  QMap<QString, QVariant> parameters;

  parameters["StorageAETitle"] = this->StorageAETitle->text();
  parameters["StoragePort"] = q->storagePort();

  return parameters;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMServerNodeWidget2Private::serverNodes()const
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
QMap<QString, QVariant> ctkDICOMServerNodeWidget2Private::serverNodeParameters(const QString &connectionName)const
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

  QLineEdit *portLineEdit = qobject_cast<QLineEdit*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::PortColumn));
  if (portLineEdit)
    {
    node["Port"] = portLineEdit->text();
    }
  QSpinBox *timeoutSpinBox = qobject_cast<QSpinBox*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::TimeoutColumn));
  if (timeoutSpinBox)
    {
    node["Timeout"] = timeoutSpinBox->value();
    }
  QComboBox *protocolComboBox = qobject_cast<QComboBox*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::ProtocolColumn));
  if (protocolComboBox)
    {
    node["Protocol"] = protocolComboBox->currentText();
    }
  QComboBox *proxyComboBox = qobject_cast<QComboBox*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::ProxyColumn));
  if (proxyComboBox)
    {
    node["Proxy"] = proxyComboBox->currentText();
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
int ctkDICOMServerNodeWidget2Private::getServerNodeRowFromConnectionName(const QString &connectionName) const
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
    logger.debug("addServerNode failed: the server has a duplicate. The connection name has to be unique \n");
    return -1;
    }

  int rowCount = this->NodeTable->rowCount();
  this->NodeTable->setRowCount(rowCount + 1);

  QTableWidgetItem *newItem;
  QString serverName = node["Name"].toString();
  newItem = new QTableWidgetItem(serverName);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::NameColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::CheckState(node["QueryRetrieveCheckState"].toInt()));
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::QueryRetrieveColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::CheckState(node["StorageCheckState"].toInt()));
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::StorageColumn, newItem);

  newItem = new QTableWidgetItem(node["Calling AETitle"].toString());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CallingAETitleColumn, newItem);

  newItem = new QTableWidgetItem(node["Called AETitle"].toString());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CalledAETitleColumn, newItem);

  newItem = new QTableWidgetItem(node["Address"].toString());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::AddressColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QLineEdit *portLineEdit = new QLineEdit();
  QIntValidator *validator = new QIntValidator(0, INT_MAX);
  portLineEdit->setValidator(validator);

  portLineEdit->setObjectName("portLineEdit");
  portLineEdit->setText(node["Port"].toString());
  portLineEdit->setAlignment(Qt::AlignHCenter);
  QObject::connect(portLineEdit, SIGNAL(textChanged(QString)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::PortColumn, portLineEdit);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::PortColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox *protocolComboBox = new QComboBox();
  protocolComboBox->setObjectName("protocolComboBox");
  protocolComboBox->addItem("CGET");
  protocolComboBox->addItem("CMOVE");
  // To Do: protocolComboBox->addItem("WADO");
  protocolComboBox->setCurrentIndex(protocolComboBox->findText(node["Protocol"].toString()));
  QObject::connect(protocolComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, newItem);
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, protocolComboBox);

  newItem = new QTableWidgetItem(QString(""));
  QSpinBox *timeoutSpinBox = new QSpinBox();
  timeoutSpinBox->setObjectName("timeoutSpinBox");
  timeoutSpinBox->setValue(node["Timeout"].toInt());
  timeoutSpinBox->setMinimum(1);
  timeoutSpinBox->setMaximum(INT_MAX);
  timeoutSpinBox->setSingleStep(1);
  timeoutSpinBox->setSuffix(" s");
  timeoutSpinBox->setAlignment(Qt::AlignHCenter);
  QObject::connect(timeoutSpinBox, SIGNAL(valueChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, timeoutSpinBox);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox *proxyComboBox = new QComboBox();
  proxyComboBox->setObjectName("proxyComboBox");
  QStringListModel* cbModel = new QStringListModel();
  proxyComboBox->setModel(cbModel);

  proxyComboBox->addItem("");
  QStringList nodesNames = this->getAllNodesName();
  nodesNames.removeOne(serverName);
  QString proxyName = node["Proxy"].toString();
  if (!nodesNames.contains(proxyName) && !proxyName.isEmpty())
    {
    nodesNames.append(proxyName);
    }
  proxyComboBox->addItems(nodesNames);
  proxyComboBox->setCurrentIndex(proxyComboBox->findText(node["Proxy"].toString()));
  QObject::connect(proxyComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::ProxyColumn, proxyComboBox);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::ProxyColumn, newItem);

  q->onSettingsModified();

  this->updateProxyComboBoxes(serverName, rowCount);

  return rowCount;
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget2Private::addServerNode(ctkDICOMServer *server)
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

  int rowCount = this->NodeTable->rowCount();
  this->NodeTable->setRowCount(rowCount + 1);

  QTableWidgetItem *newItem;
  newItem = new QTableWidgetItem(server->connectionName());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::NameColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(server->queryRetrieveEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::QueryRetrieveColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(server->storageEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::StorageColumn, newItem);

  newItem = new QTableWidgetItem(server->callingAETitle());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CallingAETitleColumn, newItem);

  newItem = new QTableWidgetItem(server->calledAETitle());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CalledAETitleColumn, newItem);

  newItem = new QTableWidgetItem(server->host());
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::AddressColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QLineEdit *portLineEdit = new QLineEdit();
  QIntValidator *validator = new QIntValidator(0, INT_MAX);
  portLineEdit->setValidator(validator);

  portLineEdit->setObjectName("portLineEdit");
  portLineEdit->setText(QString::number(server->port()));
  portLineEdit->setAlignment(Qt::AlignHCenter);
  QObject::connect(portLineEdit, SIGNAL(textChanged(QString)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::PortColumn, portLineEdit);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::PortColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox *protocolComboBox = new QComboBox();
  protocolComboBox->addItem("CGET");
  protocolComboBox->addItem("CMOVE");
  protocolComboBox->setCurrentIndex(protocolComboBox->findText(server->retrieveProtocolAsString()));
  // To Do: protocolComboBox->addItem("WADO");
  QObject::connect(protocolComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, newItem);
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::ProtocolColumn, protocolComboBox);

  newItem = new QTableWidgetItem(QString(""));
  QSpinBox *timeoutSpinBox = new QSpinBox();
  timeoutSpinBox->setObjectName("timeoutSpinBox");
  timeoutSpinBox->setValue(server->connectionTimeout());
  timeoutSpinBox->setMinimum(1);
  timeoutSpinBox->setMaximum(INT_MAX);
  timeoutSpinBox->setSingleStep(1);
  timeoutSpinBox->setSuffix(" s");
  timeoutSpinBox->setAlignment(Qt::AlignHCenter);
  QObject::connect(timeoutSpinBox, SIGNAL(valueChanged(int)),
                   q, SLOT(onSettingsModified()));
  this->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, timeoutSpinBox);
  this->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, newItem);


  newItem = new QTableWidgetItem(QString(""));
  QComboBox *proxyComboBox = new QComboBox();
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

  this->updateProxyComboBoxes(server->connectionName(), rowCount);

  if (server->proxyServer())
    {
    this->addServerNode(server->proxyServer());
    rowCount++;
    }

  q->onSettingsModified();

  return rowCount;
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMServer> ctkDICOMServerNodeWidget2Private::createServerFromServerNode(const QMap<QString, QVariant> &node)
{
  QSharedPointer<ctkDICOMServer> server =
    QSharedPointer<ctkDICOMServer>(new ctkDICOMServer);
  server->setConnectionName(node["Name"].toString());
  server->setQueryRetrieveEnabled(node["QueryRetrieveCheckState"].toInt() == 0 ? false : true);
  server->setStorageEnabled(node["StorageCheckState"].toInt() == 0 ? false : true);
  server->setCallingAETitle(node["Calling AETitle"].toString());
  server->setCalledAETitle(node["Called AETitle"].toString());
  server->setHost(node["Address"].toString());
  server->setPort(node["Port"].toInt());
  server->setRetrieveProtocolAsString(node["Protocol"].toString());
  server->setConnectionTimeout(node["Timeout"].toInt());
  server->setMoveDestinationAETitle(this->StorageAETitle->text());

  return server;
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2Private::updateProxyComboBoxes(const QString &connectionName, int rowCount) const
{
  for (int row = 0; row < rowCount; ++row)
    {
    QComboBox *proxyComboBox = qobject_cast<QComboBox*>(this->NodeTable->cellWidget(row, ctkDICOMServerNodeWidget2::ProxyColumn));
    if (proxyComboBox)
      {
      QStringListModel* cbModel = qobject_cast<QStringListModel*>(proxyComboBox->model());
      if (cbModel)
        {
        QStringList nodesNames = cbModel->stringList();
        if (nodesNames.contains(connectionName))
          {
          continue;
          }
        }
      proxyComboBox->addItem(connectionName);
      }
  }
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
  QTableWidgetItem *newItem = new QTableWidgetItem(serverName);
  d->NodeTable->setItem(rowCount, NameColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::Unchecked);
  d->NodeTable->setItem(rowCount, QueryRetrieveColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  newItem->setCheckState(Qt::Unchecked);
  d->NodeTable->setItem(rowCount, StorageColumn, newItem);

  newItem = new QTableWidgetItem();
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CallingAETitleColumn, newItem);

  newItem = new QTableWidgetItem();
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::CalledAETitleColumn, newItem);

  newItem = new QTableWidgetItem();
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::AddressColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QLineEdit *portLineEdit = new QLineEdit();
  QIntValidator *validator = new QIntValidator(0, INT_MAX);
  portLineEdit->setValidator(validator);

  portLineEdit->setObjectName("portLineEdit");
  portLineEdit->setText("80");
  portLineEdit->setAlignment(Qt::AlignHCenter);
  QObject::connect(portLineEdit, SIGNAL(textChanged(QString)),
                   this, SLOT(onSettingsModified()));
  d->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::PortColumn, portLineEdit);
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::PortColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox *protocolComboBox = new QComboBox();
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
  QSpinBox *timeoutSpinBox = new QSpinBox();
  timeoutSpinBox->setObjectName("timeoutSpinBox");
  timeoutSpinBox->setValue(10);
  timeoutSpinBox->setMinimum(1);
  timeoutSpinBox->setMaximum(INT_MAX);
  timeoutSpinBox->setSingleStep(1);
  timeoutSpinBox->setSuffix(" s");
  timeoutSpinBox->setAlignment(Qt::AlignHCenter);
  QObject::connect(timeoutSpinBox, SIGNAL(valueChanged(int)),
                   this, SLOT(onSettingsModified()));
  d->NodeTable->setCellWidget(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, timeoutSpinBox);
  d->NodeTable->setItem(rowCount, ctkDICOMServerNodeWidget2::TimeoutColumn, newItem);

  newItem = new QTableWidgetItem(QString(""));
  QComboBox *proxyComboBox = new QComboBox();
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

  this->onSettingsModified();

  return rowCount;
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onRemoveCurrentServerNode()
{
  Q_D(ctkDICOMServerNodeWidget2);

  QModelIndexList selection = d->NodeTable->selectionModel()->selectedRows();
  if (selection.count() == 0)
    {
    return;
    }

  QModelIndex index = selection.at(0);
  int row = index.row();
  d->NodeTable->removeRow(row);
  this->onSettingsModified();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onTestCurrentServerNode()
{
  Q_D(ctkDICOMServerNodeWidget2);

  QModelIndexList selection = d->NodeTable->selectionModel()->selectedRows();
  if (selection.count() == 0)
    {
    return;
    }

  QModelIndex index = selection.at(0);
  QString serverName = d->getServerNodeConnectionNameFromRow(index.row());
  ctkDICOMServer* server = this->getServer(serverName.toStdString().c_str());
  if (!server)
    {
    return;
    }

  ctkDICOMEcho echo;
  echo.setConnectionName(server->connectionName());
  echo.setCalledAETitle(server->calledAETitle());
  echo.setCallingAETitle(server->callingAETitle());
  echo.setHost(server->host());
  echo.setPort(server->port());
  echo.setConnectionTimeout(server->connectionTimeout());

  ctkMessageBox echoMessageBox(this);
  QString messageString;
  if (echo.echo())
    {
    messageString = tr("Node response was positive.");
    echoMessageBox.setIcon(QMessageBox::Information);
    }
  else
    {
    messageString = tr("Node response was negative.");
    echoMessageBox.setIcon(QMessageBox::Warning);
    }

  echoMessageBox.setText(messageString);
  echoMessageBox.exec();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::updateGUIState()
{
  Q_D(ctkDICOMServerNodeWidget2);
  QList<QTableWidgetItem*> selectedItems = d->NodeTable->selectedItems();
  d->RemoveButton->setEnabled(selectedItems.count() > 0);
  d->TestButton->setEnabled(selectedItems.count() > 0);

  if (d->RestoreButton && d->SaveButton)
    {
    d->RestoreButton->setEnabled(d->SettingsModified);
    d->SaveButton->setEnabled(d->SettingsModified);
    }

  if (d->Scheduler && d->Scheduler->isStorageListenerActive())
    {
    d->StorageStatusValueLabel->setText(QObject::tr("Active"));
    }
  else
    {
    d->StorageStatusValueLabel->setText(QObject::tr("Inactive"));
    }
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::onSettingsModified()
{
  Q_D(ctkDICOMServerNodeWidget2);
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
  int rowCount = d->NodeTable->rowCount();

  settings.remove("DICOM/ServerNodes");
  this->removeAllServers();
  this->stopAllJobs();

  settings.setValue("DICOM/ServerNodeCount", rowCount);

  QStringList proxyServers;
  for (int row = 0; row < rowCount; ++row)
    {
    QMap<QString, QVariant> node = d->serverNodeParameters(row);
    QString proxyName = node["Proxy"].toString();
    if (!proxyName.isEmpty() && node["QueryRetrieveCheckState"].toInt() > 0)
      {
      proxyServers.append(proxyName);
      }

    settings.setValue(QString("DICOM/ServerNodes/%1").arg(row), QVariant(node));
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
      QString tmpProxyName = tmpNode["Proxy"].toString();
      if (serverName == tmpProxyName)
        {
        ctkDICOMServer* server = this->getServer(tmpServerName.toStdString().c_str());
        if (server)
          {
          server->setProxyServer(proxyServer);
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

  d->SettingsModified = false;

  if (d->StorageEnabledCheckBox->isChecked() && !d->Scheduler->isStorageListenerActive())
    {
    d->Scheduler->startListener(this->storagePort(),
                                this->storageAETitle(),
                                QThread::Priority::NormalPriority);
    }

  this->updateGUIState();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget2::readSettings()
{
  Q_D(ctkDICOMServerNodeWidget2);

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
    defaultServerNode["QueryRetrieveCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["StorageCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["Calling AETitle"] = QString("CTK");
    defaultServerNode["Called AETitle"] = QString("AETITLE");
    defaultServerNode["Address"] = QString("dicom.example.com");
    defaultServerNode["Port"] = QString("11112");
    defaultServerNode["Protocol"] = QString("CGET");
    defaultServerNode["Timeout"] = QString("30");
    defaultServerNode["Proxy"] = QString("");
    d->addServerNode(defaultServerNode);

    // the uk example - see http://www.dicomserver.co.uk/
    // and http://www.medicalconnections.co.uk/
    defaultServerNode["Name"] = QString("MedicalConnections");
    defaultServerNode["QueryRetrieveCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["StorageCheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["Calling AETitle"] = QString("CTK");
    defaultServerNode["Called AETitle"] = QString("ANYAE");
    defaultServerNode["Address"] = QString("dicomserver.co.uk");
    defaultServerNode["Port"] = QString("104");
    defaultServerNode["Protocol"] = QString("CGET");
    defaultServerNode["Timeout"] = QString("30");
    defaultServerNode["Proxy"] = QString("");
    d->addServerNode(defaultServerNode);

    d->SettingsModified = false;
    d->NodeTable->clearSelection();
    this->updateGUIState();
    return;
    }

  d->StorageEnabledCheckBox->setChecked(settings.value("DICOM/StorageEnabled").toBool());
  d->StorageAETitle->setText(settings.value("DICOM/StorageAETitle").toString());
  d->StoragePort->setText(settings.value("DICOM/StoragePort").toString());

  int count = settings.value("DICOM/ServerNodeCount").toInt();
  for (int row = 0; row < count; ++row)
    {
    node = settings.value(QString("DICOM/ServerNodes/%1").arg(row)).toMap();
    d->addServerNode(node);
    }

  d->SettingsModified = false;
  d->NodeTable->clearSelection();
  this->updateGUIState();
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
QString ctkDICOMServerNodeWidget2::storageAETitle()const
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
int ctkDICOMServerNodeWidget2::storagePort()const
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
ctkDICOMScheduler* ctkDICOMServerNodeWidget2::scheduler()const
{
  Q_D(const ctkDICOMServerNodeWidget2);
  return d->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMServerNodeWidget2::schedulerShared()const
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
int ctkDICOMServerNodeWidget2::getNumberOfServers()
{
  Q_D(ctkDICOMServerNodeWidget2);
  if (!d->Scheduler)
    {
    logger.error("getNumberOfServers failed, no task pool has been set. \n");
    return -1;
    }

  return d->Scheduler->getNumberOfServers();
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
