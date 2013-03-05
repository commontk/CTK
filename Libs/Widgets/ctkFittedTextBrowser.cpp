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
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>

// CTK includes
#include "ctkFittedTextBrowser.h"

//-----------------------------------------------------------------------------
ctkFittedTextBrowser::ctkFittedTextBrowser(QWidget* _parent)
  : QTextBrowser(_parent)
{
  this->connect(this, SIGNAL(textChanged()), SLOT(heightForWidthMayHaveChanged()));
  QSizePolicy newSizePolicy = QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  this->setSizePolicy(newSizePolicy);
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
  int ret = qMax(noScrollbarHeight, _minimumHeight);

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
