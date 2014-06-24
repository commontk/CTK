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
/*=========================================================================

   Program: ParaView
   Module:  pqCheckableModelHelper.h

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2.

   See http://www.paraview.org/paraview/project/license.html for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#ifndef _ctkCheckableModelHelper_h
#define _ctkCheckableModelHelper_h

// Qt includes
#include <QModelIndex>
#include <QObject>
class QAbstractItemModel;

// CTK includes
#include "ctkWidgetsExport.h"

class ctkCheckableModelHelperPrivate;

/// \ingroup Widgets
///
/// ctkCheckableModelHelper expose functions to handle checkable models
class CTK_WIDGETS_EXPORT ctkCheckableModelHelper : public QObject
{
  Q_OBJECT;
  Q_PROPERTY(bool forceCheckability READ forceCheckability WRITE setForceCheckability);
  Q_PROPERTY(int propagateDepth READ propagateDepth WRITE setPropagateDepth);
  Q_PROPERTY(Qt::CheckState defaultCheckState READ defaultCheckState WRITE setDefaultCheckState);

public:
  ctkCheckableModelHelper(Qt::Orientation orientation, QObject *parent=0);
  virtual ~ctkCheckableModelHelper();

  Qt::Orientation orientation()const;


  ///
  /// When setting the model, if PropagateToItems is true (by default), the check
  /// state of the checkable headers is updated from the check state of the items
  /// If you want to make sure of the check state of a header, after setting the
  /// (done by myView.setHeader(myCheckableModelHelper)), you can call
  /// myModel.setHeaderData(0, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole)
  /// or myCheckableModelHelper->setCheckState(0, Qt::Checked)
  QAbstractItemModel *model()const;
  virtual void setModel(QAbstractItemModel *model);

  /// Reimplemented for internal reasons
  QModelIndex rootIndex()const;
  virtual void setRootIndex(const QModelIndex &index);

  ///
  /// A section is checkable if its CheckStateRole data is non null.
  /// One can access the same value through the model:
  /// model->headerData(orientation, section, Qt::CheckStateRole).isEmpty()
  bool isHeaderCheckable(int section)const;
  bool isCheckable(const QModelIndex& index)const;

  ///
  /// Utility function that returns the checkState of the section.
  /// One can access the same value through the model:
  /// model->headerData(orientation, section, Qt::CheckStateRole)
  Qt::CheckState headerCheckState(int section)const;
  Qt::CheckState checkState(const QModelIndex&)const;

  ///
  /// Utility function that returns the checkState of the section.
  /// One can access the same value through the model:
  /// model->headerData(orientation, section, Qt::CheckStateRole)
  bool headerCheckState(int section, Qt::CheckState& checkState )const;
  bool checkState(const QModelIndex&, Qt::CheckState& checkState )const;

  /// How deep in the model(tree) do you want the check state to be propagated
  /// A value of -1 correspond to the deepest level of the model.
  /// -1 by default
  void setPropagateDepth(int depth);
  int  propagateDepth()const;

  /// When true, the new items are automatically set to checkable
  void setForceCheckability(bool force);
  bool forceCheckability()const;

  Qt::CheckState defaultCheckState()const;
  void setDefaultCheckState(Qt::CheckState);

public Q_SLOTS:
  void setCheckState(const QModelIndex& modelIndex, Qt::CheckState checkState);
  ///
  /// Warning, setting the check state automatically set the
  /// header section checkable
  void setHeaderCheckState(int section, Qt::CheckState checkState);

  /// Utility function to toggle the checkstate of an index
  void toggleCheckState(const QModelIndex& modelIndex);
  void toggleHeaderCheckState(int section);

private Q_SLOTS:
  void onHeaderDataChanged(Qt::Orientation orient, int first, int last);

  void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
  void updateHeadersFromItems();
  void onColumnsInserted(const QModelIndex& parent, int start, int end);
  void onRowsInserted(const QModelIndex& parent, int start, int end);

protected:
  QScopedPointer<ctkCheckableModelHelperPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkCheckableModelHelper);
  Q_DISABLE_COPY(ctkCheckableModelHelper);
};

#endif
