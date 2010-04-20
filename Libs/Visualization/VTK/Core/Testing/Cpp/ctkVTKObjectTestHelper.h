#ifndef __ctkVTKObjectTestHelper_h
#define __ctkVTKObjectTestHelper_h

// Qt includes
#include <QObject>

// CTK includes
#include <ctkPimpl.h>

// CTKVTK includes
#include "ctkVTKObject.h"

class ctkVTKObjectTestPrivate;

class ctkVTKObjectTest: public QObject
{
  Q_OBJECT
  QVTK_OBJECT
public:
  ctkVTKObjectTest();

  bool test();

  bool isPublicSlotCalled()const;
  bool isProtectedSlotCalled()const;
  bool isPrivateSlotCalled()const;

  void resetSlotCalls();

public slots:
  void onVTKObjectModifiedPublic();
  void deleteConnection();

protected slots:
  void onVTKObjectModifiedProtected();

private slots:
  void onVTKObjectModifiedPrivate();

signals:
  void signalEmitted();

private:
  CTK_DECLARE_PRIVATE(ctkVTKObjectTest);
};

#endif
