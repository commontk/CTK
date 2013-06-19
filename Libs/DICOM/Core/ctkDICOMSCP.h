#ifndef CTKDCMSCP_H
#define CTKDCMSCP_H

// DCMTK includes
#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmnet/dimse.h>

// Qt includes
#include <QObject>

// ctk includes
#include "ctkDICOMCoreExport.h"

class ctkDICOMSCPPrivate;

class CTK_DICOM_CORE_EXPORT ctkDICOMSCP : public QObject
{
  Q_OBJECT
public:
  explicit ctkDICOMSCP(QObject *parent = 0);
  ~ctkDICOMSCP();

  void setAETitle(const QString& aeTitle);
  void setPort(int port);
  void setMaxPDU(int size);
  void setMaxAssociations(int count);
  void setEnableGetSupport(bool flag);
  void setEnablePatientRoot(bool flag);
  void setEnableStudyRoot(bool flag);
  void setEctnablePatientStudyOnly(bool flag);
  void setAllowShutdown(bool flag);
  void setEnableThreading(bool flag);

  bool registerSCP(ctkDICOMSCP* scp);
  T_DIMSE_Command getDimseCommand();

  void start();
  void stop();

  virtual OFCondition handleRequest(T_ASC_Association* assoc, T_DIMSE_Message& message, T_ASC_PresentationContextID presId)
  {
    Q_UNUSED(assoc);
    Q_UNUSED(message);
    Q_UNUSED(presId);

    return DIMSE_BADCOMMANDTYPE;
  }

protected:
  T_DIMSE_Command dimseCommand;

private:
  ctkDICOMSCPPrivate* d_ptr;
  Q_DECLARE_PRIVATE(ctkDICOMSCP);
  Q_DISABLE_COPY(ctkDICOMSCP);
};

#endif // CTKDCMSCP_H
