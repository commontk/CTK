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

#include "ctkPlugin_p.h"
#include "ctkPluginConstants.h"
#include "ctkRequirePlugin_p.h"

//----------------------------------------------------------------------------
ctkRequirePlugin::ctkRequirePlugin(ctkPluginPrivate* requestor,
              const QString& name, const QString& res,
              const QString& range)
                : name(name),
                resolution(res.isEmpty() ? ctkPluginConstants::RESOLUTION_MANDATORY : res),
                pluginRange(range.isEmpty() ? ctkVersionRange::defaultVersionRange() : range)
{

  if (resolution != ctkPluginConstants::RESOLUTION_MANDATORY &&
      resolution != ctkPluginConstants::RESOLUTION_OPTIONAL )
  {
    QString what = QString("Invalid directive : '")
                   + ctkPluginConstants::RESOLUTION_DIRECTIVE + ":=" + this->resolution
                   + "' in manifest header '"
                   + ctkPluginConstants::REQUIRE_PLUGIN + ": " + this->name
                   + "' of plugin with id " + requestor->id
                   + " (" + requestor->symbolicName + ")"
                   + ". The value must be either '"
                   + ctkPluginConstants::RESOLUTION_MANDATORY + "' or '"
                   + ctkPluginConstants::RESOLUTION_OPTIONAL  + "'.";
    throw ctkInvalidArgumentException(what);
    }


}

//----------------------------------------------------------------------------
bool ctkRequirePlugin::overlap(const ctkRequirePlugin& rp) const
{
  if (resolution == ctkPluginConstants::RESOLUTION_MANDATORY &&
      rp.resolution != ctkPluginConstants::RESOLUTION_MANDATORY)
  {
    return false;
  }
  return pluginRange.withinRange(rp.pluginRange);

}
