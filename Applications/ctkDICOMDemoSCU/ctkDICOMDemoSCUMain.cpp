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

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/scu.h"

// STD includes
#include <cstdlib>
#include <iostream>
#include <fstream>

void print_usage()
{
  std::cerr << "Usage: \n";
  std::cerr << "  ctkDICOMDemoSCU peer port [peerAETitle]\n";
  std::cerr << "     Issues ECHO request to the given host and given port.\n"; 
  std::cerr << "     Optional peerAETitle tells what application entity to address.\n"; 
  return;
}

int main(int argc, char** argv)
{
  // Check whether host and port are given
  if (argc < 3)
  {
    print_usage();
    return 2;
  } 
  
  OFString host(argv[1]);
  unsigned int port = atoi(argv[2]);
  OFString peerAET;
  if (argc > 3)
  {
    peerAET = argv[3];
  }
    
  // Setup SCU
  DcmSCU scu;
  scu.setPeerHostName(host);
  scu.setPeerPort(port);
  OFString verificationSOP = UID_VerificationSOPClass;
  OFList<OFString> ts;
  ts.push_back(UID_LittleEndianExplicitTransferSyntax);
  ts.push_back(UID_BigEndianExplicitTransferSyntax);  
  ts.push_back(UID_LittleEndianImplicitTransferSyntax);
  scu.addPresentationContext(verificationSOP, ts);
  if (peerAET != "")
  {
    scu.setPeerAETitle(peerAET);
  }
  OFCondition result = scu.initNetwork();
  if (result.bad())
  {
    std::cerr << "Error setting up SCU: " << result.text() << "\n";
    return 2;
  }
  
  // Negotiate association
  result = scu.negotiateAssociation();
  if (result.bad())
  {
    std::cerr << "Error negotiating association: " << result.text() << "\n";
    return 2;
  }
  
  // Issue ECHO request and let scu find presentation context itself (0)
  result = scu.sendECHORequest(0);
  if (result.bad())
  { 
    std::cerr << "Error issuing ECHO request or received rejecting response: " << result.text() << "\n";
    return 2;
  }
  std::cout << "Successfully sent DICOM Echo to host " << argv[1] << " on port " << argv[2] << "\n";
  return 0;
  
}
