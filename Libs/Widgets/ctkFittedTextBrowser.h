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

#ifndef __ctkFittedTextBrowser_h
#define __ctkFittedTextBrowser_h

// Qt includes
#include <QTextBrowser>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkFittedTextBrowserPrivate;

/// \ingroup Widgets
/// ctkFittedTextBrowser is a QTextBrowser that adapts its height depending
/// on its contents and the width available. It always tries to show the whole
/// contents. ctkFittedTextBrowser doesn't resize itself but acts on the
/// sizeHint, minimumSizeHint and heightForWidth. Here sizeHint() and 
/// minimumSizeHint() are the same as ctkFittedTextBrowser always try to
/// show the whole contents.
///
/// The widget can further optimize use of available space by collapsing
/// text. If the option is enabled then only a short teaser is shown
/// and the user has to click on "More..." to see the full text.
class CTK_WIDGETS_EXPORT ctkFittedTextBrowser : public QTextBrowser
{
  Q_OBJECT
  Q_PROPERTY(bool collapsible READ collapsible WRITE setCollapsible)
  Q_PROPERTY(bool collapsed READ collapsed WRITE setCollapsed)
  Q_PROPERTY(QString showMoreText READ showMoreText WRITE setShowMoreText)
  Q_PROPERTY(QString showLessText READ showLessText WRITE setShowLessText)

public:
  ctkFittedTextBrowser(QWidget* parent = 0);
  virtual ~ctkFittedTextBrowser();

  /// Show only first line with "More..." link to save space.
  /// When the user clicks on the link then the full text is displayed
  /// (and a "Less..." link).
  /// The teaser is the beginning of the text up to the first newline character
  /// (for plain text) or <br> tag (for html). The separator is removed when
  /// the text is expanded so that the full text can continue on the same line
  /// as the teaser.
  void setCollapsible(bool collapsible);
  /// Show only first line with "More..." link to save space.
  bool collapsible() const;

  /// Show only first line/the full text.
  /// Only has effect if collapsible = true.
  void setCollapsed(bool collapsed);
  /// Show only first line/the full text.
  bool collapsed() const;

  void setPlainText(const QString &text);
#ifndef QT_NO_TEXTHTMLPARSER
  void setHtml(const QString &text);
#endif
  void setText(const QString &text);

  /// Text that is displayed at the end of collapsed text.
  /// Clicking on the text expands the widget.
  void setShowMoreText(const QString &text);
  /// Text that is displayed at the end of collapsed text.
  QString showMoreText()const;

  /// Text that is displayed at the end of non-collapsed text.
  /// Clicking on the text collapses the widget.
  void setShowLessText(const QString &text);
  /// Text that is displayed at the end of non-collapsed text.
  QString showLessText()const;

  /// Reimplemented for internal reasons
  virtual QSize sizeHint() const;
  /// Reimplemented for internal reasons
  virtual QSize minimumSizeHint() const;
  /// Reimplemented for internal reasons
  virtual int heightForWidth(int width) const;

protected Q_SLOTS:
  void heightForWidthMayHaveChanged();
  void anchorClicked(const QUrl &url);

protected:
  QScopedPointer<ctkFittedTextBrowserPrivate> d_ptr;

  virtual void resizeEvent(QResizeEvent* e);

private:
  Q_DECLARE_PRIVATE(ctkFittedTextBrowser);
  Q_DISABLE_COPY(ctkFittedTextBrowser);
};

#endif
