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


#include "ctkMetaTypeProviderImpl_p.h"

#include "ctkObjectClassDefinitionImpl_p.h"
#include "ctkAttributeDefinitionImpl_p.h"
#include "ctkMTMsg_p.h"
#include "ctkMTDataParser_p.h"

#include <ctkPluginConstants.h>
#include <ctkException.h>
#include <service/log/ctkLogService.h>
#include <service/metatype/ctkMetaTypeService.h>

#include <QCoreApplication>
#include <QStringList>
#include <QBuffer>

const QString ctkMetaTypeProviderImpl::METADATA_NOT_FOUND = "METADATA_NOT_FOUND";
const QString ctkMetaTypeProviderImpl::OCD_ID_NOT_FOUND = "OCD_ID_NOT_FOUND";
const QString ctkMetaTypeProviderImpl::ASK_INVALID_LOCALE = "ASK_INVALID_LOCALE";

const QString ctkMetaTypeProviderImpl::META_FILE_EXT = ".XML";
const QString ctkMetaTypeProviderImpl::RESOURCE_FILE_CONN = "_";
const QString ctkMetaTypeProviderImpl::RESOURCE_FILE_EXT = ".qm";
const QChar ctkMetaTypeProviderImpl::DIRECTORY_SEP = '/';


ctkMetaTypeProviderImpl::ctkMetaTypeProviderImpl(
  const QSharedPointer<ctkPlugin>& plugin, ctkLogService* logger)
  : _plugin(plugin), logger(logger), _isThereMeta(false)
{
  // read all plugin's metadata files and build internal data structures
  _isThereMeta = readMetaFiles(plugin);

  if (!_isThereMeta)
  {
    CTK_DEBUG(logger) << QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::METADATA_NOT_FOUND)
                         .arg(plugin->getPluginId()).arg(plugin->getSymbolicName());
  }
}

ctkObjectClassDefinitionPtr ctkMetaTypeProviderImpl::getObjectClassDefinition(
  const QString& pid, const QLocale& locale)
{
  ctkObjectClassDefinitionImplPtr ocd;
  if (_allPidOCDs.contains(pid))
  {
    ocd = ctkObjectClassDefinitionImplPtr(new ctkObjectClassDefinitionImpl(*_allPidOCDs.value(pid).data()));
    ocd->setPluginLocalization(locale, _plugin);
    return ocd;
  }
  else if (_allFPidOCDs.contains(pid))
  {
    ocd = ctkObjectClassDefinitionImplPtr(new ctkObjectClassDefinitionImpl(*_allFPidOCDs.value(pid).data()));
    ocd->setPluginLocalization(locale, _plugin);
    return ocd;
  }
  else
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::OCD_ID_NOT_FOUND).arg(pid);
    throw ctkInvalidArgumentException(msg);
  }
}

QList<QLocale> ctkMetaTypeProviderImpl::getLocales() const
{
  if (!_locales.isEmpty())
    return checkForDefault(_locales);

  QStringList localizationFiles;
  // get all the localization resources for PIDS
  foreach(ctkObjectClassDefinitionImplPtr ocd, _allPidOCDs)
  {
    QString loc = ocd->getLocalization();
    if (!loc.isEmpty() && !localizationFiles.contains(loc))
    {
      localizationFiles << loc;
    }
  }
  // get all the localization resources for FPIDS
  foreach(ctkObjectClassDefinitionImplPtr ocd, _allFPidOCDs)
  {
    QString loc = ocd->getLocalization();
    if (!loc.isEmpty() && !localizationFiles.contains(loc))
    {
      localizationFiles << loc;
    }
  }

  if (localizationFiles.isEmpty())
  {
    localizationFiles << ctkPluginConstants::PLUGIN_LOCALIZATION_DEFAULT_BASENAME;
  }

  QList<QLocale> locales;
  foreach (QString localizationFile, localizationFiles)
  {
    int iSlash = localizationFile.lastIndexOf(DIRECTORY_SEP);
    QString baseDir;
    QString baseFileName;
    if (iSlash < 0)
    {
      baseDir = "";
    }
    else
    {
      baseDir = localizationFile.left(iSlash);
    }
    baseFileName = localizationFile + RESOURCE_FILE_CONN;
    QStringList resources = _plugin->getResourceList(baseDir);
    foreach(QString resourceName, resources)
    {
      QString resource = baseDir + DIRECTORY_SEP + resourceName;
      if (resource.startsWith(baseFileName) && resource.toLower().endsWith(RESOURCE_FILE_EXT))
      {
        QString localeName = resource.mid(baseFileName.size(), resource.size() - baseFileName.size() - RESOURCE_FILE_EXT.length());
        locales.push_back(QLocale(localeName));
      }
    }
  }
  _locales = locales;
  return checkForDefault(_locales);
}

bool ctkMetaTypeProviderImpl::readMetaFiles(const QSharedPointer<ctkPlugin>& plugin)
{
  bool isThereMetaHere = false;

  QStringList allFileKeys = plugin->getResourceList(ctkMetaTypeService::METATYPE_DOCUMENTS_LOCATION);
  if (allFileKeys.isEmpty())
    return isThereMetaHere;

  foreach (QString fileName, allFileKeys)
  {
    bool _isMetaDataFile = false;

    QHash<QString, ctkObjectClassDefinitionImplPtr> pidToOCD;
    QByteArray resourceContent = plugin->getResource(ctkMetaTypeService::METATYPE_DOCUMENTS_LOCATION + "/" + fileName);
    if (!resourceContent.isEmpty())
    {
      QBuffer metaData(&resourceContent);
      try
      {
        // Assume all XML files are what we want by default.
        _isMetaDataFile = true;
        metaData.open(QIODevice::ReadOnly);
        ctkMTDataParser parser(plugin, &metaData, logger);
        pidToOCD = parser.doParse();
        if (pidToOCD.isEmpty())
        {
          _isMetaDataFile = false;
        }
      }
      catch (const std::exception& )
      {
        // Ok, looks like it is not what we want.
        _isMetaDataFile = false;
      }

      if (_isMetaDataFile && !pidToOCD.isEmpty())
      {
        // We got some OCDs now.
        QHash<QString, ctkObjectClassDefinitionImplPtr>::ConstIterator end(pidToOCD.end());
        for (QHash<QString, ctkObjectClassDefinitionImplPtr>::ConstIterator it(pidToOCD.begin()); it != end; ++it)
        {
          QString pid = it.key();
          ctkObjectClassDefinitionImplPtr ocd = it.value();
          if (ocd->getType() == ctkObjectClassDefinitionImpl::PID)
          {
            isThereMetaHere = true;
            _allPidOCDs.insert(pid, ocd);
          }
          else
          {
            isThereMetaHere = true;
            _allFPidOCDs.insert(pid, ocd);
          }
        } // End of for
      }
    } // End of if(!resourceContent.isEmpty())
  } // End of foreach

  return isThereMetaHere;
}

QList<QLocale> ctkMetaTypeProviderImpl::checkForDefault(const QList<QLocale>& locales) const
{
  if (locales.isEmpty() || (locales.size() == 1 && QLocale() == locales[0]))
    return QList<QLocale>();
  return locales;
}
