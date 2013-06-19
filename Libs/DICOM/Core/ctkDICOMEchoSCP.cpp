#include "ctkDICOMEchoSCP.h"

// CTK includes
#include "ctkLogger.h"

static ctkLogger logger ( "org.commontk.dicom.DcmEchoSCP" );

//------------------------------------------------------------------------------
ctkDICOMEchoSCP::ctkDICOMEchoSCP(QObject *parent) :
  ctkDICOMSCP(parent)
{
  this->dimseCommand = DIMSE_C_ECHO_RQ;
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMEchoSCP::handleRequest(T_ASC_Association *assoc, T_DIMSE_Message& message, T_ASC_PresentationContextID presId)
{
  OFCondition cond = EC_Normal;

  T_DIMSE_C_EchoRQ * req = &message.msg.CEchoRQ;

  logger.info(QString("Received Echo SCP RQ: MsgID ") + req->MessageID);
  /* we send an echo response back */
  cond = DIMSE_sendEchoResponse(assoc, presId,
                                req, STATUS_Success, NULL);

  if (cond.bad()) {
    OFString temp_str;
    DimseCondition::dump(temp_str, cond);
    logger.info(QString("echoSCP: Echo Response Failed: ") + temp_str.c_str());
  }
  return cond;
}
