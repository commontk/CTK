/*=============================================================================

  Plugin: org.commontk.xnat

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

#ifndef ctkXnatScanResourceFile_h
#define ctkXnatScanResourceFile_h

#include "ctkXNATExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;
class ctkXnatScanResourceFilePrivate;

class CTK_XNAT_EXPORT ctkXnatScanResourceFile : public ctkXnatObject
{
  Q_OBJECT

  Q_PROPERTY(QString Name READ name WRITE setName)
  Q_PROPERTY(QString Size READ size WRITE setSize)
  Q_PROPERTY(QString URI READ uri WRITE setUri)
  Q_PROPERTY(QString collection READ collection WRITE setCollection)
  Q_PROPERTY(QString file_tags READ fileTags WRITE setFileTags)
  Q_PROPERTY(QString file_format READ fileFormat WRITE setFileFormat)
  Q_PROPERTY(QString file_content READ fileContent WRITE setFileContent)
  Q_PROPERTY(QString cat_ID READ categoryId WRITE setCategoryId)

public:
  explicit ctkXnatScanResourceFile(ctkXnatObject* parent = 0);
  virtual ~ctkXnatScanResourceFile();

  const QString& name() const;
  void setName(const QString& name);

  const QString& size() const;
  void setSize(const QString& size);

  const QString& uri() const;
  void setUri(const QString& uri);

  const QString& collection() const;
  void setCollection(const QString& collection);

  const QString& fileTags() const;
  void setFileTags(const QString& fileTags);

  const QString& fileFormat() const;
  void setFileFormat(const QString& fileFormat);

  const QString& fileContent() const;
  void setFileContent(const QString& fileContent);

  const QString& categoryId() const;
  void setCategoryId(const QString& categoryId);

  virtual void download(ctkXnatConnection* connection, const QString& zipFilename);

  virtual bool isFile() const;
  virtual bool isDeletable() const;

private:
  QScopedPointer<ctkXnatScanResourceFilePrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatScanResourceFile);
  Q_DISABLE_COPY(ctkXnatScanResourceFile);
};

#endif
