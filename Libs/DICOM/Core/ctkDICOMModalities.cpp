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

// CTK includes
#include "ctkDICOMModalities.h"

namespace ctkDICOMModalities
{

//-----------------------------------------------------------------------------
const QStringList& allModalities()
{
  static const QStringList s = {
    "Any",      // Special filter value for "all modalities"
    "AR",       // Autorefraction
    "ASMT",     // Content Assessment Results
    "AU",       // Audio
    "BDUS",     // Bone Densitometry (ultrasound)
    "BI",       // Biomagnetic imaging
    "BMD",      // Bone Densitometry (X-Ray)
    "CR",       // Computed Radiography
    "CT",       // Computed Tomography
    "DG",       // Diaphanography
    "DOC",      // Document
    "DX",       // Digital Radiography
    "ECG",      // Electrocardiography
    "EPS",      // Cardiac Electrophysiology
    "ES",       // Endoscopy
    "FID",      // Fiducials
    "GM",       // General Microscopy
    "HC",       // Hard Copy
    "HD",       // Hemodynamic Waveform
    "IO",       // Intra-Oral Radiography
    "IOL",      // Intraocular Lens Data
    "IVOCT",    // Intravascular Optical Coherence Tomography
    "IVUS",     // Intravascular Ultrasound
    "KER",      // Keratometry
    "KO",       // Key Object Selection
    "LEN",      // Lensometry
    "LS",       // Laser surface scan
    "MG",       // Mammography
    "MR",       // Magnetic Resonance
    "NM",       // Nuclear Medicine
    "OAM",      // Ophthalmic Axial Measurements
    "OCT",      // Optical Coherence Tomography (non-Ophthalmic)
    "OP",       // Ophthalmic Photography
    "OPM",      // Ophthalmic Mapping
    "OPT",      // Ophthalmic Tomography
    "OPV",      // Ophthalmic Visual Field
    "OSS",      // Optical Surface Scan
    "OT",       // Other
    "PLAN",     // Plan
    "PR",       // Presentation State
    "PT",       // Positron emission tomography (PET)
    "PX",       // Panoramic X-Ray
    "REG",      // Registration
    "RESP",     // Respiratory Waveform
    "RF",       // Radio Fluoroscopy
    "RG",       // Radiographic imaging (conventional film/screen)
    "RTDOSE",   // Radiotherapy Dose
    "RTIMAGE",  // Radiotherapy Image
    "RTPLAN",   // Radiotherapy Plan
    "RTRECORD", // RT Treatment Record
    "RTSTRUCT", // Radiotherapy Structure Set
    "RWV",      // Real World Value Map
    "SEG",      // Segmentation
    "SM",       // Slide Microscopy
    "SMR",      // Stereometric Relationship
    "SR",       // SR Document
    "SRF",      // Subjective Refraction
    "STAIN",    // Automated Slide Stainer
    "TG",       // Thermography
    "US",       // Ultrasound
    "VA",       // Visual Acuity
    "XA",       // X-Ray Angiography
    "XC"        // External-camera Photography
  };
  return s;
}

//-----------------------------------------------------------------------------
const QStringList& excludedFromThumbnailGeneration()
{
  static const QStringList s = {
    "SEG",      // Segmentation
    "SR",       // Structured Report
    "RTSTRUCT", // Radiotherapy Structure Set
    "RTPLAN",   // Radiotherapy Plan
    "RTDOSE",   // Radiotherapy Dose
    "RTRECORD", // RT Treatment Record
    "PR",       // Presentation State
    "DOC",      // Document
    "REG",      // Registration
    "PLAN",     // Plan
    "FID",      // Fiducials
    "KO",       // Key Object Selection
    "RWV",      // Real World Value Map
    "AU",       // Audio
    "ECG",      // Electrocardiography
    "EPS",      // Cardiac Electrophysiology
    "HD",       // Hemodynamic Waveform
    "RESP"      // Respiratory Waveform
  };
  return s;
}

//-----------------------------------------------------------------------------
const QStringList& commonImagingModalities()
{
  static const QStringList s = {
    "CR",       // Computed Radiography
    "CT",       // Computed Tomography
    "DX",       // Digital Radiography
    "MG",       // Mammography
    "MR",       // Magnetic Resonance
    "NM",       // Nuclear Medicine
    "PT",       // Positron emission tomography
    "RF",       // Radio Fluoroscopy
    "SEG",      // Segmentation
    "SR",       // Structured Report
    "US",       // Ultrasound
    "XA"        // X-Ray Angiography
  };
  return s;
}

#if CTK_DEPRECATED_SINCE(0, 1)
// These reference variables bind to the Meyers' singletons above.
// `extern const QStringList&` has trivial construction/destruction (reference
// semantics, no heap allocation), so clazy does not flag them as non-POD globals.
// External projects that use the old PascalCase names continue to compile without
// any source changes; they will only see deprecation warnings.
CTK_DICOM_CORE_EXPORT extern const QStringList& AllModalities = allModalities();
CTK_DICOM_CORE_EXPORT extern const QStringList& ExcludedFromThumbnailGeneration = excludedFromThumbnailGeneration();
CTK_DICOM_CORE_EXPORT extern const QStringList& CommonImagingModalities = commonImagingModalities();
#endif // CTK_DEPRECATED_SINCE(0, 1)

} // namespace ctkDICOMModalities
