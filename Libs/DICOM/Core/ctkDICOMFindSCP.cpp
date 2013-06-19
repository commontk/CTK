#include "ctkDICOMFindSCP.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmqrdb/dcmqrcbf.h>
#include <dcmtk/dcmqrdb/dcmqrdbs.h>

// ctk includes
#include "ctkDICOMFindContext.h"
#include "ctkLogger.h"

static ctkLogger logger ("org.commontk.dicom.DcmFindSCP");

class ctkDICOMFindSCPPrivate{
public:
  ctkDICOMFindSCPPrivate(ctkDICOMFindSCP* parent);
  ~ctkDICOMFindSCPPrivate();

  OFCondition findProvider(T_ASC_Association *assoc,
                           T_ASC_PresentationContextID presIdCmd,
                           T_DIMSE_C_FindRQ *request,
                           ctkDICOMFindContext* context,
                           T_DIMSE_BlockingMode blockMode, int timeout);

private:
  ctkDICOMFindSCP* q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMFindSCP);
};

//------------------------------------------------------------------------------
// ctkDICOMFindSCPPrivate class methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ctkDICOMFindSCPPrivate::ctkDICOMFindSCPPrivate(ctkDICOMFindSCP *parent):
  q_ptr(parent)
{

}

//------------------------------------------------------------------------------
OFCondition ctkDICOMFindSCPPrivate::findProvider(T_ASC_Association *assoc,
                                               T_ASC_PresentationContextID presIdCmd,
                                               T_DIMSE_C_FindRQ *request,
                                               ctkDICOMFindContext *context,
                                               T_DIMSE_BlockingMode blockMode,
                                               int timeout)
{
  T_ASC_PresentationContextID presIdData;
  T_DIMSE_C_FindRSP rsp;
  DcmDataset *statusDetail = NULL;
  DcmDataset *reqIds = NULL;
  DcmDataset *rspIds = NULL;
  OFBool cancelled = OFFalse;
  OFBool normal = OFTrue;
  int responseCount = 0;

  /* receive data (i.e. the search mask) and store it in memory */
  OFCondition cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presIdData, &reqIds, NULL, NULL);

  /* if no error occured while receiving data */
  if (cond.good())
  {
    /* check if the presentation context IDs of the C-FIND-RQ and */
    /* the search mask data are the same; if not, return an error */
    if (presIdData != presIdCmd)
    {
      cond = makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error, "DIMSE: Presentation Contexts of Command and Data Differ");
    }
    else
    {
      /* if the IDs are the same go ahead */
      /* initialize the C-FIND-RSP message variable */
      bzero((char*)&rsp, sizeof(rsp));
      rsp.DimseStatus = STATUS_Pending;

      /* as long as no error occured and the status of the C-FIND-RSP message which will */
      /* be/was sent is pending, perform this loop in which records that match the search */
      /* mask are selected (whithin the execution of the callback function) and sent over */
      /* the network to the other DICOM application using C-FIND-RSP messages. */
      while (cond.good() && DICOM_PENDING_STATUS(rsp.DimseStatus) && normal)
      {
        /* increase the counter that counts the number of response messages */
        responseCount++;

        /* check if a C-CANCEL-RQ is received */
        cond = DIMSE_checkForCancelRQ(assoc, presIdCmd, request->MessageID);
        if (cond.good())
        {
          /* if a C-CANCEL-RQ was received, we need to set status and an indicator variable */
          rsp.DimseStatus = STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest;
          cancelled = OFTrue;
        } else if (cond == DIMSE_NODATAAVAILABLE)
        {
          /* timeout */
        }
        else
        {
          /* some execption condition occured, bail out */
          normal = OFFalse;
        }

        /* if everything is still ok */
        if (normal)
        {
          /* execute callback function (note that this function always determines the next record */
          /* which matches the search mask. This record will be available here through rspIds) */
          context->callbackHandler(cancelled, request, reqIds,responseCount, &rsp, &rspIds, &statusDetail);

          /* if we encountered a C-CANCEL-RQ earlier, set a variable and possibly delete the search mask */
          if (cancelled) {
            /* make sure */
            rsp.DimseStatus =
                STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest;
            if (rspIds != NULL) {
              delete reqIds;
              reqIds = NULL;
            }
          }

          /* send a C-FIND-RSP message over the network to the other DICOM application */
          cond = DIMSE_sendFindResponse(assoc, presIdCmd, request,
                                        &rsp, rspIds, statusDetail);

          /* if there are search results, delete them */
          if (rspIds != NULL) {
            delete rspIds;
            rspIds = NULL;
          }

          /* if there is status detail information, delete it */
          if (statusDetail != NULL) {
            delete statusDetail;
            statusDetail = NULL;
          }
        }
      }
    }
  }

  /* delete search mask */
  delete reqIds;

  /* delete latest search result */
  delete rspIds;

  /* return result value */
  return cond;
}

//------------------------------------------------------------------------------
ctkDICOMFindSCPPrivate::~ctkDICOMFindSCPPrivate()
{

}

//------------------------------------------------------------------------------
// ctkDICOMFindSCP class methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ctkDICOMFindSCP::ctkDICOMFindSCP(QObject *parent) :
  ctkDICOMSCP(parent),
  d_ptr(new ctkDICOMFindSCPPrivate(this))
{
  this->dimseCommand = DIMSE_C_FIND_RQ;
}

//------------------------------------------------------------------------------
ctkDICOMFindSCP::~ctkDICOMFindSCP()
{
  delete d_ptr;
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMFindSCP::handleRequest(T_ASC_Association *assoc, T_DIMSE_Message &message, T_ASC_PresentationContextID presId)
{
  Q_D(ctkDICOMFindSCP);

  OFCondition cond = EC_Normal;
  T_DIMSE_C_FindRQ * request = &message.msg.CFindRQ;

  ctkDICOMFindContext* context = this->createContext();

  DIC_AE aeTitle;
  aeTitle[0] = '\0';
  ASC_getAPTitles(assoc->params, NULL, aeTitle, NULL);
  context->setOurAETitle(aeTitle);

  OFString temp_str;
  DIMSE_dumpMessage(temp_str, *request, DIMSE_INCOMING);
  logger.info(QString("Received Find SCP:\n") + temp_str.c_str());

  cond = d->findProvider(assoc, presId, request,
                         context, DIMSE_BLOCKING, 30);
  if (cond.bad()) {
    DimseCondition::dump(temp_str, cond);
    logger.error(QString("Find SCP Failed: ") + temp_str.c_str());
  }

  context->deleteLater();
  return cond;
}

//------------------------------------------------------------------------------
ctkDICOMFindContext* ctkDICOMFindSCP::createContext()
{
  return new ctkDICOMFindContext(STATUS_Pending);
}
