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

#include <QMap>
#include <QStringList>


  class PluginFrameworkUtil
  {
  public:

    /**
       * Parse strings of format:
       *
       *   ENTRY (, ENTRY)*
       *   ENTRY = key (; key)* (; PARAM)*
       *   PARAM = attribute '=' value
       *   PARAM = directive ':=' value
       *
       * @param a Attribute being parsed
       * @param s String to parse
       * @param single If true, only allow one key per ENTRY
       * @param unique Only allow unique parameters for each ENTRY.
       * @param single_entry If true, only allow one ENTRY is allowed.
       * @return QMap<QString, QString> mapping attributes to values.
       * @exception std::invalid_argument If syntax error in input string.
       */
      static QList<QMap<QString, QStringList> > parseEntries(const QString& a, const QString& s,
                                                 bool single, bool unique, bool single_entry);

}
