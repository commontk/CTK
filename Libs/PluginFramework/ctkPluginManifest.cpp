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

#include "ctkPluginManifest_p.h"

#include <QStringList>
#include <QIODevice>
#include <QDebug>

#include <ctkException.h>

//----------------------------------------------------------------------------
ctkPluginManifest::ctkPluginManifest()
{

}

//----------------------------------------------------------------------------
ctkPluginManifest::ctkPluginManifest(const QByteArray& in)
{
  read(in);
}

//----------------------------------------------------------------------------
void ctkPluginManifest::read(const QByteArray& in)
{
  mainAttributes.clear();
  sections.clear();

  QString key;
  QString value;
  QString currSection;

  QList<QByteArray> lines = in.split('\n');

  foreach (const QString& line, lines)
  {
    // skip empty lines and comments
    if (line.trimmed().isEmpty() | line.startsWith('#')) continue;

    // a valid new key/value pair starts with no white-space and contains
    // either a ':' or a '='
    if (!(line.startsWith(' ') || line.startsWith('\t')) && (line.contains(':') || line.contains('=')))
    {

      // see if we found a new section header
      if (line.startsWith('['))
      {
        currSection = line.mid(1, line.indexOf(']')-1);
      }
      else
      {
        // we found a new key/value pair, save the old one
        if (!key.isEmpty())
        {
          if (currSection.isEmpty())
          {
            mainAttributes.insert(key, value);
          }
          else
          {
            sections[currSection].insert(key, value);
          }
        }

        int colonIndex = line.indexOf(':');
        int equalIndex = line.indexOf('=');

        int sepIndex = -1;
        if (colonIndex < 0) sepIndex = equalIndex;
        else if (equalIndex < 0) sepIndex = colonIndex;
        else sepIndex = colonIndex < equalIndex ? colonIndex : equalIndex;

        key = line.left(sepIndex);
        value = line.right(line.size()-(sepIndex+1)).trimmed();
      }
    }
    else
    {
      // add the line to the value
      value += line;
    }
  }

  // save the last key/value pair
  if (!key.isEmpty())
  {
    if (currSection.isEmpty())
    {
      mainAttributes.insert(key, value);
    }
    else
    {
      sections[currSection].insert(key, value);
    }
  }

}

//----------------------------------------------------------------------------
ctkPluginManifest::Attributes ctkPluginManifest::getMainAttributes() const
{
  return mainAttributes;
}

//----------------------------------------------------------------------------
QString ctkPluginManifest::getAttribute(const QString& key) const
{
  if (!mainAttributes.contains(key)) return QString();
  return mainAttributes[key];
}

//----------------------------------------------------------------------------
ctkPluginManifest::Attributes ctkPluginManifest::getAttributes(const QString& section) const
{
  if (!sections.contains(section))
  {
    throw ctkInvalidArgumentException(QString("Manifest section invalid: ") + section);
  }

  return sections[section];
}

//----------------------------------------------------------------------------
QStringList ctkPluginManifest::getSections() const
{
  return sections.keys();
}
