/*
 *
 *  Copyright (C) 2008-2011, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmnet
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Base class for Service Class Users (SCUs)
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-10-10 14:01:29 $
 *  CVS/RCS Revision: $Revision: 1.58 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

///
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  Note: this class was added at the CTK Hackfest as a copy of the 
//  DcmSCU class in the then current dcmtk
//  http://git.dcmtk.org/dcmtk.git
//  90c7ac4120aec925a41d8742d685fe1671a4a343
//
//  http://www.commontk.org/index.php/CTK-Hackfest-Nov-2011
//
//  When this code is included in a release version of dcmtk, this
//  class can be removed and classes that inherit from it
//  can inherit directly from DcmSCU instead.
//
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */

#include "ctkDcmSCU.h"  // changed for CTK
#include "dcmtk/dcmnet/diutil.h"    /* for dcmnet logger */
#include "dcmtk/dcmdata/dcuid.h"    /* for dcmFindUIDName() */
#include "dcmtk/dcmdata/dcostrmf.h" /* for class DcmOutputFileStream */

#ifdef WITH_ZLIB
#include <zlib.h>     /* for zlibVersion() */
#endif

//
// CTK: copied from diutil.cc
//
static char staticBuf[256];

//
// CTK: copied from diutil.cc
//
const char *
DU_cechoStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    if (statusCode == STATUS_Success)
        s = "Success";
    else {
        sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
        s = staticBuf;
    }
    return s;
}



ctkDcmSCU::ctkDcmSCU() :
  m_assoc(NULL),
  m_net(NULL),
  m_params(NULL),
  m_assocConfigFilename(),
  m_assocConfigProfile(),
  m_presContexts(),
  m_assocConfigFile(),
  m_openDIMSERequest(NULL),
  m_maxReceivePDULength(ASC_DEFAULTMAXPDU),
  m_blockMode(DIMSE_BLOCKING),
  m_ourAETitle("ANY-SCU"),
  m_peer(),
  m_peerAETitle("ANY-SCP"),
  m_peerPort(104),
  m_dimseTimeout(0),
  m_acseTimeout(30),
  m_verbosePCMode(OFFalse),
  m_datasetConversionMode(OFFalse),
  m_storageDir(),
  m_storageMode(DCMSCU_STORAGE_DISK)
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
  WSAData winSockData;
  /* we need at least version 1.1 */
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
  WSAStartup(winSockVersionNeeded, &winSockData); // TODO: check with multiple SCU instances whether this is harmful
#endif
}

void ctkDcmSCU::freeNetwork()
{
  if ((m_assoc != NULL) || (m_net != NULL) || (m_params != NULL))
    DCMNET_DEBUG("Cleaning up internal association and network structures");
  /* destroy association parameters, i.e. free memory of T_ASC_Parameters.
     Usually this is done in ASC_destroyAssociation; however, if we already
     have association parameters but not yet an association (e.g. after calling
     initNetwork() and negotiateAssociation()), the latter approach may fail.
  */
  if (m_params)
  {
    ASC_destroyAssociationParameters(&m_params);
    m_params = NULL;
    // make sure destroyAssocation does not try to free params a second time
    // (happens in case we have already have an association structure)
    if (m_assoc)
      m_assoc->params = NULL;
  }
  // destroy the association, i.e. free memory of T_ASC_Association* structure.
  ASC_destroyAssociation(&m_assoc);
  // drop the network, i.e. free memory of T_ASC_Network* structure.
  ASC_dropNetwork(&m_net);
  // Cleanup old DIMSE request if any
  delete m_openDIMSERequest;
  m_openDIMSERequest = NULL;
}


ctkDcmSCU::~ctkDcmSCU()
{
  // abort association (if any) and destroy dcmnet data structures
  if (isConnected())
  {
    closeAssociation(DCMSCU_ABORT_ASSOCIATION); // also frees network
  } else {
    freeNetwork();
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup(); // TODO: check with multiple SCU instances whether this is harmful
#endif
}


OFCondition ctkDcmSCU::initNetwork()
{
  /* Return if SCU is already connected */
  if (isConnected())
    return NET_EC_AlreadyConnected;

  /* Be sure internal network structures are clean (delete old) */
  freeNetwork();

  OFString tempStr;
  /* initialize network, i.e. create an instance of T_ASC_Network*. */
  OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, m_acseTimeout, &m_net);
  if (cond.bad())
  {
    DimseCondition::dump(tempStr, cond);
    DCMNET_ERROR(tempStr);
    return cond;
  }

  /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
  cond = ASC_createAssociationParameters(&m_params, m_maxReceivePDULength);
  if (cond.bad())
  {
    DCMNET_ERROR(DimseCondition::dump(tempStr, cond));
    return cond;
  }

  /* sets this application's title and the called application's title in the params */
  /* structure. The default values are "ANY-SCU" and "ANY-SCP". */
  ASC_setAPTitles(m_params, m_ourAETitle.c_str(), m_peerAETitle.c_str(), NULL);

  /* Figure out the presentation addresses and copy the */
  /* corresponding values into the association parameters.*/
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  gethostname(localHost, sizeof(localHost) - 1);
  /* Since the underlying dcmnet structures reserve only 64 bytes for peer
     as well as local host name, we check here for buffer overflow.
   */
  if ((m_peer.length() + 5 /* max 65535 */) + 1 /* for ":" */ > 63)
  {
    DCMNET_ERROR("Maximum length of peer host name '" << m_peer << "' is longer than maximum of 57 characters");
    return EC_IllegalCall; // TODO: need to find better error code
  }
  if (strlen(localHost) + 1 > 63)
  {
    DCMNET_ERROR("Maximum length of local host name '" << localHost << "' is longer than maximum of 62 characters");
    return EC_IllegalCall; // TODO: need to find better error code
  }
  sprintf(peerHost, "%s:%d", m_peer.c_str(), OFstatic_cast(int, m_peerPort));
  ASC_setPresentationAddresses(m_params, localHost, peerHost);

  /* Add presentation contexts */

  // First, import from config file, if specified
  OFCondition result;
  if (!m_assocConfigFilename.empty())
  {
    DcmAssociationConfiguration assocConfig;
    result = DcmAssociationConfigurationFile::initialize(assocConfig, m_assocConfigFilename.c_str());
    if (result.bad())
    {
      DCMNET_WARN("Unable to parse association configuration file " << m_assocConfigFilename
        << " (ignored): " << result.text());
      return result;
    }
    else
    {
      /* perform name mangling for config file key */
      OFString profileName;
      const unsigned char *c = OFreinterpret_cast(const unsigned char *, m_assocConfigProfile.c_str());
      while (*c)
      {
        if (! isspace(*c)) profileName += OFstatic_cast(char, toupper(*c));
        ++c;
      }

      result = assocConfig.setAssociationParameters(profileName.c_str(), *m_params);
      if (result.bad())
      {
        DCMNET_WARN("Unable to apply association configuration file " << m_assocConfigFilename
          <<" (ignored): " << result.text());
        return result;
      }
    }
  }

  // Adapt presentation context ID to existing presentation contexts.
  // It's important that presentation context IDs are numerated 1,3,5,7...!
  Uint32 nextFreePresID = 257;
  Uint32 numContexts = ASC_countPresentationContexts(m_params);
  if (numContexts <= 127)
  {
    // Need Uint16 to avoid overflow in currPresID (unsigned char)
    nextFreePresID = 2 * numContexts + 1; /* add 1 to point to the next free ID*/
  }
  // Print warning if number of overall presentation contexts exceeds 128
  if ((numContexts + m_presContexts.size()) > 128)
  {
    DCMNET_WARN("Number of presentation contexts exceeds 128 (" << numContexts + m_presContexts.size()
      << "). Some contexts will not be negotiated");
  }
  else
  {
    DCMNET_TRACE("Configured " << numContexts << " presentation contexts from config file");
    if (m_presContexts.size() > 0)
        DCMNET_TRACE("Adding another " << m_presContexts.size() << " presentation contexts configured for SCU");
  }

  // Add presentation contexts not originating from config file
  OFListIterator(DcmSCUPresContext) contIt = m_presContexts.begin();
  OFListConstIterator(DcmSCUPresContext) endOfContList = m_presContexts.end();
  while ((contIt != endOfContList) && (nextFreePresID <= 255))
  {
    const Uint16 numTransferSyntaxes = OFstatic_cast(Uint16, (*contIt).transferSyntaxes.size());
    const char** transferSyntaxes = new const char*[numTransferSyntaxes];

    // Iterate over transfer syntaxes within one presentation context
    OFListIterator(OFString) syntaxIt = (*contIt).transferSyntaxes.begin();
    OFListIterator(OFString) endOfSyntaxList = (*contIt).transferSyntaxes.end();
    Uint16 sNum = 0;
    // copy all transfer syntaxes to array
    while (syntaxIt != endOfSyntaxList)
    {
      transferSyntaxes[sNum] = (*syntaxIt).c_str();
      ++syntaxIt;
      ++sNum;
    }

    // add the presentation context
    cond = ASC_addPresentationContext(m_params, OFstatic_cast(Uint8, nextFreePresID),
      (*contIt).abstractSyntaxName.c_str(), transferSyntaxes, numTransferSyntaxes,(*contIt).roleSelect);
    // if adding was successfull, prepare presentation context ID for next addition
    delete[] transferSyntaxes;
    transferSyntaxes = NULL;
    if (cond.bad())
      return cond;
    contIt++;
    // goto next free number, only odd presentation context IDs permitted
    nextFreePresID += 2;
  }

  numContexts = ASC_countPresentationContexts(m_params);
  if (numContexts == 0)
  {
    DCMNET_ERROR("Cannot initialize network: No presentation contexts defined");
    return NET_EC_NoPresentationContextsDefined;
  }
  DCMNET_DEBUG("Configured a total of " << numContexts << " presentation contexts for SCU");

  return cond;
}


OFCondition ctkDcmSCU::negotiateAssociation()
{
  /* Return error if SCU is already connected */
  if (isConnected())
    return NET_EC_AlreadyConnected;

  /* dump presentation contexts if required */
  OFString tempStr;
  if (m_verbosePCMode)
    DCMNET_INFO("Request Parameters:" << OFendl << ASC_dumpParameters(tempStr, m_params, ASC_ASSOC_RQ));
  else
    DCMNET_DEBUG("Request Parameters:" << OFendl << ASC_dumpParameters(tempStr, m_params, ASC_ASSOC_RQ));

  /* create association, i.e. try to establish a network connection to another */
  /* DICOM application. This call creates an instance of T_ASC_Association*. */
  DCMNET_INFO("Requesting Association");
  OFCondition cond = ASC_requestAssociation(m_net, m_params, &m_assoc);
  if (cond.bad())
  {
    if (cond == DUL_ASSOCIATIONREJECTED)
    {
      T_ASC_RejectParameters rej;
      ASC_getRejectParameters(m_params, &rej);
      DCMNET_DEBUG("Association Rejected:" << OFendl << ASC_printRejectParameters(tempStr, &rej));
      return cond;
    }
    else
    {
      DCMNET_DEBUG("Association Request Failed: " << DimseCondition::dump(tempStr, cond));
      return cond;
    }
  }

  /* dump the presentation contexts which have been accepted/refused */
  if (m_verbosePCMode)
    DCMNET_INFO("Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(tempStr, m_params, ASC_ASSOC_AC));
  else
    DCMNET_DEBUG("Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(tempStr, m_params, ASC_ASSOC_AC));

  /* count the presentation contexts which have been accepted by the SCP */
  /* If there are none, finish the execution */
  if (ASC_countAcceptedPresentationContexts(m_params) == 0)
  {
    DCMNET_ERROR("No Acceptable Presentation Contexts");
    return NET_EC_NoAcceptablePresentationContexts;
  }

  /* dump general information concerning the establishment of the network connection if required */
  DCMNET_INFO("Association Accepted (Max Send PDV: " << OFstatic_cast(unsigned long, m_assoc->sendPDVLength) << ")");
  return EC_Normal;
}


OFCondition ctkDcmSCU::addPresentationContext(const OFString &abstractSyntax,
                                           const OFList<OFString> &xferSyntaxes,
                                           const T_ASC_SC_ROLE role)

{

  DcmSCUPresContext presContext;
  presContext.abstractSyntaxName = abstractSyntax;
  OFListConstIterator(OFString) it = xferSyntaxes.begin();
  OFListConstIterator(OFString) endOfList = xferSyntaxes.end();
  while (it != endOfList)
  {
    presContext.transferSyntaxes.push_back(*it);
    it++;
  }
  presContext.roleSelect = role;
  m_presContexts.push_back(presContext);
  return EC_Normal;
}


OFCondition ctkDcmSCU::useSecureConnection(DcmTransportLayer *tlayer)
{
  OFCondition cond = ASC_setTransportLayer(m_net, tlayer, OFFalse /* do not take over ownership */);
  if (cond.good())
    cond = ASC_setTransportLayerType(m_params, OFTrue /* use TLS */);
  return cond;
}


void ctkDcmSCU::clearPresentationContexts()
{
  m_presContexts.clear();
  m_assocConfigFilename.clear();
  m_assocConfigProfile.clear();
}


// Returns usable presentation context ID for a given abstract syntax UID and
// transfer syntax UID. 0 if none matches.
T_ASC_PresentationContextID ctkDcmSCU::findPresentationContextID(const OFString &abstractSyntax,
                                                              const OFString &transferSyntax)
{
  if (!isConnected())
    return 0;

  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;
  OFBool found = OFFalse;

  if (abstractSyntax.empty()) return 0;

  /* first of all we look for a presentation context
   * matching both abstract and transfer syntax
   */
  l = &m_assoc->params->DULparams.acceptedPresentationContext;
  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);
  while (pc && !found)
  {
    found = (strcmp(pc->abstractSyntax, abstractSyntax.c_str()) == 0);
    found &= (pc->result == ASC_P_ACCEPTANCE);
    if (!transferSyntax.empty())  // ignore transfer syntax if not specified
      found &= (strcmp(pc->acceptedTransferSyntax, transferSyntax.c_str()) == 0);
    if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
  }
  if (found)
    return pc->presentationContextID;

  return 0;   /* not found */
}

// Returns the presentation context ID that best matches the given abstract syntax UID and
// transfer syntax UID.
T_ASC_PresentationContextID ctkDcmSCU::findAnyPresentationContextID(const OFString &abstractSyntax,
                                                                 const OFString &transferSyntax)
{
  if (m_assoc == NULL)
    return 0;

  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;
  OFBool found = OFFalse;

  if (abstractSyntax.empty()) return 0;

  /* first of all we look for a presentation context
   * matching both abstract and transfer syntax
   */
  l = &m_assoc->params->DULparams.acceptedPresentationContext;
  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);
  while (pc && !found)
  {
    found = (strcmp(pc->abstractSyntax, abstractSyntax.c_str()) == 0);
    found &= (pc->result == ASC_P_ACCEPTANCE);
    if (!transferSyntax.empty())  // ignore transfer syntax if not specified
      found &= (strcmp(pc->acceptedTransferSyntax, transferSyntax.c_str()) == 0);
    if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
  }
  if (found) return pc->presentationContextID;

  /* now we look for an explicit VR uncompressed PC. */
  l = &m_assoc->params->DULparams.acceptedPresentationContext;
  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);
  while (pc && !found)
  {
    found =  (strcmp(pc->abstractSyntax, abstractSyntax.c_str()) == 0)
          && (pc->result == ASC_P_ACCEPTANCE)
          && ((strcmp(pc->acceptedTransferSyntax, UID_LittleEndianExplicitTransferSyntax) == 0)
          || (strcmp(pc->acceptedTransferSyntax, UID_BigEndianExplicitTransferSyntax) == 0));
    if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
  }
  if (found) return pc->presentationContextID;

  /* now we look for an implicit VR uncompressed PC. */
  l = &m_assoc->params->DULparams.acceptedPresentationContext;
  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);
  while (pc && !found)
  {
    found = (strcmp(pc->abstractSyntax, abstractSyntax.c_str()) == 0)
          && (pc->result == ASC_P_ACCEPTANCE)
          && (strcmp(pc->acceptedTransferSyntax, UID_LittleEndianImplicitTransferSyntax) == 0);
    if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
  }
  if (found) return pc->presentationContextID;

  /* finally we accept everything we get.
     returns 0 if abstract syntax is not supported
  */
  return ASC_findAcceptedPresentationContextID(m_assoc, abstractSyntax.c_str());
}

void ctkDcmSCU::findPresentationContext(const T_ASC_PresentationContextID presID,
                                     OFString &abstractSyntax,
                                     OFString &transferSyntax)
{
  transferSyntax.clear();
  abstractSyntax.clear();
  if (m_assoc == NULL)
    return;

  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;

  /* we look for a presentation context matching
   * both abstract and transfer syntax
   */
  l = &m_assoc->params->DULparams.acceptedPresentationContext;
  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);
  while (pc)
  {
    if (presID == pc->presentationContextID)
    {
      if (pc->result == ASC_P_ACCEPTANCE)
      {
        // found a match
        transferSyntax = pc->acceptedTransferSyntax;
        abstractSyntax = pc->abstractSyntax;
      }
      break;
    }
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
  }
}


Uint16 ctkDcmSCU::nextMessageID()
{
  if (!isConnected())
    return 0;
  else
    return m_assoc->nextMsgID++;
}


void ctkDcmSCU::closeAssociation(const DcmCloseAssociationType closeType)
{
  if (!isConnected())
  {
    DCMNET_WARN("Closing of association request but no association active (ignored)");
    return;
  }

  OFCondition cond;
  OFString tempStr;

  /* tear down association, i.e. terminate network connection to SCP */
  switch (closeType)
  {
    case DCMSCU_RELEASE_ASSOCIATION:
      /* release association */
      DCMNET_INFO("Releasing Association");
      cond = ASC_releaseAssociation(m_assoc);
      if (cond.bad())
      {
        DCMNET_ERROR("Association Release Failed: " << DimseCondition::dump(tempStr, cond));
        return; // TODO: do we really need this?
      }
      break;
    case DCMSCU_ABORT_ASSOCIATION:
      /* abort association */
      DCMNET_INFO("Aborting Association");
      cond = ASC_abortAssociation(m_assoc);
      if (cond.bad())
      {
        DCMNET_ERROR("Association Abort Failed: " << DimseCondition::dump(tempStr, cond));
      }
      break;
    case DCMSCU_PEER_REQUESTED_RELEASE:
      /* peer requested release */
      DCMNET_ERROR("Protocol Error: Peer requested release (Aborting)");
      DCMNET_INFO("Aborting Association");
      cond = ASC_abortAssociation(m_assoc);
      if (cond.bad())
      {
        DCMNET_ERROR("Association Abort Failed: " << DimseCondition::dump(tempStr, cond));
      }
      break;
    case DCMSCU_PEER_ABORTED_ASSOCIATION:
      /* peer aborted association */
      DCMNET_INFO("Peer Aborted Association");
      break;
  }

  // destroy and free memory of internal association and network structures
  freeNetwork();
}


/* ************************************************************************* */
/*                            C-ECHO functionality                           */
/* ************************************************************************* */

// Sends C-ECHO request to another DICOM application
OFCondition ctkDcmSCU::sendECHORequest(const T_ASC_PresentationContextID presID)
{
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  T_ASC_PresentationContextID pcid = presID;

  /* If necessary, find appropriate presentation context */
  if (pcid == 0)
    pcid = findPresentationContextID(UID_VerificationSOPClass, UID_LittleEndianExplicitTransferSyntax);
  if (pcid == 0)
    pcid = findPresentationContextID(UID_VerificationSOPClass, UID_BigEndianExplicitTransferSyntax);
  if (pcid == 0)
    pcid = findPresentationContextID(UID_VerificationSOPClass, UID_LittleEndianImplicitTransferSyntax);
  if (pcid == 0)
  {
    DCMNET_ERROR("No presentation context found for sending C-ECHO with SOP Class / Transfer Syntax: "
      << dcmFindNameOfUID(UID_VerificationSOPClass, "") << " / "
      << DcmXfer(UID_LittleEndianImplicitTransferSyntax).getXferName());
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  /* Now, assemble DIMSE message */
  T_DIMSE_Message msg;
  T_DIMSE_C_EchoRQ* req = &(msg.msg.CEchoRQ);
  // Set type of message
  msg.CommandField = DIMSE_C_ECHO_RQ;
  // Set message ID
  req->MessageID = nextMessageID();
  // Announce no dataset
  req->DataSetType = DIMSE_DATASET_NULL;
  // Set affected SOP Class UID (always Verification SOP Class)
  OFStandard::strlcpy(req->AffectedSOPClassUID, UID_VerificationSOPClass, sizeof(req->AffectedSOPClassUID));

  /* Send request */
  OFString tempStr;
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-ECHO Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending C-ECHO Request (MsgID " << req->MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, NULL, NULL /* no callback */, NULL /* callback context */, NULL /* commandset */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-ECHO request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  /* Receive response */
  T_DIMSE_Message rsp;
  DcmDataset* statusDetail = NULL;
  cond = receiveDIMSECommand(&pcid, &rsp, &statusDetail, NULL /* not interested in the command set */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }
  /* Check whether we received C-ECHO response, otherwise print error */
  if (rsp.CommandField == DIMSE_C_ECHO_RSP)
  {
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
      DCMNET_INFO("Received C-ECHO Response");
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
    } else {
      DCMNET_INFO("Received C-ECHO Response (" << DU_cechoStatusString(rsp.msg.CEchoRSP.DimseStatus) << ")");
    }
  } else {
    DCMNET_ERROR("Expected C-ECHO response but received DIMSE command 0x"
      << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
      << OFstatic_cast(unsigned int, rsp.CommandField));
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
    delete statusDetail;
    return DIMSE_BADCOMMANDTYPE;
  }
  /* Print status detail if it was received */
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }
  return EC_Normal;
}

/* ************************************************************************* */
/*                            C-STORE functionality                          */
/* ************************************************************************* */

// Sends C-STORE request to another DICOM application
OFCondition ctkDcmSCU::sendSTORERequest(const T_ASC_PresentationContextID presID,
                                     const OFString &dicomFile,
                                     DcmDataset *dataset,
                                     Uint16 &rspStatusCode)
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  DcmDataset* statusDetail = NULL;
  T_DIMSE_Message msg;
  T_DIMSE_C_StoreRQ* req = &(msg.msg.CStoreRQ);

  // Set type of message
  msg.CommandField = DIMSE_C_STORE_RQ;
  /* Set message ID */
  req->MessageID = nextMessageID();
  /* Load file if necessary */
  DcmFileFormat *fileformat = NULL;
  if (!dicomFile.empty())
  {
    fileformat = new DcmFileFormat();
    if (fileformat == NULL)
      return EC_MemoryExhausted;
    cond = fileformat->loadFile(dicomFile.c_str());
    if (cond.bad())
    {
      delete fileformat;
      return cond;
    }
    dataset = fileformat->getDataset();
  }

  /* Fill message according to dataset to be sent */
  OFString sopClassUID;
  OFString sopInstanceUID;
  E_TransferSyntax xferSyntax = EXS_Unknown;
  cond = getDatasetInfo(dataset, sopClassUID, sopInstanceUID, xferSyntax);
  DcmXfer xfer(xferSyntax);
  /* Check whether the information is sufficient */
  if (sopClassUID.empty() || sopInstanceUID.empty() || ((pcid == 0) && (xferSyntax == EXS_Unknown)))
  {
    DCMNET_ERROR("Cannot send SOP instance, missing information:");
    if (!dicomFile.empty())
      DCMNET_ERROR("  DICOM Filename   : " << dicomFile);
    DCMNET_ERROR("  SOP Class UID    : " << sopClassUID);
    DCMNET_ERROR("  SOP Instance UID : " << sopInstanceUID);
    DCMNET_ERROR("  Transfer Syntax  : " << xfer.getXferName());
    if (pcid == 0)
      DCMNET_ERROR("  Pres. Context ID : 0 (find via SOP Class and Transfer Syntax)");
    else
      DCMNET_ERROR("  Pres. Context ID : " << OFstatic_cast(unsigned int, pcid));
    delete fileformat;
    return cond;
  }
  OFStandard::strlcpy(req->AffectedSOPClassUID, sopClassUID.c_str(), sizeof(req->AffectedSOPClassUID));
  OFStandard::strlcpy(req->AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(req->AffectedSOPInstanceUID));
  req->DataSetType = DIMSE_DATASET_PRESENT;
  req->Priority = DIMSE_PRIORITY_LOW;

  /* If no presentation context is specified by the caller ... */
  if (pcid == 0)
  {
      /* ... try to find an appropriate presentation context automatically */
    pcid = findPresentationContextID(sopClassUID, xfer.getXferID());
  }
  else if (m_datasetConversionMode)
  {
    /* Convert dataset to network transfer syntax (if required) */
    OFString abstractSyntax, transferSyntax;
    findPresentationContext(pcid, abstractSyntax, transferSyntax);
    /* Check whether given presentation context was accepted by the peer */
    if (abstractSyntax.empty() || transferSyntax.empty())
    {
      /* Mark presentation context as invalid */
      pcid = 0;
    } else {
      if (abstractSyntax != sopClassUID)
      {
        DCMNET_WARN("Inappropriate presentation context with ID " << OFstatic_cast(unsigned int, pcid)
          << ": abstract syntax does not match SOP class UID");
      }
      /* Try to convert to the negotiated transfer syntax */
      DcmXfer netXfer = DcmXfer(transferSyntax.c_str()).getXfer();
      if (netXfer.getXfer() != xferSyntax)
      {
        DCMNET_INFO("Converting transfer syntax: " << xfer.getXferName() << " -> "
          << netXfer.getXferName());
        dataset->chooseRepresentation(netXfer.getXfer(), NULL);
      }
    }
  }
  /* No appropriate presentation context for sending */
  if (pcid == 0)
  {
    OFString sopClassName = dcmFindNameOfUID(sopClassUID.c_str(), sopClassUID.c_str());
    OFString xferName = xfer.getXferName();
    DCMNET_ERROR("No presentation context found for sending C-STORE with SOP Class / Transfer Syntax: "
      << sopClassName << " / " << xferName);
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  /* Send request */
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-STORE Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending C-STORE Request (MsgID " << req->MessageID << ", "
      << dcmSOPClassUIDToModality(sopClassUID.c_str(), "OT") << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, dataset, NULL /* callback */, NULL /* callbackContext */);
  delete fileformat;
  fileformat = NULL;
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-STORE request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  /* Receive response */
  T_DIMSE_Message rsp;
  cond = receiveDIMSECommand(&pcid, &rsp, &statusDetail, NULL /* not interested in the command set */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  if (rsp.CommandField == DIMSE_C_STORE_RSP)
  {
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
      DCMNET_INFO("Received C-STORE Response");
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
    } else {
      DCMNET_INFO("Received C-STORE Response (" << DU_cstoreStatusString(rsp.msg.CStoreRSP.DimseStatus) << ")");
    }
  } else {
    DCMNET_ERROR("Expected C-STORE response but received DIMSE command 0x"
      << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
      << OFstatic_cast(unsigned int, rsp.CommandField));
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
    delete statusDetail;
    return DIMSE_BADCOMMANDTYPE;
  }
  T_DIMSE_C_StoreRSP storeRsp = rsp.msg.CStoreRSP;
  rspStatusCode = storeRsp.DimseStatus;
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }

  return cond;
}


/* ************************************************************************* */
/*                            C-MOVE functionality                           */
/* ************************************************************************* */

// Sends a C-MOVE Request on given presentation context
OFCondition ctkDcmSCU::sendMOVERequest(const T_ASC_PresentationContextID presID,
                                    const OFString &moveDestinationAETitle,
                                    DcmDataset *dataset,
                                    OFList<RetrieveResponse*> *responses)
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (dataset == NULL)
    return DIMSE_NULLKEY;

  /* Prepare DIMSE data structures for issuing request */
  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  T_DIMSE_Message msg;
  DcmDataset* statusDetail = NULL;
  T_DIMSE_C_MoveRQ* req = &(msg.msg.CMoveRQ);
  // Set type of message
  msg.CommandField = DIMSE_C_MOVE_RQ;
  // Set message ID
  req->MessageID = nextMessageID();
  // Announce dataset
  req->DataSetType = DIMSE_DATASET_PRESENT;
  // Set target for embedded C-Store's
  OFStandard::strlcpy(req->MoveDestination, moveDestinationAETitle.c_str(), sizeof(req->MoveDestination));
  // Set priority (mandatory)
  req->Priority = DIMSE_PRIORITY_LOW;

  /* Determine SOP Class from presentation context */
  OFString abstractSyntax, transferSyntax;
  findPresentationContext(pcid, abstractSyntax, transferSyntax);
  if (abstractSyntax.empty() || transferSyntax.empty())
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  OFStandard::strlcpy(req->AffectedSOPClassUID, abstractSyntax.c_str(), sizeof(req->AffectedSOPClassUID));

  /* Send request */
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-MOVE Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, dataset, pcid));
  } else {
    DCMNET_INFO("Sending C-MOVE Request (MsgID " << req->MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, dataset, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-MOVE request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  /* Receive and handle C-MOVE response messages */
  OFBool waitForNextResponse = OFTrue;
  while (waitForNextResponse)
  {
    T_DIMSE_Message rsp;
    statusDetail = NULL;

    // Receive command set
    cond = receiveDIMSECommand(&pcid, &rsp, &statusDetail, NULL /* not interested in the command set */);
    if (cond.bad())
    {
      DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
      delete statusDetail;
      break;
    }

    if (rsp.CommandField == DIMSE_C_MOVE_RSP)
    {
      if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
      {
        DCMNET_INFO("Received C-MOVE Response");
        DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
      } else {
        DCMNET_INFO("Received C-MOVE Response (" << DU_cmoveStatusString(rsp.msg.CMoveRSP.DimseStatus) << ")");
      }
    } else {
      DCMNET_ERROR("Expected C-MOVE response but received DIMSE command 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
        << OFstatic_cast(unsigned int, rsp.CommandField));
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
      delete statusDetail;
      cond = DIMSE_BADCOMMANDTYPE;
      break;
    }

    // Prepare response package for response handler
    RetrieveResponse *moveRSP = new RetrieveResponse();
    moveRSP->m_affectedSOPClassUID = rsp.msg.CMoveRSP.AffectedSOPClassUID;
    moveRSP->m_messageIDRespondedTo = rsp.msg.CMoveRSP.MessageIDBeingRespondedTo;
    moveRSP->m_status = rsp.msg.CMoveRSP.DimseStatus;
    moveRSP->m_numberOfRemainingSubops = rsp.msg.CMoveRSP.NumberOfRemainingSubOperations;
    moveRSP->m_numberOfCompletedSubops = rsp.msg.CMoveRSP.NumberOfCompletedSubOperations;
    moveRSP->m_numberOfFailedSubops = rsp.msg.CMoveRSP.NumberOfFailedSubOperations;
    moveRSP->m_numberOfWarningSubops = rsp.msg.CMoveRSP.NumberOfWarningSubOperations;
    moveRSP->m_statusDetail = statusDetail;
    //DCMNET_DEBUG("C-MOVE response has status 0x"
    //  << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
    //  << moveRSP->m_status);
    if (statusDetail != NULL)
    {
      DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    }

    // Receive dataset if there is one (status PENDING)
    DcmDataset *rspDataset = NULL;
    // Check if dataset is announced correctly
    if (rsp.msg.CMoveRSP.DataSetType != DIMSE_DATASET_NULL) // Some of the sub operations have failed, thus a dataset with a list of them is attached
    {
      // Receive dataset
      cond = receiveDIMSEDataset(&pcid, &rspDataset, NULL /* callback */, NULL /* callbackContext */);
      if (cond.bad())
      {
        DCMNET_ERROR("Unable to receive C-MOVE dataset on presentation context "
          << OFstatic_cast(unsigned int, pcid) << ": " << DimseCondition::dump(tempStr, cond));
        delete moveRSP; // includes statusDetail
        break;
      }
      moveRSP->m_dataset = rspDataset;
    }

    // Handle C-MOVE response (has to handle all possible status flags)
    cond = handleMOVEResponse(pcid, moveRSP, waitForNextResponse);
    if (cond.bad())
    {
      DCMNET_WARN("Unable to handle C-MOVE response correctly: " << cond.text() << " (ignored)");
      delete moveRSP; // includes statusDetail
      // don't return here but trust the "waitForNextResponse" variable
    }
    // if response could be handled successfully, add it to response list
    else
    {
      if (responses != NULL) // only add if desired by caller
        responses->push_back(moveRSP);
      else
        delete moveRSP; // includes statusDetail
    }
  }
  /* All responses received or break signal occured */
  return cond;
}


// Standard handler for C-MOVE message responses
OFCondition ctkDcmSCU::handleMOVEResponse( const T_ASC_PresentationContextID /* presID */,
                                        RetrieveResponse *response,
                                        OFBool &waitForNextResponse )
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (response == NULL)
    return DIMSE_NULLKEY;

  DCMNET_DEBUG("Handling C-MOVE Response");
  switch (response->m_status) {
  case STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass:
    waitForNextResponse = OFFalse;
    DCMNET_ERROR("Identifier does not match SOP class in C-MOVE response");
    break;
  case STATUS_MOVE_Failed_MoveDestinationUnknown:
    waitForNextResponse = OFFalse;
    DCMNET_ERROR("Move destination unknown");
    break;
  case STATUS_MOVE_Failed_UnableToProcess:
    waitForNextResponse = OFFalse;
    DCMNET_ERROR("Unable to process C-Move response");
    break;
  case STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication:
    waitForNextResponse = OFFalse;
    DCMNET_DEBUG("Suboperations canceled by server due to CANCEL indication");
    break;
  case STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures:
    waitForNextResponse = OFFalse;
    DCMNET_WARN("Suboperations of C-MOVE completed with one or more failures");
    break;
  case STATUS_Pending:
    /* in this case the current C-MOVE-RSP indicates that */
    /* there will be some more results */
    waitForNextResponse = OFTrue;
    DCMNET_DEBUG("One or more pending C-MOVE responses");
    break;
  case STATUS_Success:
    /* in this case, we received the last C-MOVE-RSP so there */
    /* will be no other responses we have to wait for. */
    waitForNextResponse = OFFalse;
    DCMNET_DEBUG("Received final C-MOVE response, no more C-MOVE responses expected");
    break;
  default:
    /* in all other cases, don't expect further responses to come */
    waitForNextResponse = OFFalse;
    DCMNET_WARN("Status is 0x"
      << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
      << response->m_status << " (unknown)");
    DCMNET_WARN("Will not wait for further C-MOVE responses");
    break;
  } //switch

  return EC_Normal;
}


/* ************************************************************************* */
/*               C-GET and acommpanying C-STORE functionality                */
/* ************************************************************************* */

// Sends a C-GET Request on given presentation context
OFCondition ctkDcmSCU::sendCGETRequest(const T_ASC_PresentationContextID presID,
                                    DcmDataset *dataset,
                                    OFList<RetrieveResponse*> *responses)
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (dataset == NULL)
    return DIMSE_NULLKEY;

  /* Prepare DIMSE data structures for issuing request */
  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  T_DIMSE_Message msg;
  T_DIMSE_C_GetRQ* req = &(msg.msg.CGetRQ);
  // Set type of message
  msg.CommandField = DIMSE_C_GET_RQ;
  // Set message ID
  req->MessageID = nextMessageID();
  // Announce dataset
  req->DataSetType = DIMSE_DATASET_PRESENT;
  // Specify priority
  req->Priority = DIMSE_PRIORITY_LOW;

  // Determine SOP Class from presentation context
  OFString abstractSyntax, transferSyntax;
  findPresentationContext(pcid, abstractSyntax, transferSyntax);
  if (abstractSyntax.empty() || transferSyntax.empty())
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  OFStandard::strlcpy(req->AffectedSOPClassUID, abstractSyntax.c_str(), sizeof(req->AffectedSOPClassUID));

  /* Send request */
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-GET Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, dataset, pcid));
  } else {
    DCMNET_INFO("Sending C-GET Request (MsgID " << req->MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, dataset, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-GET request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  cond = handleCGETSession(pcid, dataset, responses);
  return cond;
}


// Does the logic for switching between C-GET Response and C-STORE Requests
OFCondition ctkDcmSCU::handleCGETSession(const T_ASC_PresentationContextID /* presID */,
                                      DcmDataset * /* dataset */,
                                      OFList<RetrieveResponse*> *responses)
{
  OFCondition result;
  OFBool continueSession = OFTrue;
  OFString tempStr;

  // As long we want to continue (usually, as long as we receive more objects,
  // i.e. the final C-GET reponse has not arrived yet)
  while (continueSession)
  {
    T_DIMSE_Message rsp;
    DcmDataset *statusDetail = NULL;
    T_ASC_PresentationContextID pcid = 0;

    // Receive command set
    result = receiveDIMSECommand(&pcid, &rsp, &statusDetail, NULL /* not interested in the command set */);
    if (result.bad())
    {
      DCMNET_ERROR("Failed receiving DIMSE command: " << DimseCondition::dump(tempStr, result));
      delete statusDetail;
      break;
    }
    // Handle C-GET Response
    if (rsp.CommandField == DIMSE_C_GET_RSP)
    {
      if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
      {
        DCMNET_INFO("Received C-GET Response");
        DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
      } else {
        DCMNET_INFO("Received C-GET Response (" << DU_cgetStatusString(rsp.msg.CGetRSP.DimseStatus) << ")");
      }
      // Prepare response package for response handler
      RetrieveResponse *getRSP = new RetrieveResponse();
      getRSP->m_affectedSOPClassUID = rsp.msg.CGetRSP.AffectedSOPClassUID;
      getRSP->m_messageIDRespondedTo = rsp.msg.CGetRSP.MessageIDBeingRespondedTo;
      getRSP->m_status = rsp.msg.CGetRSP.DimseStatus;
      getRSP->m_numberOfRemainingSubops = rsp.msg.CGetRSP.NumberOfRemainingSubOperations;
      getRSP->m_numberOfCompletedSubops = rsp.msg.CGetRSP.NumberOfCompletedSubOperations;
      getRSP->m_numberOfFailedSubops = rsp.msg.CGetRSP.NumberOfFailedSubOperations;
      getRSP->m_numberOfWarningSubops = rsp.msg.CGetRSP.NumberOfWarningSubOperations;
      getRSP->m_statusDetail = statusDetail;
      if (statusDetail != NULL)
      {
        DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
        statusDetail = NULL; // forget reference to status detail, will be deleted with getRSP
      }
      result = handleCGETResponse(pcid, getRSP, continueSession);
      if (result.bad())
      {
        DCMNET_WARN("Unable to handle C-GET response correctly: " << result.text() << " (ignored)");
        delete getRSP; // includes statusDetail
        // don't return here but trust the "continueSession" variable
      }
      // if response could be handled successfully, add it to response list
      else {
        if (responses != NULL) // only add if desired by caller
          responses->push_back(getRSP);
        else
          delete getRSP; // includes statusDetail
      }
    }

    // Handle C-STORE Request
    else if (rsp.CommandField == DIMSE_C_STORE_RQ)
    {
      if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
      {
        DCMNET_INFO("Received C-STORE Request");
        DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
      } else {
        DCMNET_INFO("Received C-STORE Request (MsgID " << rsp.msg.CStoreRQ.MessageID << ")");
      }
      // Receive dataset if there is one (status PENDING)
      DcmDataset *rspDataset = NULL;
      // Check if dataset is announced correctly
      if (rsp.msg.CStoreRQ.DataSetType == DIMSE_DATASET_NULL)
      {
        DCMNET_WARN("Incoming C-STORE with no dataset, trying to receive one anyway");
      }
      Uint16 desiredCStoreReturnStatus = 0;
      // handle normal storage mode, i.e. receive in memory and store to disk
      if (m_storageMode == DCMSCU_STORAGE_DISK)
      {
        // Receive dataset
        result = receiveDIMSEDataset(&pcid, &rspDataset, NULL /* callback */, NULL /* callbackContext */);
        if (result.bad())
        {
          result = DIMSE_NULLKEY;
          desiredCStoreReturnStatus = STATUS_STORE_Error_CannotUnderstand;
        } else {
          result = handleSTORERequest(pcid, rspDataset, continueSession, desiredCStoreReturnStatus);
        }
      }
      // handle bit preserving storage mode, i.e. receive directly to disk
      else if (m_storageMode == DCMSCU_STORAGE_BIT_PRESERVING)
      {
        OFString storageFilename;
        OFStandard::combineDirAndFilename(storageFilename, m_storageDir, rsp.msg.CStoreRQ.AffectedSOPInstanceUID, OFTrue);
        result = handleSTORERequestFile(&pcid, storageFilename, &(rsp.msg.CStoreRQ), NULL, NULL);
        if (result.good())
        {
          notifyInstanceStored(storageFilename, rsp.msg.CStoreRQ.AffectedSOPClassUID, rsp.msg.CStoreRQ.AffectedSOPInstanceUID);
        }
      }
      // handle ignore storage mode, i.e. ignore received dataset and do not store at all
      else
      {
        result = ignoreSTORERequest(pcid, rsp.msg.CStoreRQ);
      }

      // Evaluate result from C-STORE request handling and send response
      if (result.bad())
      {
        desiredCStoreReturnStatus = STATUS_STORE_Error_CannotUnderstand;
        continueSession = OFFalse;
      }
      result = sendSTOREResponse(pcid, desiredCStoreReturnStatus, rsp.msg.CStoreRQ);
      if (result.bad())
      {
        continueSession = OFFalse;
      }
      delete rspDataset; // should be NULL if not existing
    }

    // Handle other DIMSE command (error since other command than GET/STORE not expected)
    else
    {
      DCMNET_ERROR("Expected C-GET response or C-STORE request but received DIMSE command 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
        << OFstatic_cast(unsigned int, rsp.CommandField));
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
      result = DIMSE_BADCOMMANDTYPE;
      continueSession = OFFalse;
    }

    delete statusDetail; // should be NULL if not existing or added to response list
    statusDetail = NULL;
  }
  /* All responses received or break signal occured */

  return result;

}


// Handles single C-GET Response
OFCondition ctkDcmSCU::handleCGETResponse(const T_ASC_PresentationContextID /* presID */,
                                       RetrieveResponse* response,
                                       OFBool& continueCGETSession)
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (response == NULL)
    return DIMSE_NULLKEY;

  DCMNET_DEBUG("Handling C-GET Response");
  switch (response->m_status) {
  case STATUS_GET_Failed_IdentifierDoesNotMatchSOPClass:
    continueCGETSession = OFFalse;
    DCMNET_ERROR("Identifier does not match SOP class in C-GET response");
    break;
  case STATUS_GET_Failed_UnableToProcess:
    continueCGETSession = OFFalse;
    DCMNET_ERROR("Unable to process C-GET response");
    break;
  case STATUS_GET_Failed_SOPClassNotSupported:
    continueCGETSession = OFFalse;
    DCMNET_ERROR("SOP class not supported");
    break;
  case STATUS_GET_Cancel_SubOperationsTerminatedDueToCancelIndication:
    continueCGETSession = OFFalse;
    DCMNET_DEBUG("Suboperations canceled by server due to CANCEL indication");
    break;
  case STATUS_GET_Warning_SubOperationsCompleteOneOrMoreFailures:
    continueCGETSession = OFFalse;
    DCMNET_WARN("Suboperations of C-GET completed with one or more failures");
    break;
  case STATUS_Pending:
    /* in this case the current C-MOVE-RSP indicates that */
    /* there will be some more results */
    continueCGETSession = OFTrue;
    DCMNET_DEBUG("One or more pending C-GET responses");
    break;
  case STATUS_Success:
    /* in this case, we received the last C-MOVE-RSP so there */
    /* will be no other responses we have to wait for. */
    continueCGETSession = OFFalse;
    DCMNET_DEBUG("Received final C-GET response, no more C-GET responses expected");
    break;
  default:
    /* in all other cases, don't expect further responses to come */
    continueCGETSession = OFFalse;
    DCMNET_WARN("Status is 0x"
      << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
      << response->m_status << " (unknown)");
    DCMNET_WARN("Will not wait for further C-GET responses");
    break;
  } //switch

  return EC_Normal;
}


// Handles single C-STORE Request received during C-GET session
OFCondition ctkDcmSCU::handleSTORERequest(const T_ASC_PresentationContextID /* presID */,
                                       DcmDataset *incomingObject,
                                       OFBool& /* continueCGETSession */,
                                       Uint16& cStoreReturnStatus)
{
  if (incomingObject == NULL)
    return DIMSE_NULLKEY;

  OFString sopClassUID;
  OFString sopInstanceUID;
  OFCondition result = incomingObject->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
  if (result.good())
    result = incomingObject->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
  if (result.bad())
  {
    DCMNET_ERROR("Cannot store received object: either SOP Instance or SOP Class UID not present");
    cStoreReturnStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
    return EC_TagNotFound;
  }

  OFString filename = createStorageFilename(incomingObject);
  result = incomingObject->saveFile(filename.c_str());
  if (result.good())
  {
    E_TransferSyntax xferSyntax;
    getDatasetInfo(incomingObject, sopClassUID, sopInstanceUID, xferSyntax);
    notifyInstanceStored(filename, sopClassUID, sopInstanceUID);
    cStoreReturnStatus = STATUS_Success;
  }
  else
  {
    cStoreReturnStatus = STATUS_STORE_Refused_OutOfResources;
  }

  return result;
}


OFCondition ctkDcmSCU::handleSTORERequestFile(T_ASC_PresentationContextID *presID,
                                           const OFString& filename,
                                           T_DIMSE_C_StoreRQ* request,
                                           DIMSE_ProgressCallback callback,
                                           void *callbackContext)
{
  if (filename.empty())
    return EC_IllegalParameter;

  /* in the following, we want to receive data over the network and write it to a file */
  /* exactly the way it was received over the network. Hence, a filestream will be created and the data */
  /* set will be received and written to the file through the call to DIMSE_receiveDataSetInFile(...).*/
  /* create filestream */
  DcmOutputFileStream *filestream = NULL;
  OFCondition cond = DIMSE_createFilestream(filename.c_str(), request, m_assoc, *presID, OFTrue, &filestream);
  if (cond.good())
  {
    cond = DIMSE_receiveDataSetInFile(m_assoc, m_blockMode, m_dimseTimeout, presID, filestream, callback, &callbackContext);
    delete filestream;
    if (cond != EC_Normal)
    {
      unlink(filename.c_str());
    }
    DCMNET_DEBUG("Received dataset on presentation context " << OFstatic_cast(unsigned int, *presID));
  }
  else
  {
    OFString tempStr;
    DCMNET_ERROR("Unable to receive and store dataset on presentation context "
      << OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
  }
  return cond;
}


OFCondition ctkDcmSCU::sendSTOREResponse(T_ASC_PresentationContextID presID,
                                      Uint16 status,
                                      const T_DIMSE_C_StoreRQ& request)
{
  // Send back response
  T_DIMSE_Message response;
  T_DIMSE_C_StoreRSP &storeRsp = response.msg.CStoreRSP;
  response.CommandField = DIMSE_C_STORE_RSP;
  storeRsp.MessageIDBeingRespondedTo = request.MessageID;
  storeRsp.DimseStatus = status;
  storeRsp.DataSetType = DIMSE_DATASET_NULL;
  storeRsp.opts = 0;
  /* Following information is optional and normally not sent by the underlying
   * dcmnet routines. However, maybe this could be changed later, so insert it.
   */
  OFStandard::strlcpy(storeRsp.AffectedSOPClassUID, request.AffectedSOPClassUID, sizeof(storeRsp.AffectedSOPClassUID));
  OFStandard::strlcpy(storeRsp.AffectedSOPInstanceUID, request.AffectedSOPInstanceUID, sizeof(storeRsp.AffectedSOPInstanceUID));

  OFString tempStr;
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-STORE Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  } else {
    DCMNET_INFO("Sending C-STORE Response (" << DU_cstoreStatusString(status) << ")");
  }
  OFCondition cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-STORE response: " << DimseCondition::dump(tempStr, cond));
  }
  return cond;
}


OFString ctkDcmSCU::createStorageFilename(DcmDataset *dataset)
{
  OFString sopClassUID, sopInstanceUID;
  E_TransferSyntax dummy;
  getDatasetInfo(dataset, sopClassUID, sopInstanceUID, dummy);
  // Create unique filename
  if (sopClassUID.empty() || sopInstanceUID.empty())
    return "";
  OFString name = dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN");
  name += ".";
  name += sopInstanceUID;
  OFString returnStr;
  OFStandard::combineDirAndFilename(returnStr, m_storageDir, name, OFTrue);
  return returnStr;
}


OFCondition ctkDcmSCU::ignoreSTORERequest(T_ASC_PresentationContextID presID,
                                       const T_DIMSE_C_StoreRQ& request)
{

  /* We cannot create the filestream, so ignore the incoming dataset and return an out-of-resources error to the SCU */
  DIC_UL bytesRead = 0;
  DIC_UL pdvCount=0;
  DCMNET_DEBUG("Ignoring incoming C-STORE dataset on presentation context "
    << OFstatic_cast(unsigned int, presID)
    << " with Affected SOP Instance UID: " << request.AffectedSOPInstanceUID );
  OFCondition result = DIMSE_ignoreDataSet(m_assoc, m_blockMode, m_dimseTimeout, &bytesRead, &pdvCount);
  if (result.good())
  {
    DCMNET_TRACE("Successfully skipped " << bytesRead << " bytes in " << pdvCount << " PDVs");
  }
  return result;
}


void ctkDcmSCU::notifyInstanceStored(const OFString& filename,
                                  const OFString& sopClassUID,
                                  const OFString& sopInstanceUID) const
{
  DCMNET_DEBUG("Stored instance to disk:");
  DCMNET_DEBUG("  Filename: " << filename);
  DCMNET_DEBUG("  SOP Class UID: " << sopClassUID);
  DCMNET_DEBUG("  SOP Instance UID: " << sopInstanceUID);
}


/* ************************************************************************* */
/*                            C-FIND functionality                           */
/* ************************************************************************* */

// Sends a C-FIND Request on given presentation context
OFCondition ctkDcmSCU::sendFINDRequest(const T_ASC_PresentationContextID presID,
                                    DcmDataset *queryKeys,
                                    OFList<QRResponse*> *responses)
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (queryKeys == NULL)
    return DIMSE_NULLKEY;

  /* Prepare DIMSE data structures for issuing request */
  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  T_DIMSE_Message msg;
  DcmDataset* statusDetail = NULL;
  T_DIMSE_C_FindRQ* req = &(msg.msg.CFindRQ);
  // Set type of message
  msg.CommandField = DIMSE_C_FIND_RQ;
  // Set message ID
  req->MessageID = nextMessageID();
  // Announce dataset
  req->DataSetType = DIMSE_DATASET_PRESENT;
  // Specify priority
  req->Priority = DIMSE_PRIORITY_LOW;

  // Determine SOP Class from presentation context
  OFString abstractSyntax, transferSyntax;
  findPresentationContext(pcid, abstractSyntax, transferSyntax);
  if (abstractSyntax.empty() || transferSyntax.empty())
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  OFStandard::strlcpy(req->AffectedSOPClassUID, abstractSyntax.c_str(), sizeof(req->AffectedSOPClassUID));

  /* Send request */
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-FIND Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, queryKeys, pcid));
  } else {
    DCMNET_INFO("Sending C-FIND Request (MsgID " << req->MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, queryKeys, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-FIND request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  /* Receive and handle response */
  OFBool waitForNextResponse = OFTrue;
  while (waitForNextResponse)
  {
    T_DIMSE_Message rsp;
    statusDetail = NULL;

    // Receive command set
    cond = receiveDIMSECommand(&pcid, &rsp, &statusDetail, NULL /* not interested in the command set */);
    if (cond.bad())
    {
      DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
      return cond;
    }

    if (rsp.CommandField == DIMSE_C_FIND_RSP)
    {
      if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
      {
        DCMNET_INFO("Received C-FIND Response");
        DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
      } else {
        DCMNET_INFO("Received C-FIND Response (" << DU_cfindStatusString(rsp.msg.CFindRSP.DimseStatus) << ")");
      }
    } else {
      DCMNET_ERROR("Expected C-FIND response but received DIMSE command 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
        << OFstatic_cast(unsigned int, rsp.CommandField));
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
      delete statusDetail;
      return DIMSE_BADCOMMANDTYPE;
    }

    // Prepare response package for response handler
    QRResponse *findRSP = new QRResponse();
    findRSP->m_affectedSOPClassUID = rsp.msg.CFindRSP.AffectedSOPClassUID;
    findRSP->m_messageIDRespondedTo = rsp.msg.CFindRSP.MessageIDBeingRespondedTo;
    findRSP->m_status = rsp.msg.CFindRSP.DimseStatus;
    findRSP->m_statusDetail = statusDetail;

    // Receive dataset if there is one (status PENDING)
    DcmDataset *rspDataset = NULL;
    if (DICOM_PENDING_STATUS(findRSP->m_status))
    {
      // Check if dataset is announced correctly
      if (rsp.msg.CFindRSP.DataSetType == DIMSE_DATASET_NULL)
      {
        DCMNET_ERROR("Received C-FIND response with PENDING status but no dataset announced, aborting");
        delete findRSP; // includes statusDetail
        return DIMSE_BADMESSAGE;
      }

      // Receive dataset
      cond = receiveDIMSEDataset(&pcid, &rspDataset, NULL /* callback */, NULL /* callbackContext */);
      if (cond.bad())
      {
        delete findRSP; // includes statusDetail
        return DIMSE_BADDATA;
      }
      findRSP->m_dataset = rspDataset;
    }

    // Handle C-FIND response (has to handle all possible status flags)
    cond = handleFINDResponse(pcid, findRSP, waitForNextResponse);
    if (cond.bad())
    {
      DCMNET_WARN("Unable to handle C-FIND response correctly: " << cond.text() << " (ignored)");
      delete findRSP; // includes statusDetail and rspDataset
      // don't return here but trust the "waitForNextResponse" variable
    }
    // if response could be handled successfully, add it to response list
    else
    {
      if (responses != NULL) // only add if desired by caller
        responses->push_back(findRSP);
      else
        delete findRSP; // includes statusDetail and rspDataset
    }
  }
  /* All responses received or break signal occured */
  return EC_Normal;
}


// Standard handler for C-FIND message responses
OFCondition ctkDcmSCU::handleFINDResponse(const T_ASC_PresentationContextID /* presID */,
                                       QRResponse *response,
                                       OFBool &waitForNextResponse)
{
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (response == NULL)
    return DIMSE_NULLKEY;

  DCMNET_DEBUG("Handling C-FIND Response");
  switch (response->m_status) {
    case STATUS_Pending:
    case STATUS_FIND_Pending_WarningUnsupportedOptionalKeys:
      /* in this case the current C-FIND-RSP indicates that */
      /* there will be some more results */
      waitForNextResponse = OFTrue;
      DCMNET_DEBUG("One or more pending C-FIND responses");
      break;
    case STATUS_Success:
      /* in this case the current C-FIND-RSP indicates that */
      /* there are no more records that match the search mask */
      waitForNextResponse = OFFalse;
      DCMNET_DEBUG("Received final C-FIND response, no more C-FIND responses expected");
      break;
    default:
      /* in all other cases, don't expect further responses to come */
      waitForNextResponse = OFFalse;
      DCMNET_DEBUG("Status tells not to wait for further C-FIND responses");
      break;
  } //switch
  return EC_Normal;
}


/* ************************************************************************* */
/*                            C-CANCEL functionality                         */
/* ************************************************************************* */

// Send C-CANCEL-REQ and, therefore, ends current C-FIND, -MOVE or -GET session
OFCondition ctkDcmSCU::sendCANCELRequest(const T_ASC_PresentationContextID presID)
{
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;

  /* Prepare DIMSE data structures for issuing request */
  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  T_DIMSE_Message msg;
  T_DIMSE_C_CancelRQ* req = &(msg.msg.CCancelRQ);
  // Set type of message
  msg.CommandField = DIMSE_C_CANCEL_RQ;
  /* Set message ID responded to. A new message ID is _not_ needed so
     we do not increment the message ID here but instead have to give the
     message ID that was used last.
     Note that that it is required to actually use the message ID of the last
     C-FIND/GET/MOVE that was issued on this presentation context channel.
     However, since we only support synchronous association mode so far,
     it is enough to take the last message ID used at all.
     For asynchronous operation, we would have to lookup the message ID
     of the last C-FIND/GET/MOVE request issued and thus, store this
     information after sending it.
   */
  req->MessageIDBeingRespondedTo = m_assoc->nextMsgID - 1;
  // Announce dataset
  req->DataSetType = DIMSE_DATASET_NULL;

  /* We do not care about the transfer syntax since no
     dataset is transported at all, i.e. we trust that the user provided
     the correct presentation context ID (could be private one).
   */
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-CANCEL Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending C-CANCEL Request (MsgID " << req->MessageIDBeingRespondedTo
      << ", PresID " << OFstatic_cast(unsigned int, pcid) <<  ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, NULL /* dataset */, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-CANCEL request: " << DimseCondition::dump(tempStr, cond));
  }

  DCMNET_TRACE("There is no C-CANCEL response in DICOM, so none expected");
  return cond;
}


/* ************************************************************************* */
/*                            N-ACTION functionality                         */
/* ************************************************************************* */

// Sends N-ACTION request to another DICOM application
OFCondition ctkDcmSCU::sendACTIONRequest(const T_ASC_PresentationContextID presID,
                                      const OFString &sopInstanceUID,
                                      const Uint16 actionTypeID,
                                      DcmDataset *reqDataset,
                                      Uint16 &rspStatusCode)
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (sopInstanceUID.empty() || (reqDataset == NULL))
    return DIMSE_NULLKEY;

  // Prepare DIMSE data structures for issuing request
  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  T_DIMSE_Message request;
  T_DIMSE_N_ActionRQ &actionReq = request.msg.NActionRQ;
  DcmDataset *statusDetail = NULL;

  request.CommandField = DIMSE_N_ACTION_RQ;
  actionReq.MessageID = nextMessageID();
  actionReq.DataSetType = DIMSE_DATASET_PRESENT;
  actionReq.ActionTypeID = actionTypeID;

  // Determine SOP Class from presentation context
  OFString abstractSyntax, transferSyntax;
  findPresentationContext(pcid, abstractSyntax, transferSyntax);
  if (abstractSyntax.empty() || transferSyntax.empty())
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  OFStandard::strlcpy(actionReq.RequestedSOPClassUID, abstractSyntax.c_str(), sizeof(actionReq.RequestedSOPClassUID));
  OFStandard::strlcpy(actionReq.RequestedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(actionReq.RequestedSOPInstanceUID));

  // Send request
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-ACTION Request");
    // Output dataset only if trace level is enabled
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, reqDataset, pcid));
    else
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending N-ACTION Request (MsgID " << actionReq.MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &request, reqDataset, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-ACTION request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  // Receive response
  T_DIMSE_Message response;
  cond = receiveDIMSECommand(&pcid, &response, &statusDetail, NULL /* commandSet */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  // Check command set
  if (response.CommandField == DIMSE_N_ACTION_RSP)
  {
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
      DCMNET_INFO("Received N-ACTION Response");
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
    } else {
      DCMNET_INFO("Received N-ACTION Response (" << DU_nactionStatusString(response.msg.NActionRSP.DimseStatus) << ")");
    }
  } else {
    DCMNET_ERROR("Expected N-ACTION response but received DIMSE command 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
        << OFstatic_cast(unsigned int, response.CommandField));
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
    delete statusDetail;
    return DIMSE_BADCOMMANDTYPE;
  }
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }

  // Set return value
  T_DIMSE_N_ActionRSP &actionRsp = response.msg.NActionRSP;
  rspStatusCode = actionRsp.DimseStatus;

  // Check whether there is a dataset to be received
  if (actionRsp.DataSetType == DIMSE_DATASET_PRESENT)
  {
    // this should never happen
    DcmDataset *tempDataset = NULL;
    T_ASC_PresentationContextID tempID;
    DCMNET_WARN("Trying to retrieve unexpected dataset in N-ACTION response");
    cond = receiveDIMSEDataset(&tempID, &tempDataset, NULL /* callback */, NULL /* callbackContext */);
    if (cond.good())
    {
      DCMNET_WARN("Received unexpected dataset after N-ACTION response, ignoring");
      delete tempDataset;
    } else {
      return DIMSE_BADDATA;
    }
  }
  if (actionRsp.MessageIDBeingRespondedTo != actionReq.MessageID)
  {
    // since we only support synchronous communication, the message ID in the response
    // should be identical to the one in the request
    DCMNET_ERROR("Received response with wrong message ID (" << actionRsp.MessageIDBeingRespondedTo
        << " instead of " << actionReq.MessageID << ")");
    return DIMSE_BADMESSAGE;
  }

  return cond;
}


/* ************************************************************************* */
/*                         N-EVENT REPORT functionality                      */
/* ************************************************************************* */

// Sends N-EVENT-REPORT request and receives N-EVENT-REPORT response
OFCondition ctkDcmSCU::sendEVENTREPORTRequest(const T_ASC_PresentationContextID presID,
                                           const OFString &sopInstanceUID,
                                           const Uint16 eventTypeID,
                                           DcmDataset *reqDataset,
                                           Uint16 &rspStatusCode)
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (sopInstanceUID.empty() || (reqDataset == NULL))
    return DIMSE_NULLKEY;

  // Prepare DIMSE data structures for issuing request
  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  T_DIMSE_Message request;
  T_DIMSE_N_EventReportRQ &eventReportReq = request.msg.NEventReportRQ;
  DcmDataset *statusDetail = NULL;

  request.CommandField = DIMSE_N_EVENT_REPORT_RQ;

  // Generate a new message ID
  eventReportReq.MessageID = nextMessageID();
  eventReportReq.DataSetType = DIMSE_DATASET_PRESENT;
  eventReportReq.EventTypeID = eventTypeID;

  // Determine SOP Class from presentation context
  OFString abstractSyntax, transferSyntax;
  findPresentationContext(pcid, abstractSyntax, transferSyntax);
  if (abstractSyntax.empty() || transferSyntax.empty())
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  OFStandard::strlcpy(eventReportReq.AffectedSOPClassUID, abstractSyntax.c_str(), sizeof(eventReportReq.AffectedSOPClassUID));
  OFStandard::strlcpy(eventReportReq.AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(eventReportReq.AffectedSOPInstanceUID));

  // Send request
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-EVENT-REPORT Request");
    // Output dataset only if trace level is enabled
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, reqDataset, pcid));
    else
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending N-EVENT-REPORT Request (MsgID " << eventReportReq.MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &request, reqDataset, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-EVENT-REPORT request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }
  // Receive response
  T_DIMSE_Message response;
  cond = receiveDIMSECommand(&pcid, &response, &statusDetail, NULL /* commandSet */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  // Check command set
  if (response.CommandField == DIMSE_N_EVENT_REPORT_RSP)
  {
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
      DCMNET_INFO("Received N-EVENT-REPORT Response");
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
    } else {
      DCMNET_INFO("Received N-EVENT-REPORT Response (" << DU_neventReportStatusString(response.msg.NEventReportRSP.DimseStatus) << ")");
    }
  } else {
    DCMNET_ERROR("Expected N-EVENT-REPORT response but received DIMSE command 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
        << OFstatic_cast(unsigned int, response.CommandField));
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
    delete statusDetail;
    return DIMSE_BADCOMMANDTYPE;
  }
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }

  // Set return value
  T_DIMSE_N_EventReportRSP &eventReportRsp = response.msg.NEventReportRSP;
  rspStatusCode = eventReportRsp.DimseStatus;

  // Check whether there is a dataset to be received
  if (eventReportRsp.DataSetType == DIMSE_DATASET_PRESENT)
  {
    // this should never happen
    DcmDataset *tempDataset = NULL;
    T_ASC_PresentationContextID tempID;
    cond = receiveDIMSEDataset(&tempID, &tempDataset, NULL /* callback */, NULL /* callbackContext */);
    if (cond.good())
    {
      DCMNET_WARN("Received unexpected dataset after N-EVENT-REPORT response, ignoring");
      delete tempDataset;
    } else {
      DCMNET_ERROR("Failed receiving unexpected dataset after N-EVENT-REPORT response: "
        << DimseCondition::dump(tempStr, cond));
      return DIMSE_BADDATA;
    }
  }

  // Check whether the message ID being responded to is equal to the message ID of the request
  if (eventReportRsp.MessageIDBeingRespondedTo != eventReportReq.MessageID)
  {
    DCMNET_ERROR("Received response with wrong message ID (" << eventReportRsp.MessageIDBeingRespondedTo
        << " instead of " << eventReportReq.MessageID << ")");
    return DIMSE_BADMESSAGE;
  }
  return cond;
}

// Receives N-EVENT-REPORT request
OFCondition ctkDcmSCU::handleEVENTREPORTRequest(DcmDataset *&reqDataset,
                                             Uint16 &eventTypeID,
                                             const int timeout)
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID presID;
  T_ASC_PresentationContextID presIDdset;
  T_DIMSE_Message request;
  T_DIMSE_N_EventReportRQ &eventReportReq = request.msg.NEventReportRQ;
  DcmDataset *dataset = NULL;
  DcmDataset *statusDetail = NULL;
  Uint16 statusCode = 0;

  if (timeout > 0)
    DCMNET_DEBUG("Handle N-EVENT-REPORT request, waiting up to " << timeout << " seconds (only for N-EVENT-REPORT message)");
  else if ((m_dimseTimeout > 0) && (m_blockMode == DIMSE_NONBLOCKING))
    DCMNET_DEBUG("Handle N-EVENT-REPORT request, waiting up to " << m_dimseTimeout << " seconds (default for all DIMSE messages)");
  else
    DCMNET_DEBUG("Handle N-EVENT-REPORT request, waiting an unlimited period of time");

  // Receive request, use specific timeout (if defined)
  cond = receiveDIMSECommand(&presID, &request, &statusDetail, NULL /* commandSet */, timeout);
  if (cond.bad())
  {
    if (cond != DIMSE_NODATAAVAILABLE)
      DCMNET_ERROR("Failed receiving DIMSE request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  // Check command set
  if (request.CommandField == DIMSE_N_EVENT_REPORT_RQ)
  {
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
      DCMNET_INFO("Received N-EVENT-REPORT Request");
    else
      DCMNET_INFO("Received N-EVENT-REPORT Request (MsgID " << eventReportReq.MessageID << ")");
  } else {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Expected N-EVENT-REPORT request but received DIMSE command 0x"
      << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
      << OFstatic_cast(unsigned int, request.CommandField));
    delete statusDetail;
    return DIMSE_BADCOMMANDTYPE;
  }
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Request has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }

  // Check if dataset is announced correctly
  if (eventReportReq.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received N-EVENT-REPORT request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset
  cond = receiveDIMSEDataset(&presIDdset, &dataset, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, NULL, presID));
    return DIMSE_BADDATA;
  }

  // Output dataset only if trace level is enabled
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, dataset, presID));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differ");
    delete dataset;
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  // Check the request dataset and return the DIMSE status code to be used
  statusCode = checkEVENTREPORTRequest(eventReportReq, dataset);

  // Send back response
  T_DIMSE_Message response;
  T_DIMSE_N_EventReportRSP &eventReportRsp = response.msg.NEventReportRSP;
  response.CommandField = DIMSE_N_EVENT_REPORT_RSP;
  eventReportRsp.MessageIDBeingRespondedTo = eventReportReq.MessageID;
  eventReportRsp.DimseStatus = statusCode;
  eventReportRsp.DataSetType = DIMSE_DATASET_NULL;
  eventReportRsp.opts = 0;
  eventReportRsp.AffectedSOPClassUID[0] = 0;
  eventReportRsp.AffectedSOPInstanceUID[0] = 0;

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-EVENT-REPORT Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  } else {
    DCMNET_INFO("Sending N-EVENT-REPORT Response (" << DU_neventReportStatusString(statusCode) << ")");
  }
  cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-EVENT-REPORT response: " << DimseCondition::dump(tempStr, cond));
    delete dataset;
    return cond;
  }

  // Set return values
  reqDataset = dataset;
  eventTypeID = eventReportReq.EventTypeID;

  return cond;
}


Uint16 ctkDcmSCU::checkEVENTREPORTRequest(T_DIMSE_N_EventReportRQ & /*eventReportReq*/,
                                       DcmDataset * /*reqDataset*/)
{
  // we default to success
  return STATUS_Success;
}


/* ************************************************************************* */
/*                            Various helpers                                */
/* ************************************************************************* */

// Sends a DIMSE command and possibly also instance data to the configured peer DICOM application
OFCondition ctkDcmSCU::sendDIMSEMessage(const T_ASC_PresentationContextID presID,
                                     T_DIMSE_Message *msg,
                                     DcmDataset *dataObject,
                                     DIMSE_ProgressCallback callback,
                                     void *callbackContext,
                                     DcmDataset **commandSet)
{
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (msg == NULL)
    return DIMSE_NULLKEY;

  OFCondition cond;
  /* call the corresponding DIMSE function to sent the message */
  cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, msg, NULL /*statusDetail*/, dataObject,
                                          callback, callbackContext, commandSet);

#if 0
  // currently disabled because it is not (yet) needed
  if (cond.good())
  {
    /* create a copy of the current DIMSE command message */
    delete m_openDIMSERequest;
    m_openDIMSERequest = new T_DIMSE_Message;
    memcpy((char*)m_openDIMSERequest, msg, sizeof(*m_openDIMSERequest));
  }
#endif

  return cond;
}


// Receive DIMSE command (excluding dataset!) over the currently open association
OFCondition ctkDcmSCU::receiveDIMSECommand(T_ASC_PresentationContextID *presID,
                                        T_DIMSE_Message *msg,
                                        DcmDataset **statusDetail,
                                        DcmDataset **commandSet,
                                        const Uint32 timeout)
{
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  if (timeout > 0)
  {
    /* call the corresponding DIMSE function to receive the command (use specified timeout)*/
    cond = DIMSE_receiveCommand(m_assoc, DIMSE_NONBLOCKING, timeout, presID,
                                msg, statusDetail, commandSet);
  } else {
    /* call the corresponding DIMSE function to receive the command (use default timeout) */
    cond = DIMSE_receiveCommand(m_assoc, m_blockMode, m_dimseTimeout, presID,
                                msg, statusDetail, commandSet);
  }
  return cond;
}


// Receives one dataset (of instance data) via network from another DICOM application
OFCondition ctkDcmSCU::receiveDIMSEDataset(T_ASC_PresentationContextID *presID,
                                        DcmDataset **dataObject,
                                        DIMSE_ProgressCallback callback,
                                        void *callbackContext)
{
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  /* call the corresponding DIMSE function to receive the dataset */
  cond = DIMSE_receiveDataSetInMemory(m_assoc, m_blockMode, m_dimseTimeout, presID,
                                      dataObject, callback, callbackContext);
  if (cond.good())
  {
    DCMNET_DEBUG("Received dataset on presentation context " << OFstatic_cast(unsigned int, *presID));
  }
  else
  {
    OFString tempStr;
    DCMNET_ERROR("Unable to receive dataset on presentation context "
      << OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
  }
  return cond;
}


void ctkDcmSCU::setMaxReceivePDULength(const unsigned long maxRecPDU)
{
  m_maxReceivePDULength = maxRecPDU;
}


void ctkDcmSCU::setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode)
{
  m_blockMode = blockingMode;
}


void ctkDcmSCU::setAETitle(const OFString &myAETtitle)
{
  m_ourAETitle = myAETtitle;
}


void ctkDcmSCU::setPeerHostName(const OFString &peerHostName)
{
  m_peer = peerHostName;
}


void ctkDcmSCU::setPeerAETitle(const OFString &peerAETitle)
{
  m_peerAETitle = peerAETitle;
}


void ctkDcmSCU::setPeerPort(const Uint16 peerPort)
{
  m_peerPort = peerPort;
}


void ctkDcmSCU::setDIMSETimeout(const Uint32 dimseTimeout)
{
  m_dimseTimeout = dimseTimeout;
}


void ctkDcmSCU::setACSETimeout(const Uint32 acseTimeout)
{
  m_acseTimeout = acseTimeout;
}


void ctkDcmSCU::setAssocConfigFileAndProfile(const OFString &filename,
                                          const OFString &profile)
{
  m_assocConfigFilename = filename;
  m_assocConfigProfile = profile;
}


void ctkDcmSCU::setVerbosePCMode(const OFBool mode)
{
  m_verbosePCMode = mode;
}


void ctkDcmSCU::setDatasetConversionMode(const OFBool mode)
{
  m_datasetConversionMode = mode;
}


void ctkDcmSCU::setStorageDir(const OFString& storeDir)
{
  m_storageDir = storeDir;
}


void ctkDcmSCU::setStorageMode(const DcmStorageMode storageMode)
{
  m_storageMode = storageMode;
}


/* Get methods */

OFBool ctkDcmSCU::isConnected() const
{
  return (m_assoc != NULL) && (m_assoc->DULassociation != NULL);
}

Uint32 ctkDcmSCU::getMaxReceivePDULength() const
{
  return m_maxReceivePDULength;
}


OFBool ctkDcmSCU::getTLSEnabled() const
{
  return OFFalse;
}


T_DIMSE_BlockingMode ctkDcmSCU::getDIMSEBlockingMode() const
{
  return m_blockMode;
}


const OFString &ctkDcmSCU::getAETitle() const
{
  return m_ourAETitle;
}


const OFString &ctkDcmSCU::getPeerHostName() const
{
  return m_peer;
}


const OFString &ctkDcmSCU::getPeerAETitle() const
{
  return m_peerAETitle;
}


Uint16 ctkDcmSCU::getPeerPort() const
{
  return m_peerPort;
}


Uint32 ctkDcmSCU::getDIMSETimeout() const
{
  return m_dimseTimeout;
}


Uint32 ctkDcmSCU::getACSETimeout() const
{
  return m_acseTimeout;
}


OFBool ctkDcmSCU::getVerbosePCMode() const
{
  return m_verbosePCMode;
}


OFBool ctkDcmSCU::getDatasetConversionMode() const
{
  return m_datasetConversionMode;
}


OFString ctkDcmSCU::getStorageDir() const
{
  return m_storageDir;
}


DcmStorageMode ctkDcmSCU::getStorageMode() const
{
  return m_storageMode;
}


OFCondition ctkDcmSCU::getDatasetInfo(DcmDataset *dataset,
                                   OFString &sopClassUID,
                                   OFString &sopInstanceUID,
                                   E_TransferSyntax &transferSyntax)
{
  OFCondition status = EC_IllegalParameter;
  sopClassUID.clear();
  sopInstanceUID.clear();
  transferSyntax = EXS_Unknown;
  if (dataset != NULL)
  {
    // ignore returned condition codes (e.g. EC_TagNotFound)
    dataset->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
    dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
    transferSyntax = dataset->getOriginalXfer();
    // check return values for validity
    if (sopClassUID.empty())
      status = NET_EC_InvalidSOPClassUID;
    else if (sopInstanceUID.empty())
      status = NET_EC_InvalidSOPInstanceUID;
    else if (transferSyntax == EXS_Unknown)
      status = NET_EC_UnknownTransferSyntax;
    else
      status = EC_Normal;
  }
  return status;
}

/* ************************************************************************* */
/*                     class RetrieveResponse                                */
/* ************************************************************************* */

void RetrieveResponse::print()
{
  DCMNET_INFO("  Number of Remaining Suboperations : " << m_numberOfRemainingSubops);
  DCMNET_INFO("  Number of Completed Suboperations : " << m_numberOfCompletedSubops);
  DCMNET_INFO("  Number of Failed Suboperations    : " << m_numberOfFailedSubops);
  DCMNET_INFO("  Number of Warning Suboperations   : " << m_numberOfWarningSubops);
}


/*
** CVS Log
** $Log: scu.cc,v $
** Revision 1.58  2011-10-10 14:01:29  uli
** Moved SCU-specific error condition to the correct place.
**
** Revision 1.57  2011-10-04 08:58:16  joergr
** Added flag that allows for specifying whether to convert a dataset to be
** transferred to the network transfer syntax. Also removed unused parameters
** "rspCommandSet" and "rspStatusDetail" from method sendSTORERequest().
**
** Revision 1.56  2011-09-29 17:12:03  joergr
** Fixed memory leak in sendSTORERequest(), a DICOM dataset was not deleted.
**
** Revision 1.55  2011-09-29 13:12:01  joergr
** Introduced new network-related error codes, e.g. in case that none of the
** proposed presentation contexts were accepted by the association acceptor.
**
** Revision 1.54  2011-09-29 13:04:09  joergr
** Added check whether the presentation context specified by the caller of the
** method was really accepted before sending a C-STORE request.
**
** Revision 1.53  2011-09-29 12:56:21  joergr
** Enhanced implementation of the function that retrieves the abstract syntax
** and transfer syntax of a particular presentation context (using the ID).
**
** Revision 1.52  2011-09-29 09:04:26  joergr
** Output message ID of request and DIMSE status of response messages to the
** INFO logger (if DEBUG level is not enabled). All tools and classes in the
** "dcmnet" module now use (more or less) the same output in verbose mode.
**
** Revision 1.51  2011-09-28 16:28:18  joergr
** Added general support for transfer syntax conversions to sendSTORERequest().
**
** Revision 1.50  2011-09-28 15:25:36  joergr
** Return a more appropriate error code in case the dataset to be sent is
** invalid. This also required to introduce a return value for getDatasetInfo().
**
** Revision 1.49  2011-09-28 14:37:01  joergr
** Output the DIMSE status in verbose mode (if debug mode is not enabled).
**
** Revision 1.48  2011-09-28 13:31:54  joergr
** Added method that allows for clearing the list of presentation contexts.
**
** Revision 1.47  2011-09-23 15:27:02  joergr
** Removed needless deletion of the "statusDetail" variable.
**
** Revision 1.46  2011-09-16 09:38:40  joergr
** Fixed some typos and other small inconsistencies.
**
** Revision 1.45  2011-09-06 16:12:53  ogazzar
** Fixed typos in a log commit message.
**
** Revision 1.44  2011-09-06 14:15:10  ogazzar
** Fixed wrong logger name which caused compiler error.
**
** Revision 1.43  2011-09-06 12:58:35  ogazzar
** Added a function to send N-EVENT-REPORT request and to receive a response.
**
** Revision 1.42  2011-08-25 15:46:20  joergr
** Further cleanup of minor inconsistencies regarding documentation, parameter
** names, log output and handling of status details information.
**
** Revision 1.41  2011-08-25 15:05:09  joergr
** Changed data structure for Q/R responses from OFVector to OFList. Also fixed
** some possible memory leaks and made the FIND/MOVE/GET code more consistent.
**
** Revision 1.40  2011-08-25 13:49:31  joergr
** Fixed minor issues in the documentation, parameter and method names. Output
** retrieve responses to main dcmnet logger instead of response logger.
**
** Revision 1.39  2011-08-25 09:31:35  onken
** Added C-GET functionality to DcmSCU class and accompanying getscu
** commandline application.
**
** Revision 1.38  2011-08-24 11:50:48  joergr
** Uncommented name of unused method parameter that caused a compiler warning.
**
** Revision 1.37  2011-07-06 11:08:48  uli
** Fixed various compiler warnings.
**
** Revision 1.36  2011-06-29 16:33:45  joergr
** Fixed various issues that are reported when compiled with "gcc -Weffc++".
**
** Revision 1.35  2011-06-01 15:04:29  onken
** Removed unused status variable from C-ECHO code.
**
** Revision 1.34  2011-05-30 20:10:44  onken
** Added dump of query/move keys in debug mode when sending C-FIND or C-MOVE.
**
** Revision 1.33  2011-05-27 10:12:18  joergr
** Fixed typos and source code formatting.
**
** Revision 1.32  2011-05-25 09:56:52  ogazzar
** Renamed a function name.
**
** Revision 1.31  2011-05-25 09:31:53  ogazzar
** Added a function to look for a presentation context ID that best matches the
** abstract syntax UID and the transfer syntax UID.
**
** Revision 1.30  2011-05-24 08:38:39  ogazzar
** Added role selection negotiation while adding a presenation context.
**
** Revision 1.29  2011-05-19 17:19:52  onken
** Fixed some documentation. Added some extra checks for NULL when handling MOVE
** and FIND responses. Simplified destructors for FIND and MOVEResponses.
**
** Revision 1.28  2011-05-19 10:51:20  onken
** Simplified C string copy by using OFStandard::strlcpy and removed debugging
** code introduced with last comit.
**
** Revision 1.27  2011-05-19 10:37:44  onken
** Removed unused variable that caused compiler warning. Fixed typo.
**
** Revision 1.26  2011-05-19 09:57:24  onken
** Fixed message ID field in C-CANCEL request (should be the one of last
** request). In case of error status codes in C-MOVE responses, the default
** behaviour is now to not wait for further responses. Fixed log output level
** to better fit the messages while receiveing C-MOVE responses. Minor
** code and comment cleanups. Renamed function parameter in sendMOVEREquest
** to better reflect the standard.
**
** Revision 1.25  2011-05-19 08:08:30  onken
** Fixed wrong usage of strlcpy in new C-CANCEL function.
**
** Revision 1.24  2011-05-17 14:26:19  onken
** Implemented C-CANCEL message. Fixed some minor formatting issues.
** Changed C-ECHO implementation to rely on sendDIMSEMesage as the other
** DIMSE functions do. Changed some public function arguments to const to be
** more correct. Fixed CVS log at the end of the scu.cc file.
**
** Revision 1.23  2011-04-28 17:50:05  onken
** Re-sorted header list to make scu.h come first (after osconfig.h).
** Protected public networking functions for creating an association
** from being called twice. Enhanced protection of DIMSE messaging
** functions from being called without being connected. Introduced
** status detail into C-FIND responses (and C-MOVE responses). Was
**  not accessible to the caller before. Minor code cleanups. Added
** C-MOVE code for retrieving DICOM objects. So far only retrieving
** on a separate connection is supported. Added function for cleaning
** up internal memory from destructor. This function also fixes a
** memory leak in case users call initNetwork more than one time.
** Added error code returned by functions if SCU is already connected.
**
** Revision 1.22  2011-04-18 07:01:03  uli
** Use global variables for the logger objects. This removes the thread-unsafe
** static local variables which were used before.
**
** Revision 1.21  2011-04-05 11:16:13  joergr
** Output DIMSE status code in hexadecimal format to the logger. Removed unused
** code (local half-implemented function). Added more comments.
**
** Revision 1.20  2011-03-09 11:13:28  onken
** Enhanced error message for missing data in store request.
**
** Revision 1.19  2011-02-23 08:11:51  joergr
** Fixed issue with undefined priority field in C-STORE and C-FIND request.
**
** Revision 1.18  2011-02-16 08:55:17  joergr
** Fixed issue in sendSTORERequest() when sending a dataset that was created
** in memory (and which has, therefore, an original transfer of EXS_Unknown).
**
** Revision 1.17  2011-02-04 12:57:40  uli
** Made sure all members are initialized in the constructor (-Weffc++).
**
** Revision 1.16  2010-12-21 09:37:36  onken
** Fixed wrong response assignment in DcmSCU's C-STORE code. Thanks to
** forum user "takeos" for the hint and fix.
**
** Revision 1.15  2010-10-20 07:41:36  uli
** Made sure isalpha() & friends are only called with valid arguments.
**
** Revision 1.14  2010-10-14 13:14:29  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.13  2010-10-01 12:25:29  uli
** Fixed most compiler warnings in remaining modules.
**
** Revision 1.12  2010-08-10 11:59:32  uli
** Fixed some cases where dcmFindNameOfUID() returning NULL could cause crashes.
**
** Revision 1.11  2010-06-24 09:26:57  joergr
** Added check on whether the presentation context ID of command and data set are
** identical. Made sure that received dataset is deleted when an error occurs.
** Used more appropriate error conditions / return codes. Further code cleanup.
**
** Revision 1.10  2010-06-22 15:48:53  joergr
** Introduced new enumeration type to be used for closeAssociation().
** Further code cleanup. Renamed some methods, variables, types and so on.
**
** Revision 1.9  2010-06-18 14:58:01  joergr
** Changed some error conditions / return codes to more appropriate values.
** Further revised logging output. Use DimseCondition::dump() where appropriate.
**
** Revision 1.8  2010-06-17 17:13:06  joergr
** Added preliminary support for N-EVENT-REPORT to DcmSCU. Some further code
** cleanups and enhancements. Renamed some methods. Revised documentation.
**
** Revision 1.7  2010-06-09 16:33:34  joergr
** Added preliminary support for N-ACTION to DcmSCU. Some further code cleanups
** and enhancements.
**
** Revision 1.6  2010-06-08 17:54:14  onken
** Added C-FIND functionality to DcmSCU. Some code cleanups. Fixed
** memory leak sometimes occuring during association configuration.
**
** Revision 1.5  2010-06-02 16:01:49  joergr
** Slightly modified some log messages and levels for reasons of consistency.
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.4  2010-04-29 16:13:25  onken
** Made SCU class independent from dcmtls, i.e. outsourced TLS API. Added
** direct API support for sending C-STORE requests. Further API changes and
** some bugs fixed.
**
** Revision 1.3  2009-12-21 15:33:58  onken
** Added documentation and refactored / enhanced some code.
**
** Revision 1.2  2009-12-17 09:12:27  onken
** Fixed other scu and scp base class compile issues.
**
** Revision 1.1  2009-12-16 17:05:35  onken
** Added base classes for SCU and SCP implementation.
**
** Revision 1.5  2009-12-02 14:26:05  uli
** Stop including dcdebug.h which was removed.
**
** Revision 1.4  2009-11-18 12:37:28  uli
** Fix compiler errors due to removal of DUL_Debug() and DIMSE_Debug().
**
** Revision 1.3  2009-01-08 18:25:34  joergr
** Replaced further OFListIterator() by OFListConstIterator() in order to
** compile when STL list classes are used.
**
** Revision 1.2  2009-01-08 13:33:31  joergr
** Replaced OFListIterator() by OFListConstIterator() in order to compile when
** STL list classes are used.
**
** Revision 1.1  2008-09-29 13:51:55  onken
** Initial checkin of module dcmppscu implementing an MPPS commandline client.
**
*/
