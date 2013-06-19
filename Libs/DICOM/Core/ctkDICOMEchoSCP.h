#ifndef CTKDICOMECHOSCP_H
#define CTKDICOMECHOSCP_H

#include "ctkDICOMCoreExport.h"
#include "ctkDICOMSCP.h"

class CTK_DICOM_CORE_EXPORT ctkDICOMEchoSCP : public ctkDICOMSCP
{
  Q_OBJECT
public:
  explicit ctkDICOMEchoSCP(QObject *parent = 0);
  
  virtual OFCondition handleRequest(T_ASC_Association *assoc, T_DIMSE_Message& message, T_ASC_PresentationContextID presId);
signals:
  
public slots:
  
};

#endif // CTKDICOMECHOSCP_H
