/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/
/*=========================================================================

   Program: ParaView
   Module:    $RCSfile: pqCheckableHeaderView.h,v $

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
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

#ifndef _ctkCheckableHeaderView_h
#define _ctkCheckableHeaderView_h

// Qt includes
#include <QHeaderView>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkCheckableHeaderViewPrivate;

///
/// ctkCheckableHeaderView is a QHeaderView that can display a checkbox 
/// for any header section.
/// If propageteToItems, the check state of the header section is set to
/// all items in the header row/column of the QAbstractItemModel if the 
/// items are checkable.
/// ctkCheckableHeaderView also supports row/column sorting.
class CTK_WIDGETS_EXPORT ctkCheckableHeaderView : public QHeaderView
{
  Q_OBJECT;
  Q_PROPERTY(bool propagateToItems READ propagateToItems WRITE setPropagateToItems);
public:
  ctkCheckableHeaderView(Qt::Orientation orient, QWidget *parent=0);
  virtual ~ctkCheckableHeaderView();

  /// Reimplemented for internal reasons
  virtual void setModel(QAbstractItemModel *model);

  /// Reimplemented for internal reasons
  virtual void setRootIndex(const QModelIndex &index);

  /// 
  /// A section is checkable if its CheckStateRole data is non null. 
  /// One can access the same value through the model:
  /// model->headerData(orientation, section, Qt::CheckStateRole).isEmpty()
  bool isCheckable(int section)const;

  ///
  /// Utility function that returns the checkState of the section. 
  /// One can access the same value through the model:
  /// model->headerData(orientation, section, Qt::CheckStateRole)
  Qt::CheckState checkState(int section)const;

  ///
  /// Utility function that returns the checkState of the section. 
  /// One can access the same value through the model:
  /// model->headerData(orientation, section, Qt::CheckStateRole)
  bool checkState(int section,Qt::CheckState& checkState )const;

  ///
  ///  Used to listen for focus in/out events.
  /// \param object The object receiving the event.
  /// \param e Event specific data.
  /// \return
  ///   True if the event should be filtered out.
  virtual bool eventFilter(QObject *object, QEvent *e);

  ///
  /// If true, the items check states in a row/column are synchronized 
  /// with the check state of the corresponding header section.
  /// When the property is set to true, the checkstate of the header is
  /// automatically updated from the checkstate of the items
  /// True by default
  void setPropagateToItems(bool propagate);
  bool propagateToItems()const;

public slots:
  ///
  /// if the check state is PartiallyChecked, the section becomes Checked
  void toggleCheckState(int section);
  ///
  /// Warning, setting the check state automatically set the 
  /// header section checkable
  void setCheckState(int section, Qt::CheckState checkState);

private slots:
  void updateHeaderData(Qt::Orientation orient, int first, int last);

  void insertHeaderSection(const QModelIndex &parent, int first, int last);
  inline void updateHeaders();

  void updateHeadersFromItems(const QModelIndex& topLeft, const QModelIndex& bottomRight);
  void updateHeadersFromItems();
  
protected:
  virtual void updateHeaders(int first, int last);
  virtual void initStyleSectionOption(QStyleOptionHeader *option, int section, QRect rect)const;
  virtual void mousePressEvent(QMouseEvent *e);
  virtual void mouseReleaseEvent(QMouseEvent *e);
  bool isPointInCheckBox(int section, QPoint pos)const;

protected:
  QScopedPointer<ctkCheckableHeaderViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkCheckableHeaderView);
  Q_DISABLE_COPY(ctkCheckableHeaderView);
};

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::updateHeaders()
{
  this->updateHeaders(0, this->count()-1);
}

#endif
