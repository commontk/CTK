#ifndef CTKDICOMFINDSCP_H
#define CTKDICOMFINDSCP_H

// DCMTK includes
#include <dcmtk/dcmqrdb/dcmqrdbs.h>

// ctk includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMSCP.h"

class ctkDICOMFindSCPPrivate;
class ctkDICOMFindContext;

class CTK_DICOM_CORE_EXPORT ctkDICOMFindSCP : public ctkDICOMSCP
{
  Q_OBJECT
public:
  explicit ctkDICOMFindSCP(QObject *parent = 0);
  ~ctkDICOMFindSCP();
  
  virtual OFCondition handleRequest(T_ASC_Association *assoc, T_DIMSE_Message &message, T_ASC_PresentationContextID presId);

protected:
  virtual ctkDICOMFindContext* createContext();

private:
  ctkDICOMFindSCPPrivate* d_ptr;
  Q_DECLARE_PRIVATE(ctkDICOMFindSCP);
  Q_DISABLE_COPY(ctkDICOMFindSCP);
};

#endif // CTKDCMFINDSCP_H
