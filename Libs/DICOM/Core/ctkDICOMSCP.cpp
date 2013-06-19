#include "ctkDICOMSCP.h"

// DCMTK includes
#include <dcmtk/dcmqrdb/dcmqrsrv.h>

// Qt includes
#include <QtConcurrentRun>

// CTK includes
#include "ctkLogger.h"

static ctkLogger logger ( "org.commontk.dicom.DcmSCP" );

class ctkDICOMSCPPrivate
{
public:
  ctkDICOMSCPPrivate(ctkDICOMSCP* parent);
  ~ctkDICOMSCPPrivate();

  QString aeTitle;
  int port;
  int maxPDU;
  int maxAssociation;
  int acseTimeout;
  bool isGetEnabled;
  bool isPatientRootEnabled;
  bool isStudyRootEnabled;
  bool isPatientStudyOnlyEnabled;
  bool isShutdownAllowed;
  bool isThreadingEnabled;
  bool terminateRequested;

  /** wait for incoming A-ASSOCIATE requests, perform association negotiation
   *  and serve the requests. May fork child processes depending on availability
   *  of the fork() system function and configuration options.
   *  @param theNet network structure for listen socket
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition waitForAssociation(T_ASC_Network *theNet);

  OFCondition negotiateAssociation(T_ASC_Association * assoc);
  OFCondition refuseAssociation(T_ASC_Association ** assoc, CTN_RefuseReason reason);
  OFCondition handleAssociation(T_ASC_Association * assoc, OFBool correctUIDPadding);
  OFCondition dispatch(T_ASC_Association *assoc, OFBool correctUIDPadding);

  void threadedStartServer();
  void threadedHandleAssociation(T_ASC_Association * assoc,
                                 OFBool correctUIDPadding);

  QMap<T_DIMSE_Command, ctkDICOMSCP*> registeredSCPs;
  T_ASC_Network* network;

private:
  ctkDICOMSCP* q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMSCP);
};

//------------------------------------------------------------------------------
// ctkDICOMSCPPrivate class methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ctkDICOMSCPPrivate::ctkDICOMSCPPrivate(ctkDICOMSCP *parent):
  q_ptr(parent)
{
  this->aeTitle = "AETitle";
  this->port = 104;
  this->maxPDU = 100000;
  this->maxAssociation = 20;
  this->acseTimeout = 30;
  this->isGetEnabled = false;
  this->isPatientRootEnabled = false;
  this->isStudyRootEnabled = true;
  this->isPatientStudyOnlyEnabled = false;
  this->isShutdownAllowed = true;
  this->terminateRequested = false;
  this->isThreadingEnabled = true;

  this->network = new T_ASC_Network();
}

//------------------------------------------------------------------------------
ctkDICOMSCPPrivate::~ctkDICOMSCPPrivate()
{
  delete this->network;
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMSCPPrivate::waitForAssociation(T_ASC_Network * theNet)
{
  Q_Q(ctkDICOMSCP);

  OFCondition cond = EC_Normal;
  OFString temp_str;

  T_ASC_Association  *assoc;
  char                buf[BUFSIZ];
  int timeout = 1000;
  OFBool go_cleanup = OFFalse;

  if (ASC_associationWaiting(theNet, timeout))
  {
    cond = ASC_receiveAssociation(theNet, &assoc, this->maxPDU);
    if (cond.bad())
    {
      DimseCondition::dump(temp_str, cond);
      logger.debug(QString("Failed to receive association: ") + temp_str.c_str());
      go_cleanup = OFTrue;
    }
  } else return EC_Normal;

  if (! go_cleanup)
  {
    logger.info(QString("Association Received (") + assoc->params->DULparams.callingPresentationAddress
                + ":" + assoc->params->DULparams.callingAPTitle + " -> "
                + assoc->params->DULparams.calledAPTitle + ")");

    ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ);
    logger.debug(QString("Parameters:") + temp_str.c_str());
  }

  if (! go_cleanup)
  {
    /* Application Context Name */
    cond = ASC_getApplicationContextName(assoc->params, buf);
    if (cond.bad() || strcmp(buf, DICOM_STDAPPLICATIONCONTEXT) != 0)
    {
      /* reject: the application context name is not supported */
      logger.info(QString("Bad AppContextName: ") + buf);
      cond = this->refuseAssociation(&assoc, CTN_BadAppContext);
      go_cleanup = OFTrue;
    }
  }

  if (! go_cleanup)
  {
    /* Implementation Class UID */
    if (strlen(assoc->params->theirImplementationClassUID) == 0)
    {
      /* reject: no implementation Class UID provided */
      logger.info(QString("No implementation Class UID provided"));
      cond = this->refuseAssociation(&assoc, CTN_NoReason);
      go_cleanup = OFTrue;
    }
  }

  // TODO: Check whether peer is in AETitle list
  /*
  if (! go_cleanup)
  {
    // Does peer AE have access to required service ??
    if (! config_->peerInAETitle(assoc->params->DULparams.calledAPTitle,
                                 assoc->params->DULparams.callingAPTitle,
                                 assoc->params->DULparams.callingPresentationAddress))
    {
      DCMQRDB_DEBUG("Peer "
                    << assoc->params->DULparams.callingPresentationAddress << ":"
                    << assoc->params->DULparams.callingAPTitle << " is not not permitted to access "
                    << assoc->params->DULparams.calledAPTitle << " (see configuration file)");
      cond = refuseAssociation(&assoc, CTN_BadAEService);
      go_cleanup = OFTrue;
    }
  }
  */

  // TODO: Check if the number of associations exceeds the maximum number of associations
  /*
  if (! go_cleanup)
  {
    // too many concurrent associations ??
    if (processtable_.countChildProcesses() >= OFstatic_cast(size_t, d->maxAssociation))
    {
      cond = d->refuseAssociation(&assoc, CTN_TooManyAssociations);
      go_cleanup = OFTrue;
    }
  }
  */

  if (! go_cleanup)
  {
    cond = this->negotiateAssociation(assoc);
    if (cond.bad()) go_cleanup = OFTrue;
  }

  if (! go_cleanup)
  {
    cond = ASC_acknowledgeAssociation(assoc);
    if (cond.bad())
    {
      DimseCondition::dump(temp_str, cond);
      logger.error(QString(temp_str.c_str()));
      go_cleanup = OFTrue;
    }
  }

  if (! go_cleanup)
  {
    logger.debug(QString("Association Acknowledged (Max Send PDV: ") + QString::number(assoc->sendPDVLength) + ")");

    QtConcurrent::run(this, &ctkDICOMSCPPrivate::threadedHandleAssociation, assoc, false);
    return EC_Normal;
  }

  // cleanup code
  OFCondition oldcond = cond;    /* store condition flag for later use */
  if (cond != ASC_SHUTDOWNAPPLICATION)
  {
    /* the child will handle the association, we can drop it */
    cond = ASC_dropAssociation(assoc);
    if (cond.bad())
    {
      DimseCondition::dump(temp_str, cond);
      logger.error(QString("Cannot Drop Association: ") + temp_str.c_str());
    }
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad())
    {
      DimseCondition::dump(temp_str, cond);
      logger.error(QString("Cannot Destroy Association: ") + temp_str.c_str());
    }
  }

  if (oldcond == ASC_SHUTDOWNAPPLICATION) cond = oldcond; /* abort flag is reported to top-level wait loop */
  return cond;
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMSCPPrivate::negotiateAssociation(T_ASC_Association * assoc)
{
  OFCondition cond = EC_Normal;
  int i;
  OFString temp_str;
  struct { const char *moveSyntax, *findSyntax; } queryRetrievePairs[] =
  {
  { UID_MOVEPatientRootQueryRetrieveInformationModel,
        UID_FINDPatientRootQueryRetrieveInformationModel },
  { UID_MOVEStudyRootQueryRetrieveInformationModel,
        UID_FINDStudyRootQueryRetrieveInformationModel },
  { UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel,
        UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel }
};

  DIC_AE calledAETitle;
  ASC_getAPTitles(assoc->params, NULL, calledAETitle, NULL);

  const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

  transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
  transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
  transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
  int numTransferSyntaxes = 3;

  const char * const nonStorageSyntaxes[] =
  {
    UID_VerificationSOPClass,
    UID_FINDPatientRootQueryRetrieveInformationModel,
    UID_MOVEPatientRootQueryRetrieveInformationModel,
    UID_GETPatientRootQueryRetrieveInformationModel,
  #ifndef NO_PATIENTSTUDYONLY_SUPPORT
    UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,
    UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel,
    UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel,
  #endif
    UID_FINDStudyRootQueryRetrieveInformationModel,
    UID_MOVEStudyRootQueryRetrieveInformationModel,
    UID_GETStudyRootQueryRetrieveInformationModel,
    UID_PrivateShutdownSOPClass
  };

  const int numberOfNonStorageSyntaxes = DIM_OF(nonStorageSyntaxes);
  const char *selectedNonStorageSyntaxes[DIM_OF(nonStorageSyntaxes)];
  int numberOfSelectedNonStorageSyntaxes = 0;
  for (i = 0; i < numberOfNonStorageSyntaxes; i++)
  {
    if (0 == strcmp(nonStorageSyntaxes[i], UID_FINDPatientRootQueryRetrieveInformationModel))
    {
      if (this->isPatientRootEnabled) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
    else if (0 == strcmp(nonStorageSyntaxes[i], UID_MOVEPatientRootQueryRetrieveInformationModel))
    {
      if (this->isPatientRootEnabled) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
    else if (0 == strcmp(nonStorageSyntaxes[i], UID_GETPatientRootQueryRetrieveInformationModel))
    {
      if (this->isPatientRootEnabled && (this->isGetEnabled)) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
    else if (0 == strcmp(nonStorageSyntaxes[i], UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel))
    {
      if (this->isPatientStudyOnlyEnabled) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
    else if (0 == strcmp(nonStorageSyntaxes[i], UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel))
    {
      if (this->isPatientStudyOnlyEnabled) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
    else if (0 == strcmp(nonStorageSyntaxes[i], UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel))
    {
      if (this->isPatientStudyOnlyEnabled && (this->isGetEnabled)) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
    else if (0 == strcmp(nonStorageSyntaxes[i], UID_FINDStudyRootQueryRetrieveInformationModel))
    {
      if (this->isStudyRootEnabled) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
    else if (0 == strcmp(nonStorageSyntaxes[i], UID_MOVEStudyRootQueryRetrieveInformationModel))
    {
      if (this->isStudyRootEnabled) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
    else if (0 == strcmp(nonStorageSyntaxes[i], UID_GETStudyRootQueryRetrieveInformationModel))
    {
      if (this->isStudyRootEnabled && (this->isGetEnabled)) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
    else if (0 == strcmp(nonStorageSyntaxes[i], UID_PrivateShutdownSOPClass))
    {
      if (this->isShutdownAllowed) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    } else {
      selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
    }
  }

  /*  accept any of the non-storage syntaxes */
  cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
        assoc->params,
        (const char**)selectedNonStorageSyntaxes, numberOfSelectedNonStorageSyntaxes,
        (const char**)transferSyntaxes, numTransferSyntaxes);
  if (cond.bad()) {
    DimseCondition::dump(temp_str, cond);
    logger.error(QString("Cannot accept presentation contexts: ") + temp_str.c_str());
  }

  /*  accept any of the storage syntaxes */
  if (!this->isGetEnabled)
  {
    /* accept storage syntaxes with default role only */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
          assoc->params,
          dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs,
          (const char**)transferSyntaxes, DIM_OF(transferSyntaxes));
    if (cond.bad()) {
      DimseCondition::dump(temp_str, cond);
      logger.error(QString("Cannot accept presentation contexts: ") + temp_str.c_str());
    }
  } else {
    /* accept storage syntaxes with proposed role */
    T_ASC_PresentationContext pc;
    T_ASC_SC_ROLE role;
    int npc = ASC_countPresentationContexts(assoc->params);
    for (i = 0; i < npc; i++)
    {
      ASC_getPresentationContext(assoc->params, i, &pc);
      if (dcmIsaStorageSOPClassUID(pc.abstractSyntax))
      {
        /*
          ** We are prepared to accept whatever role he proposes.
          ** Normally we can be the SCP of the Storage Service Class.
          ** When processing the C-GET operation we can be the SCU of the Storage Service Class.
          */
        role = pc.proposedRole;

        /*
          ** Accept in the order "least wanted" to "most wanted" transfer
          ** syntax.  Accepting a transfer syntax will override previously
          ** accepted transfer syntaxes.
          */
        for (int k = numTransferSyntaxes - 1; k >= 0; k--)
        {
          for (int j = 0; j < (int)pc.transferSyntaxCount; j++)
          {
            /* if the transfer syntax was proposed then we can accept it
               * appears in our supported list of transfer syntaxes
               */
            if (strcmp(pc.proposedTransferSyntaxes[j], transferSyntaxes[k]) == 0)
            {
              cond = ASC_acceptPresentationContext(
                    assoc->params, pc.presentationContextID, transferSyntaxes[k], role);
              if (cond.bad()) return cond;
            }
          }
        }
      }
    } /* for */
  } /* else */

  /*
     * check if we have negotiated the private "shutdown" SOP Class
     */
  if (0 != ASC_findAcceptedPresentationContextID(assoc, UID_PrivateShutdownSOPClass))
  {
    logger.info(QString("Shutting down server ... (negotiated private \"shut down\" SOP class)"));
    this->refuseAssociation(&assoc, CTN_NoReason);
    return ASC_SHUTDOWNAPPLICATION;
  }

  return cond;
}

//------------------------------------------------------------------------------
void ctkDICOMSCPPrivate::threadedStartServer()
{
  OFCondition cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, this->port, this->acseTimeout, &this->network);
  if(cond.bad()) {
    logger.error(QString("Failed to initialize network"));
  }

  while(!this->terminateRequested && cond.good())
  {
    cond = this->waitForAssociation(this->network);
  }

  cond = ASC_dropNetwork(&this->network);
  if(cond.bad()) {
    logger.debug(QString("Cannot drop network"));
  }

  this->terminateRequested = false;
}

//------------------------------------------------------------------------------
void ctkDICOMSCPPrivate::threadedHandleAssociation(T_ASC_Association *assoc, bool correctUIDPadding)
{
  this->handleAssociation(assoc, correctUIDPadding);
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMSCPPrivate::refuseAssociation(T_ASC_Association ** assoc, CTN_RefuseReason reason)
{
  OFCondition cond = EC_Normal;
  T_ASC_RejectParameters rej;
  OFString temp_str;

  const char *reason_string;
  switch (reason)
  {
  case CTN_TooManyAssociations:
    reason_string = "TooManyAssociations";
    break;
  case CTN_CannotFork:
    reason_string = "CannotFork";
    break;
  case CTN_BadAppContext:
    reason_string = "BadAppContext";
    break;
  case CTN_BadAEPeer:
    reason_string = "BadAEPeer";
    break;
  case CTN_BadAEService:
    reason_string = "BadAEService";
    break;
  case CTN_NoReason:
    reason_string = "NoReason";
    break;
  default:
    reason_string = "???";
    break;
  }
  logger.info(QString("Refusing Association (") + reason_string + ")");

  switch (reason)
  {
  case CTN_TooManyAssociations:
    rej.result = ASC_RESULT_REJECTEDTRANSIENT;
    rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
    rej.reason = ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED;
    break;
  case CTN_CannotFork:
    rej.result = ASC_RESULT_REJECTEDPERMANENT;
    rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
    rej.reason = ASC_REASON_SP_PRES_TEMPORARYCONGESTION;
    break;
  case CTN_BadAppContext:
    rej.result = ASC_RESULT_REJECTEDTRANSIENT;
    rej.source = ASC_SOURCE_SERVICEUSER;
    rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
    break;
  case CTN_BadAEPeer:
    rej.result = ASC_RESULT_REJECTEDPERMANENT;
    rej.source = ASC_SOURCE_SERVICEUSER;
    rej.reason = ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED;
    break;
  case CTN_BadAEService:
    rej.result = ASC_RESULT_REJECTEDPERMANENT;
    rej.source = ASC_SOURCE_SERVICEUSER;
    rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
    break;
  case CTN_NoReason:
  default:
    rej.result = ASC_RESULT_REJECTEDPERMANENT;
    rej.source = ASC_SOURCE_SERVICEUSER;
    rej.reason = ASC_REASON_SU_NOREASON;
    break;
  }

  cond = ASC_rejectAssociation(*assoc, &rej);

  if (cond.bad())
  {
    DimseCondition::dump(temp_str, cond);
    logger.error(QString("Association Reject Failed: ") + temp_str.c_str());
  }

  cond = ASC_dropAssociation(*assoc);
  if (cond.bad())
  {
    DimseCondition::dump(temp_str, cond);
    logger.error(QString("Cannot Drop Association: ") + temp_str.c_str());
  }
  cond = ASC_destroyAssociation(assoc);
  if (cond.bad())
  {
    DimseCondition::dump(temp_str, cond);
    logger.error(QString("Cannot Destroy Association: ") + temp_str.c_str());
  }

  return cond;
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMSCPPrivate::handleAssociation(T_ASC_Association * assoc, OFBool correctUIDPadding)
{
  OFCondition         cond = EC_Normal;
  DIC_NODENAME        peerHostName;
  DIC_AE              peerAETitle;
  DIC_AE              myAETitle;
  OFString            temp_str;

  ASC_getPresentationAddresses(assoc->params, peerHostName, NULL);
  ASC_getAPTitles(assoc->params, peerAETitle, myAETitle, NULL);

  /* now do the real work */
  cond = dispatch(assoc, correctUIDPadding);

  /* clean up on association termination */
  if (cond == DUL_PEERREQUESTEDRELEASE) {
    logger.info("Association Release");
    cond = ASC_acknowledgeRelease(assoc);
    ASC_dropSCPAssociation(assoc);
  } else if (cond == DUL_PEERABORTEDASSOCIATION) {
    logger.info("Association Aborted");
  } else {
    DimseCondition::dump(temp_str, cond);
    logger.error(QString("DIMSE Failure (aborting association): ") + temp_str.c_str());
    /* some kind of error so abort the association */
    cond = ASC_abortAssociation(assoc);
  }

  cond = ASC_dropAssociation(assoc);
  if (cond.bad()) {
    DimseCondition::dump(temp_str, cond);
    logger.error(QString("Cannot Drop Association: ") + temp_str.c_str());
  }
  cond = ASC_destroyAssociation(&assoc);
  if (cond.bad()) {
    DimseCondition::dump(temp_str, cond);
    logger.error(QString("Cannot Destroy Association: ") + temp_str.c_str());
  }

  return cond;
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMSCPPrivate::dispatch(T_ASC_Association *assoc, OFBool correctUIDPadding)
{
  OFCondition cond = EC_Normal;
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID;

  // this while loop is executed exactly once unless the "keepDBHandleDuringAssociation_"
  // flag is not set, in which case the inner loop is executed only once and this loop
  // repeats for each incoming DIMSE command. In this case, the DB handle is created
  // and released for each DIMSE command.
  while (cond.good())
  {
    // this while loop is executed exactly once unless the "keepDBHandleDuringAssociation_"
    // flag is set, in which case the DB handle remains open until something goes wrong
    // or the remote peer closes the association
    while (cond.good())
    {
      cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL);

      /* did peer release, abort, or do we have a valid message ? */
      if (cond.good())
      {
        /* process command */

        if(msg.CommandField == DIMSE_C_CANCEL_RQ)
        {
          logger.info("dispatch: late C-CANCEL-RQ, ignoring");
        }
        else
        {
          if(this->registeredSCPs.contains(msg.CommandField))
          {
            cond = this->registeredSCPs[msg.CommandField]->handleRequest(assoc, msg, presID);
          }
          else
          {
            /* we cannot handle this kind of message */
            cond = DIMSE_BADCOMMANDTYPE;
            logger.error(QString("Cannot handle command: ") + (unsigned)msg.CommandField);
          }
        }
      }
      else if ((cond == DUL_PEERREQUESTEDRELEASE)||(cond == DUL_PEERABORTEDASSOCIATION))
      {
        // association gone
      }
      else
      {
        // the condition will be returned, the caller will abort the assosiation.
      }
    }
  }

  // Association done
  return cond;
}

//------------------------------------------------------------------------------
// ctkDICOMSCP class methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ctkDICOMSCP::ctkDICOMSCP(QObject *parent) :
  QObject(parent),
  d_ptr(new ctkDICOMSCPPrivate(this))
{
  this->dimseCommand = DIMSE_NOTHING;
}

//------------------------------------------------------------------------------
ctkDICOMSCP::~ctkDICOMSCP()
{
  delete d_ptr;
}

//------------------------------------------------------------------------------
T_DIMSE_Command ctkDICOMSCP::getDimseCommand()
{
  return this->dimseCommand;
}

//------------------------------------------------------------------------------
bool ctkDICOMSCP::registerSCP(ctkDICOMSCP *scp)
{
  Q_D(ctkDICOMSCP);

  if(!scp)
  {
    return false;
  }

  T_DIMSE_Command command = scp->getDimseCommand();

  if(d->registeredSCPs.contains(command))
  {
    return false;
  }

  scp->setParent(this);
  d->registeredSCPs[command] = scp;

  return true;
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::setAETitle(const QString& aeTitle)
{
  Q_D(ctkDICOMSCP);

  d->aeTitle = aeTitle;
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::setPort(int port)
{
  Q_D(ctkDICOMSCP);

  d->port = port;
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::setEnableThreading(bool flag)
{
  Q_D(ctkDICOMSCP);

  d->isThreadingEnabled = flag;
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::setMaxPDU(int size)
{
  Q_D(ctkDICOMSCP);

  d->maxPDU = size;
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::setMaxAssociations(int count)
{
  Q_D(ctkDICOMSCP);

  d->maxAssociation = count;
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::setEnableGetSupport(bool flag)
{
  Q_D(ctkDICOMSCP);
  
  d->isGetEnabled = flag;
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::setEnablePatientRoot(bool flag)
{
  Q_D(ctkDICOMSCP);

  d->isPatientRootEnabled = flag;
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::setEnableStudyRoot(bool flag)
{
  Q_D(ctkDICOMSCP);

  d->isStudyRootEnabled = flag;
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::start()
{
  Q_D(ctkDICOMSCP);

  if(d->isThreadingEnabled)
  {
    QtConcurrent::run(d, &ctkDICOMSCPPrivate::threadedStartServer);
  }
  else
  {
    d->threadedStartServer();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMSCP::stop()
{
  Q_D(ctkDICOMSCP);

  d->terminateRequested = true;
}
