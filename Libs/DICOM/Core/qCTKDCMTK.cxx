
// qCTKDCMTK includes
#include "qCTKDCMTK.h"

//----------------------------------------------------------------------------
class qCTKDCMTKPrivate: public qCTKPrivate<qCTKDCMTK>
{
public:
  qCTKDCMTKPrivate(){}
};

//----------------------------------------------------------------------------
// qCTKDCMTKPrivate methods


//----------------------------------------------------------------------------
// qCTKDCMTKWidget methods

qCTKDCMTK::qCTKDCMTK(QObject* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(qCTKDCMTK);
}

qCTKDCMTK::~qCTKDCMTK()
{
}

