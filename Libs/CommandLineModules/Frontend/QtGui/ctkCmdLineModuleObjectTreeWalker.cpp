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

#include "ctkCmdLineModuleObjectTreeWalker_p.h"

#include <QObject>
#include <QStack>
#include <QVariant>

namespace {

static QString PREFIX_EXECUTABLE = "executable:";
static QString PREFIX_PARAMETER_GROUP = "paramGroup:";
static QString PREFIX_PARAMETER_CONTAINER = "paramContainer:";
static QString PREFIX_PARAMETER = "parameter:";

}

//----------------------------------------------------------------------------
ctkCmdLineModuleObjectTreeWalker::ctkCmdLineModuleObjectTreeWalker(QObject *root)
  : RootObject(root), CurrentObject(0), CurrentToken(NoToken),
    AtEnd(false)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleObjectTreeWalker::~ctkCmdLineModuleObjectTreeWalker()
{
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleObjectTreeWalker::setRootObject(QObject* root)
{
  RootObject = root;
  clear();
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleObjectTreeWalker::clear()
{
  CurrentToken = NoToken;
  CurrentObject = 0;
  ObjectStack.clear();
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::atEnd() const
{
  return AtEnd || RootObject == 0;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::isParameterGroup() const
{
  return CurrentToken == ParameterGroup;
}


//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::isParameterContainer() const
{
  return CurrentToken == ParameterContainer;
}


//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::isParameter() const
{
  return CurrentToken == Parameter;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleObjectTreeWalker::name() const
{
  if (CurrentObject == 0) return QString();
  switch(CurrentToken)
  {
  case Executable: return CurrentObject->objectName().mid(PREFIX_EXECUTABLE.size());
  case ParameterGroup: return CurrentObject->objectName().mid(PREFIX_PARAMETER_GROUP.size());
  case ParameterContainer: return CurrentObject->objectName().mid(PREFIX_PARAMETER_CONTAINER.size());
  case Parameter: return CurrentObject->objectName().mid(PREFIX_PARAMETER.size());
  default: return QString();
  }
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleObjectTreeWalker::label() const
{
  if (CurrentObject == 0) return QString();
  switch(CurrentToken)
  {
  case Executable: return CurrentObject->objectName().mid(PREFIX_EXECUTABLE.size());
  case ParameterGroup: return property("title").toString();
  case ParameterContainer: return property("name").toString();
  case Parameter: return property("label").toString();
  default: return QString();
  }
}

//----------------------------------------------------------------------------
QVariant ctkCmdLineModuleObjectTreeWalker::value(const QString &propertyName) const
{
  QString valProp = propertyName;
  if (valProp.isEmpty())
  {
    valProp = property("valueProperty").toString();
  }
  return property(valProp);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleObjectTreeWalker::setValue(const QVariant& value, const QString &propertyName)
{
  QString valProp = propertyName;
  if (valProp.isEmpty())
  {
    valProp = property("valueProperty").toString();
  }

  if (!valProp.isEmpty())
  {
    CurrentObject->setProperty(qPrintable(valProp), value);
  }
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleObjectTreeWalker::flag() const
{
  QVariant v = property("flag");
  return v.isValid() ? v.toString() : QString();
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleObjectTreeWalker::longFlag() const
{
  QVariant v = property("longflag");
  return v.isValid() ? v.toString() : QString();
}

//----------------------------------------------------------------------------
int ctkCmdLineModuleObjectTreeWalker::index() const
{
  QVariant v = property("index");
  return v.isValid() ? v.toInt() : -1;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::isMultiple() const
{
  QVariant v = property("multiple");
  return v.isValid() ? v.toBool() : false;
}

//----------------------------------------------------------------------------
QVariant ctkCmdLineModuleObjectTreeWalker::property(const QString &propName) const
{
  if (CurrentObject == 0) return QVariant();

  // First try to get a prefixed property
  QVariant res = prefixedProperty(propName);
  // Try to get a property with the original name
  if (!res.isValid()) res = CurrentObject->property(qPrintable(propName));
  return res;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleObjectTreeWalker::TokenType ctkCmdLineModuleObjectTreeWalker::readNext()
{
  if (AtEnd) return NoToken;

  QObject* curr = 0;
  if (CurrentObject == 0)
  {
    curr = RootObject;
    if (setCurrent(curr)) return CurrentToken;
  }
  else
  {
    curr = CurrentObject;
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
        ObjectStack.push(i.previous());
      }
      if (children.isEmpty())
      {
        curr = 0;
      }
      else
      {
        curr = ObjectStack.pop();
        if (setCurrent(curr)) return CurrentToken;
      }
      continue;
    }

    if (ObjectStack.isEmpty()) break;
    curr = ObjectStack.pop();
    if (setCurrent(curr)) return CurrentToken;
  }

  AtEnd = true;
  CurrentObject = 0;
  CurrentToken = NoToken;

  return NoToken;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::readNextExecutable()
{
  while (!(readNext() == Executable || AtEnd)) ; // deliberately empty loop body
  return !AtEnd;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::readNextParameterGroup()
{
  while (!(readNext() == ParameterGroup || AtEnd)) ; // deliberately empty loop body
  return !AtEnd;
}


//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::readNextParameterContainer()
{
  while (!(readNext() == ParameterContainer || AtEnd)) ; // deliberately empty loop body
  return !AtEnd;
}


//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::readNextParameter()
{
  while (!(readNext() == Parameter || AtEnd)) ; // deliberately empty loop body
  return !AtEnd;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleObjectTreeWalker::TokenType ctkCmdLineModuleObjectTreeWalker::tokenType() const
{
  return CurrentToken;
}

//----------------------------------------------------------------------------
QVariant ctkCmdLineModuleObjectTreeWalker::prefixedProperty(const QString& propName) const
{
  if (CurrentObject == 0) return QString();

  QString prefixedName;
  switch(CurrentToken)
  {
  case ctkCmdLineModuleObjectTreeWalker::Executable: prefixedName = PREFIX_EXECUTABLE + propName;
  case ctkCmdLineModuleObjectTreeWalker::ParameterGroup: prefixedName = PREFIX_PARAMETER_GROUP + propName;
  case ctkCmdLineModuleObjectTreeWalker::ParameterContainer: prefixedName = PREFIX_PARAMETER_CONTAINER + propName;
  case ctkCmdLineModuleObjectTreeWalker::Parameter: prefixedName = PREFIX_PARAMETER + propName;
  default: ;
  }

  return CurrentObject->property(qPrintable(prefixedName));
}

//----------------------------------------------------------------------------
ctkCmdLineModuleObjectTreeWalker::TokenType
ctkCmdLineModuleObjectTreeWalker::token(QObject* obj)
{
  if (obj == 0) return ctkCmdLineModuleObjectTreeWalker::NoToken;
  QString name = obj->objectName();
  if (name.startsWith(PREFIX_EXECUTABLE)) return ctkCmdLineModuleObjectTreeWalker::Executable;
  if (name.startsWith(PREFIX_PARAMETER_GROUP)) return ctkCmdLineModuleObjectTreeWalker::ParameterGroup;
  if (name.startsWith(PREFIX_PARAMETER_CONTAINER)) return ctkCmdLineModuleObjectTreeWalker::ParameterContainer;
  if (name.startsWith(PREFIX_PARAMETER)) return ctkCmdLineModuleObjectTreeWalker::Parameter;
  return ctkCmdLineModuleObjectTreeWalker::NoToken;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleObjectTreeWalker::setCurrent(QObject* obj)
{
  ctkCmdLineModuleObjectTreeWalker::TokenType t = token(obj);
  if (t != ctkCmdLineModuleObjectTreeWalker::NoToken)
  {
    CurrentObject = obj;
    CurrentToken = t;
    return true;
  }
  return false;
}
