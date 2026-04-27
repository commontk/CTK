/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkDICOMModalities_h
#define __ctkDICOMModalities_h

// Qt includes
#include <QStringList>
#include <QtGlobal>

// CTK includes
#include "ctkDeprecated.h"
#include "ctkDICOMCoreExport.h"

/// \ingroup DICOM_Core
///
/// \brief DICOM modality code definitions
///
/// This header provides comprehensive lists of DICOM modality codes
/// as defined in DICOM PS3.16 Annex D - DICOM Controlled Terminology Definitions.
/// Reference: https://dicom.nema.org/medical/dicom/current/output/chtml/part16/chapter_D.html
///
/// \deprecated These lists are deprecated as of CTK 0.1. Applications should
/// query the attached ctkDICOMDatabase instance instead, which exposes
/// configurable equivalents:
///
///   ctkDICOMModalities::AllModalities                    ->
///       ctkDICOMDatabase::supportedModalities()
///   ctkDICOMModalities::CommonImagingModalities          ->
///       ctkDICOMDatabase::defaultModalities()
///   ctkDICOMModalities::ExcludedFromThumbnailGeneration  ->
///       ctkDICOMDatabase::modalitiesExcludedFromThumbnailGeneration()
///
/// The deprecated aliases below are defined as references to Meyers'
/// singletons in ctkDICOMModalities.cpp, which fixes the clazy
/// non-pod-global-static warning that the previous in-header
/// `static const QStringList` definitions produced (one heap-allocated
/// copy per translation unit, plus static-initialization-order risk).
///
namespace ctkDICOMModalities
{
#if CTK_DEPRECATED_SINCE(0, 1)

  /// \deprecated Use ctkDICOMDatabase::supportedModalities() instead.
  /// Complete list of all current (non-retired) DICOM modality codes.
  CTK_DICOM_CORE_EXPORT extern const QStringList& AllModalities;

  /// \deprecated Use ctkDICOMDatabase::modalitiesExcludedFromThumbnailGeneration()
  /// instead. Modalities that should be excluded from thumbnail generation
  /// because they do not have meaningful image thumbnails.
  CTK_DICOM_CORE_EXPORT extern const QStringList& ExcludedFromThumbnailGeneration;

  /// \deprecated Use ctkDICOMDatabase::defaultModalities() instead.
  /// Common imaging modalities typically used for filtering in UI.
  CTK_DICOM_CORE_EXPORT extern const QStringList& CommonImagingModalities;

#endif
}

#endif // __ctkDICOMModalities_h
