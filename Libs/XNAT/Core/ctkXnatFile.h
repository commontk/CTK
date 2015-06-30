/*=============================================================================

  Library: XNAT/Core

  Copyright (c) University College London,
    Centre for Medical Image Computing

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

#ifndef ctkXnatFile_h
#define ctkXnatFile_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"
#include "ctkXnatDefaultSchemaTypes.h"

class ctkXnatConnection;
class ctkXnatFilePrivate;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatFile : public ctkXnatObject
{

public:

  ctkXnatFile(ctkXnatObject* parent = 0, const QString& schemaType = ctkXnatDefaultSchemaTypes::XSI_FILE);

  virtual ~ctkXnatFile();

  virtual QString resourceUri() const;

  virtual void setName(const QString &name);
  virtual QString name() const;

  void setFileFormat(const QString& fileFormat);
  QString fileFormat() const;

  void setFileTags(const QString& fileTags);
  QString fileTags() const;

  void setFileContent(const QString& fileContent);
  QString fileContent() const;

  void setLocalFilePath(const QString& filepath);
  QString localFilePath() const;

  void reset();

  static const QString FILE_NAME;
  static const QString FILE_TAGS;
  static const QString FILE_FORMAT;
  static const QString FILE_CONTENT;

private:

  virtual void fetchImpl();

  virtual void downloadImpl(const QString&);

  /**
    * @brief Uploads the file to the server
    * Before calling save() the localFilePath has to be set
    */
  virtual void saveImpl(bool overwrite);

  Q_DECLARE_PRIVATE(ctkXnatFile)
};

#endif
