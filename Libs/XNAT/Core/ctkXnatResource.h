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

#ifndef ctkXnatResource_h
#define ctkXnatResource_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"
#include "ctkXnatDefaultSchemaTypes.h"

class ctkXnatResourcePrivate;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatResource : public ctkXnatObject
{

public:

  ctkXnatResource(ctkXnatObject* parent = 0,
                      const QString& schemaType = ctkXnatDefaultSchemaTypes::XSI_RESOURCE);

  virtual ~ctkXnatResource();

  virtual QString resourceUri() const;

  virtual QString id() const;
  virtual void setId(const QString &id);

  virtual QString name() const;
  virtual void setName(const QString &name);

  /// Gets the label of the object.
  QString label() const;

  /// Sets the label of the object.
  void setLabel(const QString& label);

  void setFormat(const QString& format);
  QString format() const;

  void setTags(const QString& tags);
  QString tags() const;

  void setContent(const QString& content);
  QString content() const;

  void reset();

  void saveImpl(bool overwrite);

  static const QString ID;
  static const QString TAGS;
  static const QString FORMAT;
  static const QString CONTENT;

private:

  friend class qRestResult;

  virtual void fetchImpl();

  virtual void downloadImpl(const QString&);

  Q_DECLARE_PRIVATE(ctkXnatResource)

};

#endif
