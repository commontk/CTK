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

/// \brief ctkWorkflowGroupBox is a widget displaying the user interface elements, title,
/// description and/or error status of the current step of a ctkWorkflow.

class CTK_WIDGETS_EXPORT ctkWorkflowGroupBox : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString preText READ preText WRITE setPreText)
  Q_PROPERTY(QString postText READ postText WRITE setPostText)
  Q_PROPERTY(bool hideWidgetsOfNonCurrentSteps READ hideWidgetsOfNonCurrentSteps WRITE setHideWidgetsOfNonCurrentSteps)

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

  ///
  /// If hideWidgetsOfNonCurrentSteps is turned on, then a step's
  /// widgets will be hidden when that step is not the current step.
  /// If it is turned off, then they will be shown but disabled.
  /// (Default OFF).
  bool hideWidgetsOfNonCurrentSteps()const;
  void setHideWidgetsOfNonCurrentSteps(bool newHideWidgetsOfNonCurrentSteps);

public slots:

  virtual void updateGroupBox(ctkWorkflowStep* currentStep);

protected:
  virtual void setTitle(const QString& newTitle);
  virtual void setSubTitle(const QString& newSubTitle);
  virtual void setErrorText(const QString& newErrorText);

protected:
  QScopedPointer<ctkWorkflowGroupBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowGroupBox);
  Q_DISABLE_COPY(ctkWorkflowGroupBox);

};

#endif

