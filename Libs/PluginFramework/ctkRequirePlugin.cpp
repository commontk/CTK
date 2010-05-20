/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkRequirePlugin_p.h"

#include "ctkPluginConstants.h"
#include "ctkPluginPrivate_p.h"


  ctkRequirePlugin::ctkRequirePlugin(ctkPluginPrivate* requestor,
                const QString& name, const QString& res,
                const QString& range)
                  : name(name),
                  resolution(res.isEmpty() ? PluginConstants::RESOLUTION_MANDATORY : res),
                  pluginRange(range.isEmpty() ? ctkVersionRange::defaultVersionRange() : range)
  {

    if (resolution != PluginConstants::RESOLUTION_MANDATORY &&
        resolution != PluginConstants::RESOLUTION_OPTIONAL )
    {
      QString what = QString("Invalid directive : '")
                     + PluginConstants::RESOLUTION_DIRECTIVE + ":=" + this->resolution
                     + "' in manifest header '"
                     + PluginConstants::REQUIRE_PLUGIN + ": " + this->name
                     + "' of plugin with id " + requestor->id
                     + " (" + requestor->symbolicName + ")"
                     + ". The value must be either '"
                     + PluginConstants::RESOLUTION_MANDATORY + "' or '"
                     + PluginConstants::RESOLUTION_OPTIONAL  + "'.";
      throw std::invalid_argument(what.toStdString());
      }


  }

  bool ctkRequirePlugin::overlap(const ctkRequirePlugin& rp) const
  {
    if (resolution == PluginConstants::RESOLUTION_MANDATORY &&
        rp.resolution != PluginConstants::RESOLUTION_MANDATORY)
    {
      return false;
    }
    return pluginRange.withinRange(rp.pluginRange);

}
