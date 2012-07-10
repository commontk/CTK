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

#ifndef __ctkCompleter_h
#define __ctkCompleter_h

// Qt includes
#include <QCompleter>
#include <QMetaType>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkCompleterPrivate;

/// \ingroup Widgets
/// ctkCompleter is a QCompleter that allows different way of filtering
/// the model, not just by filtering strings that start with the
/// \sa completionPrefix (default behavior).
/// ctkCompleter is a bit hackish as it reimplements a methods (splitPath)
/// from QCompleter in a way that is not intended.
/// Disclaimer, it might not work in all contexts, but seems to work
/// fine with a QLineEdit.
/// e.g.:
/// QStringList model;
/// model << "toto tata tutu";
/// model << "tata toto tutu";
/// ctkCompleter completer(model);
/// completer.setModelFiltering(ctkCompleter::FilterWordStartsWith);
/// QLineEdit lineEdit;
/// lineEdit.setCompleter(&completer);
/// ...
/// If the user types "ta", both entries will show up in the completer
/// If the user types "ot", no entries will show up in the completer
/// however using \sa FilterContains would have shown both.
class CTK_WIDGETS_EXPORT ctkCompleter: public QCompleter
{
  Q_OBJECT
  Q_ENUMS(ModelFiltering)
  /// FilterStartsWith is the default behavior (same as QCompleter).The
  /// completer filters out strings that don't start with \sa completionPrefix
  /// FilterContains is the most permissive filter, the completer filters out
  /// only strings that don't contain the characters from \sa completionPrefix
  /// FilterWordStartsWith is useful when strings contain space separated words
  /// and \sa completionPrefix applies to the beginnig of any of the words in the
  /// string.
  Q_PROPERTY(ModelFiltering modelFiltering READ modelFiltering WRITE setModelFiltering)

public:
  ctkCompleter(QObject* parent = 0);
  ctkCompleter(QAbstractItemModel* model, QObject* parent = 0);
  ctkCompleter(const QStringList& list, QObject* parent = 0 );
  virtual ~ctkCompleter();

  enum ModelFiltering
    {
    FilterStartsWith=0,
    FilterContains,
    FilterWordStartsWith
    };

  ModelFiltering modelFiltering()const;
  void setModelFiltering(ModelFiltering filter);

  virtual QStringList splitPath(const QString& s)const;

  /// ctkCompleter::model() might return a filtered model
  /// (QSortFilterAbstractModel) different from the one that was set.
  /// QCompleter::setModel should not be used and setSourceModel used
  /// instead.
  QAbstractItemModel* sourceModel()const;
  void setSourceModel(QAbstractItemModel* model);

protected:
  QScopedPointer<ctkCompleterPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkCompleter);
  Q_DISABLE_COPY(ctkCompleter);  
};

Q_DECLARE_METATYPE(ctkCompleter::ModelFiltering)

#endif // __ctkCompleter_h
