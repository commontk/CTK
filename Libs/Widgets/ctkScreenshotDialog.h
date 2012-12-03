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

#ifndef __ctkScreenshotDialog_h
#define __ctkScreenshotDialog_h

// Qt includes
#include <QDialog>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkScreenshotDialogPrivate;

/** 
    \ingroup Widgets
    
    ctkScreenshotDialog is simple dialog allowing to grab the content 
    of any widget and save it into a PNG file.
    It can be used as a tool and can take screenshots without being opened nor
    executed.
    \code
    ctkScreenshotDialog screenshot;
    screenshot.setWidgetToGrab(myWidget);
    screenshot.instantScreenshot();
    \endcode
 \sa ctkScreenshotDialog::setWidgetToGrab, QDialog
*/
class CTK_WIDGETS_EXPORT ctkScreenshotDialog : public QDialog
{
  Q_OBJECT
  Q_PROPERTY(QString baseFileName READ baseFileName WRITE setBaseFileName)
  Q_PROPERTY(QString directory READ directory WRITE setDirectory)
  Q_PROPERTY(int delay READ delay WRITE setDelay)

public:
  typedef QDialog Superclass;
  ctkScreenshotDialog(QWidget* parent = 0);
  virtual ~ctkScreenshotDialog();

  /// Get widget to grab content from. If no widget is set, no screenshot will
  /// be taken.
  /// 0 by default.
  /// TODO: if widgetToGrab -> screenshot the entire application
  void setWidgetToGrab(QWidget* newWidgetToGrab);
  QWidget* widgetToGrab()const;
  
  /// Set screenshot base name used to generate unique file names to save the
  /// screenshot images. The base name doesn't contain the file extension
  /// (automatically set to ".png")
  /// "Untitled" by default
  void setBaseFileName(const QString& newImageName);
  QString baseFileName() const;

  /// Set directory where screenshot files are saved. If path is empty, the
  /// program's working directory, ("."), is used.
  /// Current working directory by default.
  void setDirectory(const QString& path);
  QString directory()const;

  /// Set the delay in seconds before the screenshot is taken.
  /// 0 seconds by default.
  void setDelay(int seconds);
  int delay()const;

  /// Disable scaling or output resolution control and take a fixed
  /// resolution screenshot. Default output resolution is (300,300)
  void enforceResolution(int width = 300, int height = 300);
  void enforceResolution(QSize size = QSize(300,300));

  /// Get the original widget size
  QSize widgetSize();

  /// Allow/Disallow transparency in the output screenshot
  void enableTransparency(bool enable = true);

public Q_SLOTS:
  /// Instantanely grabs the content of \a widgetToGrag. Generates a
  /// png file into \a directory. It automatically increments the image name
  /// index suffix.
  void instantScreenshot();

  /// Calls instantScreenshot() after a countdown of \a delay seconds
  void saveScreenshot();

protected:
  QScopedPointer<ctkScreenshotDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkScreenshotDialog);
  Q_DISABLE_COPY(ctkScreenshotDialog);
};

#endif
