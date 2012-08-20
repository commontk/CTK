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

#include "ctkCmdLineModuleXmlMsgHandler_p.h"

#include <QXmlStreamReader>

//----------------------------------------------------------------------------
QString ctkCmdLineModuleXmlMsgHandler::statusMessage() const
{
  return Description;
}

//----------------------------------------------------------------------------
int ctkCmdLineModuleXmlMsgHandler::line() const
{
  return SourceLocation.line();
}

//----------------------------------------------------------------------------
int ctkCmdLineModuleXmlMsgHandler::column() const
{
  return SourceLocation.column();
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleXmlMsgHandler::handleMessage(QtMsgType type, const QString& description,
                                                  const QUrl& identifier, const QSourceLocation& sourceLocation)
{
  Q_UNUSED(identifier)

  MessageType = type;
  SourceLocation = sourceLocation;

  QXmlStreamReader reader(description);
  Description.clear();
  Description.reserve(description.size());
  while(!reader.atEnd())
  {
    reader.readNext();

    switch(reader.tokenType())
    {
    case QXmlStreamReader::Characters:
    {
      Description.append(reader.text().toString());
      continue;
    }
    case QXmlStreamReader::StartElement:
      /* Fallthrough, */
    case QXmlStreamReader::EndElement:
      /* Fallthrough, */
    case QXmlStreamReader::StartDocument:
          /* Fallthrough, */
    case QXmlStreamReader::EndDocument:
      continue;
    default:
      Q_ASSERT_X(false, Q_FUNC_INFO,
                 "Unexpected node.");
    }
  }
}
