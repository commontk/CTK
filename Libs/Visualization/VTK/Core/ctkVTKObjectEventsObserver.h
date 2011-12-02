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

#ifndef __ctkVTKObjectEventsObserver_h
#define __ctkVTKObjectEventsObserver_h

/// CTK includes
#include <ctkSingleton.h>

/// Qt includes
#include <QObject>
#include <QList>
#include <QString>

/// VTK includes
#include <vtkCommand.h>

#include "ctkVisualizationVTKCoreExport.h"

class ctkVTKConnection;
class vtkObject;
class ctkVTKObjectEventsObserver;
class ctkVTKObjectEventsObserverPrivate;

//-----------------------------------------------------------------------------
/// \ingroup Visualization_VTK_Core
class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKConnectionFactory
{
public:
  static ctkVTKConnectionFactory* instance();

  /// The singleton takes ownerchip of the new factory instance and will take care
  /// of cleaning the memory.
  /// \note If \a newInstance is not null, the current factory instance will be
  /// deleted. Note also that setting a null \a newInstance is a no-op.
  static void setInstance(ctkVTKConnectionFactory* newInstance);

  virtual ctkVTKConnection* createConnection(ctkVTKObjectEventsObserver*)const;
protected:
  CTK_SINGLETON_DECLARE(ctkVTKConnectionFactory)
};
CTK_SINGLETON_DECLARE_INITIALIZER(CTK_VISUALIZATION_VTK_CORE_EXPORT, ctkVTKConnectionFactory)

//-----------------------------------------------------------------------------
/// \ingroup Visualization_VTK_Core
class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKObjectEventsObserver : public QObject
{
Q_OBJECT
  Q_PROPERTY(bool strictTypeCheck READ strictTypeCheck WRITE setStrictTypeCheck)
public:
  typedef QObject Superclass;
  explicit ctkVTKObjectEventsObserver(QObject* parent = 0);
  virtual ~ctkVTKObjectEventsObserver();

  virtual void printAdditionalInfo();
  
  /// The property strictTypeCheck control wether or not you can replace a
  /// connection by a connection from an object of a different VTK class tha
  /// the first.
  /// For example, if strictTypeCheck is on, the following will generate an error
  /// <code>
  /// vtkActor* actor = vtkActor::New();
  /// objectEventsObserver->addConnection(actor, vtkCommand::ModifiedEvent, ...);
  /// vtkMapper* mapper = vtkMapper::New();
  /// objectEventsObserver->addConnection(actor, mapper, vtkCommand::ModifiedEvent, ...);
  /// </code>
  /// False by default.
  bool strictTypeCheck()const;
  void setStrictTypeCheck(bool check);

  ///
  /// Add a connection, an Id allowing to uniquely identify the connection is
  /// returned. It is a no-op if vtk_obj is NULL, the parameters are invalid or
  /// if the connection already exist.
  /// Warning the slot must have its signature order:
  /// vtkObject*, vtkObject* : sender, callData
  /// or
  /// vtkObject*, void*, unsigned long, void*: sender, callData, eventId, clientData
  /// Of course the slot can contain less parameters, but always the same order
  /// though.
  QString addConnection(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot, float priority = 0.0,
    Qt::ConnectionType connectionType = Qt::AutoConnection);

  ///
  /// Utility function that remove a connection on old_vtk_obj and add a connection
  /// to vtk_obj (same event, object, slot, priority)
  /// Warning the slot must have its signature order:
  /// vtkObject*, vtkObject* : sender, callData
  /// or
  /// vtkObject*, void*, unsigned long, void*: sender, callData, eventId, clientData
  /// Of course the slot can contain less parameters, but always the same order
  /// though.
  QString addConnection(vtkObject* old_vtk_obj, vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot, float priority = 0.0,
    Qt::ConnectionType connectionType = Qt::AutoConnection);

  ///
  /// Utility function that remove a connection on old_vtk_obj and add a connection
  /// to vtk_obj (same event, object, slot, priority)
  /// Warning the slot must have its signature order:
  /// vtkObject*, vtkObject* : sender, callData
  /// or
  /// vtkObject*, void*, unsigned long, void*: sender, callData, eventId, clientData
  /// Of course the slot can contain less parameters, but always the same order
  /// though.
  QString reconnection(vtkObject* vtk_obj, unsigned long vtk_event,
                       const QObject* qt_obj, const char* qt_slot,
                       float priority = 0.0,
                       Qt::ConnectionType connectionType = Qt::AutoConnection);

  ///
  /// Remove all the connections matching vtkobj, event, qtobj and slot using
  /// wildcards or not.
  /// Returns the number of connection removed.
  int removeConnection(vtkObject* vtk_obj, unsigned long vtk_event = vtkCommand::NoEvent,
                       const QObject* qt_obj = 0, const char* qt_slot = 0);

  ///
  /// Remove all the connections
  inline int removeAllConnections();

  ///
  /// Temporarilly block all the connection
  /// Returns the previous value of connectionsBlocked()
  bool blockAllConnections(bool block);

  ///
  /// Returns true if connections are blocked; otherwise returns false.
  /// Connections are not blocked by default.
  bool connectionsBlocked()const;

  ///
  /// Block/Unblock one or multiple connection.
  /// Return the number of connections blocked/unblocked
  int blockConnection(bool block, vtkObject* vtk_obj,
                      unsigned long vtk_event, const QObject* qt_obj);
  /// Block/Unblock a connection
  /// Return true if the connection exists and was blocked, otherwise returns
  /// false.
  bool blockConnection(const QString& id, bool blocked);
  
  /// Return true if there is at least 1 connection that match the parameter
  bool containsConnection(vtkObject* vtk_obj, unsigned long vtk_event = vtkCommand::NoEvent,
                          const QObject* qt_obj =0, const char* qt_slot =0)const;

protected:
  QScopedPointer<ctkVTKObjectEventsObserverPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKObjectEventsObserver);
  Q_DISABLE_COPY(ctkVTKObjectEventsObserver);

};

//-----------------------------------------------------------------------------
int ctkVTKObjectEventsObserver::removeAllConnections()
{
  return this->removeConnection(0, vtkCommand::NoEvent, 0, 0);
}

#endif
