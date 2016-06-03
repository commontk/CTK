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

#ifndef __ctkAxesWidget_h
#define __ctkAxesWidget_h

// Qt includes
#include <QMetaType>
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkAxesWidgetPrivate;

/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkAxesWidget : public QWidget
{
  Q_OBJECT
  Q_ENUMS(Axis)
  Q_PROPERTY(Axis currentAxis READ currentAxis WRITE setCurrentAxis NOTIFY currentAxisChanged)
  Q_PROPERTY(bool autoReset READ autoReset WRITE setAutoReset)
  Q_PROPERTY(QStringList axesLabels READ axesLabels WRITE setAxesLabels)
public : 

  enum Axis
    {
    None=0,
    Right,
    Left,
    Superior,
    Inferior,
    Anterior,
    Posterior,
    };
  
  ctkAxesWidget(QWidget *parent = 0);
  virtual ~ctkAxesWidget();

  ///
  /// Current selected axis. None by default. 
  Axis currentAxis() const;

  ///
  /// If autoReset is true, anytime the current axis is changed, the current
  /// axis is automatically reset to None.
  /// False by default.
  bool autoReset() const;

Q_SIGNALS:
  void currentAxisChanged(ctkAxesWidget::Axis axis);

public slots :
  ///
  /// Select the current axis and emit the currentAxisChanged signal if it is
  /// a new one. Warning, if autoReset is true, the currentAxis will automatically
  /// be reset to None. 
  void setCurrentAxis(Axis axis);
  
  ///
  /// Utility slot that set the current axis to none
  void setCurrentAxisToNone();

  ///
  /// Set the autoReset property to None anytime the currentAxis is changed.
  void setAutoReset(bool reset);

  /// \brief Set the axes \a labels.
  ///
  /// At least 6 labels are required. If more than 6 labels are given, the
  /// additional strings will be ignored.
  ///
  /// Returns True if the given \a labels are either successfully set or if the
  /// current values match the provided ones.
  ///
  /// \sa axesLabels()
  bool setAxesLabels(const QStringList& labels);

  /// Get the axes labels
  QStringList axesLabels() const;

  /// Size hints
  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;
  virtual bool hasHeightForWidth()const;
  virtual int heightForWidth(int width)const;

protected: 
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *mouseEvent);
  void mouseMoveEvent(QMouseEvent *mouseEvent);
  void mouseReleaseEvent(QMouseEvent *mouseEvent);

  QScopedPointer<ctkAxesWidgetPrivate> d_ptr;
private :
  Q_DECLARE_PRIVATE(ctkAxesWidget);
  Q_DISABLE_COPY(ctkAxesWidget);
};

Q_DECLARE_METATYPE(ctkAxesWidget::Axis)

#endif

