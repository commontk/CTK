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


#ifndef CTKPLUGINGENERATORABSTRACTEXTENSION_H
#define CTKPLUGINGENERATORABSTRACTEXTENSION_H

#include <QObject>
#include <QHash>

#include <org_commontk_plugingenerator_core_Export.h>

class ctkPluginGeneratorAbstractExtensionPrivate;
class ctkPluginGeneratorCodeModel;

class org_commontk_plugingenerator_core_EXPORT ctkPluginGeneratorAbstractExtension : public QObject
{
  Q_OBJECT

public:
    ctkPluginGeneratorAbstractExtension();
    virtual ~ctkPluginGeneratorAbstractExtension();

    //virtual void getCommandLineArgs() const = 0;

    void setParameter(const QHash<QString, QVariant>& params);
    void setParameter(const QString& name, const QVariant& value);
    QHash<QString, QVariant> getParameter() const;

    bool isValid() const;
    void validate();
    void updateCodeModel();

    QString getErrorMessage() const;

    ctkPluginGeneratorCodeModel* getCodeModel() const;

Q_SIGNALS:

    void errorMessageChanged(const QString&);

protected:

    void setErrorMessage(const QString& errMsg);

    virtual bool verifyParameters(const QHash<QString, QVariant>& params) = 0;
    virtual void updateCodeModel(const QHash<QString, QVariant>& params) = 0;

private:

    Q_DECLARE_PRIVATE(ctkPluginGeneratorAbstractExtension)

    const QScopedPointer<ctkPluginGeneratorAbstractExtensionPrivate> d_ptr;

};

#endif // CTKPLUGINGENERATORABSTRACTEXTENSION_H
