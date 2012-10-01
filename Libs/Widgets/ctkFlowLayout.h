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

#ifndef __ctkFlowLayout_h
#define __ctkFlowLayout_h

// Qt includes
#include <QLayout>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkFlowLayoutPrivate;

/// \ingroup Widgets
/// Acts like a QBoxLayout but if the space is horizontally/vertically limited,
/// it displays items ona a new row/column based on the orientation.
/// Please note that using a Qt::Vertical orientation without the property
/// alignItems set to true might result to weird layout behavior.
class CTK_WIDGETS_EXPORT ctkFlowLayout : public QLayout
{
  Q_OBJECT
  /// If orientation is Qt::Horizontal, items are layed out from left to right
  /// then top to bottom if there is no more horizontal space.
  /// If orientation is Qt::Vertical, items are layed out from top to bottom
  /// then left to right if there is no more vertical space.
  /// Qt::Horizontal by default
  /// \sa preferredExpandingDirections
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

  /// Indicates how the size hint of the layout should behave. The preferred
  /// expanding direction can be different than the \a orientation of the
  /// layout.
  /// It can be a combination of Qt::Horizontal and Qt::Vertical, in that case
  /// the layout will try to expand in a square shape (evenly distribute the
  /// number of rows and columns).
  /// Qt::Horizontal | Qt::Vertical by default.
  /// \sa orientation
  Q_PROPERTY(Qt::Orientations preferredExpandingDirections READ preferredExpandingDirections WRITE setPreferredExpandingDirections)

  /// Force the items to be horizontally aligned based on the largest item
  /// to display.
  /// True by default.
  /// \sa orientation
  Q_PROPERTY(bool alignItems READ alignItems WRITE setAlignItems)

  /// Horizontal space between items, if the spacing is <0, a default spacing
  /// set on the parent/style is used.
  /// -1 by default.
  /// \sa verticalSpacing
  Q_PROPERTY(int horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing)

  /// Vertical space between items, if the spacing is <0, a default spacing
  /// set on the parent/style is used.
  /// -1 by default.
  /// \sa horizontalSpacing
  Q_PROPERTY(int verticalSpacing READ verticalSpacing WRITE setVerticalSpacing)

public:
  typedef QLayout Superclass;
  explicit ctkFlowLayout(Qt::Orientation orientation, QWidget* parent = 0);
  explicit ctkFlowLayout(QWidget* parent);
  explicit ctkFlowLayout();
  virtual ~ctkFlowLayout();

  void setOrientation(Qt::Orientation orientation);
  Qt::Orientation orientation()const;

  void setPreferredExpandingDirections(Qt::Orientations directions);
  Qt::Orientations preferredExpandingDirections()const;

  int horizontalSpacing() const;
  void setHorizontalSpacing(int);

  int verticalSpacing() const;
  void setVerticalSpacing(int);

  bool alignItems()const;
  void setAlignItems(bool);

  /// Replace the existing BoxLayout of the widget with a
  /// flow layout.
  /// When using the Designer, it is not possible to use a flow layout.
  /// Instead, you can use a H/VBoxLayout and replace it programatically
  /// in the setupUi() function with a flow layout. replaceLayout() makes
  /// the operation easier.
  /// \todo replaceLayout should take an existing layout instead of a widget,
  /// indeed, a layout can have another layout as a parent, not only a widget.
  static ctkFlowLayout* replaceLayout(QWidget* widget);

  /// When the orientation is Qt::Vertical, heightForWidth doesn't work
  /// correctly with ctkFlowLayout. Ideally widthForHeight should be used
  /// instead.
  /// \sa orientation widthForHeight hasHeightForWidth
  virtual bool hasWidthForHeight() const;
  virtual int widthForHeight(int) const;

  /// Reimplemented for internal reasons
  virtual void addItem(QLayoutItem *item);
  virtual Qt::Orientations expandingDirections() const;
  virtual bool hasHeightForWidth() const;
  virtual int heightForWidth(int) const;
  virtual int count() const;
  virtual QLayoutItem *itemAt(int index) const;
  virtual QSize minimumSize() const;
  virtual void setGeometry(const QRect &rect);
  virtual QSize sizeHint() const;
  virtual QLayoutItem *takeAt(int index);
  
protected:
  QScopedPointer<ctkFlowLayoutPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkFlowLayout);
  Q_DISABLE_COPY(ctkFlowLayout);
};

#endif
