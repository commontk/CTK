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


#ifndef CTKPLUGINLOCALIZATION_H
#define CTKPLUGINLOCALIZATION_H

#include <ctkPluginFrameworkExport.h>

#include <QLocale>
#include <QSharedDataPointer>
#include <QSharedPointer>

class ctkPlugin;
struct ctkPluginLocalizationData;

/**
 * \ingroup PluginFramework
 *
 * Translate text into different languages.
 *
 * Use this class to dynamically translate human-readable text
 * in your plugin. You can get an instance of this class
 * corresponding to a specific locale via the method
 * <code>ctkPlugin::getPluginLocalization()</code>.
 *
 * @see ctkPlugin::getPluginLocalization()
 */
class CTK_PLUGINFW_EXPORT ctkPluginLocalization
{
public:

  /**
   * Creates a default ctkPluginLocalization instance, using
   * the default locale.
   *
   * Note that getLocalized() will always return a null QString
   * for a default constructed ctkPluginLocalization object.
   * Use <code>ctkPlugin::getPluginLocalization()</code> to create
   * a valid instance.
   */
  ctkPluginLocalization();

  ctkPluginLocalization(const ctkPluginLocalization& pl);
  ~ctkPluginLocalization();

  ctkPluginLocalization& operator=(const ctkPluginLocalization& other);

  /**
   * Translate <code>str</code> to a specific locale, using the
   * specified <code>context</code>.
   *
   * @return The translation or a null QString, if no translation
   *         was found.
   */
  QString getLocalized(const QString& context, const QString& str) const;

  /**
   * Get the locale for which this <code>ctkPluginLocalization</code>
   * object was constructed.
   *
   * @return The locale for this object.
   */
  QLocale getLocale() const;

private:

  friend class ctkPlugin;

  ctkPluginLocalization(const QString& msgFileName,
                        const QLocale& locale, const QSharedPointer<ctkPlugin>& plugin);

  mutable QSharedDataPointer<ctkPluginLocalizationData> d;
};

#endif // CTKPLUGINLOCALIZATION_H
