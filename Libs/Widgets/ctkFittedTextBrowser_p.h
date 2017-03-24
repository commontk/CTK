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

#ifndef __ctkFittedTextBrowser_p_h
#define __ctkFittedTextBrowser_p_h

// CTK includes
#include "ctkFittedTextBrowser.h"

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkFittedTextBrowserPrivate
{
  Q_DECLARE_PUBLIC(ctkFittedTextBrowser);

protected:
  ctkFittedTextBrowser* const q_ptr;

public:
  ctkFittedTextBrowserPrivate(ctkFittedTextBrowser& object);
  virtual ~ctkFittedTextBrowserPrivate();

  // Update collapsed/expanded text in the widget.
  void updateCollapsedText();

  // Get collapsed/expanded text in html format from plain text.
  QString collapsedTextFromPlainText() const;
  // Get collapsed/expanded text in html format from html.
  QString collapsedTextFromHtml() const;

  // Get more/less link in html format
  QString collapseLinkText() const;

  bool Collapsed;

  QString ShowDetailsText;
  QString HideDetailsText;

  // Stores the text that the user originally set.
  QString CollapsibleText;

  enum CollapsibleTextSetMethod
  {
    Text,
    PlainText,
    Html
  };

  // Stores what method the user called to set text
  CollapsibleTextSetMethod CollapsibleTextSetter;
};

#endif
