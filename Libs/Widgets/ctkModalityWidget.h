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

#ifndef __ctkModalityWidget_h
#define __ctkModalityWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkModalityWidgetPrivate;

///
/// ctkModalityWidget allows the user to ...
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
  explicit ctkModalityWidget(QWidget* parent = 0);
  
  /// Destructor
  virtual ~ctkModalityWidget();

  QStringList selectedModalities()const;
  void setSelectedModalities(const QStringList& modalities);
  
  QStringList visibleModalities()const;
  void setVisibleModalities(const QStringList& modalities);
  
  void selectModality(const QString& modality, bool select = true);
  void showModality(const QString& modality, bool show = true);
  
  void showAll();
  void hideAll();
public slots:
  void selectAll();
  void unselectAll();

signals:
  void selectedModalitiesChanged(const QStringList modalities);
protected slots:
  void onAnyChanged(int state);
  void onModalityChecked(bool);

protected:
  QScopedPointer<ctkModalityWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkModalityWidget);
  Q_DISABLE_COPY(ctkModalityWidget);
};

#endif
