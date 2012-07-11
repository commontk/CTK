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

#include "ctkCmdLineModuleObjectHierarchyReader.h"

#include <QObject>
#include <QStack>
#include <QVariant>

namespace {

static QString PREFIX_EXECUTABLE = "executable:";
static QString PREFIX_PARAMETER_GROUP = "paramGroup:";
static QString PREFIX_PARAMETER = "parameter:";

}

class ctkCmdLineModuleObjectHierarchyReaderPrivate
{
public:

  ctkCmdLineModuleObjectHierarchyReaderPrivate(QObject* root)
    : rootObject(root), currentObject(0), currentToken(ctkCmdLineModuleObjectHierarchyReader::NoToken),
      atEnd(false)
  {
  }

  QVariant property(const QString& propName) const
  {
    if (currentObject == 0) return QString();

    QString prefixedName;
    switch(currentToken)
    {
    case ctkCmdLineModuleObjectHierarchyReader::Executable: prefixedName = PREFIX_EXECUTABLE + propName;
    case ctkCmdLineModuleObjectHierarchyReader::ParameterGroup: prefixedName = PREFIX_PARAMETER_GROUP + propName;
    case ctkCmdLineModuleObjectHierarchyReader::Parameter: prefixedName = PREFIX_PARAMETER + propName;
    default: ;
    }

    return currentObject->property(qPrintable(prefixedName));
  }

  ctkCmdLineModuleObjectHierarchyReader::TokenType token(QObject* obj)
  {
    if (obj == 0) return ctkCmdLineModuleObjectHierarchyReader::NoToken;
    QString name = obj->objectName();
    if (name.startsWith(PREFIX_EXECUTABLE)) return ctkCmdLineModuleObjectHierarchyReader::Executable;
    if (name.startsWith(PREFIX_PARAMETER_GROUP)) return ctkCmdLineModuleObjectHierarchyReader::ParameterGroup;
    if (name.startsWith(PREFIX_PARAMETER)) return ctkCmdLineModuleObjectHierarchyReader::Parameter;
    return ctkCmdLineModuleObjectHierarchyReader::NoToken;
  }

  bool setCurrent(QObject* obj)
  {
    ctkCmdLineModuleObjectHierarchyReader::TokenType t = token(obj);
    if (t != ctkCmdLineModuleObjectHierarchyReader::NoToken)
    {
      currentObject = obj;
      currentToken = t;
      return true;
    }
    return false;
  }

  QObject* rootObject;
  QObject* currentObject;

  ctkCmdLineModuleObjectHierarchyReader::TokenType currentToken;
  bool atEnd;

  QStack<QObject*> objectStack;
};

ctkCmdLineModuleObjectHierarchyReader::ctkCmdLineModuleObjectHierarchyReader(QObject *root)
  : d(new ctkCmdLineModuleObjectHierarchyReaderPrivate(root))
{
}

ctkCmdLineModuleObjectHierarchyReader::~ctkCmdLineModuleObjectHierarchyReader()
{
}

void ctkCmdLineModuleObjectHierarchyReader::setRootObject(QObject* root)
{
  d->rootObject = root;
  clear();
}

void ctkCmdLineModuleObjectHierarchyReader::clear()
{
  d->currentToken = NoToken;
  d->currentObject = 0;
  d->objectStack.clear();
}

bool ctkCmdLineModuleObjectHierarchyReader::atEnd() const
{
  return d->atEnd || d->rootObject == 0;
}

bool ctkCmdLineModuleObjectHierarchyReader::isParameterGroup() const
{
  return d->currentToken == ParameterGroup;
}

bool ctkCmdLineModuleObjectHierarchyReader::isParameter() const
{
  return d->currentToken == Parameter;
}

QString ctkCmdLineModuleObjectHierarchyReader::name() const
{
  if (d->currentObject == 0) return QString();
  switch(d->currentToken)
  {
  case Executable: return d->currentObject->objectName().mid(PREFIX_EXECUTABLE.size());
  case ParameterGroup: return d->currentObject->objectName().mid(PREFIX_PARAMETER_GROUP.size());
  case Parameter: return d->currentObject->objectName().mid(PREFIX_PARAMETER.size());
  default: return QString();
  }
}

QString ctkCmdLineModuleObjectHierarchyReader::label() const
{
  if (d->currentObject == 0) return QString();
  switch(d->currentToken)
  {
  case Executable: return d->currentObject->objectName().mid(PREFIX_EXECUTABLE.size());
  case ParameterGroup: return d->property("title").toString();
  case Parameter: return property("label").toString();
  default: return QString();
  }
}

QVariant ctkCmdLineModuleObjectHierarchyReader::value() const
{
  QString valProp = property("valueProperty").toString();
  return property(valProp);
}

void ctkCmdLineModuleObjectHierarchyReader::setValue(const QVariant& value)
{
  QVariant valProp = property("valueProperty");
  if (valProp.isValid())
  {
    d->currentObject->setProperty(qPrintable(valProp.toString()), value);
  }
}

QString ctkCmdLineModuleObjectHierarchyReader::flag() const
{
  QVariant v = property("flag");
  return v.isValid() ? v.toString() : QString();
}

QString ctkCmdLineModuleObjectHierarchyReader::longFlag() const
{
  QVariant v = property("longflag");
  return v.isValid() ? v.toString() : QString();
}

int ctkCmdLineModuleObjectHierarchyReader::index() const
{
  QVariant v = property("index");
  return v.isValid() ? v.toInt() : -1;
}

bool ctkCmdLineModuleObjectHierarchyReader::isMultiple() const
{
  QVariant v = property("multiple");
  return v.isValid() ? v.toBool() : false;
}

QVariant ctkCmdLineModuleObjectHierarchyReader::property(const QString &propName) const
{
  if (d->currentObject == 0) return QVariant();

  // First try to get a prefixed property
  QVariant res = d->property(propName);
  // Try to get a property with the original name
  if (!res.isValid()) res = d->currentObject->property(qPrintable(propName));
  return res;
}

ctkCmdLineModuleObjectHierarchyReader::TokenType ctkCmdLineModuleObjectHierarchyReader::readNext() const
{
  if (d->atEnd) return NoToken;

  QObject* curr = 0;
  if (d->currentObject == 0)
  {
    curr = d->rootObject;
    if (d->setCurrent(curr)) return d->currentToken;
  }
  else
  {
    curr = d->currentObject;
  }

  while (true)
  {
    if (curr)
    {
      QObjectList children = curr->children();
      QListIterator<QObject*> i(children);
      i.toBack();
      while (i.hasPrevious())
      {
        d->objectStack.push(i.previous());
      }
      if (children.isEmpty())
      {
        curr = 0;
      }
      else
      {
        curr = d->objectStack.pop();
        if (d->setCurrent(curr)) return d->currentToken;
      }
      continue;
    }

    if (d->objectStack.isEmpty()) break;
    curr = d->objectStack.pop();
    if (d->setCurrent(curr)) return d->currentToken;
  }

  d->atEnd = true;
  d->currentObject = 0;
  d->currentToken = NoToken;

  return NoToken;
}

bool ctkCmdLineModuleObjectHierarchyReader::readNextExecutable() const
{
  while (!(readNext() == Executable || d->atEnd));
  return !d->atEnd;
}

bool ctkCmdLineModuleObjectHierarchyReader::readNextParameterGroup() const
{
  while (!(readNext() == ParameterGroup || d->atEnd));
  return !d->atEnd;
}

bool ctkCmdLineModuleObjectHierarchyReader::readNextParameter() const
{
  while (!(readNext() == Parameter || d->atEnd));
  return !d->atEnd;
}

ctkCmdLineModuleObjectHierarchyReader::TokenType ctkCmdLineModuleObjectHierarchyReader::tokenType() const
{
  return d->currentToken;
}
