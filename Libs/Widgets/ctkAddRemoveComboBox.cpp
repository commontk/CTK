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

// CTK includes
#include <ctkPimpl.h>
#include "ctkAddRemoveComboBox.h"
#include "ui_ctkAddRemoveComboBox.h"

//-----------------------------------------------------------------------------
class ctkAddRemoveComboBoxPrivate : public Ui_ctkAddRemoveComboBox
{
  Q_DECLARE_PUBLIC(ctkAddRemoveComboBox);
protected:
  ctkAddRemoveComboBox* const q_ptr;
public:
  ctkAddRemoveComboBoxPrivate(ctkAddRemoveComboBox& object);

  /// 
  /// Insert 'None' item
  /// Note: Also make sure that no signal is emitted while the item is inserted
  /// That function doesn't prevent from inserting multiple time the 'None' item
  void insertEmptyItem();

  void connectComboBox(QComboBox* combobox);

public:
  /// Empty item
  QString EmptyText;

  /// Set to true when inserting the 'None' item.
  /// Will prevent the itemAdded signal from being sent
  bool    AddingEmptyItem; 

  /// Set to true when removing the 'None' item.
  /// Will prevent the itemRemoved signal from being sent
  bool    RemovingEmptyItem;

  /// Actions state
  bool    AddEnabled;
  bool    RemoveEnabled;
  bool    EditEnabled;

  /// If true, it means there is no item beside of the 'None' one
  bool    HasEmptyItem;
};

// --------------------------------------------------------------------------
// ctkAddRemoveComboBoxPrivate methods

// --------------------------------------------------------------------------
ctkAddRemoveComboBoxPrivate::ctkAddRemoveComboBoxPrivate(ctkAddRemoveComboBox& object)
  :q_ptr(&object)
{
  this->EmptyText = "None";

  this->AddingEmptyItem = false;
  this->RemovingEmptyItem = false;

  this->AddEnabled = true;
  this->RemoveEnabled = true;
  this->EditEnabled = true;

  this->HasEmptyItem = false;
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBoxPrivate::insertEmptyItem()
{
  if (!this->HasEmptyItem )
    {
    this->AddingEmptyItem = true;
    this->ComboBox->insertItem(0, this->EmptyText);
    this->AddingEmptyItem = false;
    this->HasEmptyItem = true;
    }
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBoxPrivate::connectComboBox(QComboBox* comboBox)
{
  Q_Q(ctkAddRemoveComboBox);
  QObject::connect(comboBox, SIGNAL(activated(int)),
                q, SIGNAL(activated(int)));
  QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)),
                q, SIGNAL(currentIndexChanged(int)));
  /*
  this->connect(d->ComboBox->model(),
  SIGNAL(rowsAboutToBeInserted(QModelIndex&_parent,int start,int end)),
  SLOT(onRowsAboutToBeInserted(QModelIndex&_parent,int start,int end)));
  */
  QObject::connect(comboBox->model(),
                SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                q, SLOT(onRowsAboutToBeRemoved(QModelIndex,int,int)));

  QObject::connect(comboBox->model(),
                SIGNAL(rowsInserted(QModelIndex,int,int)),
                q, SLOT(onRowsInserted(QModelIndex,int,int)));
  QObject::connect(comboBox->model(),
                SIGNAL(rowsRemoved(QModelIndex,int,int)),
                q, SLOT(onRowsRemoved(QModelIndex,int,int)));
}

// --------------------------------------------------------------------------
// ctkAddRemoveComboBox methods

// --------------------------------------------------------------------------
ctkAddRemoveComboBox::ctkAddRemoveComboBox(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkAddRemoveComboBoxPrivate(*this))
{
  Q_D(ctkAddRemoveComboBox);
  d->setupUi(this);
  
  // connect
  d->connectComboBox(d->ComboBox);
    
  this->connect(d->AddPushButton, SIGNAL(pressed()), SLOT(onAdd()));
  this->connect(d->RemovePushButton, SIGNAL(pressed()), SLOT(onRemove()));
  this->connect(d->EditPushButton, SIGNAL(pressed()), SLOT(onEdit()));

  // Add default 'empty item'
  d->insertEmptyItem();
}

// --------------------------------------------------------------------------
ctkAddRemoveComboBox::~ctkAddRemoveComboBox()
{
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::printAdditionalInfo()
{
  Q_D(ctkAddRemoveComboBox);
  #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    qDebug() << "ctkAddRemoveComboBox:" << this << Qt::endl
           << " EmptyText:" << d->EmptyText << Qt::endl
           << " AddingEmptyItem:" << d->AddingEmptyItem << Qt::endl
           << " RemovingEmptyItem:" << d->RemovingEmptyItem << Qt::endl
           << " AddEnabled:" << d->AddEnabled << Qt::endl
           << " RemoveEnabled:" << d->RemoveEnabled << Qt::endl
           << " EditEnabled:" << d->EditEnabled << Qt::endl
           << " HasEmptyItem:" << d->HasEmptyItem;
  #else
  qDebug() << "ctkAddRemoveComboBox:" << this << endl
           << " EmptyText:" << d->EmptyText << endl
           << " AddingEmptyItem:" << d->AddingEmptyItem << endl
           << " RemovingEmptyItem:" << d->RemovingEmptyItem << endl
           << " AddEnabled:" << d->AddEnabled << endl
           << " RemoveEnabled:" << d->RemoveEnabled << endl
           << " EditEnabled:" << d->EditEnabled << endl
           << " HasEmptyItem:" << d->HasEmptyItem;
  #endif
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setComboBox(QComboBox* comboBox)
{
  Q_D(ctkAddRemoveComboBox);
  if ((comboBox == d->ComboBox) ||
      comboBox->count())
    {
    return;
    }
  
  QLayoutItem* oldComboBoxItem = this->layout()->takeAt(0);
  QComboBox* oldComboBox = qobject_cast<QComboBox*>(oldComboBoxItem->widget());
  comboBox->setSizePolicy(oldComboBox->sizePolicy());
  comboBox->setEnabled(this->comboBoxEnabled());

  delete oldComboBoxItem;

  dynamic_cast<QBoxLayout*>(this->layout())->insertWidget(0, comboBox);
  d->connectComboBox(comboBox);
  d->ComboBox = comboBox;
  delete oldComboBox;

  // Add default 'empty item'
  d->insertEmptyItem();
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setEmptyText(const QString& text)
{
  Q_D(ctkAddRemoveComboBox);
  
  if (d->HasEmptyItem)
    {
    Q_ASSERT(d->ComboBox->count() == 1);
    this->setItemText(0, text);
    }
  d->EmptyText = text;
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkAddRemoveComboBox, QString, emptyText, EmptyText);

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::onRowsInserted(const QModelIndex & _parent, int start, int end)
{
  Q_D(ctkAddRemoveComboBox);
  
  if (_parent != d->ComboBox->rootModelIndex())
    {// Rows that are to be added in the model are not displayed by the combobox
    return;
    }
    
  if (d->HasEmptyItem && !d->AddingEmptyItem)
    {
    // Remove the Empty item as some real items have been added
    d->HasEmptyItem = false;
    d->RemovingEmptyItem = true;
    d->ComboBox->removeItem(start == 0 ? end + 1 : 0);
    d->RemovingEmptyItem = false;
    
    if (d->RemoveEnabled)
      {
      d->RemovePushButton->setEnabled(true);
      }
    if (d->EditEnabled)
      {
      d->EditPushButton->setEnabled(true);
      }
    // Since we just removed the empty item, we need to shift the start/end items if needed
    if (start > 0 )
      {
      --start;
      --end;
      }
    }

  // Emit signal only if the items added is *NOT* the Empty item
  if (!d->AddingEmptyItem)
    {
    for (int i = start; i <= end; ++i)
      {
      emit this->itemAdded(i);
      }
    }
 }

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::onRowsAboutToBeRemoved(const QModelIndex & _parent, int start, int end)
{
  Q_D(ctkAddRemoveComboBox);

  if (_parent != d->ComboBox->rootModelIndex())
    {//rows that are to be added in the model are not displayed by the combobox
    return;
    }

  // if the user try to remove the Empty item, don't send event
  if (d->RemovingEmptyItem)
    {
    return;
    }
  for (int i = start; i <= end; ++i)
    {
    emit this->itemAboutToBeRemoved(i);
    }
}


// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::onRowsRemoved(const QModelIndex & _parent, int start, int end)
{
  Q_D(ctkAddRemoveComboBox);
  
  if (_parent != d->ComboBox->rootModelIndex())
    {//rows that are to be added in the model are not displayed by the combobox
    return;
    }
  // the combobox is now empty, add the EmptyItem if needed
  if (d->ComboBox->count() == 0)
    {
    // Add default 'empty item'
    d->insertEmptyItem();
   
    if (d->RemoveEnabled)
      {
      d->RemovePushButton->setEnabled(false);
      }
    if (d->EditEnabled)
      {
      d->EditPushButton->setEnabled(false);
      }
    }

  if (!d->RemovingEmptyItem)
    {
    for (int i = start; i <= end; ++i)
      {
      emit this->itemRemoved(i);
      }
    }
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setComboBoxEnabled(bool enable)
{
  Q_D(ctkAddRemoveComboBox);
  d->ComboBox->setEnabled(enable);
}

// --------------------------------------------------------------------------
bool ctkAddRemoveComboBox::comboBoxEnabled()const
{
  Q_D(const ctkAddRemoveComboBox);
  //const cast as I'm not sure why isEnabledTo doesn't take a const
  return d->ComboBox->isEnabledTo(const_cast<ctkAddRemoveComboBox*>(this));
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setAddEnabled(bool enable)
{
  Q_D(ctkAddRemoveComboBox);
  
  d->AddPushButton->setEnabled(enable);
  d->AddEnabled = enable;
}

// --------------------------------------------------------------------------
bool ctkAddRemoveComboBox::addEnabled()const
{
  Q_D(const ctkAddRemoveComboBox);
  return d->AddEnabled;
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setRemoveEnabled(bool enable)
{
  Q_D(ctkAddRemoveComboBox);
  
  if (this->count() > 0)
    {
    d->RemovePushButton->setEnabled(enable);
    }
  d->RemoveEnabled = enable;
}

// --------------------------------------------------------------------------
bool ctkAddRemoveComboBox::removeEnabled()const
{
  Q_D(const ctkAddRemoveComboBox);
  return d->RemoveEnabled;
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setEditEnabled(bool enable)
{
  Q_D(ctkAddRemoveComboBox);
  
  if (this->count() > 0)
    { 
    d->EditPushButton->setEnabled(enable);
    }
  d->EditEnabled = enable;
}

// --------------------------------------------------------------------------
bool ctkAddRemoveComboBox::editEnabled()const
{
  Q_D(const ctkAddRemoveComboBox);
  return d->EditEnabled;
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::onAdd()
{
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::onRemove()
{
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::onEdit()
{
}

// --------------------------------------------------------------------------
int ctkAddRemoveComboBox::count()const
{
  Q_D(const ctkAddRemoveComboBox);
  return (d->HasEmptyItem ? 0 : d->ComboBox->count());
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkAddRemoveComboBox, bool, empty, HasEmptyItem);

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setCurrentIndex(int index)
{
  Q_D(ctkAddRemoveComboBox);
  return d->ComboBox->setCurrentIndex(index);
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::insertItem(int index, const QString &text, const QVariant &userDataVariable)
{
  Q_D(ctkAddRemoveComboBox);
  //qDebug() << __FUNCTION__ << " " << index <<  " " << text << " " << userDataVariable ;
  d->ComboBox->insertItem(index, text, userDataVariable);
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::insertItem(int index, const QIcon &icon, const QString &text, const QVariant &userDataVariable)
{
  Q_D(ctkAddRemoveComboBox);
  d->ComboBox->insertItem(index, icon, text, userDataVariable);
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::insertItems(int index, const QStringList &texts)
{
  Q_D(ctkAddRemoveComboBox);
  d->ComboBox->insertItems(index, texts);
}
  
// --------------------------------------------------------------------------
int ctkAddRemoveComboBox::findText(const QString & text, Qt::MatchFlags flags)const
{
  Q_D(const ctkAddRemoveComboBox);
  
  if (d->HasEmptyItem)
    { // if the scene is empty, don't even try to find the text (it could be the
      // one of the EmptyText prop.
    return -1;
    }
  return d->ComboBox->findText(text, flags);
}

// --------------------------------------------------------------------------
int ctkAddRemoveComboBox::findData(const QVariant &dataVariable, int role, Qt::MatchFlags flags)const
{
  Q_D(const ctkAddRemoveComboBox);
  
  if (d->HasEmptyItem)
    { // if the scene is empty, don't even try to find the dataVariable 
    return -1;
    }
  return d->ComboBox->findData(dataVariable, role, flags);
}

// --------------------------------------------------------------------------
QString ctkAddRemoveComboBox::itemText(int index) const
{
  Q_D(const ctkAddRemoveComboBox);
  
  if (d->HasEmptyItem)
    {
    return QString();
    }
  return d->ComboBox->itemText(index);
}
  
// --------------------------------------------------------------------------
QVariant ctkAddRemoveComboBox::itemData(int index, int role) const
{
  Q_D(const ctkAddRemoveComboBox);
  
  if (d->HasEmptyItem)
    {
    return QVariant();
    }
  return d->ComboBox->itemData(index,role);
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setItemText(int index, const QString& text)
{
  Q_D(ctkAddRemoveComboBox);
  
  if (d->HasEmptyItem)
    {
    return;
    }
  return d->ComboBox->setItemText(index, text);
}
  
// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setItemData(int index, const QVariant& dataVariable, int role)
{
  Q_D(ctkAddRemoveComboBox);
  
  if (d->HasEmptyItem)
    {
    return;
    }
  d->ComboBox->setItemData(index, dataVariable, role);
}

// --------------------------------------------------------------------------
int ctkAddRemoveComboBox::currentIndex() const
{
  Q_D(const ctkAddRemoveComboBox);
  
  return d->HasEmptyItem ? -1 : d->ComboBox->currentIndex();
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::removeItem(int index)
{
  Q_D(ctkAddRemoveComboBox);
  
  if (d->HasEmptyItem)
    {
    return; 
    }
  d->ComboBox->removeItem(index);
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::clear()
{
  Q_D(ctkAddRemoveComboBox);
  
  if (d->HasEmptyItem)
    {
    return;
    }
  d->ComboBox->clear();
}

// --------------------------------------------------------------------------
QModelIndex ctkAddRemoveComboBox::rootModelIndex()const
{
  Q_D(const ctkAddRemoveComboBox);
  return d->ComboBox->rootModelIndex();
}

// --------------------------------------------------------------------------
void ctkAddRemoveComboBox::setRootModelIndex(const QModelIndex& root)
{
  Q_D(ctkAddRemoveComboBox);
  d->ComboBox->setRootModelIndex(root);
}

// --------------------------------------------------------------------------
int ctkAddRemoveComboBox::modelColumn()const
{
  Q_D(const ctkAddRemoveComboBox);
  return d->ComboBox->modelColumn();
}

// --------------------------------------------------------------------------
QAbstractItemModel* ctkAddRemoveComboBox::model()const
{
  Q_D(const ctkAddRemoveComboBox);
  return d->ComboBox->model();
}

