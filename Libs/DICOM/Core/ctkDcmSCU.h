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
 *  CVS/RCS Revision: $Revision: 1.39 $
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

#ifndef ctkDcmSCU_H
#define ctkDcmSCU_H

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"     /* Covers most common dcmdata classes */
#include "dcmtk/dcmnet/dcompat.h"
#include "dcmtk/dcmnet/dimse.h"     /* DIMSE network layer */
#include "dcmtk/dcmnet/dcasccff.h"  /* For reading a association config file */
#include "dcmtk/dcmnet/dcasccfg.h"  /* For holding association cfg file infos */
#include "dcmtk/ofstd/oflist.h"

#include "ctkDICOMCoreExport.h"

//
// CTK: these return conditions copied from cond.cc
//
const OFConditionConst NET_ECC_AlreadyConnected                (OFM_dcmnet, 1010, OF_error, "Already connected");
const OFConditionConst NET_ECC_NoPresentationContextsDefined   (OFM_dcmnet, 1005, OF_error, "No Presentation Contexts defined");
const OFConditionConst NET_ECC_NoAcceptablePresentationContexts(OFM_dcmnet, 1006, OF_error, "No acceptable Presentation Contexts");
const OFConditionConst NET_ECC_InvalidSOPClassUID              (OFM_dcmnet, 1000, OF_error, "Invalid SOP Class UID");
const OFConditionConst NET_ECC_InvalidSOPInstanceUID           (OFM_dcmnet, 1002, OF_error, "Invalid SOP Instance UID");
const OFConditionConst NET_ECC_UnknownStorageSOPClass          (OFM_dcmnet, 1001, OF_error, "Unknown Storage SOP Class");
const OFConditionConst NET_ECC_UnknownTransferSyntax           (OFM_dcmnet, 1004, OF_error, "Unknown Transfer Syntax");

const OFCondition NET_EC_AlreadyConnected                (NET_ECC_AlreadyConnected);
const OFCondition NET_EC_NoPresentationContextsDefined   (NET_ECC_NoPresentationContextsDefined);
const OFCondition NET_EC_NoAcceptablePresentationContexts(NET_ECC_NoAcceptablePresentationContexts);
const OFCondition NET_EC_InvalidSOPClassUID              (NET_ECC_InvalidSOPClassUID);
const OFCondition NET_EC_UnknownStorageSOPClass          (NET_ECC_UnknownStorageSOPClass);
const OFCondition NET_EC_InvalidSOPInstanceUID           (NET_ECC_InvalidSOPInstanceUID);
const OFCondition NET_EC_UnknownTransferSyntax           (NET_ECC_UnknownTransferSyntax);


/** Different types of closing an association
 */
enum DcmCloseAssociationType
{
  /// Release the current association
  DCMSCU_RELEASE_ASSOCIATION,
  /// Abort the current association
  DCMSCU_ABORT_ASSOCIATION,
  /// Peer requested release (Aborting)
  DCMSCU_PEER_REQUESTED_RELEASE,
  /// Peer aborted the association
  DCMSCU_PEER_ABORTED_ASSOCIATION
};

/** Storage mode used for DICOM objects received via C-STORE
 */
enum DcmStorageMode
{
  /// Ignore any objects received via C-STORE
  DCMSCU_STORAGE_IGNORE,
  /// Try to store the objects to disk
  DCMSCU_STORAGE_DISK,
  /// Try to store to disk in bit-preserving mode. This is especially useful
  /// for huge files that cannot fully be received in memory since the
  /// data is directly streamed to disk. Originally, this was introduced for
  /// DICOM signatures which can be kept valid this way.
  DCMSCU_STORAGE_BIT_PRESERVING
};


/** Base class for C-FIND, C-MOVE and C-GET responses
 */
class CTK_DICOM_CORE_EXPORT QRResponse
{
  public:

  /** Standard constructor.
   */
  QRResponse() :
    m_messageIDRespondedTo(0),
    m_affectedSOPClassUID(),
    m_dataset(NULL),
    m_status(0),
    m_statusDetail(NULL) {}

  /** Destructor, cleans up internal memory (dataset if present).
   */
  virtual ~QRResponse() { delete m_dataset; delete m_statusDetail; }

  /// The message ID responded to (mandatory response field,
  /// equals message ID from request)
  Uint16 m_messageIDRespondedTo;

  /// Optional response field according to part 7 of the standard
  /// If present, equals SOP Class UID from request.
  OFString m_affectedSOPClassUID;

  /// Conditional response field (NULL if absent). From the standard (2009,
  /// part 4, C.4.2.1.4.2), for C-MOVE: In Q/R if no C-STORE sub-operation
  /// failed, Failed SOP Instance UID List (0008,0058) is absent and
  /// therefore no Data Set shall be sent in the C-MOVE response. Further
  /// rules: Statuses of Canceled, Failure, Refused, or Warning shall
  /// contain the Failed SOP Instance UID List Attribute; status of
  /// Pending shall not.
  DcmDataset *m_dataset;

  /// The returned DIMSE status (mandatory Response Field)
  Uint16 m_status;

  /// Status detail (NULL if absent). For some DIMSE return status codes,
  /// an additional dataset is sent which gives further information (i.e.
  /// in case of warnings or errors).
  DcmDataset *m_statusDetail;

private:

  /** Private undefined copy constructor.
   * @param other The find response to copy from
   */
  QRResponse(const QRResponse& other);

  /** Private undefined assignment operator.
   *  @param other The find response that should be assigned from
   */
  QRResponse& operator=(const QRResponse& other);
};


/// Base class representing for single C-GET or C-MOVE response
class CTK_DICOM_CORE_EXPORT RetrieveResponse : public QRResponse
{
public:
  /** Standard constructor
   */
  RetrieveResponse() :
        m_numberOfRemainingSubops(0),
        m_numberOfCompletedSubops(0),
        m_numberOfFailedSubops(0),
        m_numberOfWarningSubops(0) {}

  /** Destructor, cleans up internal memory
   */
  virtual ~RetrieveResponse() {}

  /** Prints response to INFO log level.
   */
  void print();

  /// Number of remaining sub operations (in Q/R: C-STORE calls).
  /// For Q/R MOVE and GET, for status of pending this field shall be filled.
  /// For others, the field may be filled.
  Uint16 m_numberOfRemainingSubops;

  /// Number of successfully completed sub operations (in Q/R: C-STORE calls).
  /// For Q/R MOVE and GET, for status of pending this field shall be filled.
  /// For others, the field may be filled.
  Uint16 m_numberOfCompletedSubops;

  /// Number of failed sub operations (in Q/R: C-STORE calls).
  /// For Q/R MOVE and GET, for status of pending this field shall be filled.
  /// For others, the field may be filled.
  Uint16 m_numberOfFailedSubops;

  /// Number generated warnings generated by sub operations (in Q/R: C-STORE calls).
  /// For Q/R MOVE and GET, for status of pending this field shall be filled.
  /// For others, the field may be filled.
  Uint16 m_numberOfWarningSubops;

private:

  /** Private undefined copy constructor
   *  @param other Response to copy from
   */
  RetrieveResponse(const RetrieveResponse& other);

  /** Private undefined assignment operator
   *  @param other Response that should be assigned from
   */
  RetrieveResponse& operator=(const RetrieveResponse& other);
};


/** Base class for implementing DICOM Service Class User functionality. The class offers
 *  support for negotiating associations and sending and receiving arbitrary DIMSE messages
 *  on that connection. DcmSCU has built-in C-ECHO support so derived classes do not have to
 *  implement that capability on their own.
 *  @warning This class is EXPERIMENTAL. Be careful to use it in production environment.
 *  @warning This is a copy of the DcmSCU class - see not at top of file
 */
class CTK_DICOM_CORE_EXPORT ctkDcmSCU
{

public:

  /** Constructor, just initializes internal class members
   */
  ctkDcmSCU();

  /** Virtual destructor
   */
  virtual ~ctkDcmSCU();

  /** Add presentation context to be used for association negotiation
   *  @param abstractSyntax [in] Abstract syntax name in UID format
   *  @param xferSyntaxes   [in] List of transfer syntaxes to be added for the given abstract
   *                             syntax
   *  @param role           [in] The role to be negotiated
   *  @return EC_Normal if adding was successful, otherwise error code
   */
  OFCondition addPresentationContext(const OFString &abstractSyntax,
                                     const OFList<OFString> &xferSyntaxes,
                                     const T_ASC_SC_ROLE role = ASC_SC_ROLE_DEFAULT);

  /** Initialize network, i.e.\ prepare for association negotiation. If the SCU is already
   *  connected, the call will not be successful and the old connection keeps open.
   *  @return EC_Normal if initialization was successful, otherwise error code.
   *          NET_EC_AlreadyConnected if SCU is already connected.
   */
  virtual OFCondition initNetwork();

  /** Negotiate association by using presentation contexts and parameters as defined by
   *  earlier function calls. If negotiation fails, there is no need to close the association
   *  or to do anything else with this class.
   *  @return EC_Normal if negotiation was successful, otherwise error code.
   *          NET_EC_AlreadyConnected if SCU is already connected.
   */
  virtual OFCondition negotiateAssociation();

  /** After negotiation association, this call returns the first usable presentation context
   *  given the desired abstract syntax and transfer syntax
   *  @param abstractSyntax [in] The abstract syntax (UID) to look for
   *  @param transferSyntax [in] The transfer syntax (UID) to look for. If empty, the transfer
   *                             syntax is not checked.
   *  @return Adequate Presentation context ID that can be used. 0 if none found.
   */
  T_ASC_PresentationContextID findPresentationContextID(const OFString &abstractSyntax,
                                                        const OFString &transferSyntax);

  /** After a successful association negotiation, this function is called to return the
   *  presentation context ID that best matches the desired abstract syntax and transfer
   *  syntax (TS). The function tries to do the following:
   *  - If possible finds a presentation context with matching TS
   *  - Else then tries to find an explicit VR uncompressed TS presentation context
   *  - Else then tries to find an implicit VR uncompressed TS presentation context
   *  - Else finally accepts each matching presentation ctx independent of TS.
   *  @param abstractSyntax [in] The abstract syntax (UID) to look for
   *  @param transferSyntax [in] The transfer syntax (UID) to look for. If empty, the transfer
   *                             syntax is not checked.
   *  @return Adequate Presentation context ID that can be used. 0 if no appropriate
   *  presentation context could be found at all.
   */
   T_ASC_PresentationContextID findAnyPresentationContextID(const OFString &abstractSyntax,
                                                            const OFString &transferSyntax);

  /** This function sends a C-ECHO command via network to another DICOM application
   *  @param presID [in] Presentation context ID to use. A value of 0 lets SCP class tries
   *                     to choose one on its own.
   *  @return EC_Normal if echo was successful, an error code otherwise
   *
   */
  virtual OFCondition sendECHORequest(const T_ASC_PresentationContextID presID);

  /** This function sends a C-STORE request on the currently opened association and receives
   *  the corresponding response then.  If required and supported, the dataset of the SOP
   *  instance can be converted automatically to the network transfer syntax that was
   *  negotiated (and is specified by the parameter 'presID').  However, this feature is
   *  disabled by default. See setDatasetConversionMode() on how to enable it.
   *  @param presID        [in]  Contains in the end the ID of the presentation context which
   *                             was specified in the DIMSE command. If 0 is given, the
   *                             function tries to find an approriate presentation context
   *                             itself (based on SOP class and original transfer syntax of
   *                             the 'dicomFile' or 'dataset').
   *  @param dicomFile     [in]  The filename of the DICOM file to be sent. Alternatively, a
   *                             dataset can be given in the next parameter. If both are given
   *                             the dataset from the file name is used.
   *  @param dataset       [in]  The dataset to be sent. Alternatively, a filename can be
   *                             specified in the previous parameter. If both are given the
   *                             dataset from the filename is used.
   *  @param rspStatusCode [out] The response status code received. 0 means success, others
   *                             can be found in the DICOM standard.
   *  @return EC_Normal if request could be issued and response was received successfully,
   *          error code otherwise. That means that if the receiver sends a response denoting
   *          failure of the storage request, EC_Normal will be returned.
   */
  virtual OFCondition sendSTORERequest(const T_ASC_PresentationContextID presID,
                                       const OFString &dicomFile,
                                       DcmDataset *dataset,
                                       Uint16 &rspStatusCode);

  /** Sends a C-MOVE Request on given presentation context and receives list of responses.
   *  The function receives the first response and then calls the function handleMOVEResponse()
   *  which gets the relevant presentation context together with the response dataset and
   *  status information. Then it waits again for the next response, if there are more to
   *  come (i.e. response status is PENDING). In the end, after receiving all responses, the
   *  full list of responses is returned to the caller. If he is not interested, he just sets
   *  responses=NULL when calling the function.
   *  This function can be overwritten by actual SCU implementations but just should work fine
   *  for most people.
   *  @param presID      [in]  The presentation context ID that should be used. Must be an odd
   *                           number.
   *  @param moveDestinationAETitle [in]  The move destination's AE title, i.e.\ the one that
   *                           is used for connection to the storage server.
   *  @param dataset     [in]  The dataset containing the information about the object(s) to
   *                           be retrieved
   *  @param responses   [out] The incoming C-MOVE responses for this request. The caller is
   *                           responsible for providing a non-NULL pointer for this case.
   *                           After receiving the results, the caller is responsible for
   *                           freeing the memory of this variable. If NULL is specified, the
   *                           responses will not bereturned to the caller.
   *  @return EC_Normal if everything went fine, i.e.\ if request could be send and responses
   *          (with whatever status) could be received.
   */
  virtual OFCondition sendMOVERequest(const T_ASC_PresentationContextID presID,
                                      const OFString &moveDestinationAETitle,
                                      DcmDataset *dataset,
                                      OFList<RetrieveResponse*> *responses);

  /** This is the standard handler for C-MOVE message responses: It just adds up all responses
   *  it receives and prints a DEBUG message. Therefore, it is called for each response
   *  received in sendMOVERequest(). The idea is of course to overwrite this function in a
   *  derived, actual SCU implementation if required. Thus, after each response, the caller of
   *  sendMOVERequest() can decide on its own whether he wants to cancel the C-MOVE session,
   *  terminate the association, do something useful or whatever. Thus this function is a more
   *  object-oriented kind of callback.
   *  @param presID   [in] The presentation context ID where the response was received on.
   *  @param response [in] The C-MOVE response received.
   *  @param waitForNextResponse [out] Denotes whether SCU should try to receive another
   *                                   response. If set to OFTrue, then sendMOVERequest() will
   *                                   continue waiting for responses. The current
   *                                   implementation does that for all responses do not have
   *                                   status Failed, Warning, Success or unknown. If set to
   *                                   OFFalse, sendMOVERequest() will return control to the
   *                                   caller.
   *  @return EC_Normal, if response could be handled. Error code otherwise.
   *          The current implementation always returns EC_Normal.
   */
  virtual OFCondition handleMOVEResponse(const T_ASC_PresentationContextID presID,
                                         RetrieveResponse *response,
                                         OFBool &waitForNextResponse);

  /** Sends a C-GET Request on given presentation context and receives list of responses. It
   *  then switches control to the function handleCGETSession().
   *  The full list of responses is returned to the caller. If he is not interested, he can
   *  set responses=NULL when calling the function.
   *  This function can be overwritten by actual SCU implementations but just should work fine
   *  for most people.
   *  @param presID    [in]  The presentation context ID that should be used. Must be an odd
   *                         number.
   *  @param dataset   [in]  The dataset containing the information about the
   *                         object(s) to be retrieved
   *  @param responses [out] The incoming C-GET responses for this request. If the caller
   *                         specifies NULL, no responses will be returned; otherwise there
   *                         should be at least one final C-GET response (mandatory). C-GET
   *                         responses after each DICOM object received are optional and may
   *                         have been ommitted by the server.
   *  @return EC_Normal if everything went fine, i.e.\ if request could be sent and expected
   *          responses (with whatever status) could be received.
   */
  virtual OFCondition sendCGETRequest(const T_ASC_PresentationContextID presID,
                                      DcmDataset *dataset,
                                      OFList<RetrieveResponse*> *responses);

  /** Does the logic for switching between C-GET Response and C-STORE Requests. Sends a C-GET
   *  Request on given presentation context and receives list of responses. Ihe full list of
   *  responses is returned to the caller. If he is not interested, he can set responses=NULL
   *  when calling the function. After sending a C-GET Request, there might be two different
   *  responses coming in: C-GET-RSP (optional after each received object and mandatory after
   *  the last object) or a mandatory C-STORE for each incoming object that is received due to
   *  the request. This function therefore either calls handleCGETResponse() or
   *  handleSTORERequest() in order to deal with the incoming message. All other messages lead
   *  to an error within this handler.
   *  This function can be overwritten by actual SCU implementations but just should work fine
   *  for most people.
   *  @param presID    [in]  The presentation context ID that should be used. Must be an odd
   *                         number.
   *  @param dataset   [in]  The dataset containing the information about the object(s) to be
   *                         retrieved
   *  @param responses [out] The incoming C-GET responses for this request. If the caller
   *                         specifies NULL, no responses will be returned; otherwise there
   *                         should be at least one final C-GET response (mandatory). C-GET
   *                         responses after each DICOM object received are optional and may
   *                         have been ommitted by the server.
   *  @return EC_Normal if everything went fine, i.e.\ if request could be send
   *          and expected responses (with whatever status) could be received.
   */
  virtual OFCondition handleCGETSession(const T_ASC_PresentationContextID presID,
                                        DcmDataset *dataset,
                                        OFList<RetrieveResponse*> *responses);

  /** Function handling a single C-GET Response. This standard handler reads the status of the
   *  response and decides whether to receive any further messages related to the original
   *  C-GET Request or whether the last response was received or an error occured.
   *  @param presID              [in]  The presentation context the C-GET Response was
   *                                   received on.
   *  @param response            [in]  The response received
   *  @param continueCGETSession [out] Defines whether it is decided to wait for further C-GET
   *                                   Responses/C-STORE Requests within this C-GET session
   *  @return If no errors occur (dataset response NULL, SCU not connected), this method will
   *          return EC_Normal, otherwise error code.
   */
  virtual OFCondition handleCGETResponse(const T_ASC_PresentationContextID presID,
                                         RetrieveResponse* response,
                                         OFBool& continueCGETSession);

  /** Function handling a single C-STORE Request. If storage mode is set to disk (default),
   *  this function is called and the incoming object stored to disk.
   *  @param presID               [in]  The presentation context the C-STORE Response was
   *                                    received on.
   *  @param incomingObject       [in]  The dataset (the object) received
   *  @param continueCGETSession  [out] Defines whether it is decided to wait for further
   *                                    C-GET Responses/C-STORE requests within this C-GET
   *                                    session.
   *  @param cStoreReturnStatus   [out] Denotes the desired C-STORE return status.
   *  @return If errors occur (incomingObject NULL or SCU not connected or file could not be
   *          stored), this method will return an error code otherwise EC_Normal.
   */
  virtual OFCondition handleSTORERequest(const T_ASC_PresentationContextID presID,
                                         DcmDataset *incomingObject,
                                         OFBool& continueCGETSession,
                                         Uint16& cStoreReturnStatus);

  /** Function handling a single C-STORE Request. If storage mode is set to bit preserving,
   *  this function is called and the incoming object stored directly to disk, i.e. not stored
   *  fully in memory.
   *  @param presID          [in] The presentation context the C-STORE Response was received
   *                              on.
   *  @param filename        [in] The filename to store to
   *  @param request         [in] The incoming C-STORE request command set
   *  @param callback        [in] The desired user callback
   *  @param callbackContext [in] The desired user callback data
   *  @return If errors occur (incomingObject NULL or SCU not connected filename not
   *          specified), this method will return an error code otherwise EC_Normal.
   */
  virtual OFCondition handleSTORERequestFile(T_ASC_PresentationContextID *presID,
                                             const OFString& filename,
                                             T_DIMSE_C_StoreRQ* request,
                                             DIMSE_ProgressCallback callback,
                                             void *callbackContext);

  /** This function is called if an object was receveid due to a C-GET request and can be
   *  overwritten by a user in order to be informed about such an event. The standard handles
   *  just prints a message. Note that this function is not called if the SCU is in storage
   *  mode DCMSCU_STORAGE_IGNORE.
   *  @param filename       [in] The filename written
   *  @param sopClassUID    [in] The SOP Class UID of the object written
   *  @param sopInstanceUID [in] The SOP Instance UID of the object written
   */
  virtual void notifyInstanceStored(const OFString& filename,
                                    const OFString& sopClassUID,
                                    const OFString& sopInstanceUID) const;

  /** Sends a C-FIND Request on given presentation context and receives list of responses.
   *  The function receives the first response and then calls the function handleFINDResponse
   *  which gets the relevant presentation context together with the response dataset and
   *  status information. Then it waits again for the next response, if there are more to
   *  come (i.e. response status is PENDING). In the end, after receiving all responses, the
   *  full list of responses is returned to the caller. If he is not interested, he just sets
   *  responses=NULL when calling the function.
   *  This function can be overwritten by actual SCU implementations but just should work fine
   *  for most people.
   *  @param presID      [in]  The presentation context ID that should be used. Must be an odd
   *                           number.
   *  @param queryKeys   [in]  The dataset containing the query keys to be searched for on the
   *                           server (SCP).
   *  @param responses   [out] The incoming C-FIND responses for this request. The caller is
   *                           responsible for providing a non-NULL pointer for this case.
   *                           After receiving the results, the caller is responsible for
   *                           freeing the memory of this variable. If NULL is specified, the
   *                           responses will be not returned to the caller.
   *  @return EC_Normal if everything went fine, i.e.\ if request could be send and responses
   *          (with whatever status) could be received.
   */
  virtual OFCondition sendFINDRequest(const T_ASC_PresentationContextID presID,
                                      DcmDataset *queryKeys,
                                      OFList<QRResponse*> *responses);

  /** This is the standard handler for C-FIND message responses: It just adds up all responses
   *  it receives and prints a DEBUG message. Therefore, it is called for each response
   *  received in sendFINDRequest(). The idea is of course to overwrite this function in a
   *  derived, actual SCU implementation if required. Thus, after each response, the caller of
   *  sendFINDRequest() can decide on its own whether he wants to cancel the C-FIND session,
   *  terminate the association, do something useful or whatever. That way this is a more
   *  object-oriented kind of callback.
   *  @param presID   [in] The presentation context ID where the response was received on.
   *  @param response [in] The C-FIND response received.
   *  @param waitForNextResponse [out] Denotes whether SCU should try to receive another
   *                                   response. If set to OFTrue, then sendFINDRequest()
   *                                   will continue waiting for responses. The current
   *                                   implementation does that for all responses do not have
   *                                   status SUCESSS. If set to OFFalse, sendFINDRequest()
   *                                   will return control to the caller.
   *  @return EC_Normal, if response could be handled. Error code otherwise.
   *          The current implementation always returns EC_Normal.
   */
  virtual OFCondition handleFINDResponse(const T_ASC_PresentationContextID  presID,
                                         QRResponse *response,
                                         OFBool &waitForNextResponse);

  /** Send C-CANCEL and, therefore, ends the C-FIND -GET or -MOVE session, i.e.\ no further
   *  responses will be handled. A call to this function only makes sense if an association
   *  is open, the given presentation context represents a valid C-FIND/GET/MOVE-enabled SOP
   *  class and usually only, if the last command send on that presentation context was a
   *  C-FIND message.
   *  @param presID [in] The presentation context ID where the C-CANCEL should be sent on.
   *  @return The current implementation always returns EC_Normal.
   */
  virtual OFCondition sendCANCELRequest(const T_ASC_PresentationContextID presID);

  /** This function sends a N-ACTION request on the currently opened association and receives
   *  the corresponding response then
   *  @param presID         [in]  The ID of the presentation context to be used for sending
   *                              the request message. Should not be 0.
   *  @param sopInstanceUID [in]  The requested SOP Instance UID
   *  @param actionTypeID   [in]  The action type ID to be used
   *  @param reqDataset     [in]  The dataset to be sent
   *  @param rspStatusCode  [out] The response status code received. 0 means success,
   *                              others can be found in the DICOM standard.
   *  @return EC_Normal if request could be issued and response was received successfully,
   *          an error code otherwise
   */
  virtual OFCondition sendACTIONRequest(const T_ASC_PresentationContextID presID,
                                        const OFString &sopInstanceUID,
                                        const Uint16 actionTypeID,
                                        DcmDataset *reqDataset,
                                        Uint16 &rspStatusCode);

  /** This function sends N-EVENT-REPORT request and receives the corresponding response
   *  @param presID         [in]  The ID of the presentation context to be used for sending
   *                              the request message. Should not be 0.
   *  @param sopInstanceUID [in]  The requested SOP Instance UID
   *  @param eventTypeID    [in]  The event type ID to be used
   *  @param reqDataset     [in]  The request dataset to be sent
   *  @param rspStatusCode  [out] The response status code received. 0 means success,
   *                              others can be found in the DICOM standard.
   *  @return EC_Normal if request could be issued and response was received successfully,
   *          an error code otherwise
   */
  virtual OFCondition sendEVENTREPORTRequest(const T_ASC_PresentationContextID presID,
                                             const OFString &sopInstanceUID,
                                             const Uint16 eventTypeID,
                                             DcmDataset *reqDataset,
                                             Uint16 &rspStatusCode);

  /** Receives N-EVENT-REPORT request on the currently opened association and sends a
   *  corresponding response. Calls checkEVENTREPORTRequest() in order to determine the
   *  DIMSE status code to be used for the N-EVENT-REPORT response.
   *  @param reqDataset  [out] Pointer to the dataset received
   *  @param eventTypeID [out] Event Type ID from the command set received
   *  @param timeout     [in]  Optional timeout in seconds for receiving data. This value
   *                           (if not 0) overwrites the standard DIMSE timeout and also
   *                           enables the non-blocking mode for receiving the request.
   *  @return status, EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition handleEVENTREPORTRequest(DcmDataset *&reqDataset,
                                               Uint16 &eventTypeID,
                                               const int timeout = 0);

  /** Closes the association created by this SCU. Also resets the current association.
   *  @param closeType [in] Define whether to release or abort the association
   */
  virtual void closeAssociation(const DcmCloseAssociationType closeType);

  /* Set methods */

  /** Set maximum PDU length (to be received by SCU)
   *  @param maxRecPDU [in] The maximum PDU size to use in bytes
   */
  void setMaxReceivePDULength(const unsigned long maxRecPDU);

  /** Set whether to send in DIMSE blocking or non-blocking mode
   *  @param blockingMode [in] Either blocking or non-blocking mode
   */
  void setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode);

  /** Set SCU's AETitle to be used in association negotiation
   *  @param myAETtitle [in] The SCU's AETitle to be used
   */
  void setAETitle(const OFString &myAETtitle);

  /** Set SCP's host (hostname or IP address) to talk to in association negotiation
   *  @param peerHostName [in] The SCP's hostname or IP address to be used
   */
  void setPeerHostName(const OFString &peerHostName);

  /** Set SCP's AETitle to talk to in association negotiation
   *  @param peerAETitle [in] The SCP's AETitle to be used
   */
  void setPeerAETitle(const OFString &peerAETitle);

  /** Set SCP's port number to connect to for association negotiation
   *  @param peerPort [in] The SCP's port number
   */
  void setPeerPort(const Uint16 peerPort);

  /** Set timeout for receiving DIMSE messages
   *  @param dimseTimeout [in] DIMSE Timeout in seconds for receiving data. If the blocking
   *                           mode is DIMSE_NONBLOCKING and we are trying to read data from
   *                           the incoming socket stream and no data has been received.
   */
  void setDIMSETimeout(const Uint32 dimseTimeout);

  /** Set timeout for receiving ACSE messages
   *  @param acseTimeout [in] ACSE Timeout in seconds used by timer for message timeouts
   *                          during association negotiation
   */
  void setACSETimeout(const Uint32 acseTimeout);

  /** Set an association configuration file and profile to be used
   *  @param filename [in] File name of the association configuration file
   *  @param profile  [in] Profile inside the association negotiation file
   */
  void setAssocConfigFileAndProfile(const OFString &filename,
                                    const OFString &profile);

  /** Set the directory that should be used by the standard C-GET handler to store objects
   *  that come in with the corresponding C-STORE rqeuests
   *  @param storeDir [in] The directory to store to. It is checked in handleSTORERequest()
   *                       whether the directory is writeable and readable. Per default, the
   *                       received objects are stored in the current working directory.
   */
  void setStorageDir(const OFString& storeDir);

  /** Set the storage mode to be used. Default is DCMSCU_STORAGE_DISK.
   *  @param storageMode The storage mode to be used.
   */
  void setStorageMode(const DcmStorageMode storageMode);

  /** Set whether to show presentation contexts in verbose or debug mode
   *  @param mode [in] Show presentation contexts in verbose mode if OFTrue. By default, the
   *                   presentation contexts are shown in debug mode.
   */
  void setVerbosePCMode(const OFBool mode);

  /** Set the mode that specifies whether the transfer syntax of the dataset can be changed
   *  for network transmission.  This mainly covers the compression/decompression of datasets,
   *  which is disabled by default.
   *  @param mode [in] Allow dataset conversion if OFTrue
   */
  void setDatasetConversionMode(const OFBool mode);

  /* Get methods */

  /** Get current connection status
   *  @return OFTrue if SCU is currently connected, OFFalse otherwise
   */
  OFBool isConnected() const;

  /** Returns maximum PDU length configured to be received by SCU
   *  @return Maximum PDU length in bytes
   */
  Uint32 getMaxReceivePDULength() const;

  /** Returns whether DIMSE messaging is configured to be blocking or unblocking
   *  @return The blocking mode configured
   */
  T_DIMSE_BlockingMode getDIMSEBlockingMode() const;

  /** Returns the SCU's own configured AETitle
   *  @return The AETitle configured for this SCU
   */
  const OFString &getAETitle() const;

  /** Returns the SCP's (peer's) host name configured
   *  @return The hostname (or IP) configured to be talked to
   */
  const OFString &getPeerHostName() const;

  /** Returns the SCP's (peer's) AETitle configured
   *  @return The AETitle configured to be talked to
   */
  const OFString &getPeerAETitle() const;

  /** Returns the SCP's (peer's) TCP/IP port configured
   *  @return The port configured to talked to
   */
  Uint16 getPeerPort() const;

  /** Returns the DIMSE timeout configured defining how long SCU will wait for DIMSE responses
   *  @return The DIMSE timeout configured
   */
  Uint32 getDIMSETimeout() const;

  /** Returns the timeout configured defining how long SCU will wait for messages during ACSE
   *  messaging (association negotiation)
   *  @return The ACSE timeout configured
   */
  Uint32 getACSETimeout() const;

  /** Returns the verbose presentation context mode configured specifying whether details on
   *  the presentation contexts (negotiated during association setup) should be shown in
   *  verbose or debug mode. The latter is the default.
   *  @return The verbose presentation context mode configured
   */
  OFBool getVerbosePCMode() const;

  /** Returns the mode that specifies whether the transfer syntax of the dataset can be
   *  changed for network transmission.  This mainly covers the compression/decompression
   *  of datasets, which is disabled by default.
   *  @return The transfer syntax conversion mode
   */
  OFBool getDatasetConversionMode() const;

  /** Returns the storage directory used for storing objects received with C-STORE requests
   *  in the context of C-GET sessions. Default is empty string which refers to the current
   *  working directory.
   *  @return The storage directory
   */
  OFString getStorageDir() const;

  /** Returns the storage mode enabled
   *  @return The storage mode enabled
   */
  DcmStorageMode getStorageMode() const;

  /** Returns whether SCU is configured to create a TLS connection with the SCP
   *  @return OFFalse for this class but may be overriden by derived classes
   */
  OFBool getTLSEnabled() const;

  /** Deletes internal networking structures from memory */
  void freeNetwork();

protected:

  /** Sends a DIMSE command and possibly also a dataset from a data object via network to
   *  another DICOM application
   *  @param presID          [in]  Presentation context ID to be used for message
   *  @param msg             [in]  Structure that represents a certain DIMSE command which
   *                               shall be sent
   *  @param dataObject      [in]  The instance data which shall be sent to the other DICOM
   *                               application; NULL, if there is none
   *  @param callback        [in]  Pointer to a function which shall be called to indicate
   *                               progress
   *  @param callbackContext [in]  Pointer to data which shall be passed to the progress
   *                               indicating function
   *  @param commandSet      [out] If this parameter is not NULL it will return a copy of the
   *                               DIMSE command which is sent to the other DICOM application
   *  @return EC_Normal if sending request was successful, an error code otherwise
   */
  OFCondition sendDIMSEMessage(const T_ASC_PresentationContextID presID,
                               T_DIMSE_Message *msg,
                               DcmDataset *dataObject,
                               DIMSE_ProgressCallback callback,
                               void *callbackContext,
                               DcmDataset **commandSet = NULL);

  /** Returns SOP Class UID, SOP Instance UID and original transfer syntax for a given dataset.
   *  If the dataset is NULL, all returned values will be undefined (i.e. empty or EXS_Unknown).
   *  @param dataset        [in]  The dataset to read from
   *  @param sopClassUID    [out] The value of attribute SOP Class UID if present
   *  @param sopInstanceUID [out] The value of attribute SOP Instance UID if present
   *  @param transferSyntax [out] The value of transfer syntax that originally was read from
   *                              disk.  Will be unknown if the dataset was created in memory.
   *  @return EC_Normal if all information could be retrieved and is valid, an error code
   *    otherwise
   */
  OFCondition getDatasetInfo(DcmDataset *dataset,
                             OFString &sopClassUID,
                             OFString &sopInstanceUID,
                             E_TransferSyntax &transferSyntax);

  /** Tells DcmSCU to use a secure TLS connection described by the given TLS layer
   *  @param tlayer [in] The TLS transport layer including all TLS parameters
   *  @return EC_Normal if given transport layer is ok, an error code otherwise
   */
  OFCondition useSecureConnection(DcmTransportLayer *tlayer);

  /** Receive DIMSE command (excluding dataset!) over the currently open association
   *  @param presID       [out] Contains in the end the ID of the presentation context
   *                            which was specified in the DIMSE command received
   *  @param msg          [out] The message received
   *  @param statusDetail [out] If a non-NULL value is passed this variable will in the end
   *                            contain detailed information with regard to the status
   *                            information which is captured in the status element
   *                            (0000,0900). Note that the value for element (0000,0900) is
   *                            not contained in this return value but in internal msg. For
   *                            details on the structure of this object, see DICOM standard
   *                            part 7, annex C).
   *  @param commandSet   [out] If this parameter is not NULL, it will return a copy of the
   *                            DIMSE command which was received from the other DICOM
   *                            application. The caller is responsible to de-allocate the
   *                            returned object!
   *  @param timeout      [in]  If this parameter is not 0, it specifies the timeout (in
   *                            seconds) to be used for receiving the DIMSE command.
   *                            Otherwise, the default timeout value is used (see
   *                            setDIMSETimeout()).
   *  @return EC_Normal if command could be received successfully, an error code otherwise
   */
  OFCondition receiveDIMSECommand(T_ASC_PresentationContextID *presID,
                                  T_DIMSE_Message *msg,
                                  DcmDataset **statusDetail,
                                  DcmDataset **commandSet = NULL,
                                  const Uint32 timeout = 0);

  /** Receives one dataset (of instance data) via network from another DICOM application
   *  @param presID          [out] Contains in the end the ID of the presentation context
   *                               which was used in the PDVs that were received on the
   *                               network. If the PDVs show different presentation context
   *                               IDs, this function will return an error.
   *  @param dataObject      [out] Contains in the end the information which was received
   *                               over the network
   *  @param callback        [in]  Pointer to a function which shall be called to indicate
   *                               progress
   *  @param callbackContext [in]  Pointer to data which shall be passed to the progress
   *                               indicating function
   *  @return EC_Normal if dataset could be received successfully, an error code otherwise
   */
  OFCondition receiveDIMSEDataset(T_ASC_PresentationContextID *presID,
                                  DcmDataset **dataObject,
                                  DIMSE_ProgressCallback callback,
                                  void *callbackContext);

  /** clear list of presentation contexts.  In addition, any currently selected association
   *  configuration file is disabled.
   */
  void clearPresentationContexts();

   /** After negotiation association, this call returns the presentation context belonging
    *  to the given presentation context ID
    *  @param presID         [in]  The presentation context ID to look for
    *  @param abstractSyntax [out] The abstract syntax (UID) for that ID.
    *                              Empty, if such a presentation context does not exist.
    *  @param transferSyntax [out] The transfer syntax (UID) for that ID.
    *                              Empty, if such a presentation context does not exist.
    */
  void findPresentationContext(const T_ASC_PresentationContextID presID,
                               OFString &abstractSyntax,
                               OFString &transferSyntax);

  /** Check given N-EVENT-REPORT request and dataset for validity. This method is called by
   *  handleEVENTREPORTRequest() before sending the response in order to determine the
   *  DIMSE status code to be used for the response message.
   *  @param request    [in] The N-EVENT-REPORT request message data structure
   *  @param reqDataset [in] The N-EVENT-REPORT request dataset received. Might be NULL.
   *  @return DIMSE status code to be used for the N-EVENT-REPORT response.
   *          Always returns STATUS_Success (0). Derived classes should, therefore,
   *          overwrite this method and return a more appropriate value based on the
   *          result of the checks performed.
   */
  virtual Uint16 checkEVENTREPORTRequest(T_DIMSE_N_EventReportRQ &request,
                                         DcmDataset *reqDataset);

  /** Sends back a C-STORE response on the given presentation context, with the designated
   *  status, fitting the corresponding C-STORE request.
   *  @param presID  [in] The presentation context ID to be used.
   *  @param status  [in] The storage DIMSE status to be used.
   *  @param request [in] The C-STORE request that should be responded to.
   *  @result EC_Normal if the response could be sent, error otherwise.
   */
  virtual OFCondition sendSTOREResponse(T_ASC_PresentationContextID presID,
                                        Uint16 status,
                                        const T_DIMSE_C_StoreRQ& request);

  /** Helper function that generates a storage filename by extracting SOP Class and SOP
   *  Instance UID from a dataset and combining that with the configured storage directory.
   *  The SOP class is used to create an initial two letter abbreviation for the
   *  corresponding modality, e.g. CT. An example for a storage filename created by this
   *  function is /storage_dir/CT.1.2.3.4.5 for a CT with SOP Instance UID 1.2.3.4.
   *  This function might be overwritten to change the filename behaviour completely. This
   *  function is only called if the SCU is in DCMSCU_STORAGE_DISK mode.
   *  @param dataset [in] The dataset that should be stored to disk
   *  @result Non-empty string if successful, otherwise empty string.
   */
  virtual OFString createStorageFilename(DcmDataset *dataset);

  /** Receives a DICOM dataset on a given presentation context ID but does not store it in
   *  memory or disk, thus ignoring it.
   *  @param presID  [in] The presentation context to be used
   *  @param request [in] The corresponding C-STORE request
   *  @return EC_Normal if ignoring worked, error code otherwise.
   */
  virtual OFCondition ignoreSTORERequest(T_ASC_PresentationContextID presID,
                                         const T_DIMSE_C_StoreRQ& request);

private:

  /** Private undefined copy-constructor. Shall never be called.
   *  @param src Source object
   */
  ctkDcmSCU(const ctkDcmSCU &src);

  /** Private undefined operator=. Shall never be called.
   *  @param src Source object
   *  @return Reference to this
   */
  ctkDcmSCU &operator=(const ctkDcmSCU &src);

  /// Associaton of this SCU. This class only handles 1 association at a time.
  T_ASC_Association *m_assoc;

  /// The DICOM network the association is based on
  T_ASC_Network *m_net;

  /// Association parameters
  T_ASC_Parameters *m_params;

  /// Configuration file for presentation contexts (optional)
  OFString m_assocConfigFilename;

  /// Profile in configuration file that should be used (optional)
  OFString m_assocConfigProfile;

  /// Defines presentation context, consisting of one abstract syntax name
  /// and a list of transfer syntaxes for this abstract syntax
  struct DcmSCUPresContext {
    /** Default constructor
     */
    DcmSCUPresContext()
    : abstractSyntaxName()
    , transferSyntaxes()
    , roleSelect(ASC_SC_ROLE_DEFAULT)
    {
    }
    /// Abstract Syntax Name of Presentation Context
    OFString abstractSyntaxName;
    /// List of Transfer Syntaxes for Presentation Context
    OFList<OFString> transferSyntaxes;
    /// Role Selection
    T_ASC_SC_ROLE roleSelect;
  };

  /// List of presentation contexts that should be negotiated
  OFList<DcmSCUPresContext> m_presContexts;

  /// Configuration file containing association parameters
  OFString m_assocConfigFile;

  /// The last DIMSE successfully sent, unresponded DIMSE request
  T_DIMSE_Message *m_openDIMSERequest;

  /// Maximum PDU size
  Uint32 m_maxReceivePDULength;

  /// DIMSE blocking mode
  T_DIMSE_BlockingMode m_blockMode;

  /// AEtitle of this application
  OFString m_ourAETitle;

  /// Peer hostname
  OFString m_peer;

  /// AEtitle of remote application
  OFString m_peerAETitle;

  /// Port of remote application entity
  Uint16 m_peerPort;

  /// DIMSE timeout
  Uint32 m_dimseTimeout;

  /// ACSE timeout
  Uint32 m_acseTimeout;

  /// Verbose PC mode
  OFBool m_verbosePCMode;

  /// Dataset conversion mode
  OFBool m_datasetConversionMode;

  /// Storage directory for objects received with C-STORE due to a
  /// running C-GET session. Per default, the received objects
  /// are stored in the current working directory.
  OFString m_storageDir;

  /// Set whether bit preserving storage should be enabled, i.e.\ any objects
  /// retrieved via C-STORE should be written directly to disk without
  /// any data correction/re-computation (e.g.\ group length calculations,
  /// padding, etc.). This is especially interesting for retaining valid
  /// signatures, and also to receive huge files which cannot be fully received
  /// in memory. Default is OFFalse (no bit preserving) storage.
  DcmStorageMode m_storageMode;

  /** Returns next available message ID free to be used by SCU
   *  @return Next free message ID
   */
  Uint16 nextMessageID();
};

#endif // ctkDcmSCU_H

/*
** CVS Log
** $Log: scu.h,v $
** Revision 1.39  2011-10-10 14:01:29  uli
** Moved SCU-specific error condition to the correct place.
**
** Revision 1.38  2011-10-04 08:58:14  joergr
** Added flag that allows for specifying whether to convert a dataset to be
** transferred to the network transfer syntax. Also removed unused parameters
** "rspCommandSet" and "rspStatusDetail" from method sendSTORERequest().
**
** Revision 1.37  2011-09-28 16:28:20  joergr
** Added general support for transfer syntax conversions to sendSTORERequest().
**
** Revision 1.36  2011-09-28 15:25:34  joergr
** Return a more appropriate error code in case the dataset to be sent is
** invalid. This also required to introduce a return value for getDatasetInfo().
**
** Revision 1.35  2011-09-28 13:31:56  joergr
** Added method that allows for clearing the list of presentation contexts.
**
** Revision 1.34  2011-09-22 13:49:03  joergr
** Fixed incorrect comment on return code of some sendXXXRequest() methods.
**
** Revision 1.33  2011-09-06 16:11:06  ogazzar
** Fixed typos in a log commit message.
**
** Revision 1.32  2011-09-06 12:57:36  ogazzar
** Added a function to send N-EVENT-REPORT request and receive a reponse.
**
** Revision 1.31  2011-08-25 15:46:18  joergr
** Further cleanup of minor inconsistencies regarding documentation, parameter
** names, log output and handling of status details information.
**
** Revision 1.30  2011-08-25 15:05:06  joergr
** Changed data structure for Q/R responses from OFVector to OFList. Also fixed
** some possible memory leaks and made the FIND/MOVE/GET code more consistent.
**
** Revision 1.29  2011-08-25 13:46:28  joergr
** Fixed minor issues in the API documentation, parameter and method names.
**
** Revision 1.28  2011-08-25 09:31:33  onken
** Added C-GET functionality to DcmSCU class and accompanying getscu
** commandline application.
**
** Revision 1.27  2011-05-25 09:57:54  ogazzar
** Renamed a function name.
**
** Revision 1.26  2011-05-25 09:30:12  ogazzar
** Added a function to look for a presentation context ID that best matches
** the abstract syntax and the transfer syntax.
**
** Revision 1.25  2011-05-24 13:17:58  onken
** Added missing default initializiation of role flag in presentation context
** constructor.
**
** Revision 1.24  2011-05-24 08:28:08  ogazzar
** Added support for role selection negotiation.
**
** Revision 1.23  2011-05-19 17:20:09  onken
** Fixed some documentation.
**
** Revision 1.22  2011-05-19 09:57:26  onken
** Fixed message ID field in C-CANCEL request (should be the one of last
** request). In case of error status codes in C-MOVE responses, the default
** behaviour is now to not wait for further responses. Fixed log output level
** to better fit the messages while receiveing C-MOVE responses. Minor
** code and comment cleanups. Renamed function parameter in sendMOVEREquest
** to better reflect the standard.
**
** Revision 1.21  2011-05-17 14:43:55  onken
** Removed some windows line endings.
**
** Revision 1.20  2011-05-17 14:34:32  onken
** Completed doxygen documentation of DcmSCU class. Fixed some minor formatting
** issues. Fixed CVS log at end of the file. Implemented C-CANCEL message.
** Fixed some minor formatting issues. Changed C-ECHO implementation to rely
** on sendDIMSEMesage as the other DIMSE functions do. Changed some public
** function arguments to const to be more correct. Fixed CVS log at the end
** of the file. Re-formatted CHANGES log entry from 2011-04-28.
**
** Revision 1.19 2011-05-17 14:26:19  onken
** Implemented C-CANCEL message. Fixed some minor formatting issues.
** Changed C-ECHO implementation to rely on sendDIMSEMesage as the other
** DIMSE functions do. Changed some public function arguments to const to be
** more correct. Fixed CVS log at the end of the scu.cc file.
**
** Revision 1.18 2011-04-28 17:50:05  onken
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
** Revision 1.17  2011-02-16 08:55:13  joergr
** Fixed issue in sendSTORERequest() when sending a dataset that was created
** in memory (and which has, therefore, an original transfer of EXS_Unknown).
**
** Revision 1.16  2011-02-04 12:57:40  uli
** Made sure all members are initialized in the constructor (-Weffc++).
**
** Revision 1.15  2011-02-04 11:24:40  uli
** Added private undefined functions where gcc's -Weffc++ warns otherwise.
**
** Revision 1.14  2011-02-01 09:01:19  joergr
** Added "const" specifier to parameter in order to be consistent with the
** source file (caused warnings/errors with certain compilers).
**
** Revision 1.13  2010-10-14 13:17:22  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.12  2010-10-07 12:54:07  joergr
** Fixed minor Doxygen API documentation issues (added backslash in order to
** avoid that the short description ends at the first period).
**
** Revision 1.11  2010-06-24 09:21:54  joergr
** Revised comment to make clear that the parameter "presID" shall never be 0
** for the sendACTIONRequest() method.
**
** Revision 1.10  2010-06-22 15:45:27  joergr
** Introduced new enumeration type to be used for closeAssociation().
** Further code cleanup. Renamed some methods, variables, types and so on.
**
** Revision 1.9  2010-06-17 17:11:27  joergr
** Added preliminary support for N-EVENT-REPORT to DcmSCU. Some further code
** cleanups and enhancements. Renamed some methods. Revised documentation.
**
** Revision 1.8  2010-06-09 16:09:01  joergr
** Added preliminary support for N-ACTION to DcmSCU. Some further code cleanups
** and enhancements.
**
** Revision 1.7  2010-06-08 17:54:12  onken
** Added C-FIND functionality to DcmSCU. Some code cleanups. Fixed
** memory leak sometimes occuring during association configuration.
**
** Revision 1.6  2010-04-29 16:13:28  onken
** Made SCU class independent from dcmtls, i.e. outsourced TLS API. Added
** direct API support for sending C-STORE requests. Further API changes and
** some bugs fixed.
**
** Revision 1.5  2009-12-21 17:00:32  onken
** Fixed API documentation to keep doxygen quiet.
**
** Revision 1.4  2009-12-21 15:33:55  onken
** Added documentation and refactored / enhanced some code.
**
** Revision 1.3  2009-12-17 09:12:10  onken
** Fixed other scu and scp base class compile issues.
**
** Revision 1.2  2009-12-17 09:05:15  onken
** Fixed typo resulting in build failure.
**
** Revision 1.1  2009-12-17 09:02:44  onken
** Added base classes for SCU and SCP implementations.
**
** Revision 1.2  2009-12-02 14:26:05  uli
** Stop including dcdebug.h which was removed.
**
** Revision 1.1  2008-09-29 13:51:52  onken
** Initial checkin of module dcmppscu implementing an MPPS commandline client.
**
*/
