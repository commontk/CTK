/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// Qt includes
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QHash>
#include <QDebug>

// CTK includes
#include "ctkVTKObjectEventsObserver.h"
#include "ctkVTKConnection.h"

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class ctkVTKObjectEventsObserverPrivate: public ctkPrivate<ctkVTKObjectEventsObserver>
{
public:
  ctkVTKObjectEventsObserverPrivate();

  /// 
  /// Check if a connection has already been added
  bool containsConnection(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot);

  /// 
  /// Return a reference toward the corresponding connection or 0 if doesn't exist
  ctkVTKConnection* findConnection(const QString& id);

  /// 
  /// Return a reference toward the corresponding connection or 0 if doesn't exist
  ctkVTKConnection* findConnection(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot);

  /// 
  /// Return all the references that match the given parameters
  QList<ctkVTKConnection*> findConnections(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot);
  
  inline QList<ctkVTKConnection*> connections()const
  {
    return ctk_p()->findChildren<ctkVTKConnection*>();
  }
  
  bool                            AllEnabled;
  bool                            AllBlocked;
};

//-----------------------------------------------------------------------------
// ctkVTKObjectEventsObserver methods

//-----------------------------------------------------------------------------
ctkVTKObjectEventsObserver::ctkVTKObjectEventsObserver(QObject* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkVTKObjectEventsObserver);
  this->setProperty("QVTK_OBJECT", true);
}

//-----------------------------------------------------------------------------
void ctkVTKObjectEventsObserver::printAdditionalInfo()
{
  this->Superclass::dumpObjectInfo();
  CTK_D(ctkVTKObjectEventsObserver);
  qDebug() << "ctkVTKObjectEventsObserver:" << this << endl
           << " AllEnabled:" << d->AllEnabled << endl
           << " AllBlocked:" << d->AllBlocked << endl
           << " Parent:" << (this->parent()?this->parent()->objectName():"NULL") << endl
           << " Connection count:" << d->connections().count();

  // Loop through all connection
  foreach (ctkVTKConnection* connection, d->connections())
    {
    connection->printAdditionalInfo();
    }
}

//-----------------------------------------------------------------------------
bool ctkVTKObjectEventsObserver::allEnabled()const
{
  return ctk_d()->AllEnabled;
}

//-----------------------------------------------------------------------------
void ctkVTKObjectEventsObserver::setAllEnabled(bool enable)
{
  CTK_D(ctkVTKObjectEventsObserver);
  // FIXME: maybe a particular module has been enabled/disabled
  if (d->AllEnabled == enable) 
    { 
    return; 
    }
  // Loop through VTKQtConnections to enable/disable
  foreach(ctkVTKConnection* connection, d->connections())
    {
    connection->setEnabled(enable);
    }
  d->AllEnabled = enable;
}

//-----------------------------------------------------------------------------
QString ctkVTKObjectEventsObserver::addConnection(vtkObject* old_vtk_obj, vtkObject* vtk_obj,
  unsigned long vtk_event, const QObject* qt_obj, const char* qt_slot, float priority)
{
  QString connectionId; 
  if (old_vtk_obj)
    {
    // Check that old_object and new_object are the same type
    if (vtk_obj && !vtk_obj->IsA(old_vtk_obj->GetClassName()))
      {
      qCritical() << "Old vtkObject (type:" << old_vtk_obj->GetClassName() << ") to disconnect and "
                  << "the new VtkObject (type:" << vtk_obj->GetClassName() << ") to connect"
                  << "should have the same type.";
      return connectionId;
      }
    // Disconnect old vtkObject
    this->removeConnection(old_vtk_obj, vtk_event, qt_obj, qt_slot);
    }
  if (vtk_obj)
    {
    connectionId = this->addConnection(vtk_obj, vtk_event, qt_obj, qt_slot, priority);
    }
  return connectionId; 
}

//-----------------------------------------------------------------------------
QString ctkVTKObjectEventsObserver::reconnection(vtkObject* vtk_obj,
  unsigned long vtk_event, const QObject* qt_obj, 
  const char* qt_slot, float priority)
{
  QString connectionId; 
  this->removeConnection(0, vtk_event, qt_obj, qt_slot);
  if (vtk_obj)
    {
    connectionId = this->addConnection(vtk_obj, vtk_event, qt_obj, qt_slot, priority);
    }
  return connectionId; 
}

//-----------------------------------------------------------------------------
QString ctkVTKObjectEventsObserver::addConnection(vtkObject* vtk_obj, unsigned long vtk_event,
  const QObject* qt_obj, const char* qt_slot, float priority)
{
  CTK_D(ctkVTKObjectEventsObserver);
  if (!ctkVTKConnection::ValidateParameters(vtk_obj, vtk_event, qt_obj, qt_slot))
    {
    qDebug() << "ctkVTKObjectEventsObserver::addConnection(...) - Invalid parameters - "
             << ctkVTKConnection::shortDescription(vtk_obj, vtk_event, qt_obj, qt_slot);
    return QString();
    }

  // Check if such event is already observed
  if (d->containsConnection(vtk_obj, vtk_event, qt_obj, qt_slot))
    {
    qWarning() << "ctkVTKObjectEventsObserver::addConnection - [vtkObject:"
               << vtk_obj->GetClassName()
               << ", event:" << vtk_event << "]"
               << " is already connected with [qObject:" << qt_obj->objectName()
               << ", slot:" << qt_slot << "]";
    return QString();
    }

  // Instantiate a new connection, set its parameters and add it to the list
  ctkVTKConnection * connection = new ctkVTKConnection(this);
  connection->SetParameters(vtk_obj, vtk_event, qt_obj, qt_slot, priority);

  // If required, establish connection
  connection->setEnabled(d->AllEnabled);
  connection->setBlocked(d->AllBlocked);

  return connection->id();
}

//-----------------------------------------------------------------------------
void ctkVTKObjectEventsObserver::blockAllConnections(bool block)
{
  CTK_D(ctkVTKObjectEventsObserver);
  this->printAdditionalInfo();
  if (d->AllBlocked == block) { return; }

  foreach (ctkVTKConnection* connection, d->connections())
    {
    connection->setBlocked(block);
    }
  d->AllBlocked = block;
}

//-----------------------------------------------------------------------------
void ctkVTKObjectEventsObserver::blockConnection(const QString& id, bool blocked)
{
  CTK_D(ctkVTKObjectEventsObserver);
  ctkVTKConnection* connection = d->findConnection(id);
  if (connection == 0)
    {
    return;
    }
  connection->setBlocked(blocked);
}

//-----------------------------------------------------------------------------
int ctkVTKObjectEventsObserver::blockConnection(bool block, vtkObject* vtk_obj,
  unsigned long vtk_event, const QObject* qt_obj)
{
  CTK_D(ctkVTKObjectEventsObserver);
  if (!vtk_obj)
    {
    qDebug() << "ctkVTKObjectEventsObserver::blockConnectionRecursive"
             << "- Failed to " << (block?"block":"unblock") <<" connection"
             << "- vtkObject is NULL";
    return 0;
    }
  QList<ctkVTKConnection*> connections =
    d->findConnections(vtk_obj, vtk_event, qt_obj, 0);
  foreach (ctkVTKConnection* connection, connections)
    {
    connection->setBlocked(block);
    }
  return connections.size();
}

//-----------------------------------------------------------------------------
int ctkVTKObjectEventsObserver::removeConnection(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot)
{
  CTK_D(ctkVTKObjectEventsObserver);

  QList<ctkVTKConnection*> connections =
    d->findConnections(vtk_obj, vtk_event, qt_obj, qt_slot);
  
  foreach (ctkVTKConnection* connection, connections)
    {
    connection->deleteConnection();
    }
  return connections.count();
}

//-----------------------------------------------------------------------------
// ctkVTKObjectEventsObserverPrivate methods

//-----------------------------------------------------------------------------
ctkVTKObjectEventsObserverPrivate::ctkVTKObjectEventsObserverPrivate()
{
  this->AllEnabled = true;
  this->AllBlocked = false;
}

//-----------------------------------------------------------------------------
bool ctkVTKObjectEventsObserverPrivate::containsConnection(vtkObject* vtk_obj, unsigned long vtk_event,
  const QObject* qt_obj, const char* qt_slot)
{
  return (this->findConnection(vtk_obj, vtk_event, qt_obj, qt_slot) != 0);
}

//-----------------------------------------------------------------------------
ctkVTKConnection*
ctkVTKObjectEventsObserverPrivate::findConnection(const QString& id)
{
  foreach(ctkVTKConnection* connection, this->connections())
    {
    if (connection->id() == id)
      {
      return connection;
      }
    }
  return 0;
}

//-----------------------------------------------------------------------------
ctkVTKConnection*
ctkVTKObjectEventsObserverPrivate::findConnection(vtkObject* vtk_obj, unsigned long vtk_event,
                                         const QObject* qt_obj, const char* qt_slot)
{
  QList<ctkVTKConnection*> foundConnections =
    this->findConnections(vtk_obj, vtk_event, qt_obj, qt_slot);

  return foundConnections.size() ? foundConnections[0] : 0;
}

//-----------------------------------------------------------------------------
QList<ctkVTKConnection*>
ctkVTKObjectEventsObserverPrivate::findConnections(
  vtkObject* vtk_obj, unsigned long vtk_event,
  const QObject* qt_obj, const char* qt_slot)
{
  bool all_info = true;
  if(qt_slot == NULL || qt_obj == NULL || vtk_event == vtkCommand::NoEvent)
    {
    all_info = false;
    }

  QList<ctkVTKConnection*> foundConnections;
  // Loop through all connection
  foreach (ctkVTKConnection* connection, this->connections())
    {
    if (connection->isEqual(vtk_obj, vtk_event, qt_obj, qt_slot)) 
      {
      foundConnections.append(connection); 
      if (all_info)
        {
        break;
        }
      }
    }
  return foundConnections;
}
