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

#ifndef __ctkScreenshotDialog_h
#define __ctkScreenshotDialog_h

// Qt includes
#include <QDialog>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkScreenshotDialogPrivate;

///
/// ctkScreenshotDialog is simple dialog allowing to grab the content 
/// of any widget and save it into a PNG file.
/// \sa ctkScreenshotDialog::setWidgetToGrab, QDialog
class CTK_WIDGETS_EXPORT ctkScreenshotDialog : public QDialog
{
  Q_OBJECT
public:
  typedef QDialog Superclass;
  ctkScreenshotDialog(QWidget* newParent = 0);
  virtual ~ctkScreenshotDialog();

  /// Get widget to grab content from
  QWidget* widgetToGrab()const;

  /// Set widget to grab content from
  void setWidgetToGrab(QWidget* newWidgetToGrab);
  
  /// Set image name 
  void setImageName(const QString& newImageName);
  
  /// Get image name
  QString imageName() const;

  /// Set directory where image should be saved
  void setImageDirectory(const QString& newDirectory);

  /// Get directory were images are saved
  QString imageDirectory()const;

public slots:

  /// Grab the content of specified widget after \a delayInSeconds
  /// \sa setWidgetToGrab
  void saveScreenshot(int delayInSeconds = 0);

protected:
  QScopedPointer<ctkScreenshotDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkScreenshotDialog);
  Q_DISABLE_COPY(ctkScreenshotDialog);
};

#endif
