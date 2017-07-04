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

#ifndef CTKCMDLINEMODULEFRONTENDQTWEBKIT_H
#define CTKCMDLINEMODULEFRONTENDQTWEBKIT_H

#include "ctkCmdLineModuleFrontend.h"

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5,6,0)
class QWebView;
#else
class QWebEngineView;
#endif

/**
 * \class ctkCmdLineModuleFrontendQtWebKit
 * \brief QtWebKit specific implementation of ctkCmdLineModuleFrontend
 * \ingroup CommandLineModulesFrontendQtWebKit_API
 */
class ctkCmdLineModuleFrontendQtWebKit : public ctkCmdLineModuleFrontend
{

public:

  ctkCmdLineModuleFrontendQtWebKit(const ctkCmdLineModuleReference& moduleRef);

  // ctkCmdLineModuleFrontend overrides

  virtual QObject* guiHandle() const;

  virtual QVariant value(const QString& parameter, int role = LocalResourceRole) const;
  virtual void setValue(const QString& parameter, const QVariant& value, int role = DisplayRole);

  //virtual QList<QString> parameterNames() const;

private:
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
  mutable QWebView* WebView;
#else
  mutable QWebEngineView* WebView;
#endif
};

#endif // CTKCMDLINEMODULEFRONTENDQTWEBKIT_H
