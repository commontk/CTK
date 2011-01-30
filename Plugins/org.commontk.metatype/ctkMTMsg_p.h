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


#ifndef CTKMTMSG_P_H
#define CTKMTMSG_P_H

struct ctkMTMsg
{
  static const char* CONTEXT;

  static const char* SERVICE_DESCRIPTION;

  static const char* UNEXPECTED_ELEMENT;
  static const char* UNEXPECTED_TEXT;
  static const char* MISSING_ATTRIBUTE;
  static const char* OCD_ID_NOT_FOUND;
  static const char* NULL_DEFAULTS;
  static const char* MISSING_ELEMENT;

  static const char* EXCEPTION_MESSAGE;
  static const char* NULL_IS_INVALID;
  static const char* VALUE_OUT_OF_RANGE;
  static const char* VALUE_OUT_OF_OPTION;
  static const char* TOO_MANY_VALUES;
  static const char* NULL_OPTIONS;
  static const char* INCONSISTENT_OPTIONS;
  static const char* INVALID_OPTIONS;
  static const char* INVALID_DEFAULTS;

  static const char* METADATA_NOT_FOUND;
  static const char* ASK_INVALID_LOCALE;
  static const char* TOKENIZER_GOT_INVALID_DATA;
  static const char* INVALID_PID_METATYPE_PROVIDER_IGNORED;
};

#endif // CTKMTMSG_P_H
