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

#ifndef __ctkTransferFunctionControlPointsItem_h
#define __ctkTransferFunctionControlPointsItem_h

/// Qt includes
#include <QGraphicsObject>

/// CTK includes
#include "ctkWidgetsExport.h"
#include "ctkPimpl.h"
#include "ctkTransferFunctionItem.h"

class ctkTransferFunctionControlPointsItemPrivate;

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkTransferFunctionControlPointsItem: public ctkTransferFunctionItem
{
  Q_OBJECT
public:

  enum MOVE_TYPE{
    STOP_MOVE = 0,
    SWITCH_MOVE,
    DRAW_MOVE,
    FUSION_MOVE
  };

  explicit ctkTransferFunctionControlPointsItem(QGraphicsItem* parent = 0);
  ctkTransferFunctionControlPointsItem(ctkTransferFunction* transferFunction, 
                                       QGraphicsItem* parent = 0);
  virtual ~ctkTransferFunctionControlPointsItem();

  virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* e);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
  virtual void keyPressEvent (QKeyEvent * keyEvent);
protected:
  QScopedPointer<ctkTransferFunctionControlPointsItemPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTransferFunctionControlPointsItem);
  Q_DISABLE_COPY(ctkTransferFunctionControlPointsItem);

  /*
   * \brief Update the position of a point when a move mouse event occurs
   */
  void updatePointPosition( QPointF iPoint );
  /*
   * \brief Stop "moving point" if its position reaches another point
   */
  void stopPoints( QPointF iPointF );
  /*
   * \brief Switch "moving point" and "reached point" when "moving point" reaches a point.
   * Position of "reached point" - IS NOT - lost.
   */
  void switchPoints( QPointF iPointF );
  /*
   * \brief Switch "moving point" and "reached point" when "moving point" reaches a point.
   * Position of "reached point" - IS - lost.
   */
  void drawPoints( QPointF iPointF );
  /*
   * \brief The moving (or reached point?) is deleted
   */
  void fusionPoints( QPointF iPointF );
};

#endif
