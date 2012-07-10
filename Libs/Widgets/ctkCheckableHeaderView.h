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
   Module:  pqCheckableHeaderView.h

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

#ifndef _ctkCheckableHeaderView_h
#define _ctkCheckableHeaderView_h

// Qt includes
#include <QHeaderView>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkCheckableHeaderViewPrivate;
class ctkCheckableModelHelper;

/// \ingroup Widgets
///
/// ctkCheckableHeaderView is a QHeaderView that can display a checkbox 
/// for any header section.
/// If propageteToItems, the check state of the header section is set to
/// all items in the header row/column of the QAbstractItemModel if the 
/// items are checkable.
/// ctkCheckableHeaderView also supports row/column sorting.
/// TBD: It should probably be a QSortFilterProxyModel that adds a checkability
/// data on top of the indexes.
class CTK_WIDGETS_EXPORT ctkCheckableHeaderView : public QHeaderView
{
  Q_OBJECT;
public:
  ctkCheckableHeaderView(Qt::Orientation orient, QWidget *parent=0);
  virtual ~ctkCheckableHeaderView();

  ///
  /// When setting the model, if PropagateToItems is true (by default), the check
  /// state of the checkable headers is updated from the check state of the items
  /// If you want to make sure of the check state of a header, after setting the
  /// (done by myView.setHeader(myCheckableHeaderView)), you can call
  /// myModel.setHeaderData(0, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole)
  /// or myCheckableHeaderView->setCheckState(0, Qt::Checked)
  virtual void setModel(QAbstractItemModel *model);

  /// Reimplemented for internal reasons
  virtual void setRootIndex(const QModelIndex &index);

  ///
  ///  Used to listen for focus in/out events.
  /// \param object The object receiving the event.
  /// \param e Event specific data.
  /// \return
  ///   True if the event should be filtered out.
  virtual bool eventFilter(QObject *object, QEvent *e);
  
  
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
  
  ctkCheckableModelHelper* checkableModelHelper()const;

public Q_SLOTS:
  ///
  /// Warning, setting the check state automatically set the 
  /// header section checkable
  void setCheckState(int section, Qt::CheckState checkState);

private Q_SLOTS:
  void onHeaderDataChanged(Qt::Orientation orient, int first, int last);
  void onHeaderSectionInserted();
  inline void updateHeaderPixmaps();
  
protected:
  virtual void updateHeaderPixmaps(int first, int last);
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
void ctkCheckableHeaderView::updateHeaderPixmaps()
{
  this->updateHeaderPixmaps(0, this->count()-1);
}

#endif
