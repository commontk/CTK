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

/// Qt includes
#include <QObject>
#include <QList>
#include <QString>

/// CTK includes
#include <ctkSingleton.h>
#include "ctkVisualizationVTKCoreExport.h"
class ctkVTKConnection;
class ctkVTKObjectEventsObserverPrivate;

/// VTK includes
#include <vtkCommand.h>
class vtkObject;

//-----------------------------------------------------------------------------
/// \ingroup Visualization_VTK_Core
/// \brief Connect vtkObject events with QObject slots.
/// Helper class that provides utility methods for connecting vtkObjects with
/// QObjects.
class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKObjectEventsObserver : public QObject
{
Q_OBJECT
  /// This property controls wether or not you can replace a
  /// connection by a connection from an object of a different VTK class tha
  /// the first.
  /// For example, if strictTypeCheck is on, the following will generate an error
  /// \code
  /// vtkActor* actor = vtkActor::New();
  /// objectEventsObserver->addConnection(actor, vtkCommand::ModifiedEvent, ...);
  /// vtkMapper* mapper = vtkMapper::New();
  /// objectEventsObserver->addConnection(actor, mapper, vtkCommand::ModifiedEvent, ...);
  /// \endcode
  /// False by default.
  /// \sa strictTypeCheck(), setStrictTypeCheck(),
  /// addConnection()
  Q_PROPERTY(bool strictTypeCheck READ strictTypeCheck WRITE setStrictTypeCheck)
public:
  typedef QObject Superclass;
  explicit ctkVTKObjectEventsObserver(QObject* parent = 0);
  virtual ~ctkVTKObjectEventsObserver();

  virtual void printAdditionalInfo();

  /// Return the strictTypeCheck value.
  /// \sa strictTypeCheck, setStrictTypeCheck()
  bool strictTypeCheck()const;
  /// Set the strictTypeCheck value.
  /// \sa strictTypeCheck, strictTypeCheck()
  /// \note By default, strict type checking is disabled.
  void setStrictTypeCheck(bool check);

  ///
  /// Add a connection between a \c vtkObject and a \c QObject.
  /// When the \a vtk_obj \c vtkObject invokes the \a vtk_event event,
  /// the slot \a qt_slot of the \c QObject \a qt_obj is called
  /// \a priority is used for the \c vtkObject observation and
  /// \a connectionType controls when the slot is called.
  /// The slot must have the signature
  /// \code (vtkObject*,void*,unsigned long,void*) \endcode where the
  /// parameters are respectively \code (sender,callData,eventId,clientData)
  /// \endcode.
  /// Or with the signature
  /// \code (vtkObject*,vtkObject*) \endcode where the first \c vtkObject* is
  /// the sender and the second \c vtkObject* is the callData casted into a
  /// \c vtkObject.
  /// The slot can contain less parameters, but must be in the same order.
  /// An ID allowing to uniquely identify the connection is
  /// returned. It is a no-op if vtk_obj is NULL, the parameters are invalid or
  /// if the connection already exist.
  /// \sa addConnection(vtkObject* old_vtk_obj,vtkObject* vtk_obj,
  /// unsigned long vtk_event, const QObject* qt_obj, const char* qt_slot,
  /// float priority = 0.0, Qt::ConnectionType connectionType =
  /// Qt::AutoConnection), reconnection(), removeConnection(),
  /// removeAllConnections(), containsConnection()
  QString addConnection(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot, float priority = 0.0,
    Qt::ConnectionType connectionType = Qt::AutoConnection);

  ///
  /// Remove any connection to \a old_vtk_obj and add a connection
  /// to \a vtk_obj (with same event, object, slot, priority and type).
  /// \sa addConnection(), reconnection(),
  /// removeConnection(), removeAllConnections(), containsConnection()
  QString addConnection(vtkObject* old_vtk_obj,
    vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot, float priority = 0.0,
    Qt::ConnectionType connectionType = Qt::AutoConnection);

  ///
  /// Utility function that remove a connection on old_vtk_obj and add a connection
  /// to vtk_obj (with same event, object, slot, priority)
  /// \sa addConnection(), removeConnection(), removeAllConnections(),
  /// containsConnection()
  QString reconnection(vtkObject* vtk_obj, unsigned long vtk_event,
                       const QObject* qt_obj, const char* qt_slot,
                       float priority = 0.0,
                       Qt::ConnectionType connectionType = Qt::AutoConnection);

  ///
  /// Remove all the connections matching vtkobj, event, qtobj and slot using
  /// wildcards or not.
  /// Returns the number of connection removed.
  /// \sa addConnection(), reconnection(), removeAllConnections(),
  /// containsConnection()
  int removeConnection(vtkObject* vtk_obj, unsigned long vtk_event = vtkCommand::NoEvent,
                       const QObject* qt_obj = 0, const char* qt_slot = 0);

  ///
  /// Remove all the connections
  /// \sa removeConnection()
  int removeAllConnections();

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

  /// Return true if there is at least 1 connection matching the parameters,
  /// false otherwise.
  /// \sa addConnection(), reconnection(), removeConnection(),
  /// removeAllConnections()
  bool containsConnection(vtkObject* vtk_obj, unsigned long vtk_event = vtkCommand::NoEvent,
                          const QObject* qt_obj =0, const char* qt_slot =0)const;

protected:
  QScopedPointer<ctkVTKObjectEventsObserverPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKObjectEventsObserver);
  Q_DISABLE_COPY(ctkVTKObjectEventsObserver);

};

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
  ctkVTKConnectionFactory();
  virtual ~ctkVTKConnectionFactory();
  CTK_SINGLETON_DECLARE(ctkVTKConnectionFactory)
};
CTK_SINGLETON_DECLARE_INITIALIZER(CTK_VISUALIZATION_VTK_CORE_EXPORT, ctkVTKConnectionFactory)

#endif
