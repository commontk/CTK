/*=========================================================================

  Library:   CTK

  Copyright (c) University College London.

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

#ifndef __ctkPathListButtonsWidget_h
#define __ctkPathListButtonsWidget_h

// Qt includes
#include <QWidget>


// QtGUI includes
#include "ctkWidgetsExport.h"
#include "ctkPathListWidget.h"

class ctkPathListButtonsWidgetPrivate;

class QToolButton;


/// \ingroup Widgets
///
/// \brief A widget with add, remove and edit buttons to be used together with ctkPathListWidget.
///
/// This widget should be initialized with a ctkPathListWidget instance in order to work properly.
///
/// \sa init(ctkPathListWidget*)
///
/// \author m.clarkson@ucl.ac.uk
/// \author s.zelzer@dkfz-heidelberg.de
///
class CTK_WIDGETS_EXPORT ctkPathListButtonsWidget : public QWidget
{
  Q_OBJECT

  Q_PROPERTY(bool showAddFilesButton READ isAddFilesButtonVisible WRITE setAddFilesButtonVisible NOTIFY showAddFilesButtonChanged);
  Q_PROPERTY(bool showAddDirectoryButton READ isAddDirectoryButtonVisible WRITE setAddDirectoryButtonVisible NOTIFY showAddDirectoryButtonChanged);
  Q_PROPERTY(bool showRemoveButton READ isRemoveButtonVisible WRITE setRemoveButtonVisible NOTIFY showRemoveButtonChanged);
  Q_PROPERTY(bool showEditButton READ isEditButtonVisible WRITE setEditButtonVisible NOTIFY showEditButtonChanged);

  Q_PROPERTY(QString textAddFilesButton READ textAddFilesButton WRITE setTextAddFilesButton NOTIFY textAddFilesButtonChanged);
  Q_PROPERTY(QString textAddDirectoryButton READ textAddDirectoryButton WRITE setTextAddDirectoryButton NOTIFY textAddDirectoryButtonChanged);
  Q_PROPERTY(QString textRemoveButton READ textRemoveButton WRITE setTextRemoveButton NOTIFY textRemoveButtonChanged);
  Q_PROPERTY(QString textEditButton READ textEditButton WRITE setTextEditButton NOTIFY textEditButtonChanged);

  Q_PROPERTY(QString toolTipAddFilesButton READ toolTipAddFilesButton WRITE setToolTipAddFilesButton NOTIFY toolTipAddFilesButtonChanged);
  Q_PROPERTY(QString toolTipAddDirectoryButton READ toolTipAddDirectoryButton WRITE setToolTipAddDirectoryButton NOTIFY toolTipAddDirectoryButtonChanged);
  Q_PROPERTY(QString toolTipRemoveButton READ toolTipRemoveButton WRITE setToolTipRemoveButton NOTIFY toolTipRemoveButtonChanged);
  Q_PROPERTY(QString toolTipEditButton READ toolTipEditButton WRITE setToolTipEditButton NOTIFY toolTipEditButtonChanged);

  Q_PROPERTY(QIcon iconAddFilesButton READ iconAddFilesButton WRITE setIconAddFilesButton RESET unsetIconAddFilesButton NOTIFY iconAddFilesButtonChanged)
  Q_PROPERTY(QIcon iconAddDirectoryButton READ iconAddDirectoryButton WRITE setIconAddDirectoryButton RESET unsetIconAddDirectoryButton NOTIFY iconAddDirectoryButtonChanged)
  Q_PROPERTY(QIcon iconRemoveButton READ iconRemoveButton WRITE setIconRemoveButton RESET unsetIconRemoveButton NOTIFY iconRemoveButtonChanged)
  Q_PROPERTY(QIcon iconEditButton READ iconEditButton WRITE setIconEditButton RESET unsetIconEditButton NOTIFY iconEditButtonChanged)

  Q_PROPERTY(bool buttonsAutoRaise READ isButtonsAutoRaise WRITE setButtonsAutoRaise NOTIFY buttonsAutoRaiseChanged);
  Q_PROPERTY(int buttonSpacing READ buttonSpacing WRITE setButtonSpacing NOTIFY buttonSpacingChanged);

  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged);

Q_SIGNALS:
  void showAddFilesButtonChanged(bool);
  void showAddDirectoryButtonChanged(bool);
  void showRemoveButtonChanged(bool);
  void showEditButtonChanged(bool);
  void textAddFilesButtonChanged(const QString &);
  void textAddDirectoryButtonChanged(const QString &);
  void textRemoveButtonChanged(const QString &);
  void textEditButtonChanged(const QString &);
  void toolTipAddFilesButtonChanged(const QString &);
  void toolTipAddDirectoryButtonChanged(const QString &);
  void toolTipRemoveButtonChanged(const QString &);
  void toolTipEditButtonChanged(const QString &);
  void iconAddFilesButtonChanged(const QIcon &);
  void iconAddDirectoryButtonChanged(const QIcon &);
  void iconRemoveButtonChanged(const QIcon &);
  void iconEditButtonChanged(const QIcon &);

  void buttonsAutoRaiseChanged(bool);
  void buttonSpacingChanged(int);
  void orientationChanged(const Qt::Orientation &);
public:

  /// Superclass typedef
  typedef QWidget Superclass;

  ctkPathListButtonsWidget(QWidget* parent = 0);
  virtual ~ctkPathListButtonsWidget();

  /// Initialize this widget with a ctkPathListWidget.
  void init(ctkPathListWidget* pathListWidget);

  bool isAddFilesButtonVisible() const;
  void setAddFilesButtonVisible(bool visible);

  bool isAddDirectoryButtonVisible() const;
  void setAddDirectoryButtonVisible(bool visible);

  bool isRemoveButtonVisible() const;
  void setRemoveButtonVisible(bool visible);

  bool isEditButtonVisible() const;
  void setEditButtonVisible(bool visible);

  QString textAddFilesButton() const;
  QString textAddDirectoryButton() const;
  QString textRemoveButton() const;
  QString textEditButton() const;

  void setTextAddFilesButton(const QString& text);
  void setTextAddDirectoryButton(const QString& text);
  void setTextRemoveButton(const QString& text);
  void setTextEditButton(const QString& text);

  QString toolTipAddFilesButton() const;
  QString toolTipAddDirectoryButton() const;
  QString toolTipRemoveButton() const;
  QString toolTipEditButton() const;

  void setToolTipAddFilesButton(const QString& toolTip);
  void setToolTipAddDirectoryButton(const QString& toolTip);
  void setToolTipRemoveButton(const QString& toolTip);
  void setToolTipEditButton(const QString& toolTip);

  QIcon iconAddFilesButton() const;
  QIcon iconAddDirectoryButton() const;
  QIcon iconRemoveButton() const;
  QIcon iconEditButton() const;

  void setIconAddFilesButton(const QIcon& icon);
  void setIconAddDirectoryButton(const QIcon& icon);
  void setIconRemoveButton(const QIcon& icon);
  void setIconEditButton(const QIcon& icon);

  void unsetIconAddFilesButton();
  void unsetIconAddDirectoryButton();
  void unsetIconRemoveButton();
  void unsetIconEditButton();

  bool isButtonsAutoRaise() const;
  void setButtonsAutoRaise(bool autoRaise);

  int buttonSpacing() const;
  void setButtonSpacing(int spacing);

  Qt::Orientation orientation() const;
  void setOrientation(Qt::Orientation orientation);

  QToolButton* buttonAddFiles() const;
  QToolButton* buttonAddDirectory() const;
  QToolButton* buttonEdit() const;
  QToolButton* buttonRemove() const;

protected:
  QScopedPointer<ctkPathListButtonsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkPathListButtonsWidget)
  Q_DISABLE_COPY(ctkPathListButtonsWidget)
};

#endif
