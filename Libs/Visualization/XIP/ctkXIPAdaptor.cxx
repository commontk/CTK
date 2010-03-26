
// Qt includes
#include <QDebug>

// ctkXIPAdaptor includes
#include "ctkXIPAdaptor.h"

//----------------------------------------------------------------------------
class ctkXIPAdaptorPrivate: public qCTKPrivate<ctkXIPAdaptor>
{
public:
  ctkXIPAdaptorPrivate();
};

//----------------------------------------------------------------------------
// ctkXIPAdaptorPrivate methods

//------------------------------------------------------------------------------
ctkXIPAdaptorPrivate::ctkXIPAdaptorPrivate()
{
}

//----------------------------------------------------------------------------
// ctkXIPAdaptorWidget methods

//------------------------------------------------------------------------------
ctkXIPAdaptor::ctkXIPAdaptor(QObject* _parent): Superclass(_parent)
{
  QCTK_INIT_PRIVATE(ctkXIPAdaptor);
}

//----------------------------------------------------------------------------
ctkXIPAdaptor::~ctkXIPAdaptor()
{
}
