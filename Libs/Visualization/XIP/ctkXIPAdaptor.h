#ifndef __ctkXIPAdaptor_h
#define __ctkXIPAdaptor_h

// QT includes 
#include <QObject>

// CTK includes
#include <ctkPimpl.h>

#include "ctkXIPAdaptorCoreExport.h"

class ctkXIPAdaptorPrivate;
class CTK_XIP_EXPORT ctkXIPAdaptor : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  explicit ctkXIPAdaptor(QObject* parent = 0);
  virtual ~ctkXIPAdaptor();
  
private:
  QCTK_DECLARE_PRIVATE(ctkXIPAdaptor);
};

#endif
