#ifndef __qCTKDCMTK_h
#define __qCTKDCMTK_h

// QT includes 
#include <QObject>

// qCTK includes
#include <qCTKPimpl.h>

#include "qCTKDMCTKExport.h"

class qCTKDCMTKPrivate;
class Q_CTKDMCTK_EXPORT qCTKDCMTK : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  explicit qCTKDCMTK(QObject* parent = 0);
  virtual ~qCTKDCMTK();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTK);
};

#endif
