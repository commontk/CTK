/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

#ifndef __ctkIconEnginePlugin_h
#define __ctkIconEnginePlugin_h

// Qt includes
#include <QIconEngineV2>
#include <QIconEnginePluginV2>

// CTK includes
#include "ctkPimpl.h"
#include "ctkPixmapIconEngine.h"
#include "CTKWidgetsExport.h"

class ctkIconEnginePluginPrivate;
class ctkIconEnginePrivate;

/// if you want to use it in your application, don't forget to declare
/// Q_EXPORT_PLUGIN2(yourplugin, ctkIconEnginePlugin)
/// Currently
class CTK_WIDGETS_EXPORT ctkIconEnginePlugin: public QIconEnginePluginV2
{
  Q_OBJECT;
public:
  ctkIconEnginePlugin(QObject* parent = 0);

  virtual QIconEngineV2* create(const QString& filename=QString());
  /// Support all the Qt image formats by default
  virtual QStringList keys()const;

  /// Directory list given to the created icon engines
  /// Subdirectories where the icons should be searched, typically:
  /// "Small", "Medium", "Large", "XLarge" or
  /// "16x16", "32x32", "64x64", "128x128" or
  /// "LowDef", "HighDef"
  /// \sa ctkIconEnginePlugin::setSizeDirectories
  void setSizeDirectories(const QStringList& sizeDirectories);
  QStringList sizeDirectories()const;
private:
  CTK_DECLARE_PRIVATE(ctkIconEnginePlugin);
};

//------------------------------------------------------------------------------
/// Only support files in ressources.
/// TODO: support any files (not just files in resources)
class CTK_WIDGETS_EXPORT ctkIconEngine: public ctkPixmapIconEngine
{
public:
  typedef ctkPixmapIconEngine Superclass;
  ctkIconEngine();
  virtual void addFile(const QString& fileName, const QSize& size,
                       QIcon::Mode mode, QIcon::State state);
  /// Subdirectories where the icons should be searched, typically:
  /// "Small", "Medium", "Large", "XLarge" or
  /// "16x16", "32x32", "64x64", "128x128" or
  /// "LowDef", "HighDef"
  void setSizeDirectories(const QStringList& sizeDirectories);
  QStringList sizeDirectories()const;

private:
  CTK_DECLARE_PRIVATE(ctkIconEngine);
};

#endif
