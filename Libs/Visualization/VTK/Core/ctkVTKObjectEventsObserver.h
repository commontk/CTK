/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkVTKObjectEventsObserver_h
#define __ctkVTKObjectEventsObserver_h

/// CTK includes
#include <ctkPimpl.h>

/// Qt includes
#include <QObject>
#include <QList>
#include <QString>

/// VTK includes
#include <vtkCommand.h>

#include "ctkVisualizationVTKCoreExport.h"

class ctkVTKConnection;
class vtkObject;
class ctkVTKObjectEventsObserverPrivate;

class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKObjectEventsObserver : public QObject
{
Q_OBJECT

public:
  typedef QObject Superclass;
  explicit ctkVTKObjectEventsObserver(QObject* parent = 0);
  virtual ~ctkVTKObjectEventsObserver();

  virtual void printAdditionalInfo();

  ///
  /// Add a connection, an Id allowing to uniquely identify the connection is also returned
  /// Warning the slot must have its signature order:
  /// vtkObject*, vtkObject* : sender, callData
  /// or
  /// vtkObject*, void*, unsigned long, void*: sender, callData, eventId, clientData
  /// Of course the slot can contain less parameters, but always the same order
  /// though.
  QString addConnection(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot, float priority = 0.0);

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
    const QObject* qt_obj, const char* qt_slot, float priority = 0.0);

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
                       float priority = 0.0);

  ///
  /// Remove a connection
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
