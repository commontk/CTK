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

#include "ctkPluginFrameworkDebugOptions_p.h"

#include <ctkPluginContext.h>
#include <ctkException.h>

#include "ctkPluginFrameworkLauncher.h"
#include "ctkPluginFrameworkProperties_p.h"

#include <QSettings>
#include <QMutex>
#include <QDir>

const QString ctkPluginFrameworkDebugOptions::CTK_DEBUG = "ctk.debug";
const QString ctkPluginFrameworkDebugOptions::OPTIONS = ".ctk.options";


//----------------------------------------------------------------------------
ctkPluginFrameworkDebugOptions::ctkPluginFrameworkDebugOptions()
  : enabled(false)
  , context(NULL)
{
  // if no debug option was specified, don't even bother to try.
  // Must ensure that the options slot is null as this is the signal to the
  // platform that debugging is not enabled.
  QVariant debugOptions = ctkPluginFrameworkProperties::getProperty(CTK_DEBUG);
  if (debugOptions.isNull()) return;

  QString debugOptionsFilename = debugOptions.toString();

  if (debugOptionsFilename.isEmpty())
  {
    // default options location is user.dir (install location may be r/o so
    // is not a good candidate for a trace options that need to be updatable by
    // by the user)
    QDir userDir(ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_USER_DIR).toString());
    debugOptionsFilename = userDir.absoluteFilePath(OPTIONS);
  }
  QFile optionsFile(debugOptionsFilename);
  QString msgState;
  if (!optionsFile.exists())
  {
    msgState = "not found";
  }
  else
  {
    QSettings settings(debugOptionsFilename, QSettings::IniFormat);
    if (settings.status() != QSettings::NoError)
    {
      msgState = ".... did not parse";
    }
    else
    {
      foreach (const QString& key, settings.allKeys())
      {
        this->options.insert(key, settings.value(key));
      }
      this->enabled = true;
    }
  }
  qDebug() << "Debug options:\n    " << optionsFile.fileName() << "    " << msgState;
}

//----------------------------------------------------------------------------
ctkPluginFrameworkDebugOptions*ctkPluginFrameworkDebugOptions::getDefault()
{
  static ctkPluginFrameworkDebugOptions singleton;
  return &singleton;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebugOptions::start(ctkPluginContext* pc)
{
  this->context = pc;
  this->listenerTracker.reset(new ctkServiceTracker<ctkDebugOptionsListener*>(pc, this));
  this->listenerTracker->open();
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebugOptions::stop(ctkPluginContext* /*pc*/)
{
  this->listenerTracker->close();
  this->listenerTracker.reset();
  this->context = NULL;
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkDebugOptions::getBooleanOption(const QString& option, bool defaultValue) const
{
  return this->getOption(option, defaultValue).toBool();
}

//----------------------------------------------------------------------------
QVariant ctkPluginFrameworkDebugOptions::getOption(const QString& option) const
{
  return getOption(option, QVariant());
}

//----------------------------------------------------------------------------
QVariant ctkPluginFrameworkDebugOptions::getOption(const QString& option, const QVariant& defaultValue) const
{
  QMutexLocker lock(&this->mutex);
  return this->options.value(option, defaultValue);
}

//----------------------------------------------------------------------------
int ctkPluginFrameworkDebugOptions::getIntegerOption(const QString& option, int defaultValue) const
{
  return this->options.value(option, defaultValue).toInt();
}

//----------------------------------------------------------------------------
QHash<QString, QVariant> ctkPluginFrameworkDebugOptions::getOptions() const
{
  QHash<QString, QVariant> snapShot;
  {
    QMutexLocker lock(&this->mutex);
    if (this->isDebugEnabled())
    {
      snapShot = this->options;
    }
    else
    {
      snapShot = this->disabledOptions;
    }
  }
  return snapShot;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebugOptions::setOption(const QString& option, const QVariant& value)
{
  if (!this->isDebugEnabled()) return;

  QString fireChangedEvent;
  {
    QMutexLocker lock(&this->mutex);
    // get the current value
    QHash<QString, QVariant>::iterator currentValue = this->options.find(option);
    if (currentValue != this->options.end())
    {
      if (currentValue.value() != value)
      {
        fireChangedEvent = this->getSymbolicName(option);
      }
    }
    else
    {
      if (!value.isNull())
      {
        fireChangedEvent = this->getSymbolicName(option);
      }
    }
    if (!fireChangedEvent.isEmpty())
    {
      this->options.insert(option, value);
    }
  }
  // Send the options change event outside the sync block
  if (!fireChangedEvent.isEmpty())
  {
    this->optionsChanged(fireChangedEvent);
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebugOptions::setOptions(const QHash<QString, QVariant>& ops)
{
  QHash<QString, QVariant> newOptions = ops;
  QSet<QString> fireChangesTo;
  {
    QMutexLocker lock(&this->mutex);
    if (!this->isDebugEnabled())
    {
      this->disabledOptions = newOptions;
      // no events to fire
      return;
    }
    // first check for removals
    foreach (const QString& key, this->options.keys())
    {
      if (!newOptions.contains(key))
      {
        QString symbolicName = this->getSymbolicName(key);
        if (!symbolicName.isEmpty())
        {
          fireChangesTo.insert(symbolicName);
        }
      }
    }
    // now check for changes to existing values
    for(QHash<QString, QVariant>::iterator iter = newOptions.begin(); iter != newOptions.end(); ++iter)
    {
      QVariant existingValue = this->options.value(iter.key());
      if (iter.value() != existingValue)
      {
        QString symbolicName = this->getSymbolicName(iter.key());
        if (!symbolicName.isEmpty())
        {
          fireChangesTo.insert(symbolicName);
        }
      }
    }
    // finally set the actual options
    this->options = newOptions;
  }
  foreach (const QString& symbolicName, fireChangesTo)
  {
    this->optionsChanged(symbolicName);
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebugOptions::removeOption(const QString& option)
{
  if (!this->isDebugEnabled()) return;
  QString fireChangedEvent;
  {
    QMutexLocker lock(&this->mutex);
    if (this->options.remove(option))
    {
      fireChangedEvent = this->getSymbolicName(option);
    }
  }
  // Send the options change event outside the sync block
  if (!fireChangedEvent.isEmpty())
  {
    this->optionsChanged(fireChangedEvent);
  }
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkDebugOptions::isDebugEnabled() const
{
  QMutexLocker lock(&this->mutex);
  return this->enabled;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebugOptions::setDebugEnabled(bool enabled)
{
  bool fireChangedEvent = false;
  {
    QMutexLocker lock(&this->mutex);
    if (enabled)
    {
      if (this->isDebugEnabled()) return;

      // enable platform debugging - there is no .options file
      ctkPluginFrameworkProperties::setProperty(CTK_DEBUG, QString(""));
      this->options = this->disabledOptions;
      this->disabledOptions.clear();
      this->enabled = true;
      if (!this->options.isEmpty())
      {
        // fire changed event to indicate some options were re-enabled
        fireChangedEvent = true;
      }
    }
    else
    {
      if (!this->isDebugEnabled()) return;
      // disable platform debugging.
      ctkPluginFrameworkProperties::clearProperty(CTK_DEBUG);
      if (!this->options.isEmpty())
      {
        // Save the current options off in case debug is re-enabled
        this->disabledOptions = this->options;
        // fire changed event to indicate some options were disabled
        fireChangedEvent = true;
      }
      this->options.clear();
      this->enabled = false;
    }
  }
  if (fireChangedEvent)
  {
    // need to fire event to listeners that options have been disabled
    this->optionsChanged("*");
  }
}

//----------------------------------------------------------------------------
QString ctkPluginFrameworkDebugOptions::getSymbolicName(const QString& option) const
{
  int firstSlashIndex = option.indexOf("/");
  if (firstSlashIndex > 0)
    return option.left(firstSlashIndex);
  return QString();
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebugOptions::optionsChanged(const QString& pluginSymbolicName)
{
  // use osgi services to get the listeners
  if (context == NULL)
    return;
  // do not use the service tracker because that is only used to call all listeners initially when they are registered
  // here we only want the services with the specified name.
  QList<ctkServiceReference> listenerRefs;
  try
  {
    listenerRefs = context->getServiceReferences<ctkDebugOptionsListener>( "(" + ctkDebugOptions::LISTENER_SYMBOLICNAME + "=" + pluginSymbolicName + ")");
  }
  catch (const ctkInvalidArgumentException& /*e*/)
  {
    // consider logging; should not happen
  }
  if (listenerRefs.empty()) return;

  foreach (const ctkServiceReference& ref, listenerRefs)
  {
    ctkDebugOptionsListener* service = context->getService<ctkDebugOptionsListener>(ref);
    if (service == NULL) continue;

    try
    {
      service->optionsChanged(*this);
    }
    catch (const std::exception& /*e*/)
    {
      // TODO consider logging
    }
    context->ungetService(ref);
  }
}

//----------------------------------------------------------------------------
ctkDebugOptionsListener* ctkPluginFrameworkDebugOptions::addingService(const ctkServiceReference& reference)
{
  ctkDebugOptionsListener* listener = context->getService<ctkDebugOptionsListener>(reference);
  listener->optionsChanged(*this);
  return listener;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebugOptions::modifiedService(const ctkServiceReference& /*reference*/,
                                                     ctkDebugOptionsListener* /*service*/)
{
  // nothing
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebugOptions::removedService(const ctkServiceReference& reference,
                                                    ctkDebugOptionsListener* /*service*/)
{
  context->ungetService(reference);
}
