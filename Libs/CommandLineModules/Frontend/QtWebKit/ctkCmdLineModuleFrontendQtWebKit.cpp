/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkCmdLineModuleFrontendQtWebKit_p.h"

#include "ctkCmdLineModuleXslTransform.h"
#include "ctkCmdLineModuleReference.h"

#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QBuffer>
#include <QFile>

#include <QDebug>

//----------------------------------------------------------------------------
ctkCmdLineModuleFrontendQtWebKit::ctkCmdLineModuleFrontendQtWebKit(const ctkCmdLineModuleReference& moduleRef)
  : ctkCmdLineModuleFrontend(moduleRef)
  , WebView(NULL)
{

}

//----------------------------------------------------------------------------
QObject* ctkCmdLineModuleFrontendQtWebKit::guiHandle() const
{
  if (WebView) return WebView;

  QBuffer input;
  input.setData(moduleReference().rawXmlDescription());

  QBuffer htmlOutput;
  htmlOutput.open(QIODevice::ReadWrite);
  ctkCmdLineModuleXslTransform xslTransform(&input, &htmlOutput);
  QFile htmlTransformation(":/ctkCmdLineModuleXmlToPlainHtml.xsl");

  xslTransform.setXslTransformation(&htmlTransformation);
  if (!xslTransform.transform())
  {
    // maybe throw an exception
    qCritical() << xslTransform.errorString();
    return 0;
  }

  this->WebView = new QWebView;
  QByteArray htmlContent = htmlOutput.readAll();
  this->WebView->setHtml(htmlContent);
  return this->WebView;
}

//----------------------------------------------------------------------------
QVariant ctkCmdLineModuleFrontendQtWebKit::value(const QString &parameter, int role) const
{
  Q_UNUSED(role)
  QWebElement webElement = this->WebView->page()->currentFrame()->findFirstElement("input[name=" + parameter + "]");
  if (webElement.isNull()) return QVariant();
  // Work around bug https://bugs.webkit.org/show_bug.cgi?id=32865 for input elements
  QVariant value = webElement.evaluateJavaScript("this.value");
  qDebug() << "Found element" << webElement.tagName() << "with value" << value;
  return value;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtWebKit::setValue(const QString &parameter, const QVariant &value, int role)
{
  if (!this->WebView || role != DisplayRole) return;

  QWebElement webElement = this->WebView->page()->currentFrame()->findFirstElement("input[name=" + parameter + "]");
  if (webElement.isNull()) return;

  // Work around bug https://bugs.webkit.org/show_bug.cgi?id=32865 for input elements
  webElement.evaluateJavaScript(QString("this.value='%1'").arg(value.toString()));
}
