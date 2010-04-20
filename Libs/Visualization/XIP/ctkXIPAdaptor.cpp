
// Qt includes
#include <QDebug>

// ctkXIPAdaptor includes
#include "ctkXIPAdaptor.h"

//----------------------------------------------------------------------------
class ctkXIPAdaptorPrivate: public ctkPrivate<ctkXIPAdaptor>
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
  CTK_INIT_PRIVATE(ctkXIPAdaptor);
}

//----------------------------------------------------------------------------
ctkXIPAdaptor::~ctkXIPAdaptor()
{
}
