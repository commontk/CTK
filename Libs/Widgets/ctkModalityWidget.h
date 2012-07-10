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

#ifndef __ctkModalityWidget_h
#define __ctkModalityWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkModalityWidgetPrivate;

/// \ingroup Widgets
///
/// ctkModalityWidget allows the user to select DICOM modalities
class CTK_WIDGETS_EXPORT ctkModalityWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QStringList selectedModalities READ selectedModalities WRITE setSelectedModalities)
  Q_PROPERTY(QStringList visibleModalities READ visibleModalities WRITE setVisibleModalities)
public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  /// If \li parent is null, ctkModalityWidget will be a top-level widget
  /// \note The \li parent can be set later using QWidget::setParent()
  /// By default, all the modalities are selected and only the following list 
  /// of modalities is visible: ("CT", "MR", "US", "CR", "XA", "NM", "PT")
  explicit ctkModalityWidget(QWidget* parent = 0);
  
  /// Destructor
  virtual ~ctkModalityWidget();

  /// Return the current list of selected modalities, e.g. ("CR", "CT", "NM")
  QStringList selectedModalities()const;

  /// Select a list of modalities, e.g ("XA", "RX", "MG")
  void setSelectedModalities(const QStringList& modalities);
  
  /// Return the current list of visible modalities
  QStringList visibleModalities()const;

  /// Show a list of modalities
  void setVisibleModalities(const QStringList& modalities);

  /// Select a modality (visible or not). Does nothing if \a modality doesn't
  /// exist.
  void selectModality(const QString& modality, bool select = true);
  /// Select a modality. Does nothing if \a modality doesn't exist.
  void showModality(const QString& modality, bool show = true);
  
  /// Show all the modalities
  void showAll();

  /// Hide all the modalities
  void hideAll();

  /// Return true if all the modalities (visible or not) are selected.
  bool areAllModalitiesSelected() const;

  /// Return true if all the modalities (checked or not) are visible.
  bool areAllModalitiesVisible() const;

  /// Return a list of all the modalities: visible or not, selected or not.
  /// Please note the order of the modalities might be different than in
  /// selectedModalities and visibleModalities.
  QStringList modalities() const;

public Q_SLOTS:
  /// Select all the modalities (visible or not)
  /// Note: only emit the signal selectedModalitiesChanged once.
  void selectAll();

  /// Unselect all the modalities (visible or not)
  /// Note: only emit the signal selectedModalitiesChanged once.
  void unselectAll();

Q_SIGNALS:
  /// Fired anytime a modality is selected or unselected.
  /// Note: When the user click on "Any", it only emits the signal
  /// once (and not after each item is selected/unselected).
  void selectedModalitiesChanged(const QStringList modalities);

protected Q_SLOTS:
  void onAnyChanged(int state);
  void onModalityChecked(bool);

protected:
  QScopedPointer<ctkModalityWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkModalityWidget);
  Q_DISABLE_COPY(ctkModalityWidget);
};

#endif
