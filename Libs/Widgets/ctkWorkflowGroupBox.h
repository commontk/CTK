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

#ifndef __ctkWorkflowGroupBox_h
#define __ctkWorkflowGroupBox_h

// Qt includes
//#include <QGroupBox>
#include <QWidget>
class QString;
class QVBoxLayout;
class ctkWorkflowStep;

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"

class ctkWorkflowGroupBoxPrivate;

/// \ingroup Widgets
/// \brief ctkWorkflowGroupBox is a widget displaying the user interface elements, title,
/// description and/or error status of the current step of a ctkWorkflow.

class CTK_WIDGETS_EXPORT ctkWorkflowGroupBox : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString preText READ preText WRITE setPreText)
  Q_PROPERTY(QString postText READ postText WRITE setPostText)
  /// This property controls the text, icon and tooltip of the title button.
  /// "{current:name}" by default.
  /// \sa titleFormat(), setTitleFormat(), ctkWorkflow::formatButton()
  Q_PROPERTY(QString titleFormat READ titleFormat WRITE setTitleFormat)
  /// This property controls the text of the subtitle view.
  /// "{current:description}" by default.
  /// \sa subTitleFormat(), setSubTitleFormat(), ctkWorkflow::formatButton(),
  /// titleFormat, errorTextFormat
  Q_PROPERTY(QString subTitleFormat READ subTitleFormat WRITE setSubTitleFormat)
  /// This property controls the textof the error view.
  /// "{current:statusText}" by default.
  /// \sa errorTextFormat(), setErrorTextFormat(), ctkWorkflow::formatButton(),
  /// titleFormat, subTitleFormat
  Q_PROPERTY(QString errorTextFormat READ errorTextFormat WRITE setErrorTextFormat)
  Q_PROPERTY(bool hideWidgetsOfNonCurrentSteps READ hideWidgetsOfNonCurrentSteps WRITE setHideWidgetsOfNonCurrentSteps)
  Q_PROPERTY(bool errorTextEnabled READ errorTextEnabled WRITE setErrorTextEnabled)

public:

  typedef QWidget Superclass;
  explicit ctkWorkflowGroupBox(QWidget* parent = 0);
  virtual ~ctkWorkflowGroupBox();

  /// \brief Get the title text (usually a few words), located in the top area.
  ///
  /// Automatically displays the name of the ctkWorkflow's current step.
  QString title()const;

  /// \brief Set/get the subtitle text (usually a short sentence or two),
  /// located in the top area below the title.
  ///
  /// Automatically displays the description of the ctkWorkflow's current step.
  QString subTitle()const;

  /// \brief Set/get the pre-text, i.e. the contents of a convenience text
  /// section placed just above the client area.
  ///
  /// (Text is constant, regardless of the ctkWorkflow's current step).
  QString preText()const;
  void setPreText(const QString& newPreText);

  /// \brief Set/get the post-text, i.e. the contents of a convenience text
  /// section placed just below the client area.
  ///
  /// (Text is constant, regardless of the ctkWorkflow's current step).
  QString postText()const;
  void setPostText(const QString& newPostText);

  /// \brief Set/get the error text.
  ///
  /// Automatically displays the error text of the ctkWorkflow's current step.
  QString errorText()const;

  /// \brief Get the layout onto which step specific widgets are placed.
  QLayout* clientAreaLayout()const;

  /// Return the titleFormat property value.
  /// \sa titleFormat, setTitleFormat()
  QString titleFormat()const;

  /// Set the titleFormat property value.
  /// \sa titleFormat, titleFormat()
  void setTitleFormat(const QString& format);

  /// Return the subTitleFormat property value.
  /// \sa subTitleFormat, setSubTitleFormat()
  QString subTitleFormat()const;

  /// Set the subTitleFormat property value.
  /// \sa subTitleFormat, subTitleFormat()
  void setSubTitleFormat(const QString& format);

  /// Return the errorTextFormat property value.
  /// \sa errorTextFormat, setErrorTextFormat()
  QString errorTextFormat()const;

  /// Set the errorTextFormat property value.
  /// \sa errorTextFormat, errorTextFormat()
  void setErrorTextFormat(const QString& format);

  ///
  /// If hideWidgetsOfNonCurrentSteps is turned on, then a step's
  /// widgets will be hidden when that step is not the current step.
  /// If it is turned off, then they will be shown but disabled.
  /// (Default OFF).
  bool hideWidgetsOfNonCurrentSteps()const;
  void setHideWidgetsOfNonCurrentSteps(bool newHideWidgetsOfNonCurrentSteps);

  ///
  /// If errorTextEnabled is turned on, then a possible error text
  /// will be shown just underneath the client area.
  /// If it is turned off, then a possible error text will never
  /// be shown.
  /// (Default ON).
  bool errorTextEnabled()const;
  void setErrorTextEnabled(bool newErrorTextEnabled);

public Q_SLOTS:

  virtual void updateGroupBox(ctkWorkflowStep* currentStep);

protected:
  virtual void setSubTitle(const QString& newSubTitle);
  virtual void setErrorText(const QString& newErrorText);

protected:
  QScopedPointer<ctkWorkflowGroupBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowGroupBox);
  Q_DISABLE_COPY(ctkWorkflowGroupBox);

};

#endif

