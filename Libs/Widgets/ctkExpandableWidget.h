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

#ifndef __ctkExpandableWidget_h
#define __ctkExpandableWidget_h

// Qt includes
#include <QFrame>
class QResizeEvent;

// CTK includes
#include "ctkWidgetsExport.h"
class ctkExpandableWidgetPrivate;

/**
 * \ingroup Widgets
 * \brief Frame that can be resized by the user.
*/
class CTK_WIDGETS_EXPORT ctkExpandableWidget: public QFrame
{
  Q_OBJECT
  // Qt::Horizontal|Qt::Vertical by default
  Q_PROPERTY(Qt::Orientations orientations READ orientations WRITE setOrientations)
  // true by default.
  Q_PROPERTY(bool sizeGripInside READ isSizeGripInside WRITE setSizeGripInside)
  Q_PROPERTY(QSize sizeGripMargins READ sizeGripMargins WRITE setSizeGripMargins)
public:
  typedef QFrame Superclass;

  /** Constructor
   *  /param parent       Parent widget
  */
  ctkExpandableWidget(QWidget *parent=0);
  virtual ~ctkExpandableWidget();

  void setOrientations(Qt::Orientations orientations);
  Qt::Orientations orientations()const;

  void setSizeGripInside(bool);
  bool isSizeGripInside()const;

  void setSizeGripMargins(QSize margins);
  QSize sizeGripMargins()const;

  //virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;

public Q_SLOTS:
  void updateSizeHint();

protected:
  QScopedPointer<ctkExpandableWidgetPrivate> d_ptr;

  virtual void resizeEvent(QResizeEvent* event);
  virtual bool event(QEvent* event);

private:
  Q_DECLARE_PRIVATE(ctkExpandableWidget);
  Q_DISABLE_COPY(ctkExpandableWidget);

  //Q_PRIVATE_SLOT(d_ptr, void _q_recomputeCompleterPopupSize())
};

#endif // __ctkExpandableWidget_h
