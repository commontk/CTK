#ifndef __ctkXIPAdaptor_h
#define __ctkXIPAdaptor_h

// Qt includes 
#include <QObject>

// CTK includes
#include <ctkPimpl.h>

#include "CTKVisualizationXIPExport.h"

class ctkXIPAdaptorPrivate;
class CTK_VISUALIZATION_XIP_EXPORT ctkXIPAdaptor : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  explicit ctkXIPAdaptor(QObject* parent = 0);
  virtual ~ctkXIPAdaptor();
  
private:
  CTK_DECLARE_PRIVATE(ctkXIPAdaptor);
};

#endif
