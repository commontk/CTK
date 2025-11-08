/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkIconEnginePlugin_h
#define __ctkIconEnginePlugin_h

// Qt includes
# include <QIconEngine>
# include <QIconEnginePlugin>

// CTK includes
#include "ctkPimpl.h"
#include "ctkPixmapIconEngine.h"
#include "ctkWidgetsExport.h"

class ctkIconEnginePluginPrivate;
class ctkIconEnginePrivate;

/// \ingroup Widgets
/// ctkIconEnginePlugin must be loaded when starting the application.
/// \code
/// QApplication myApp;
/// QCoreApplication::addLibraryPath("MyApp-build/plugins");
/// \endcode
/// where the plugin must be located in "MyApp-build/plugins/iconengines"
class CTK_WIDGETS_EXPORT ctkIconEnginePlugin
  : public QIconEnginePlugin
{
  Q_OBJECT;
public:
  ctkIconEnginePlugin(QObject* parent = 0);
  virtual ~ctkIconEnginePlugin();

  virtual QIconEngine* create(const QString& filename=QString());

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

protected:
  QScopedPointer<ctkIconEnginePluginPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkIconEnginePlugin);
  Q_DISABLE_COPY(ctkIconEnginePlugin);
};

//------------------------------------------------------------------------------
/// \ingroup Widgets
/// ctkIconEngine is an icon engine that behaves like the default Qt icon engine
/// QPixmapIconEngine(ctkPixmapIconEngine)), but can automatically support icons
/// in multiple size. When adding a file to an icon, it will automatically check
/// if the same file name exists in a different directory. This allows the
/// application to contains icons in different size,e.g. :/Icons/Small/edit.png
/// and :/Icons/Large/edit.png.
/// Without ctkIconEngine, QIcon already support mutltiple files:
/// \code
///  QIcon editIcon;
///  editIcon.addFile(":/Icons/Small/edit.png");
///  editIcon.addFile(":/Icons/Large/edit.png");
/// \endcode
/// Using ctkIconEngine, adding a file to an icon will automatically search for
/// any icon in a different directory:
/// \code
///  ctkIconEngine* autoIconEngine;
///  autoIconEngine->setSizeDirectories(QStringList() << "Large" << "Small";
///  QIcon editIcon(autoIconEngine);
///  editIcon.addFile(":/Icons/Small/edit.png");
/// \endcode
/// where the large version of the icon is automatically added.
/// It is mostly useful when using the designer, where only 1 icon file can
/// be specified. It must be used with ctkIconEnginePlugin
/// TODO: support more than just files in resources.
class CTK_WIDGETS_EXPORT ctkIconEngine: public ctkPixmapIconEngine
{
public:
  typedef ctkPixmapIconEngine Superclass;
  ctkIconEngine();
  virtual ~ctkIconEngine();
  virtual void addFile(const QString& fileName, const QSize& size,
                       QIcon::Mode mode, QIcon::State state);
  /// Subdirectories where the icons should be searched, typically:
  /// "Small", "Medium", "Large", "XLarge" or
  /// "16x16", "32x32", "64x64", "128x128" or
  /// "LowDef", "HighDef"
  void setSizeDirectories(const QStringList& sizeDirectories);
  QStringList sizeDirectories()const;

  virtual QString key()const;

protected:
  QScopedPointer<ctkIconEnginePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkIconEngine);
  Q_DISABLE_COPY(ctkIconEngine);
};
#endif
