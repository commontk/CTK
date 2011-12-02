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

#ifndef __ctkAddRemoveComboBox_h
#define __ctkAddRemoveComboBox_h

// Qt includes
#include <QWidget>
#include <QVariant>
#include <QModelIndex>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class QComboBox;
class ctkAddRemoveComboBoxPrivate;

/// \ingroup Widgets
///
/// QComboBox with tool buttons to add/remove entries to the menu list
class CTK_WIDGETS_EXPORT ctkAddRemoveComboBox : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString emptyText READ emptyText WRITE setEmptyText)
  Q_PROPERTY(bool addEnabled READ addEnabled WRITE setAddEnabled)
  Q_PROPERTY(bool removeEnabled READ removeEnabled WRITE setRemoveEnabled)
  Q_PROPERTY(bool editEnabled READ editEnabled WRITE setEditEnabled)
  
public:
  /// Superclass typedef
  typedef QWidget Superclass;
  
  /// Constructors
  explicit ctkAddRemoveComboBox(QWidget* parent = 0);
  virtual ~ctkAddRemoveComboBox();
  virtual void printAdditionalInfo();
  
  /// 
  /// Set text that should be displayed in the comboBox when it is empty
  void setEmptyText(const QString& text); 
  QString emptyText()const;
  
  /// 
  /// Enable/Disable the add button. 
  void setComboBoxEnabled(bool enable);
  bool comboBoxEnabled()const; 
  
  /// 
  /// Enable/Disable the add button. 
  void setAddEnabled(bool enable);
  bool addEnabled()const; 
  
  /// 
  /// Enable/Disable the add button. 
  void setRemoveEnabled(bool enable);
  bool removeEnabled()const;
  
  /// 
  /// Enable/Disable the edit button. 
  void setEditEnabled(bool enable); 
  bool editEnabled()const;
  
  inline void addItem(const QString &text, const QVariant &userDataVariable = QVariant() )
    {this->insertItem(this->count(), text, userDataVariable);}
  inline void addItem(const QIcon &icon, const QString &text, const QVariant &userDataVariable = QVariant() )
    {this->insertItem(this->count(), icon, text, userDataVariable);}
  inline void addItems(const QStringList &texts )
    {this->insertItems(this->count(), texts);}
    
  void insertItem(int index, const QString &text, const QVariant &userDataVariable = QVariant() );
  void insertItem(int index, const QIcon &icon, const QString &text, const QVariant &userDataVariable = QVariant() );
  void insertItems(int index, const QStringList &texts);  
  
  /// 
  /// Return the number of item
  int count()const;
  bool empty()const;
    
  /// 
  /// Returns the index of the item containing the given text; otherwise returns -1.
  /// The flags specify how the items in the combobox are searched.
  int findText(const QString& text, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive ) const;
  int findData(const QVariant & data, int role = Qt::UserRole, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive ) const;

  /// 
  QString   itemText(int index) const;
  QVariant  itemData(int index, int role = Qt::UserRole) const;

  void setItemText(int index, const QString& text);
  void setItemData(int index, const QVariant& data, int role = Qt::UserRole);

  /// 
  /// Return the current item
  int       currentIndex() const;
  inline QString  currentText() const
    {return this->itemText(this->currentIndex());}
  inline QVariant currentData(int role = Qt::UserRole) const
    {return this->itemData(this->currentIndex(), role);}

  /// 
  /// Remove the item currently selected. See signal 'itemRemoved'
  void removeItem(int index);
  inline void removeCurrentItem()
    {this->removeItem(this->currentIndex());}

  /// 
  /// Remove all the items
  void clear();

Q_SIGNALS:
  void currentIndexChanged(int index);
  void activated(int index);

  /// 
  /// This signal is sent after the method 'addItem' has been called programmatically
  void itemAdded(int index);
  
  /// 
  void itemAboutToBeRemoved(int index);
  void itemRemoved(int index);
    
public Q_SLOTS:
  /// 
  /// Select the current index
  void setCurrentIndex(int index);

protected Q_SLOTS:
  /// 
  virtual void onAdd();
  virtual void onRemove();
  virtual void onEdit();

protected:
  void setComboBox(QComboBox* comboBox);
  QModelIndex rootModelIndex()const;
  void setRootModelIndex(const QModelIndex& root);
  int modelColumn()const;
  QAbstractItemModel* model()const;

private Q_SLOTS:
  //void onRowsAboutToBeInserted(const QModelIndex & parent, int start, int end );
  void onRowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
  void onRowsInserted(const QModelIndex & parent, int start, int end);
  void onRowsRemoved(const QModelIndex & parent, int start, int end);

protected:
  QScopedPointer<ctkAddRemoveComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkAddRemoveComboBox);
  Q_DISABLE_COPY(ctkAddRemoveComboBox);
};

#endif
