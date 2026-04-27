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

#if CTK_DEPRECATED_SINCE(0, 1)

namespace
{
  // Meyers' singletons. Defining the lists in a single translation unit and
  // exposing them through references prevents the per-TU heap allocation that
  // the previous in-header `static const QStringList` definitions caused, and
  // sidesteps clazy's non-pod-global-static warning because a reference to a
  // function-local static has trivial construction at namespace scope.
  const QStringList& allModalitiesSingleton()
  {
    static const QStringList list = {
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
    return list;
  }

  const QStringList& excludedFromThumbnailGenerationSingleton()
  {
    static const QStringList list = {
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
    return list;
  }

  const QStringList& commonImagingModalitiesSingleton()
  {
    static const QStringList list = {
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
    return list;
  }
}

namespace ctkDICOMModalities
{
  const QStringList& AllModalities = allModalitiesSingleton();
  const QStringList& ExcludedFromThumbnailGeneration = excludedFromThumbnailGenerationSingleton();
  const QStringList& CommonImagingModalities = commonImagingModalitiesSingleton();
}

#endif // CTK_DEPRECATED_SINCE(0, 1)
