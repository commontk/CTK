/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QApplication>

// CTKVTK includes
#include "ctkVTKObjectTestHelper.h"

// VTK includes
#include <vtkObject.h>
#include <vtkTimerLog.h>
#include <vtkSmartPointer.h>

//------------------------------------------------------------------------------
class ctkVTKObjectTestPrivate
{
public:
  ctkVTKObjectTestPrivate();
  int PublicSlotCalled ;
  int ProtectedSlotCalled;
  int PrivateSlotCalled;
};

//------------------------------------------------------------------------------
ctkVTKObjectTestPrivate::ctkVTKObjectTestPrivate()
{
  this->PublicSlotCalled = 0;
  this->ProtectedSlotCalled = 0;
  this->PrivateSlotCalled = 0;
}

//------------------------------------------------------------------------------
ctkVTKObjectTest::ctkVTKObjectTest(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkVTKObjectTestPrivate)
{
}

//------------------------------------------------------------------------------
ctkVTKObjectTest::~ctkVTKObjectTest()
{
}

//------------------------------------------------------------------------------
bool ctkVTKObjectTest::test()
{
  Q_D(ctkVTKObjectTest);
  // should do nothing but shouldn't fail neither
  qDebug() << "The following can generate error message.";
  qDebug() << "Disconnect:";
  this->qvtkDisconnect(0, static_cast<unsigned long>(-1), this, SLOT(onVTKObjectModifiedPublic()));
  qDebug() << "Connect:";
  QString connection = this->qvtkConnect(0, static_cast<unsigned long>(-1), this, SLOT(onVTKObjectModifiedPublic()));
  if (!connection.isEmpty())
    {
    qDebug() << "ctkVTKObject::qvtkConnect() failed: "<< connection;
    return false;
    }
  qDebug() << "Reconnect:";
  connection = this->qvtkReconnect(0, 0, static_cast<unsigned long>(-1), this, SLOT(onVTKObjectModifiedPublic()));
  if (!connection.isEmpty())
    {
    qDebug() << "ctkVTKObject::qvtkReconnect() failed: "<< connection;
    return false;
    }
  qDebug() << "End of possible error messages.";
  
  vtkObject* object = vtkObject::New();


  int numberOfConnections=10000;

  qDebug() << "Create "<<numberOfConnections<<" connections...";
  vtkSmartPointer<vtkTimerLog> timerLog = vtkSmartPointer<vtkTimerLog>::New();
  timerLog->StartTimer();
  for (int i = 1; i <= numberOfConnections; ++i)
    {
    this->qvtkConnect(object, i, this, SLOT(onVTKObjectModifiedPublic()));
    }
  timerLog->StopTimer();
  double t1 = timerLog->GetElapsedTime();
  qDebug() << "  elapsed time: " << t1 << "seconds";
  
  qDebug() << "Remove "<<numberOfConnections<<" connections...";
  timerLog->StartTimer();
  for (int i = numberOfConnections; i>=1 ; --i)
    {
    this->qvtkDisconnect(object, i, this, SLOT(onVTKObjectModifiedPublic()));
    }
  timerLog->StopTimer();
  t1 = timerLog->GetElapsedTime();
  qDebug() << "  elapsed time: " << t1 << "seconds";

  connection = this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                                 this, SLOT(onVTKObjectModifiedPublic()));
  if (connection.isEmpty() || object->GetReferenceCount() != 1)
    {
    qDebug() << "ctkVTKObject::qvtkConnect() failed: "<< connection;
    return false;
    }

  object->Modified();

  if (d->PublicSlotCalled != 1)
    {
    qDebug() << "qvtkConnect failed";
    return false;
    }
  
  this->resetSlotCalls();
  // should do nothing...
  connection = this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                                 this, SLOT(onVTKObjectModifiedPublic()));
  if (!connection.isEmpty())
    {
    qDebug() << __LINE__ << "ctkVTKObject::qvtkConnect() failed: "<< connection;
    return false;
    }

  object->Modified();
  
  if (d->PublicSlotCalled != 1)
    {
    qDebug() << __LINE__ << "qvtkConnect failed";
    return false;
    }

  this->resetSlotCalls();

  this->qvtkDisconnect(object, vtkCommand::WarningEvent, 
                       this, SLOT(onVTKObjectModifiedPublic()));
  object->Modified();

  if (d->PublicSlotCalled != 1)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" << d->PublicSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  this->qvtkDisconnect(object, vtkCommand::ModifiedEvent, 
                       this, SLOT(onVTKObjectModifiedPublic()));
  QCoreApplication::instance()->processEvents();
  object->Modified();

  if (d->PublicSlotCalled != 0)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" << d->PublicSlotCalled;
    return false;
    }
  this->resetSlotCalls();
  
  // Set a new connection (protected)
  connection = this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                                 this, SLOT(onVTKObjectModifiedProtected()));
  if (connection.isEmpty())
    {
    qDebug() << __LINE__ << "ctkVTKObject::qvtkConnect() failed: "<< connection;
    return false;
    }

  object->Modified();
  
  if (d->ProtectedSlotCalled != 1)
    {
    qDebug() << __LINE__ << "ctkVTKObject::qvtkConnect failed" << d->ProtectedSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  // remove the connection using flags, 0 means any event, qt object or slot
  this->qvtkDisconnect(object, vtkCommand::NoEvent, 0, 0);
  object->Modified();

  if (d->ProtectedSlotCalled != 0)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" << d->ProtectedSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  // Set new connections
  this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                    this, SLOT(onVTKObjectModifiedProtected()));
  this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                    this, SLOT(onVTKObjectModifiedPrivate()));
  object->Modified();
  if (d->ProtectedSlotCalled != 1 || 
      d->PrivateSlotCalled != 1)
    {
    qDebug() << __LINE__ << "qvtkConnect failed" 
             << d->ProtectedSlotCalled 
             << d->PrivateSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  // remove the connection using flags, 0 means any event, qt object or slot
  this->qvtkDisconnect(object, vtkCommand::ModifiedEvent, this, 0);
  object->Modified();
  if (d->ProtectedSlotCalled != 0 || d->PrivateSlotCalled != 0)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" 
             << d->ProtectedSlotCalled
             << d->PrivateSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  // Set new connections
  this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                    this, SLOT(onVTKObjectModifiedPublic()));
  this->qvtkConnect(object, vtkCommand::WarningEvent, 
                    this, SLOT(onVTKObjectModifiedPublic()));
  int disconnected = this->qvtkDisconnect(object, vtkCommand::NoEvent, 
                                          this, SLOT(onVTKObjectModifiedPublic()));
  if (disconnected != 2)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" << disconnected;
    return false;
    }

  object->InvokeEvent(vtkCommand::ModifiedEvent, 0);
  object->InvokeEvent(vtkCommand::WarningEvent, 0);

  if (d->PublicSlotCalled != 0)
    {
    qDebug() << __LINE__ << "qvtkConnect failed" 
             << d->PublicSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  disconnected = this->qvtkDisconnectAll();
  if (disconnected != 0)
    {
    qDebug() << __LINE__ << "qvtkDisconnectAll failed" << disconnected;
    return false;
    }
  this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                    this, SLOT(deleteConnection()));
  object->InvokeEvent(vtkCommand::ModifiedEvent, 0);

  object->Delete();
  
  return true;
}

//------------------------------------------------------------------------------
void ctkVTKObjectTest::resetSlotCalls()
{
  Q_D(ctkVTKObjectTest);
  d->PublicSlotCalled = 0;
  d->ProtectedSlotCalled = 0;
  d->PrivateSlotCalled = 0;
}

//------------------------------------------------------------------------------
void ctkVTKObjectTest::emitSignalEmitted()
{
  emit signalEmitted();
}

//------------------------------------------------------------------------------
void ctkVTKObjectTest::onVTKObjectModifiedPublic()
{
  Q_D(ctkVTKObjectTest);
  //qDebug() << __FUNCTION__;
  d->PublicSlotCalled = true;
}

//------------------------------------------------------------------------------
void ctkVTKObjectTest::deleteConnection()
{
  //qDebug() << __FUNCTION__;
  this->qvtkDisconnect(0, vtkCommand::NoEvent, 0, 0);
}

//------------------------------------------------------------------------------
void ctkVTKObjectTest::onVTKObjectModifiedProtected()
{
  Q_D(ctkVTKObjectTest);
  //qDebug() << __FUNCTION__;
  d->ProtectedSlotCalled = true;
}

//------------------------------------------------------------------------------
void ctkVTKObjectTest::onVTKObjectModifiedPrivate()
{
  Q_D(ctkVTKObjectTest);
  //qDebug() << __FUNCTION__;
  d->PrivateSlotCalled = true;
}
