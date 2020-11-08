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

#ifndef __ctkMessageBox_h
#define __ctkMessageBox_h

// Qt includes
#include <QMessageBox>
#include <QList>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkMessageBoxPrivate;

/// \ingroup Widgets
/// ctkMessageBox is an advanced QMessageBox with extra features such as a
/// checkbox to automatically accept the messagebox next time exec() is called.
class CTK_WIDGETS_EXPORT ctkMessageBox : public QMessageBox
{
  Q_OBJECT
  /// This property holds wether the dialog should be shown next time exec()
  /// is called.
  /// \a dontShowAgain can be changed programatically by calling
  ///  setDontShowAgain or when the user checks the "Dont show again"
  /// checkbox and the dialog is accepted.
  /// If the checkbox is checked by the user but the dialog is rejected (click
  /// on Cancel), \a dontShowAgain is not changed.
  /// If \a dontShowAgainSettingsKey is set, the \a dontShowAgain is set with
  /// the value of the key.
  /// By default, dontShowAgain is false.
  Q_PROPERTY(bool dontShowAgain READ dontShowAgain WRITE setDontShowAgain)
  /// This list contains button roles that are saved in dontShowAgain settings
  /// if dontShowAgain flag is set. By default the choice is only saved if the
  /// role of the pushed button is QMessageBox::AcceptRole.
  /// For example, if a message box has Yes, No, and Cancel buttons then it
  /// QMessageBox::YesRole and QMessageBox::YesRole roles have to be added to the list
  /// to allow saving yes/no selection in settings.
  Q_PROPERTY(QList<QMessageBox::ButtonRole> dontShowAgainButtonRoles READ dontShowAgainButtonRoles WRITE setDontShowAgainButtonRoles)

  /// This property holds the settings key that is used to synchronize the state
  /// of the checkbox "Don't show this message again"
  /// with the given key value within QSettings. If the settings value is !=
  /// QMessageBox::InvalidRole, the dialog is shown, otherwise it is skipped by
  /// simulating a click on the button corresponding to the settings value (QMessageButton::StandardButton or
  /// QMessageBox::ButtonRole)
  /// If a non empty key is set, the check box gets visible, otherwise it is
  /// hidden.
  /// By default, no key is set.
  Q_PROPERTY(QString dontShowAgainSettingsKey READ dontShowAgainSettingsKey WRITE setDontShowAgainSettingsKey)

  /// This property controls the visibility of the "Don't show again" checkbox.
  /// Even if the checkbox is not visible, the dialog is not not shown if
  /// \a dontShowAgain is true.
  /// By default, the "Don't show again" checkbox is not visible.
  Q_PROPERTY(bool dontShowAgainVisible READ isDontShowAgainVisible WRITE setDontShowAgainVisible)

public:
  typedef QMessageBox Superclass;
  ctkMessageBox(QWidget* newParent = 0);
  ctkMessageBox(Icon icon, const QString & title, const QString & text, StandardButtons buttons = NoButton,
                QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
  virtual ~ctkMessageBox();

  bool dontShowAgain()const;

  void setDontShowAgainSettingsKey(const QString& key);
  QString dontShowAgainSettingsKey()const;

  void setDontShowAgainVisible(bool visible);
  bool isDontShowAgainVisible()const;

  /// Get the list of button roles that can be saved in settings when
  /// "Don't show again" checkbox is checked.
  /// \sa setDontShowAgainButtonRoles(), addDontShowAgainButtonRole()
  QList<QMessageBox::ButtonRole> dontShowAgainButtonRoles()const;

  /// Set the list of button roles that can be saved in settings when
  /// "Don't show again" checkbox is checked.
  /// \sa dontShowAgainButtonRoles(), addDontShowAgainButtonRole()
  void setDontShowAgainButtonRoles(const QList<QMessageBox::ButtonRole>& roles);

  /// Add one role to the list of button roles that can be saved in settings when
  /// "Don't show again" checkbox is checked.
  /// \sa dontShowAgainButtonRoles(), setDontShowAgainButtonRoles()
  Q_INVOKABLE void addDontShowAgainButtonRole(QMessageBox::ButtonRole role);

  /// Utility function that opens a dialog to confirm exit.
  /// If \a dontShowAgainKey is empty, the dontShowAgain checkbox is hidden
  /// and the message box is always open for the user to confirm exit.
  static bool confirmExit(const QString& dontShowAgainKey = QString(),
                          QWidget* parentWidget = 0);

  /// Reimplemented for internal reasons
  virtual void setVisible(bool visible);

  /// Reimplemented for internal reasons
  virtual void done(int resultCode);

public Q_SLOTS:
  /// Change the checkbox and the settings if any
  void setDontShowAgain(bool dontShow);

  void onFinished(int resultCode);

protected:
  QScopedPointer<ctkMessageBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkMessageBox);
  Q_DISABLE_COPY(ctkMessageBox);
};

#endif
