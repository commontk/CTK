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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

#ifndef __ctkDICOMPatientDelegate_h
#define __ctkDICOMPatientDelegate_h

// Qt includes
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPixmap>
#include <QRect>

// CTK includes
#include "ctkDICOMWidgetsExport.h"
#include "ctkDICOMPatientView.h"

class ctkDICOMPatientDelegatePrivate;

/// \ingroup DICOM_Widgets
/// \brief Custom delegate for rendering DICOM patients in list views
///
/// This delegate provides custom rendering for DICOM patient items including:
/// - Patient information (name, ID, birth date, sex)
/// - Different rendering for TabMode vs ListMode
/// - Selection states and hover effects
/// - Progress indicators for loading operations
/// - Integrated study list view area for selected patients
///
/// The delegate is designed to work with ctkDICOMPatientModel and ctkDICOMPatientView.
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMPatientDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
  Q_PROPERTY(int iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
  Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
  Q_PROPERTY(int tabModeHeight READ tabModeHeight WRITE setTabModeHeight NOTIFY tabModeHeightChanged)
  Q_PROPERTY(int patientHeaderHeight READ patientHeaderHeight WRITE setPatientHeaderHeight NOTIFY patientHeaderHeightChanged)

public:
  typedef QStyledItemDelegate Superclass;
  explicit ctkDICOMPatientDelegate(QObject* parent = nullptr);
  virtual ~ctkDICOMPatientDelegate();

  /// Reimplemented from QStyledItemDelegate
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

  /// Set spacing between elements
  /// \param spacing Spacing in pixels
  void setSpacing(int spacing);
  int spacing() const;

  /// Set icon size between elements
  /// \param iconSize icon size in pixels
  void setIconSize(int iconSize);
  int iconSize() const;

  /// Set corner radius for rounded rectangles
  /// \param radius Corner radius in pixels
  void setCornerRadius(int radius);
  int cornerRadius() const;

  /// Set heights for different display modes
  /// \param height Height in pixels for TabMode items
  void setTabModeHeight(int height);
  int tabModeHeight() const;

  /// \param height Height in pixels for the patient header
  void setPatientHeaderHeight(int height);
  int patientHeaderHeight() const;

  /// Get the display mode from the view
  Q_INVOKABLE ctkDICOMPatientView::DisplayMode getDisplayMode(const QStyleOptionViewItem& option) const;

  /// Get the study area rect within the item rect
  Q_INVOKABLE QRect patientsRect(const QRect& itemRect, const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const;

  /// Get the patient header rect within the item rect
  Q_INVOKABLE QRect patientHeaderRect(const QRect& patientsRect,
                                      const QStyleOptionViewItem& option) const;

  /// Get the context menu button rect (3-dots) within the patient item
  Q_INVOKABLE QRect contextMenuButtonRect(const QRect& itemRect, const QStyleOptionViewItem& option) const;

  /// Check if a point is within the context menu button area
  Q_INVOKABLE bool isContextMenuButtonAt(const QPoint& pos, const QRect& itemRect, const QStyleOptionViewItem& option) const;

  /// Get the select all icon rect within the patient item
  Q_INVOKABLE QRect selectAllIconRect(const QRect& itemRect, const QStyleOptionViewItem& option) const;

  /// Check if a point is within the select all icon area
  Q_INVOKABLE bool isSelectAllIconAt(const QPoint& pos, const QRect& itemRect, const QStyleOptionViewItem& option) const;

Q_SIGNALS:
  /// Emitted when spacing changes
  void spacingChanged(int spacing);

  /// Emitted when icon size changes
  void iconSizeChanged(int iconSize);

  /// Emitted when corner radius changes
  void cornerRadiusChanged(int radius);

  /// Emitted when TabMode height changes
  void tabModeHeightChanged(int height);

  /// Emitted when patient header height changes
  void patientHeaderHeightChanged(int height);

protected:
  /// Format patient birth date for display
  QString formatPatientBirthDate(const QString& date) const;

  /// Paint for TabMode
  void paintTabMode(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
  void paintPatientsTabs(QPainter* painter, const QRect& rect, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const;
  void paintPatientHeader(QPainter* painter, const QRect &rect,
                          const QStyleOptionViewItem& option, const QModelIndex& index) const;

  /// Paint for ListMode
  void paintListMode(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

  /// Paint the context menu button (3 dots)
  void paintContextMenuButton(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const;

  /// Paint the select all icon
  void paintSelectAllIcon(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const;

private:
  Q_DECLARE_PRIVATE(ctkDICOMPatientDelegate);
  Q_DISABLE_COPY(ctkDICOMPatientDelegate);

protected:
  QScopedPointer<ctkDICOMPatientDelegatePrivate> d_ptr;
};

#endif
