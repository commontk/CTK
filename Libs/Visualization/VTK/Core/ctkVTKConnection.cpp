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
#include <QPointer>
#include <QRegExp>
#include <QString>
#include <QTextStream>

// CTK includes
#include "ctkVTKConnection.h"
#include "ctkVTKConnection_p.h"

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkCallbackCommand.h>

//-----------------------------------------------------------------------------
QString convertPointerToString(void* pointer)
{
  QString pointerAsString;
  QTextStream(&pointerAsString) << pointer;
  return pointerAsString;
}

//-----------------------------------------------------------------------------
// ctkVTKConnectionPrivate methods

//-----------------------------------------------------------------------------
ctkVTKConnectionPrivate::ctkVTKConnectionPrivate(ctkVTKConnection& object)
  :q_ptr(&object)
{
  this->Callback    = vtkSmartPointer<vtkCallbackCommand>::New();
  this->Callback->SetCallback(ctkVTKConnectionPrivate::DoCallback);
  this->Callback->SetClientData(this);
  this->QtObject    = 0;
  this->VTKEvent    = vtkCommand::NoEvent;
  this->Priority    = 0.0;
  this->ConnectionType = Qt::AutoConnection;
  this->SlotType    = ARG_UNKNOWN;
  this->Connected   = false;
  this->Blocked     = false;
  this->Id          = convertPointerToString(this);
  this->ObserveDeletion = false;
}

//-----------------------------------------------------------------------------
ctkVTKConnectionPrivate::~ctkVTKConnectionPrivate()
{

}

//-----------------------------------------------------------------------------
void ctkVTKConnectionPrivate::connect()
{
  Q_Q(ctkVTKConnection);
  
  if (this->Connected)
    {
    qDebug() << "ctkVTKConnection already connected.";
    return;
    }

  switch (this->SlotType)
    {
    case ctkVTKConnectionPrivate::ARG_VTKOBJECT_AND_VTKOBJECT:
      QObject::connect(q, SIGNAL(emitExecute(vtkObject*,vtkObject*)),
        this->QtObject, this->QtSlot.toLatin1(), this->ConnectionType);
      break;
    case ctkVTKConnectionPrivate::ARG_VTKOBJECT_VOID_ULONG_VOID:
      QObject::connect(q, SIGNAL(emitExecute(vtkObject*,void*,ulong,void*)),
                       this->QtObject, this->QtSlot.toLatin1(), this->ConnectionType);
      break;
    default:
      Q_ASSERT(false);
      qCritical() << "Failed to connect - "
                  << "The slot (" << this->QtSlot <<  ") owned by "
                  << "QObject(" << this->QtObject->objectName() << ")"
                  << " seems to have a wrong signature.";
      break;
    }

  // Make a connection between this and the vtk object
  q->addObserver(this->VTKObject, this->VTKEvent, this->Callback, this->Priority);

  // If necessary, observe vtk DeleteEvent
  if(this->ObserveDeletion)
    {
    // don't observe it twice
    if (this->VTKEvent != vtkCommand::DeleteEvent)
      {
      this->VTKObject->AddObserver(vtkCommand::DeleteEvent, this->Callback);
      }
    }

  // When Qt object is destroyed: (1) remove VTK observers and
  // (2) set QtObject pointer to 0.
  QObject::connect(this->QtObject, SIGNAL(destroyed(QObject*)),
                   q, SLOT(qobjectDeleted()));
  this->Connected = true;
}

//-----------------------------------------------------------------------------
void ctkVTKConnectionPrivate::disconnectSlots()
{
  Q_Q(ctkVTKConnection);
  
  if (!this->Connected) 
    { 
    return; 
    }

  if (this->QtObject)
    {
    switch (this->SlotType)
      {
      case ctkVTKConnectionPrivate::ARG_VTKOBJECT_AND_VTKOBJECT:
        QObject::disconnect(q, SIGNAL(emitExecute(vtkObject*,vtkObject*)),
                            this->QtObject,this->QtSlot.toLatin1().data());
        break;
      case ctkVTKConnectionPrivate::ARG_VTKOBJECT_VOID_ULONG_VOID:
        QObject::disconnect(q, SIGNAL(emitExecute(vtkObject*,void*,ulong,void*)),
                            this->QtObject, this->QtSlot.toLatin1().data());
        break;
      default:
        Q_ASSERT(false);
        qCritical() << "Failed to disconnect - "
                    << "The slot (" << this->QtSlot <<  ") owned by "
                    << "QObject(" << this->QtObject->objectName() << ")"
                    << " seems to have a wrong signature.";
        break;
      }
    }

  this->Connected = false;
}

//-----------------------------------------------------------------------------
void ctkVTKConnectionPrivate::disconnectVTKObject()
{
  Q_Q(ctkVTKConnection);
  if (this->VTKObject)
    {
    q->removeObserver(this->VTKObject, this->VTKEvent, this->Callback);
    if (this->ObserveDeletion)
      {
      this->VTKObject->RemoveObservers(vtkCommand::DeleteEvent, this->Callback);
      }
    }
}

//-----------------------------------------------------------------------------
bool ctkVTKConnectionPrivate::IsSameQtSlot(const char* qt_slot)const
{
  if (qt_slot == 0)
    {
    return true;
    }
  const char* ptr = qt_slot;
  for (QString::const_iterator it = this->QtSlot.begin();
       it != this->QtSlot.end() && *ptr != '\0'; )
    {
    if (*it == *ptr)
      {
      ++it;
      ++ptr;
      }
    else if (*it == ' ')
      {
      ++it;
      }
    else if (*ptr == ' ')
      {
      ++ptr;
      }
    else
      {
      return false;
      }
    }
  return true;
}

//-----------------------------------------------------------------------------
// ctkVTKConnection methods

//-----------------------------------------------------------------------------
ctkVTKConnection::ctkVTKConnection(QObject* _parent):
  Superclass(_parent)
  , d_ptr(new ctkVTKConnectionPrivate(*this))
{
}

// --------------------------------------------------------------------------
ctkVTKConnection::ctkVTKConnection(ctkVTKConnectionPrivate* pimpl, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(pimpl)
{
}

//-----------------------------------------------------------------------------
ctkVTKConnection::~ctkVTKConnection()
{
  Q_D(ctkVTKConnection);
  d->disconnectVTKObject();
}

//-----------------------------------------------------------------------------
QString ctkVTKConnection::id()const
{
  Q_D(const ctkVTKConnection);
  return d->Id;
}

//-----------------------------------------------------------------------------
QObject* ctkVTKConnection::object()const
{
  Q_D(const ctkVTKConnection);
  return const_cast<QObject*>(d->QtObject);
}

//-----------------------------------------------------------------------------
vtkObject* ctkVTKConnection::vtkobject() const
{
  Q_D(const ctkVTKConnection);
  return const_cast<vtkObject*>(d->VTKObject.GetPointer());
}

//-----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, const ctkVTKConnection& connection)
{
  const ctkVTKConnectionPrivate* d = connection.d_func();
  #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
  dbg.nospace() << "ctkVTKConnection:" << &connection << Qt::endl
                << "Id:" << d->Id << Qt::endl
                << " VTKObject:" << (d->VTKObject ? d->VTKObject->GetClassName() : "<null>")
                << "(" << d->VTKObject << ")" << Qt::endl
                << " QtObject:" << d->QtObject << Qt::endl
                << " VTKEvent:" << d->VTKEvent << Qt::endl
                << " QtSlot:" << d->QtSlot << Qt::endl
                << " SlotType:" << d->SlotType << Qt::endl
                << " Priority:" << d->Priority << Qt::endl
                << " Connected:" << d->Connected << Qt::endl
                << " Blocked:" << d->Blocked;
  #else
  dbg.nospace() << "ctkVTKConnection:" << &connection << endl
                << "Id:" << d->Id << endl
                << " VTKObject:" << (d->VTKObject ? d->VTKObject->GetClassName() : "<null>")
                << "(" << d->VTKObject << ")" << endl
                << " QtObject:" << d->QtObject << endl
                << " VTKEvent:" << d->VTKEvent << endl
                << " QtSlot:" << d->QtSlot << endl
                << " SlotType:" << d->SlotType << endl
                << " Priority:" << d->Priority << endl
                << " Connected:" << d->Connected << endl
                << " Blocked:" << d->Blocked;
  #endif
  return dbg.space();
}

//-----------------------------------------------------------------------------
QString ctkVTKConnection::shortDescription()
{
  Q_D(ctkVTKConnection);
  
  return ctkVTKConnection::shortDescription(d->VTKObject, d->VTKEvent, d->QtObject, d->QtSlot.toLatin1());
}

//-----------------------------------------------------------------------------
QString ctkVTKConnection::shortDescription(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot)
{
  QString ret;
  QTextStream ts( &ret );
  ts << (vtk_obj ? vtk_obj->GetClassName() : "NULL") << " "
     << vtk_event << " " << qt_obj << " " << (qt_slot ? qt_slot : "");
  return ret;
}

//-----------------------------------------------------------------------------
bool ctkVTKConnection::isValid(vtkObject* vtk_obj, unsigned long vtk_event,
                               const QObject* qt_obj, const char* qt_slot)
{
  Q_UNUSED(vtk_event);
  if (!vtk_obj)
    {
    return false;
    }
  if (!qt_obj)
    {
    return false;
    }
  if (qt_slot == 0 || strlen(qt_slot) == 0)
    {
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
void ctkVTKConnection::setup(vtkObject* vtk_obj, unsigned long vtk_event,
                             const QObject* qt_obj, const char* qt_slot, 
                             float priority,
                             Qt::ConnectionType connectionType)
{
  Q_D(ctkVTKConnection);
  
  if (!ctkVTKConnection::isValid(vtk_obj, vtk_event, qt_obj, qt_slot)) 
    { 
    return; 
    }

  d->VTKObject = vtk_obj;
  d->QtObject = qt_obj;
  d->VTKEvent = vtk_event;
  d->QtSlot = qt_slot;
  d->Priority = priority;
  d->ConnectionType = connectionType;

  if (d->QtSlot.contains(QRegExp(QString("\\( ?vtkObject ?\\* ?, ?vtkObject ?\\* ?\\)"))))
    {
    d->SlotType = ctkVTKConnectionPrivate::ARG_VTKOBJECT_AND_VTKOBJECT;
    }
  else
    {
    d->SlotType = ctkVTKConnectionPrivate::ARG_VTKOBJECT_VOID_ULONG_VOID;
    }
  d->connect();
}

//-----------------------------------------------------------------------------
void ctkVTKConnection::setBlocked(bool block)
{
  Q_D(ctkVTKConnection);
  d->Blocked = block;
}

//-----------------------------------------------------------------------------
bool ctkVTKConnection::isBlocked()const
{
  Q_D(const ctkVTKConnection);
  return d->Blocked;
}

//-----------------------------------------------------------------------------
bool ctkVTKConnection::isEqual(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot)const
{
  Q_D(const ctkVTKConnection);
  
  if (vtk_obj && d->VTKObject != vtk_obj)
    {
    return false;
    }
  if (vtk_event != vtkCommand::NoEvent && d->VTKEvent != vtk_event)
    {
    return false;
    }
  if (qt_obj && d->QtObject != qt_obj)
    {
    return false;
    }
  if (!d->IsSameQtSlot(qt_slot))
    {
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
void ctkVTKConnectionPrivate::DoCallback(vtkObject* vtk_obj, unsigned long event,
                                 void* client_data, void* call_data)
{
  ctkVTKConnectionPrivate* conn = reinterpret_cast<ctkVTKConnectionPrivate*>(client_data);
  Q_ASSERT(conn);
  conn->execute(vtk_obj, event, client_data, call_data);
}

//-----------------------------------------------------------------------------
// callback from VTK to emit signal
void ctkVTKConnectionPrivate::execute(vtkObject* vtk_obj, unsigned long vtk_event,
  void* client_data, void* call_data)
{
  Q_Q(ctkVTKConnection);
  
  Q_ASSERT(this->Connected);
  if (this->Blocked) 
    { 
    return; 
    }

  QPointer<ctkVTKConnection> connection(q);
  if(!this->ObserveDeletion ||
     vtk_event != vtkCommand::DeleteEvent ||
     this->VTKEvent == vtkCommand::DeleteEvent)
    {
    vtkObject* callDataAsVtkObject = 0;
    switch (this->SlotType)
      {
      case ctkVTKConnectionPrivate::ARG_VTKOBJECT_AND_VTKOBJECT:
        if (this->VTKEvent == vtk_event)
          {
          callDataAsVtkObject = reinterpret_cast<vtkObject*>( call_data );
          emit q->emitExecute(vtk_obj, callDataAsVtkObject);
          }
        break;
      case ctkVTKConnectionPrivate::ARG_VTKOBJECT_VOID_ULONG_VOID:
        emit q->emitExecute(vtk_obj, call_data, vtk_event, client_data);
        break;
      default:
        // Should never reach
        qCritical() << "Unknown SlotType:" << this->SlotType;
        return;
        break;
      }
    }

  if (!connection.isNull() &&
      vtk_event == vtkCommand::DeleteEvent)
    {
    q->vtkObjectDeleted();
    }
}

//-----------------------------------------------------------------------------
void ctkVTKConnection::observeDeletion(bool enable)
{
  Q_D(ctkVTKConnection);
  d->ObserveDeletion = enable;
}

//-----------------------------------------------------------------------------
bool ctkVTKConnection::deletionObserved()const
{
  Q_D(const ctkVTKConnection);
  return d->ObserveDeletion;
}

//-----------------------------------------------------------------------------
void ctkVTKConnection::disconnect()
{
  Q_D(ctkVTKConnection);
  d->disconnectVTKObject();
}

//-----------------------------------------------------------------------------
void ctkVTKConnection::vtkObjectDeleted()
{
  Q_D(ctkVTKConnection);
  d->disconnectSlots();
}

//-----------------------------------------------------------------------------
void ctkVTKConnection::qobjectDeleted()
{
  Q_D(ctkVTKConnection);
  d->QtObject = 0;
  d->disconnectVTKObject();
}

//-----------------------------------------------------------------------------
void ctkVTKConnection::addObserver(vtkObject* caller, unsigned long vtk_event,
  vtkCallbackCommand* callback, float priority)
{
  caller->AddObserver(vtk_event, callback, priority);
}

//-----------------------------------------------------------------------------
void ctkVTKConnection::removeObserver(vtkObject* caller, unsigned long vtk_event, vtkCallbackCommand* callback)
{
  caller->RemoveObservers(vtk_event, callback);
}
