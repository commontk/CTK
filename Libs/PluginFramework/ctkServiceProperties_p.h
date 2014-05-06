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

#ifndef CTKSERVICEPROPERTIES_P_H
#define CTKSERVICEPROPERTIES_P_H

#include <QVarLengthArray>
#include <QVariant>

#include "ctkPluginFramework_global.h"

class ctkServiceProperties
{

private:

  QVarLengthArray<QString,10> ks;
  QVarLengthArray<QVariant,10> vs;

  QMap<QString, QVariant> map;

public:

  ctkServiceProperties(const ctkProperties& props);

  QVariant value(const QString& key) const;
  QVariant value(int index) const;

  int find(const QString& key) const;
  int findCaseSensitive(const QString& key) const;

  QStringList keys() const;

};

#endif // CTKSERVICEPROPERTIES_P_H
