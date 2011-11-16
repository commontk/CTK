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

#ifndef CTKPLUGINGENERATORABSTRACTUIEXTENSION_H
#define CTKPLUGINGENERATORABSTRACTUIEXTENSION_H

#include <QObject>
#include <QIcon>

#include <ctkPluginGeneratorAbstractExtension.h>

#include <org_commontk_plugingenerator_ui_Export.h>

class QWidget;
class ctkPluginGeneratorAbstractUiExtensionPrivate;

class org_commontk_plugingenerator_ui_EXPORT ctkPluginGeneratorAbstractUiExtension : public ctkPluginGeneratorAbstractExtension
{

  Q_OBJECT

public:

  ctkPluginGeneratorAbstractUiExtension();
  virtual ~ctkPluginGeneratorAbstractUiExtension();

  QWidget* getWidget();

  QString getDescription() const;
  QString getTitle() const;

  QString getMessage() const;

  QIcon getIcon() const;

Q_SIGNALS:

  void descriptionChanged(const QString&);
  void titleChanged(const QString&);
  void messageChanged(const QString&);
  void iconChanged(const QIcon&);

protected:

  virtual QWidget* createWidget() = 0;

  void setDescription(const QString& description);
  void setTitle(const QString& title);

  void setMessage(const QString& msg);

  void setIcon(const QIcon& icon);

private:

  Q_DECLARE_PRIVATE(ctkPluginGeneratorAbstractUiExtension)

  const QScopedPointer<ctkPluginGeneratorAbstractUiExtensionPrivate> d_ptr;

};

#endif // CTKPLUGINGENERATORABSTRACTUIEXTENSION_H
