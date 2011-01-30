/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License") = QT_TRANSLATE_NOOP("ctkMTMsg", "");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#include "ctkMTMsg_p.h"

#include <QtGlobal>

const char* ctkMTMsg::CONTEXT = "ctkMTMsg";

const char* ctkMTMsg::SERVICE_DESCRIPTION = QT_TRANSLATE_NOOP("ctkMTMsg", "CTK Metatype Service - CommonTK Implementation");

const char* ctkMTMsg::UNEXPECTED_ELEMENT = QT_TRANSLATE_NOOP("ctkMTMsg", "Unexpected element %1.");
const char* ctkMTMsg::UNEXPECTED_TEXT = QT_TRANSLATE_NOOP("ctkMTMsg", "Unexpected text %1.");
const char* ctkMTMsg::MISSING_ATTRIBUTE = QT_TRANSLATE_NOOP("ctkMTMsg", "Missing attribute %1 in tag %2.");
const char* ctkMTMsg::OCD_ID_NOT_FOUND = QT_TRANSLATE_NOOP("ctkMTMsg", "Object Class Definition ID not found %1.");
const char* ctkMTMsg::NULL_DEFAULTS = QT_TRANSLATE_NOOP("ctkMTMsg", "Default value cannot be null when cardinality is zero.");
const char* ctkMTMsg::MISSING_ELEMENT = QT_TRANSLATE_NOOP("ctkMTMsg", "Missing element %1 (Reference ID = %2.)");

const char* ctkMTMsg::EXCEPTION_MESSAGE = QT_TRANSLATE_NOOP("ctkMTMsg", "Unexpected exception %1 with message %2.");
const char* ctkMTMsg::NULL_IS_INVALID = QT_TRANSLATE_NOOP("ctkMTMsg", "Cannot validate an empty value.");
const char* ctkMTMsg::VALUE_OUT_OF_RANGE = QT_TRANSLATE_NOOP("ctkMTMsg", "Value %1 is out of range.");
const char* ctkMTMsg::VALUE_OUT_OF_OPTION = QT_TRANSLATE_NOOP("ctkMTMsg", "Value %1 is out of Option.");
const char* ctkMTMsg::TOO_MANY_VALUES = QT_TRANSLATE_NOOP("ctkMTMsg", "Found too many values in %1 when expecting %2 values.");
const char* ctkMTMsg::NULL_OPTIONS = QT_TRANSLATE_NOOP("ctkMTMsg", "Cannot set Option labels or values empty.");
const char* ctkMTMsg::INCONSISTENT_OPTIONS = QT_TRANSLATE_NOOP("ctkMTMsg", "Labels and Values of Option have different sizes.");
const char* ctkMTMsg::INVALID_OPTIONS = QT_TRANSLATE_NOOP("ctkMTMsg", "Option value %1 is invalid because of %2.");
const char* ctkMTMsg::INVALID_DEFAULTS = QT_TRANSLATE_NOOP("ctkMTMsg", "Defaults value %1 is invalid because of %2.");

const char* ctkMTMsg::METADATA_NOT_FOUND = QT_TRANSLATE_NOOP("ctkMTMsg", "Plugin(ID=\"%1\", name=\"%2\") has no MetaData file.");
const char* ctkMTMsg::ASK_INVALID_LOCALE = QT_TRANSLATE_NOOP("ctkMTMsg", "OCD(ID=\"%1\") cannot support this locale \"%2\".");
const char* ctkMTMsg::TOKENIZER_GOT_INVALID_DATA = QT_TRANSLATE_NOOP("ctkMTMsg", "The Tokenizer got invalid data.");
const char* ctkMTMsg::INVALID_PID_METATYPE_PROVIDER_IGNORED = QT_TRANSLATE_NOOP("ctkMTMsg", "Plugin %1 with ID %2 provided a ctkMetaTypeProvider with an invalid property. Property %3 with value %4 was not of the expected type (QString or QStringList) and will be ignored.");
