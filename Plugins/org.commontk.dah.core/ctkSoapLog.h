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

#ifndef CTKSOAPLOG_H
#define CTKSOAPLOG_H

#define CTK_SOAP_LOG_LOWLEVEL(msg) qDebug() msg;
//#define CTK_SOAP_LOG_LOWLEVEL(msg)

#define CTK_SOAP_LOG(msg) qDebug() msg;
//#define CTK_SOAP_LOG(msg)

#define CTK_SOAP_LOG_HIGHLEVEL(msg) qDebug() msg;
//#define CTK_SOAP_LOG_HIGHLEVEL(msg)

//#define CTK_SOAP_LOG_REQUEST(msg) 
#define CTK_SOAP_LOG_REQUEST(msg) qDebug() msg;

#define CTK_SOAP_LOG_RECEPTION(msg) qDebug() msg;
//#define CTK_SOAP_LOG_RECEPTION(msg)

#endif // CTKSOAPLOG_H
