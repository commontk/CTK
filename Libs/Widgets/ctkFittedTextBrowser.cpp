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

// Qt includes
#include <QDebug>
#include <QScrollBar>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>

// CTK includes
#include "ctkFittedTextBrowser.h"
#include "ctkFittedTextBrowser_p.h"

static const char moreAnchor[] = "show_details";
static const char lessAnchor[] = "hide_details";

//-----------------------------------------------------------------------------
ctkFittedTextBrowserPrivate::ctkFittedTextBrowserPrivate(ctkFittedTextBrowser& object)
  :q_ptr(&object)
{
  this->Collapsed = true;
  this->CollapsibleTextSetter = ctkFittedTextBrowserPrivate::Text;
  this->ShowDetailsText = object.tr("Show details...");
  this->HideDetailsText = object.tr("Hide details.");
  QString HideDetailsText;
}

//-----------------------------------------------------------------------------
ctkFittedTextBrowserPrivate::~ctkFittedTextBrowserPrivate()
{
}

//-----------------------------------------------------------------------------
void ctkFittedTextBrowserPrivate::updateCollapsedText()
{
  Q_Q(ctkFittedTextBrowser);
  bool html = (this->CollapsibleTextSetter == ctkFittedTextBrowserPrivate::Html || this->CollapsibleText.indexOf("<html>") >= 0);
  if (html)
  {
    q->setHtml(this->collapsedTextFromHtml());
  }
  else
  {
    q->setHtml(this->collapsedTextFromPlainText());
  }
}

//-----------------------------------------------------------------------------
QString ctkFittedTextBrowserPrivate::collapsedTextFromPlainText() const
{
  int teaserEndPosition = this->CollapsibleText.indexOf("\n");
  if (teaserEndPosition < 0)
  {
    return this->CollapsibleText;
  }
  QString finalText;
  finalText.append("<html>");
  finalText.append(this->Collapsed ? this->CollapsibleText.left(teaserEndPosition) : this->CollapsibleText);
  finalText.append(this->collapseLinkText());
  finalText.append("</html>");
  // Remove line break to allow continuation of line.
  finalText.replace(finalText.indexOf('\n'), 1, "");
  // In plain text line breaks were indicated by newline, but we now use html,
  // so line breaks must use <br>
  finalText.replace("\n", "<br>");
  return finalText;
}

//-----------------------------------------------------------------------------
QString ctkFittedTextBrowserPrivate::collapsedTextFromHtml() const
{
  const QString lineBreak("<br>");
  int teaserEndPosition = this->CollapsibleText.indexOf(lineBreak);
  if (teaserEndPosition < 0)
  {
    return this->CollapsibleText;
  }

  QString finalText = this->CollapsibleText;
  if (this->Collapsed)
  {
    finalText = finalText.left(teaserEndPosition) + this->collapseLinkText();
    // By truncating the full text we might have deleted the closing </html> tag
    // restore it now.
    if (finalText.contains("<html") && !finalText.contains("</html"))
    {
      finalText.append("</html>");
    }
  }
  else
  {
    // Remove <br> to allow continuation of line and avoid extra space
    // when <p> element is used as well.
    finalText.replace(finalText.indexOf(lineBreak), lineBreak.size(), "");
    // Add link text before closing </body> or </html> tag
    if (finalText.contains("</body>"))
    {
      finalText.replace("</body>", this->collapseLinkText() + "</body>");
    }
    else if (finalText.contains("</html>"))
    {
      finalText.replace("</html>", this->collapseLinkText() + "</html>");
    }
    else
    {
      finalText.append(this->collapseLinkText());
    }
  }
  return finalText;
}

//-----------------------------------------------------------------------------
QString ctkFittedTextBrowserPrivate::collapseLinkText() const
{
  if (this->Collapsed)
  {
    return QString("&hellip; <a href=\"#") + moreAnchor + "\">" + this->ShowDetailsText + "</a>";
  }
  else
  {
    return QString(" <a href=\"#") + lessAnchor + "\">" + this->HideDetailsText + "</a>";
  }
}

//-----------------------------------------------------------------------------
ctkFittedTextBrowser::ctkFittedTextBrowser(QWidget* _parent)
  : QTextBrowser(_parent)
  , d_ptr(new ctkFittedTextBrowserPrivate(*this))
{
  this->connect(this, SIGNAL(textChanged()), SLOT(heightForWidthMayHaveChanged()));
  QSizePolicy newSizePolicy = QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  this->setSizePolicy(newSizePolicy);
  this->connect(this, SIGNAL(anchorClicked(QUrl)), SLOT(anchorClicked(QUrl)));
}

//-----------------------------------------------------------------------------
ctkFittedTextBrowser::~ctkFittedTextBrowser()
{

}

//-----------------------------------------------------------------------------
void ctkFittedTextBrowser::heightForWidthMayHaveChanged()
{
  this->updateGeometry();
}

//-----------------------------------------------------------------------------
int ctkFittedTextBrowser::heightForWidth(int _width) const
{
  QTextDocument* doc = this->document();
  qreal savedWidth = doc->textWidth();

  // Fudge factor. This is the difference between the frame and the
  // viewport.
  int fudge = 2 * this->frameWidth();

  int horizontalScrollbarHeight = 0;
  if (this->horizontalScrollBar()->isVisible())
  {
    horizontalScrollbarHeight = this->horizontalScrollBar()->height() + fudge;
  }

  // Do the calculation assuming no scrollbars
  doc->setTextWidth(_width - fudge);
  int noScrollbarHeight =
    doc->documentLayout()->documentSize().height() + fudge;

  // (If noScrollbarHeight is greater than the maximum height we'll be
  // allowed, then there will be scrollbars, and the actual required
  // height will be even higher. But since in this case we've already
  // hit the maximum height, it doesn't matter that we underestimate.)

  // Get minimum height (even if string is empty): one line of text
  int _minimumHeight = QFontMetrics(doc->defaultFont()).lineSpacing() + fudge;
  int ret = qMax(noScrollbarHeight, _minimumHeight) + horizontalScrollbarHeight;

  doc->setTextWidth(savedWidth);
  return ret;
}

//-----------------------------------------------------------------------------
QSize ctkFittedTextBrowser::minimumSizeHint() const {
  QSize s(this->size().width(), 0);
  if (s.width() == 0)
    {
    //s.setWidth(400); // arbitrary value
    return QTextBrowser::minimumSizeHint();
    }
  s.setHeight(this->heightForWidth(s.width()));
  return s;
}

//-----------------------------------------------------------------------------
QSize ctkFittedTextBrowser::sizeHint() const {
  return this->minimumSizeHint();
}

//-----------------------------------------------------------------------------
void ctkFittedTextBrowser::resizeEvent(QResizeEvent* e)
{
  this->QTextBrowser::resizeEvent(e);
  if (e->size().height() != this->heightForWidth(e->size().width()))
    {
    this->heightForWidthMayHaveChanged();
    }
}

//-----------------------------------------------------------------------------
void ctkFittedTextBrowser::setCollapsibleText(const QString &text)
{
  Q_D(ctkFittedTextBrowser);
  d->CollapsibleTextSetter = ctkFittedTextBrowserPrivate::Text;
  d->CollapsibleText = text;
  d->updateCollapsedText();
}

//-----------------------------------------------------------------------------
QString ctkFittedTextBrowser::collapsibleText() const
{
  Q_D(const ctkFittedTextBrowser);
  return d->CollapsibleText;
}


//-----------------------------------------------------------------------------
void ctkFittedTextBrowser::setCollapsiblePlainText(const QString &text)
{
  Q_D(ctkFittedTextBrowser);
  d->CollapsibleTextSetter = ctkFittedTextBrowserPrivate::PlainText;
  d->CollapsibleText = text;
  d->updateCollapsedText();
}

//-----------------------------------------------------------------------------
void ctkFittedTextBrowser::setCollapsibleHtml(const QString &text)
{
  Q_D(ctkFittedTextBrowser);
  d->CollapsibleTextSetter = ctkFittedTextBrowserPrivate::Html;
  d->CollapsibleText = text;
  d->updateCollapsedText();
}

//-----------------------------------------------------------------------------
void ctkFittedTextBrowser::anchorClicked(const QUrl &url)
{
  if (url.path().isEmpty())
  {
    if (url.fragment() == moreAnchor)
    {
      this->setCollapsed(false);
    }
    else if (url.fragment() == lessAnchor)
    {
      this->setCollapsed(true);
    }
  }
}

//-----------------------------------------------------------------------------
void ctkFittedTextBrowser::setCollapsed(bool collapsed)
{
  Q_D(ctkFittedTextBrowser);
  if (d->Collapsed == collapsed)
  {
    // no change
    return;
  }
  d->Collapsed = collapsed;
  d->updateCollapsedText();
}

//-----------------------------------------------------------------------------
bool ctkFittedTextBrowser::collapsed() const
{
  Q_D(const ctkFittedTextBrowser);
  return d->Collapsed;
}

//-----------------------------------------------------------------------------
void ctkFittedTextBrowser::setShowDetailsText(const QString &text)
{
  Q_D(ctkFittedTextBrowser);
  if (d->ShowDetailsText == text)
  {
    // no change
    return;
  }
  d->ShowDetailsText = text;
  d->updateCollapsedText();
}

//-----------------------------------------------------------------------------
QString ctkFittedTextBrowser::showDetailsText() const
{
  Q_D(const ctkFittedTextBrowser);
  return d->ShowDetailsText;
}

//-----------------------------------------------------------------------------
void ctkFittedTextBrowser::setHideDetailsText(const QString &text)
{
  Q_D(ctkFittedTextBrowser);
  if (d->HideDetailsText == text)
  {
    // no change
    return;
  }
  d->HideDetailsText = text;
  d->updateCollapsedText();
}

//-----------------------------------------------------------------------------
QString ctkFittedTextBrowser::hideDetailsText() const
{
  Q_D(const ctkFittedTextBrowser);
  return d->HideDetailsText;
}
