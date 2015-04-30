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

#ifndef CTKPLUGINFRAMEWORKDEBUGOPTIONS_H
#define CTKPLUGINFRAMEWORKDEBUGOPTIONS_H

#include <service/debug/ctkDebugOptions.h>
#include <service/debug/ctkDebugOptionsListener.h>

#include <ctkServiceTracker.h>
#include <ctkServiceTrackerCustomizer.h>

/**
 * Used to get debug options settings.
 */
class ctkPluginFrameworkDebugOptions : public QObject, public ctkDebugOptions,
    private ctkServiceTrackerCustomizer<ctkDebugOptionsListener*>
{
  Q_OBJECT
  Q_INTERFACES(ctkDebugOptions)

public:

  ctkPluginFrameworkDebugOptions();

  static ctkPluginFrameworkDebugOptions* getDefault();

  void start(ctkPluginContext* pc);
  void stop(ctkPluginContext* pc);

  bool getBooleanOption(const QString& option, bool defaultValue) const;
  QVariant getOption(const QString& option) const;
  QVariant getOption(const QString& option, const QVariant& defaultValue) const;

  int getIntegerOption(const QString& option, int defaultValue) const;

  QHash<QString, QVariant> getOptions() const;

  void setOption(const QString& option, const QVariant& value);

  void setOptions(const QHash<QString, QVariant>& ops);

  void removeOption(const QString& option);

  bool isDebugEnabled() const;

  void setDebugEnabled(bool enabled);

private:

  static const QString CTK_DEBUG;

  /** mutex used to lock the options maps */
  mutable QMutex mutex;
  /** A boolean value indicating if debug was enabled */
  bool enabled;
  /** A current map of all the options with values set */
  QHash<QString, QVariant> options;
  /** A map of all the disabled options with values set at the time debug was disabled */
  QHash<QString, QVariant> disabledOptions;
  /** The singleton object of this class */
  //static Impl* singleton = nullptr;
  /** The default name of the .options file if loading when the -debug command-line argument is used */
  static const QString OPTIONS;

  ctkPluginContext* context;
  QScopedPointer<ctkServiceTracker<ctkDebugOptionsListener*> > listenerTracker;

  QString getSymbolicName(const QString& option) const;

  /**
   * Notifies the trace listener for the specified plug-in that its option-path has changed.
   * @param pluginSymbolicName The plug-in of the owning trace listener to notify.
   */
  void optionsChanged(const QString& bundleSymbolicName);

  ctkDebugOptionsListener* addingService(const ctkServiceReference& reference);
  void modifiedService(const ctkServiceReference& reference,
                       ctkDebugOptionsListener* service);
  void removedService(const ctkServiceReference& reference,
                     ctkDebugOptionsListener* service);

};

#endif // CTKPLUGINFRAMEWORKDEBUGOPTIONS_H
