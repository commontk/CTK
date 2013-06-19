#ifndef CTKDICOMFINDCONTEXT_H
#define CTKDICOMFINDCONTEXT_H

// DCMTK includes
#include <dcmtk/config/osconfig.h>    /* make sure OS specific configuration is included first */
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmqrdb/qrdefine.h>
#include <dcmtk/dcmqrdb/dcmqrdba.h>

// Qt includes
#include <QDebug>
#include <QObject>

#include "ctkDICOMCoreExport.h"

class CTK_DICOM_CORE_EXPORT ctkDICOMFindContext : public QObject
{
  Q_OBJECT
public:
    /** constructor
     *  @param options options for the Q/R service
     *  @param priorStat prior DIMSE status
     */
    ctkDICOMFindContext(DIC_US priorStat, QObject *parent = 0);

    /** set the AEtitle under which this application operates
     *  @param ae AEtitle, is copied into this object.
     */
    void setOurAETitle(const char *ae)
    {
      if (ae) ourAETitle = ae; else ourAETitle.clear();
    }

    /** callback handler called by the DIMSE_storeProvider callback function.
     *  @param cancelled (in) flag indicating whether a C-CANCEL was received
     *  @param request original find request (in)
     *  @param requestIdentifiers original find request identifiers (in)
     *  @param responseCount find response count (in)
     *  @param response find response (out)
     *  @param responseIdentifiers find response identifiers (out)
     *  @param stDetail status detail for find response (out)
     */
    void callbackHandler(
        OFBool cancelled, T_DIMSE_C_FindRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        T_DIMSE_C_FindRSP *response,
        DcmDataset **responseIdentifiers,
        DcmDataset **stDetail);

    virtual OFCondition startFindRequest (const char *SOPClassUID, DcmDataset *findRequestIdentifiers, DcmQueryRetrieveDatabaseStatus *status)
    {
      return OFCondition(OFM_dcmdata, 0x001, OF_error, "DcmQR Index Database Error");
    }

    virtual OFCondition nextFindResponse (DcmDataset **findResponseIdentifiers, DcmQueryRetrieveDatabaseStatus *status)
    {
      return OFCondition(OFM_dcmdata, 0x001, OF_error, "DcmQR Index Database Error");
    }

    virtual OFCondition cancelFindRequest (DcmQueryRetrieveDatabaseStatus *status)
    {
      return OFCondition(OFM_dcmdata, 0x001, OF_error, "DcmQR Index Database Error");
    }

private:
    /// prior DIMSE status
    DIC_US priorStatus;

    /// our current title
    OFString ourAETitle;
};

#endif // CTKDCMFINDCONTEXT_H
