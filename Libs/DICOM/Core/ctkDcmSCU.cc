/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V.
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
 */

// just a hack for debian, has to be sorted out
// see http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=637687
//
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */

#include "ctkDcmSCU.h"
#include "dcmtk/dcmnet/diutil.h"    /* for dcmnet logger */
#include "dcmtk/dcmdata/dcuid.h"    /* for dcmFindUIDName() */
#include "dcmtk/dcmdata/dcostrmf.h" /* for class DcmOutputFileStream */

#ifdef WITH_ZLIB
#include <zlib.h>                   /* for zlibVersion() */
#endif

/* Remove below if changing to more current DCMTK */
const OFConditionConst ECE_AlreadyConnected(                   OFM_dcmnet, ECC_AlreadyConnected,                  OF_error, "Already Connected");
const OFConditionConst ECE_NoAcceptablePresentationContexts(   OFM_dcmnet, ECC_NoAcceptablePresentationContexts,  OF_error, "No Acceptable Presentation Contexts");
const OFConditionConst ECE_NoPresentationContextsDefined(      OFM_dcmnet, ECC_NoPresentationContextsDefined,     OF_error, "No Presentation Contexts defined");
const OFConditionConst ECE_InvalidSOPClassUID(                 OFM_dcmnet, ECC_InvalidSOPClassUID,                OF_error, "Invalid SOP Class UID");
const OFConditionConst ECE_InvalidSOPInstanceUID(              OFM_dcmnet, ECC_InvalidSOPInstanceUID,             OF_error, "Invalid SOP Instance UID");
const OFConditionConst ECE_UnknownTransferSyntax(              OFM_dcmnet, ECC_UnknownTransferSyntax,             OF_error, "Unknown Transfer Syntax");

const OFCondition NET_EC_AlreadyConnected(                ECE_AlreadyConnected);
const OFCondition NET_EC_NoAcceptablePresentationContext( ECE_NoAcceptablePresentationContexts);
const OFCondition NET_EC_NoPresentationContextsDefined(   ECE_NoPresentationContextsDefined);
/* Remove above if changing to more current DCMTK */

DcmSCU::DcmSCU() :
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
  m_storageDir(),
  m_storageMode(DCMSCU_STORAGE_DISK),
  m_verbosePCMode(OFFalse),
  m_datasetConversionMode(OFFalse),
  m_progressNotificationMode(OFTrue)
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

void DcmSCU::freeNetwork()
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


DcmSCU::~DcmSCU()
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


OFCondition DcmSCU::initNetwork()
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


OFCondition DcmSCU::negotiateAssociation()
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


OFCondition DcmSCU::addPresentationContext(const OFString &abstractSyntax,
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


OFCondition DcmSCU::useSecureConnection(DcmTransportLayer *tlayer)
{
  OFCondition cond = ASC_setTransportLayer(m_net, tlayer, OFFalse /* do not take over ownership */);
  if (cond.good())
    cond = ASC_setTransportLayerType(m_params, OFTrue /* use TLS */);
  return cond;
}


void DcmSCU::clearPresentationContexts()
{
  m_presContexts.clear();
  m_assocConfigFilename.clear();
  m_assocConfigProfile.clear();
}


// Returns usable presentation context ID for a given abstract syntax UID and
// transfer syntax UID. 0 if none matches.
T_ASC_PresentationContextID DcmSCU::findPresentationContextID(const OFString &abstractSyntax,
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
T_ASC_PresentationContextID DcmSCU::findAnyPresentationContextID(const OFString &abstractSyntax,
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

void DcmSCU::findPresentationContext(const T_ASC_PresentationContextID presID,
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


Uint16 DcmSCU::nextMessageID()
{
  if (!isConnected())
    return 0;
  else
    return m_assoc->nextMsgID++;
}


void DcmSCU::closeAssociation(const DcmCloseAssociationType closeType)
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
OFCondition DcmSCU::sendECHORequest(const T_ASC_PresentationContextID presID)
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
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-ECHO Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending C-ECHO Request (MsgID " << req->MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, NULL /*dataObject*/);
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
    if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
      DCMNET_INFO("Received C-ECHO Response");
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
    } else {
      DCMNET_INFO("Received C-ECHO Response (" << rsp.msg.CEchoRSP.DimseStatus << ")");
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
OFCondition DcmSCU::sendSTORERequest(const T_ASC_PresentationContextID presID,
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
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-STORE Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending C-STORE Request (MsgID " << req->MessageID << ", "
      << dcmSOPClassUIDToModality(sopClassUID.c_str(), "OT") << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, dataset);
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
    if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
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
OFCondition DcmSCU::sendMOVERequest(const T_ASC_PresentationContextID presID,
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
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-MOVE Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, dataset, pcid));
  } else {
    DCMNET_INFO("Sending C-MOVE Request (MsgID " << req->MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, dataset);
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
      if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
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
      cond = receiveDIMSEDataset(&pcid, &rspDataset);
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
OFCondition DcmSCU::handleMOVEResponse( const T_ASC_PresentationContextID /* presID */,
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
OFCondition DcmSCU::sendCGETRequest(const T_ASC_PresentationContextID presID,
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
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-GET Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, dataset, pcid));
  } else {
    DCMNET_INFO("Sending C-GET Request (MsgID " << req->MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, dataset);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-GET request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  cond = handleCGETSession(pcid, dataset, responses);
  return cond;
}


// Does the logic for switching between C-GET Response and C-STORE Requests
OFCondition DcmSCU::handleCGETSession(const T_ASC_PresentationContextID /* presID */,
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
      if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
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
      if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
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
        result = receiveDIMSEDataset(&pcid, &rspDataset);
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
        result = handleSTORERequestFile(&pcid, storageFilename, &(rsp.msg.CStoreRQ));
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
OFCondition DcmSCU::handleCGETResponse(const T_ASC_PresentationContextID /* presID */,
                                       RetrieveResponse* response,
                                       OFBool& continueCGETSession)
{
  // Do some basic validity checks
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (response == NULL)
    return DIMSE_NULLKEY;

  DCMNET_DEBUG("Handling C-GET Response");

  /* First, perform separate check for 0xCxxx error codes */
  Uint16 highNibble = response->m_status & 0xf000;
  if (highNibble == STATUS_GET_Failed_UnableToProcess)
  {
    continueCGETSession = OFFalse;
    DCMNET_ERROR("Unable to Process");
    DCMNET_WARN("Full status is 0x"
      << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
      << response->m_status);
    return EC_Normal;
  }

  /* Check for other error codes */
  switch (response->m_status) {
  case STATUS_GET_Refused_OutOfResourcesNumberOfMatches:
    continueCGETSession = OFFalse;
    DCMNET_ERROR("Out of Resouces - Unable to calculate number of matches");
    break;
  case STATUS_GET_Refused_OutOfResourcesSubOperations:
    continueCGETSession = OFFalse;
    DCMNET_ERROR("Out of Resouces - Unable to perform sub-operations");
    break;
  case STATUS_GET_Failed_IdentifierDoesNotMatchSOPClass:
    continueCGETSession = OFFalse;
    DCMNET_ERROR("Identifier does not match SOP class");
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
OFCondition DcmSCU::handleSTORERequest(const T_ASC_PresentationContextID /* presID */,
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

OFCondition DcmSCU::handleSTORERequestFile(T_ASC_PresentationContextID *presID,
                                           const OFString& filename,
                                           T_DIMSE_C_StoreRQ* request)
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
    if (m_progressNotificationMode)
    {
      cond = DIMSE_receiveDataSetInFile(m_assoc, m_blockMode, m_dimseTimeout, presID, filestream,
                                        callbackRECEIVEProgress, this /*callbackData*/);
    } else {
      cond = DIMSE_receiveDataSetInFile(m_assoc, m_blockMode, m_dimseTimeout, presID, filestream,
                                        NULL /*callback*/, NULL /*callbackData*/);
    }
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


OFCondition DcmSCU::sendSTOREResponse(T_ASC_PresentationContextID presID,
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
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-STORE Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  } else {
    DCMNET_INFO("Sending C-STORE Response (" << DU_cstoreStatusString(status) << ")");
  }
  OFCondition cond = sendDIMSEMessage(presID, &response, NULL /*dataObject*/);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-STORE response: " << DimseCondition::dump(tempStr, cond));
  }
  return cond;
}


OFString DcmSCU::createStorageFilename(DcmDataset *dataset)
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


OFCondition DcmSCU::ignoreSTORERequest(T_ASC_PresentationContextID presID,
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


void DcmSCU::notifyInstanceStored(const OFString& filename,
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
OFCondition DcmSCU::sendFINDRequest(const T_ASC_PresentationContextID presID,
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
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-FIND Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, queryKeys, pcid));
  } else {
    DCMNET_INFO("Sending C-FIND Request (MsgID " << req->MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, queryKeys);
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
      if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
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
      cond = receiveDIMSEDataset(&pcid, &rspDataset);
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
OFCondition DcmSCU::handleFINDResponse(const T_ASC_PresentationContextID /* presID */,
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
OFCondition DcmSCU::sendCANCELRequest(const T_ASC_PresentationContextID presID)
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
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-CANCEL Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending C-CANCEL Request (MsgID " << req->MessageIDBeingRespondedTo
      << ", PresID " << OFstatic_cast(unsigned int, pcid) <<  ")");
  }
  cond = sendDIMSEMessage(pcid, &msg, NULL /*dataObject*/);
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
OFCondition DcmSCU::sendACTIONRequest(const T_ASC_PresentationContextID presID,
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
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-ACTION Request");
    // Output dataset only if trace level is enabled
    if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, reqDataset, pcid));
    else
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending N-ACTION Request (MsgID " << actionReq.MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &request, reqDataset);
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
    if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
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
    cond = receiveDIMSEDataset(&tempID, &tempDataset);
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
OFCondition DcmSCU::sendEVENTREPORTRequest(const T_ASC_PresentationContextID presID,
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
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-EVENT-REPORT Request");
    // Output dataset only if trace level is enabled
    if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, reqDataset, pcid));
    else
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending N-EVENT-REPORT Request (MsgID " << eventReportReq.MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &request, reqDataset);
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
    if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
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
    cond = receiveDIMSEDataset(&tempID, &tempDataset);
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
OFCondition DcmSCU::handleEVENTREPORTRequest(DcmDataset *&reqDataset,
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
    if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
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
  cond = receiveDIMSEDataset(&presIDdset, &dataset);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, NULL, presID));
    return DIMSE_BADDATA;
  }

  // Output dataset only if trace level is enabled
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
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

  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-EVENT-REPORT Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  } else {
    DCMNET_INFO("Sending N-EVENT-REPORT Response (" << DU_neventReportStatusString(statusCode) << ")");
  }
  cond = sendDIMSEMessage(presID, &response, NULL /*dataObject*/);
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


Uint16 DcmSCU::checkEVENTREPORTRequest(T_DIMSE_N_EventReportRQ & /*eventReportReq*/,
                                       DcmDataset * /*reqDataset*/)
{
  // we default to success
  return STATUS_Success;
}


/* ************************************************************************* */
/*                         General message handling                          */
/* ************************************************************************* */

void DcmSCU::notifySENDProgress(const unsigned long byteCount)
{
  DCMNET_TRACE("Bytes sent: " << byteCount);
}


void DcmSCU::notifyRECEIVEProgress(const unsigned long byteCount)
{
  DCMNET_TRACE("Bytes received: " << byteCount);
}


/* ************************************************************************* */
/*                            Various helpers                                */
/* ************************************************************************* */

// Sends a DIMSE command and possibly also instance data to the configured peer DICOM application
OFCondition DcmSCU::sendDIMSEMessage(const T_ASC_PresentationContextID presID,
                                     T_DIMSE_Message *msg,
                                     DcmDataset *dataObject,
                                     DcmDataset **commandSet)
{
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;
  if (msg == NULL)
    return DIMSE_NULLKEY;

  OFCondition cond;
  /* call the corresponding DIMSE function to send the message */
  if (m_progressNotificationMode)
  {
    cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, msg, NULL /*statusDetail*/, dataObject,
                                            callbackSENDProgress, this /*callbackData*/, commandSet);
  } else {
    cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, msg, NULL /*statusDetail*/, dataObject,
                                            NULL /*callback*/, NULL /*callbackData*/, commandSet);
  }

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
OFCondition DcmSCU::receiveDIMSECommand(T_ASC_PresentationContextID *presID,
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
OFCondition DcmSCU::receiveDIMSEDataset(T_ASC_PresentationContextID *presID,
                                        DcmDataset **dataObject)
{
  if (!isConnected())
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  /* call the corresponding DIMSE function to receive the dataset */
  if (m_progressNotificationMode)
  {
    cond = DIMSE_receiveDataSetInMemory(m_assoc, m_blockMode, m_dimseTimeout, presID, dataObject,
                                        callbackRECEIVEProgress, this /*callbackData*/);
  } else {
    cond = DIMSE_receiveDataSetInMemory(m_assoc, m_blockMode, m_dimseTimeout, presID, dataObject,
                                        NULL /*callback*/, NULL /*callbackData*/);
  }

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


void DcmSCU::setMaxReceivePDULength(const unsigned long maxRecPDU)
{
  m_maxReceivePDULength = maxRecPDU;
}


void DcmSCU::setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode)
{
  m_blockMode = blockingMode;
}


void DcmSCU::setAETitle(const OFString &myAETtitle)
{
  m_ourAETitle = myAETtitle;
}


void DcmSCU::setPeerHostName(const OFString &peerHostName)
{
  m_peer = peerHostName;
}


void DcmSCU::setPeerAETitle(const OFString &peerAETitle)
{
  m_peerAETitle = peerAETitle;
}


void DcmSCU::setPeerPort(const Uint16 peerPort)
{
  m_peerPort = peerPort;
}


void DcmSCU::setDIMSETimeout(const Uint32 dimseTimeout)
{
  m_dimseTimeout = dimseTimeout;
}


void DcmSCU::setACSETimeout(const Uint32 acseTimeout)
{
  m_acseTimeout = acseTimeout;
}


void DcmSCU::setAssocConfigFileAndProfile(const OFString &filename,
                                          const OFString &profile)
{
  m_assocConfigFilename = filename;
  m_assocConfigProfile = profile;
}


void DcmSCU::setStorageDir(const OFString& storeDir)
{
  m_storageDir = storeDir;
}


void DcmSCU::setStorageMode(const DcmStorageMode storageMode)
{
  m_storageMode = storageMode;
}


void DcmSCU::setVerbosePCMode(const OFBool mode)
{
  m_verbosePCMode = mode;
}


void DcmSCU::setDatasetConversionMode(const OFBool mode)
{
  m_datasetConversionMode = mode;
}


void DcmSCU::setProgressNotificationMode(const OFBool mode)
{
  m_progressNotificationMode = mode;
}


/* Get methods */

OFBool DcmSCU::isConnected() const
{
  return (m_assoc != NULL) && (m_assoc->DULassociation != NULL);
}

Uint32 DcmSCU::getMaxReceivePDULength() const
{
  return m_maxReceivePDULength;
}


OFBool DcmSCU::getTLSEnabled() const
{
  return OFFalse;
}


T_DIMSE_BlockingMode DcmSCU::getDIMSEBlockingMode() const
{
  return m_blockMode;
}


const OFString &DcmSCU::getAETitle() const
{
  return m_ourAETitle;
}


const OFString &DcmSCU::getPeerHostName() const
{
  return m_peer;
}


const OFString &DcmSCU::getPeerAETitle() const
{
  return m_peerAETitle;
}


Uint16 DcmSCU::getPeerPort() const
{
  return m_peerPort;
}


Uint32 DcmSCU::getDIMSETimeout() const
{
  return m_dimseTimeout;
}


Uint32 DcmSCU::getACSETimeout() const
{
  return m_acseTimeout;
}


OFString DcmSCU::getStorageDir() const
{
  return m_storageDir;
}


DcmStorageMode DcmSCU::getStorageMode() const
{
  return m_storageMode;
}


OFBool DcmSCU::getVerbosePCMode() const
{
  return m_verbosePCMode;
}


OFBool DcmSCU::getDatasetConversionMode() const
{
  return m_datasetConversionMode;
}


OFBool DcmSCU::getProgressNotificationMode() const
{
  return m_progressNotificationMode;
}


OFCondition DcmSCU::getDatasetInfo(DcmDataset *dataset,
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
/*                            Callback functions                             */
/* ************************************************************************* */

void DcmSCU::callbackSENDProgress(void *callbackContext,
                                  const unsigned long byteCount)
{
  if (callbackContext != NULL)
    OFreinterpret_cast(DcmSCU *, callbackContext)->notifySENDProgress(byteCount);
}


void DcmSCU::callbackRECEIVEProgress(void *callbackContext,
                                     const unsigned long byteCount)
{
  if (callbackContext != NULL)
    OFreinterpret_cast(DcmSCU *, callbackContext)->notifyRECEIVEProgress(byteCount);
}


/* ************************************************************************* */
/*                          class RetrieveResponse                           */
/* ************************************************************************* */

void RetrieveResponse::print()
{
  DCMNET_INFO("  Number of Remaining Suboperations : " << m_numberOfRemainingSubops);
  DCMNET_INFO("  Number of Completed Suboperations : " << m_numberOfCompletedSubops);
  DCMNET_INFO("  Number of Failed Suboperations    : " << m_numberOfFailedSubops);
  DCMNET_INFO("  Number of Warning Suboperations   : " << m_numberOfWarningSubops);
}
