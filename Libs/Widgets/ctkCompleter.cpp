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
#include <QSortFilterProxyModel>
#include <QStringList>

// CTK includes
#include "ctkCompleter.h"

// -------------------------------------------------------------------------
class ctkCompleterPrivate
{
  Q_DECLARE_PUBLIC(ctkCompleter);
protected:
  ctkCompleter* const q_ptr;

public:
  ctkCompleterPrivate(ctkCompleter& object);
  ~ctkCompleterPrivate();
  void init();
  
  QStringList splitPath(const QString& path);
  void updateSortFilterProxyModel();

  ctkCompleter::ModelFiltering Filtering;
  QSortFilterProxyModel* SortFilterProxyModel;
protected:
  Q_DISABLE_COPY(ctkCompleterPrivate);
};

// -------------------------------------------------------------------------
ctkCompleterPrivate::ctkCompleterPrivate(ctkCompleter& object)
  :q_ptr(&object)
{
  qRegisterMetaType<ctkCompleter::ModelFiltering>("ctkCompleter::ModelFiltering");
  this->Filtering = ctkCompleter::FilterStartsWith;
  this->SortFilterProxyModel = 0;
}

// -------------------------------------------------------------------------
ctkCompleterPrivate::~ctkCompleterPrivate()
{
  delete this->SortFilterProxyModel;
}

// -------------------------------------------------------------------------
void ctkCompleterPrivate::init()
{
  this->SortFilterProxyModel = new QSortFilterProxyModel(0);
}

// -------------------------------------------------------------------------
QStringList ctkCompleterPrivate::splitPath(const QString& s)
{
  Q_Q(ctkCompleter);
  QStringList paths;
  switch(q->modelFiltering())
    {
    default:
    case ctkCompleter::FilterStartsWith:
      paths = q->QCompleter::splitPath(s);
      break;
    case ctkCompleter::FilterContains:
      this->updateSortFilterProxyModel();
      this->SortFilterProxyModel->setFilterWildcard(s);
      paths = QStringList();
      break;
    case ctkCompleter::FilterWordStartsWith:
      {
      this->updateSortFilterProxyModel();
      QRegExp regexp = QRegExp(QRegExp::escape(s));
      regexp.setCaseSensitivity(q->caseSensitivity());
      this->SortFilterProxyModel->setFilterRegExp(regexp);
      paths = QStringList();
      break;
      }
    }
  return paths;
}

// -------------------------------------------------------------------------
void ctkCompleterPrivate::updateSortFilterProxyModel()
{
  Q_Q(ctkCompleter);
  this->SortFilterProxyModel->setFilterCaseSensitivity(q->caseSensitivity());
  this->SortFilterProxyModel->setFilterKeyColumn(q->completionColumn());
}

// -------------------------------------------------------------------------
ctkCompleter::ctkCompleter(QObject* parent)
  : QCompleter(parent)
  , d_ptr(new ctkCompleterPrivate(*this))
{
  Q_D(ctkCompleter);
  d->init();
}

// -------------------------------------------------------------------------
ctkCompleter::ctkCompleter(QAbstractItemModel* model, QObject* parent)
  : QCompleter(model, parent)
  , d_ptr(new ctkCompleterPrivate(*this))
{
  Q_D(ctkCompleter);
  d->init();
}

// -------------------------------------------------------------------------
ctkCompleter::ctkCompleter(const QStringList& list, QObject* parent)
  : QCompleter(list, parent)
  , d_ptr(new ctkCompleterPrivate(*this))
{
  Q_D(ctkCompleter);
  d->init();
}

// -------------------------------------------------------------------------
ctkCompleter::~ctkCompleter()
{
}

// -------------------------------------------------------------------------
ctkCompleter::ModelFiltering ctkCompleter::modelFiltering() const
{
  Q_D(const ctkCompleter);
  return d->Filtering;
}

// -------------------------------------------------------------------------
void ctkCompleter::setModelFiltering(ModelFiltering filter)
{
  Q_D(ctkCompleter);
  if (filter == d->Filtering)
    {
    return;
    }
  QAbstractItemModel* source = this->sourceModel();
  d->Filtering = filter;
  this->setSourceModel(source);
  Q_ASSERT(this->sourceModel());
  // Update the filtering
  this->setCompletionPrefix(this->completionPrefix());
}

// -------------------------------------------------------------------------
QStringList ctkCompleter::splitPath(const QString& s)const
{
  Q_D(const ctkCompleter);
  return const_cast<ctkCompleterPrivate*>(d)->splitPath(s);
}

// -------------------------------------------------------------------------
QAbstractItemModel* ctkCompleter::sourceModel()const
{
  Q_D(const ctkCompleter);
  if (d->Filtering != ctkCompleter::FilterStartsWith)
    {
    return d->SortFilterProxyModel->sourceModel();
    }
  return this->QCompleter::model();
}

// -------------------------------------------------------------------------
void ctkCompleter::setSourceModel(QAbstractItemModel* source)
{
  Q_D(ctkCompleter);
  QAbstractItemModel* model = source;
  if (d->Filtering != ctkCompleter::FilterStartsWith)
    {
    d->SortFilterProxyModel->setSourceModel(source);
    if (source && source->parent() == this)
      {
      source->setParent(d->SortFilterProxyModel);
      }
    model = d->SortFilterProxyModel;
    }
  else if (source && source->parent() == d->SortFilterProxyModel)
    {
    source->setParent(this);
    }
  this->setModel(model);
}
