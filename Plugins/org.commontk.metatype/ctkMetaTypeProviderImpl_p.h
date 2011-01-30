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


#ifndef CTKMETATYPEPROVIDERIMPL_P_H
#define CTKMETATYPEPROVIDERIMPL_P_H

#include <service/metatype/ctkMetaTypeProvider.h>

class ctkPlugin;
struct ctkLogService;
class ctkObjectClassDefinitionImpl;

/**
 * Implementation of ctkMetaTypeProvider
 */
class ctkMetaTypeProviderImpl : public ctkMetaTypeProvider
{

public:

  static const QString METADATA_NOT_FOUND; // = "METADATA_NOT_FOUND"
  static const QString OCD_ID_NOT_FOUND; // = "OCD_ID_NOT_FOUND"
  static const QString ASK_INVALID_LOCALE; // = "ASK_INVALID_LOCALE"

  static const QString META_FILE_EXT; // = ".XML";
  static const QString RESOURCE_FILE_CONN; // = "_"
  static const QString RESOURCE_FILE_EXT; // = ".qm"
  static const QChar DIRECTORY_SEP; // = '/'

protected:

  QSharedPointer<ctkPlugin> _plugin;

  QHash<QString, QSharedPointer<ctkObjectClassDefinitionImpl> > _allPidOCDs;
  QHash<QString, QSharedPointer<ctkObjectClassDefinitionImpl> > _allFPidOCDs;

  ctkLogService* logger;

private:

  mutable QList<QLocale> _locales;
  bool _isThereMeta;

  friend class ctkMetaTypeServiceImpl;

public:

  /**
   * Constructor of class MetaTypeProviderImpl.
   */
  ctkMetaTypeProviderImpl(const QSharedPointer<ctkPlugin>& plugin, ctkLogService* logger);

  /*
   * @see ctkMetaTypeProvider#getObjectClassDefinition(const QString&, const QLocale&)
   */
  ctkObjectClassDefinitionPtr getObjectClassDefinition(const QString& pid, const QLocale& locale);

  /*
   * @see ctkMetaTypeProvider#getLocales()
   */
  QList<QLocale> getLocales() const;

private:

  /**
   * This method should do the following:
   *
   * <p> - Pass the XML Parser the location for the plugin's METADATA.XML file
   * <p> - Handle the callbacks from the parser and build the appropriate
   * MetaType objects - ObjectClassDefinitions & AttributeDefinitions
   *
   * @param plugin The plugin object for which the metadata should be read
   * @return void
   */
  bool readMetaFiles(const QSharedPointer<ctkPlugin>& plugin);

  /**
   * Internal Method - checkForDefault
   */
  QList<QLocale> checkForDefault(const QList<QLocale>& locales) const;

};

#endif // CTKMETATYPEPROVIDERIMPL_P_H
