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

#ifndef CTKCMDLINEMODULEBACKENDFUNCTIONPOINTER_H
#define CTKCMDLINEMODULEBACKENDFUNCTIONPOINTER_H

#include "ctkCmdLineModuleBackend.h"

#include "ctkCommandLineModulesBackendFunctionPointerExport.h"
#include "ctkCmdLineModuleBackendFPTypeTraits.h"
#include "ctkCmdLineModuleBackendFPUtil_p.h"

#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>
#include <QUrl>
#include <QMetaType>

#include <QDebug>


namespace ctk {
namespace CmdLineModuleBackendFunctionPointer {

struct FunctionPointerProxy;

template<typename T>
QString GetParameterTypeName();

struct ImageType {};

// default parameter description
template<typename T, typename Enable = void>
struct CreateXmlFor
{
  static QString parameter(int index, const QString& typeName, const QString& label = QString(), const QString& description = QString())
  {
    QString xmlParameter;
    QTextStream str(&xmlParameter);
    str << "    <" << typeName << ">\n";
    str << "      <name>" << QString("param%1").arg(index) << "</name>\n";
    str << "      <index>" << index << "</index>\n";
    str << "      <description>" << (description.isEmpty() ? "Description not available." : description) << "</description>\n";
    str << "      <label>" << (label.isEmpty() ? QString("Parameter %1").arg(index) : label) << "</label>\n";
    str << "    </" << typeName << ">\n";
    return xmlParameter;
  }
};

// specialization for input image types
template<typename T>
struct CreateXmlFor<T, typename EnableIf<IsBaseOf<ImageType, T>::value >::Type >
{
  static QString parameter(int index, const QString& typeName, const QString& label = QString(), const QString& description = QString())
  {
    QString xmlParameter;
    QTextStream str(&xmlParameter);
    str << "    <" << typeName << ">\n";
    str << "      <name>" << QString("param%1").arg(index) << "</name>\n";
    str << "      <index>" << index << "</index>\n";
    str << "      <description>" << (description.isEmpty() ? "Description not available." : description) << "</description>\n";
    str << "      <label>" << (label.isEmpty() ? QString("Parameter %1").arg(index) : label) << "</label>\n";
    str << "      <channel>input</channel>\n";
    str << "    </" << typeName << ">\n";
    return xmlParameter;
  }
};

}
}

Q_DECLARE_METATYPE(QList<int>*)

struct ctkCmdLineModuleBackendFunctionPointerPrivate;

/**
 * \class ctkCmdLineModuleBackendFunctionPointer
 * \brief Provides a back-end implementation to enable directly calling a function pointer.
 * \ingroup CommandLineModulesBackendFunctionPointer_API
 *
 * \warning This back-end is highly experimental and will not work for most function pointers when
 *          trying to register them via registerFunctionPointer().
 */
class CTK_CMDLINEMODULEBACKENDFP_EXPORT ctkCmdLineModuleBackendFunctionPointer : public ctkCmdLineModuleBackend
{

public:

  class DescriptionPrivate;

  class Description
  {
  public:

    Description();
    ~Description();

    QUrl moduleLocation() const;

    QString moduleCategory() const;
    void setModuleCategory(const QString &category);

    QString moduleTitle() const;
    void setModuleTitle(const QString& title);

    QString moduleDescription() const;
    void setModuleDescription(const QString& description);

    QString moduleVersion() const;
    void setModuleVersion(const QString& version);

    QString moduleContributor() const;
    void setModuleContributor(const QString& contributor);

  private:

    friend class ctkCmdLineModuleBackendFunctionPointer;
    friend class ctkCmdLineModuleFunctionPointerTask;
    Description(const QUrl& location, const ctk::CmdLineModuleBackendFunctionPointer::FunctionPointerProxy& fpProxy);

    QSharedPointer<DescriptionPrivate> d;

  };

  ctkCmdLineModuleBackendFunctionPointer();
  ~ctkCmdLineModuleBackendFunctionPointer();

  virtual QString name() const;
  virtual QString description() const;

  virtual QList<QString> schemes() const;

  virtual qint64 timeStamp(const QUrl &location) const;

  virtual QByteArray rawXmlDescription(const QUrl& location, int timeout);

  QList<QUrl> registeredFunctionPointers() const;

  template<typename A>
  Description* registerFunctionPointer(const QString& title, void (*fp)(A),
                                       const QString& paramLabel = QString(), const QString& paramDescr = QString())
  {
    typedef typename ctk::CmdLineModuleBackendFunctionPointer::TypeTraits<A>::RawType RawTypeA;

    QList<QString> params;
    params << ctk::CmdLineModuleBackendFunctionPointer::CreateXmlFor<RawTypeA>::
              parameter(0,
                        ctk::CmdLineModuleBackendFunctionPointer::GetParameterTypeName<RawTypeA>(),
                        paramLabel, paramDescr);
    return this->registerFunctionPointerProxy(title, ctk::CmdLineModuleBackendFunctionPointer::FunctionPointerProxy(fp), params);
  }

  template<typename A, typename B>
  Description* registerFunctionPointer(const QString& title, void (*fp)(A,B),
                                       const QString& paramLabel0 = QString(), const QString& paramDescr0 = QString(),
                                       const QString& paramLabel1 = QString(), const QString& paramDescr1 = QString())
  {
    typedef typename ctk::CmdLineModuleBackendFunctionPointer::TypeTraits<A>::RawType RawTypeA;
    typedef typename ctk::CmdLineModuleBackendFunctionPointer::TypeTraits<B>::RawType RawTypeB;

    QList<QString> params;
    params << ctk::CmdLineModuleBackendFunctionPointer::CreateXmlFor<RawTypeA>::
              parameter(0,
                        ctk::CmdLineModuleBackendFunctionPointer::GetParameterTypeName<RawTypeA>(),
                        paramLabel0, paramDescr0);
    params << ctk::CmdLineModuleBackendFunctionPointer::CreateXmlFor<RawTypeB>::
              parameter(1,
                        ctk::CmdLineModuleBackendFunctionPointer::GetParameterTypeName<RawTypeB>(),
                        paramLabel1, paramDescr1);
    return this->registerFunctionPointerProxy(title, ctk::CmdLineModuleBackendFunctionPointer::FunctionPointerProxy(fp), params);
  }

protected:

  virtual ctkCmdLineModuleFuture run(ctkCmdLineModuleFrontend* frontend);

  virtual QList<QVariant> arguments(ctkCmdLineModuleFrontend* frontend) const;

private:

  Description* registerFunctionPointerProxy(const QString &title,
                                            const ctk::CmdLineModuleBackendFunctionPointer::FunctionPointerProxy& proxy,
                                            const QList<QString>& params);


  QScopedPointer<ctkCmdLineModuleBackendFunctionPointerPrivate> d;

};


#endif // CTKCMDLINEMODULEBACKENDFUNCTIONPOINTER_H
