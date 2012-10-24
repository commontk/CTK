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

#ifndef __ctkVTKObject_h
#define __ctkVTKObject_h

// ctkVTK includes
#include "ctkVTKObjectEventsObserver.h"

/// \ingroup Visualization_VTK_Core
/// @{

//-----------------------------------------------------------------------------
/// Define qvtkConnect()
/// \sa qvtkConnect(), QVTK_OBJECT
#define QVTK_OBJECT_ADD_CONNECTION_METHOD                               \
/** \brief Connect a vtkObject event with a QObject slot.*/             \
/** Utility function that calls addConnection() on ctkVTKObjectEventsObserver.*/\
/** \sa ctkVTKObjectEventsObserver::addConnection(),*/                  \
/** qvtkReconnect(), qvtkDisconnect(), qvtkDisconnectAll(),*/           \
/** qvtkIsConnected(), QVTK_OBJECT_ADD_CONNECTION_METHOD*/              \
QString qvtkConnect(vtkObject* vtk_obj, unsigned long vtk_event,        \
                    const QObject* qt_obj, const char* qt_slot,         \
                    float priority = 0.0,                               \
                    Qt::ConnectionType connectionType = Qt::AutoConnection)\
{                                                                       \
  return MyQVTK.addConnection(vtk_obj, vtk_event,                       \
                              qt_obj, qt_slot, priority, connectionType);\
}

//-----------------------------------------------------------------------------
/// Define qvtkReconnect()
/// \sa qvtkReconnect(), QVTK_OBJECT
#define QVTK_OBJECT_RECONNECT_METHOD                                    \
/** \brief Disconnect and reconnect a QObject slot with a new vtkObject event.*/ \
/** Internally calls ctkVTKObjectEventsObserver::addConnection().*/     \
/** The existing connection between \a old_vtk_obj and \a qt_obj*/      \
/** for the event \a vtk_event and the slot \a qt_slot is disconnected.*/\
/** And the \a vtk_obj is then connected using the same parameters.*/   \
/** \sa ctkVTKObjectEventsObserver::addConnection(),*/                  \
/** qvtkReconnect(vtkObject*,unsigned long,const QObject*,const char*,float,*/\
/** Qt::ConnectionType), qvtkConnect(), qvtkDisconnect(), */            \
/** qvtkIsConnected(), QVTK_OBJECT_RECONNECT_METHOD*/                   \
QString qvtkReconnect(vtkObject* old_vtk_obj, vtkObject* vtk_obj,       \
                      unsigned long vtk_event, const QObject* qt_obj,   \
                      const char* qt_slot, float priority = 0.0,        \
                      Qt::ConnectionType connectionType = Qt::AutoConnection)\
{                                                                       \
  return MyQVTK.addConnection(old_vtk_obj, vtk_obj, vtk_event,          \
                              qt_obj, qt_slot, priority, connectionType);\
}

//-----------------------------------------------------------------------------
/// Define qvtkReconnect()
/// \sa qvtkReconnect(), QVTK_OBJECT
#define QVTK_OBJECT_RECONNECT_METHOD_2                                  \
/** \brief Disconnect and reconnect a new vtkObject event with a QObject slot.*/\
/** Utility function that calls reconnection() on ctkVTKObjectEventsObserver.*/\
/** Any existing connection between a vtkObject and \a qt_obj*/         \
/** for the event \a vtk_event and the slot \a qt_slot is disconnected.*/\
/** And the \a vtk_obj is then connected using the same parameters.*/   \
/** \sa ctkVTKObjectEventsObserver::reconnection(),*/                   \
/** qvtkReconnect(vtkObject*,vtkObject*,unsigned long,const QObject*,*/ \
/** const char*,float,Qt::ConnectionType), qvtkConnect(),*/             \
/** qvtkDisconnect(), qvtkIsConnected(), QVTK_OBJECT_RECONNECT_METHOD_2*/\
QString qvtkReconnect(vtkObject* vtk_obj,                               \
                      unsigned long vtk_event, const QObject* qt_obj,   \
                      const char* qt_slot, float priority = 0.0,        \
                      Qt::ConnectionType connectionType = Qt::AutoConnection)\
{                                                                       \
  return MyQVTK.reconnection(vtk_obj, vtk_event,                        \
                             qt_obj, qt_slot, priority, connectionType);\
}

//-----------------------------------------------------------------------------
/// Define qvtkDisconnect()
/// \sa qvtkDisconnect(), QVTK_OBJECT
#define QVTK_OBJECT_REMOVE_CONNECTION_METHOD                            \
/** \brief Disconnect a vtkObject event with a QObject slot.*/          \
/** Utility function that calls removeConnection() on*/                 \
/** ctkVTKObjectEventsObserver.*/                                       \
/** Remove any existing connection between \a vtk_obj and \a qt_obj that*/\
/** match the \a vtk_event event and \a qt_slot slot.*/                 \
/** 0 (or vtkCommand::NoEvent for \a vtk_event) for any parameter is considered*/\
/** as a wildcard, it makes the parameter criteria to be ignored*/      \
/** (matches all connections).*/                                        \
/** \sa ctkVTKObjectEventsObserver::removeConnection(),*/               \
/** qvtkConnect(), qvtkReconnect(), qvtkDisconnectAll(),*/              \
/** qvtkIsConnected(), QVTK_OBJECT_REMOVE_CONNECTION_METHOD*/           \
int qvtkDisconnect(vtkObject* vtk_obj, unsigned long vtk_event,         \
  const QObject* qt_obj, const char* qt_slot)                           \
{                                                                       \
  return MyQVTK.removeConnection(vtk_obj, vtk_event,                    \
    qt_obj, qt_slot);                                                   \
}

//-----------------------------------------------------------------------------
/// Define qvtkDisconnectAll()
/// \sa qvtkDisconnectAll(), QVTK_OBJECT
#define QVTK_OBJECT_REMOVEALL_CONNECTION_METHOD                         \
/** \brief Disconnect all created connections between vtkObjects and QObjects. */\
/** Utility function that calls removeAllConnections() on */            \
/** ctkVTKObjectEventsObserver. */                                      \
/** \sa ctkVTKObjectEventsObserver::removeAllConnections(),*/           \
/** qvtkConnect(), qvtkReconnect(), qvtkDisconnect() */                 \
/** qvtkIsConnected(), QVTK_OBJECT_REMOVEALL_CONNECTION_METHOD */       \
int qvtkDisconnectAll()                                                 \
{                                                                       \
  return MyQVTK.removeAllConnections();                                 \
}

//-----------------------------------------------------------------------------
/// Define qvtkIsConnected()
/// \sa qvtkIsConnected(), QVTK_OBJECT
#define QVTK_OBJECT_IS_CONNECTION_METHOD                                \
/** \brief Check if a connection exists.*/                              \
/** Utility function that calls and returns containsConnection() from*/ \
/** ctkVTKObjectEventsObserver.*/                                       \
/** \sa ctkVTKObjectEventsObserver::containsConnection(),*/             \
/** qvtkConnect(), qvtkReconnect(), qvtkDisconnect()*/                  \
/** qvtkDisconnectAll(), QVTK_OBJECT_IS_CONNECTION_METHOD*/             \
bool qvtkIsConnected(vtkObject* vtk_obj = 0,                            \
                     unsigned long vtk_event = vtkCommand::NoEvent,     \
                     const QObject* qt_obj = 0,                         \
                     const char* qt_slot = 0)                           \
{                                                                       \
  return MyQVTK.containsConnection(vtk_obj, vtk_event,                  \
                                   qt_obj, qt_slot);                    \
}

//-----------------------------------------------------------------------------
/// Define qvtkBlock(vtkObject* vtk_obj, unsigned long vtk_event,
/// const QObject* qt_obj)
/// \sa qvtkBlock(), QVTK_OBJECT
#define QVTK_OBJECT_BLOCK_CONNECTION_METHOD                              \
void qvtkBlock(vtkObject* vtk_obj, unsigned long vtk_event,              \
  const QObject* qt_obj)                                                 \
{                                                                        \
  MyQVTK.blockConnection(true, vtk_obj, vtk_event, qt_obj);              \
}

//-----------------------------------------------------------------------------
/// Define qvtkBlock(const QString& id, bool blocked)
/// \sa qvtkBlock(const QString& id, bool blocked), QVTK_OBJECT
#define QVTK_OBJECT_BLOCK_CONNECTION_METHOD2                             \
void qvtkBlock(const QString& id, bool blocked)                          \
{                                                                        \
  MyQVTK.blockConnection(id, blocked);                                   \
}

//-----------------------------------------------------------------------------
/// Define qvtkBlockAll()
/// \sa qvtkBlockAll(), QVTK_OBJECT
#define QVTK_OBJECT_BLOCKALL_CONNECTION_METHOD                          \
bool qvtkBlockAll()                                                     \
{                                                                       \
  return MyQVTK.blockAllConnections(true);                              \
}

//-----------------------------------------------------------------------------
/// Define qvtkBlockAll()
/// \sa qvtkBlockAll(), QVTK_OBJECT
#define QVTK_OBJECT_BLOCKALL_CONNECTION_METHOD2                         \
bool qvtkBlockAll(bool block)                                           \
{                                                                       \
  return MyQVTK.blockAllConnections(block);                             \
}

//-----------------------------------------------------------------------------
/// Define qvtkUnblock()
/// \sa qvtkUnblock(), QVTK_OBJECT
#define QVTK_OBJECT_UNBLOCK_CONNECTION_METHOD                           \
void qvtkUnblock(vtkObject* vtk_obj, unsigned long vtk_event,           \
  const QObject* qt_obj)                                                \
{                                                                       \
  MyQVTK.blockConnection(false, vtk_obj, vtk_event, qt_obj);            \
}

//-----------------------------------------------------------------------------
/// Define qvtkUnblockAll()
/// \sa qvtkUnblockAll(), QVTK_OBJECT
#define QVTK_OBJECT_UNBLOCKALL_CONNECTION_METHOD                        \
bool qvtkUnblockAll()                                                   \
{                                                                       \
  return MyQVTK.blockAllConnections(false);                             \
}

//-----------------------------------------------------------------------------
/// \brief Define VTK/Qt event/slot connection utility methods.
/// It is a convenient macro that declares and defines utility methods for
/// connecting vtkObjects with QObjects. It connects the vtkObject event
/// mechanism with the QObject slot mechanism.
/// Can be called in any QObject derived class declaration,typically after the
/// Q_OBJECT macro.
/// \sa ctkVTKObjectEventsObserver, qvtkConnect(),
/// qvtkReconnect(),qvtkDisconnect(), qvtkDisconnectAll(), qvtkIsConnected(),
/// QVTK_OBJECT_ADD_CONNECTION_METHOD, QVTK_OBJECT_RECONNECT_METHOD,
/// QVTK_OBJECT_RECONNECT_METHOD_2, QVTK_OBJECT_REMOVE_CONNECTION_METHOD,
/// QVTK_OBJECT_REMOVEALL_CONNECTION_METHOD, QVTK_OBJECT_IS_CONNECTION_METHOD,
/// QVTK_OBJECT_BLOCK_CONNECTION_METHOD, QVTK_OBJECT_BLOCK_CONNECTION_METHOD2,
/// QVTK_OBJECT_UNBLOCK_CONNECTION_METHOD,
/// QVTK_OBJECT_BLOCKALL_CONNECTION_METHOD,
/// QVTK_OBJECT_BLOCKALL_CONNECTION_METHOD2,
/// QVTK_OBJECT_UNBLOCKALL_CONNECTION_METHOD
#define QVTK_OBJECT                                \
protected:                                         \
  QVTK_OBJECT_ADD_CONNECTION_METHOD                \
  QVTK_OBJECT_RECONNECT_METHOD                     \
  QVTK_OBJECT_RECONNECT_METHOD_2                   \
  QVTK_OBJECT_REMOVE_CONNECTION_METHOD             \
  QVTK_OBJECT_REMOVEALL_CONNECTION_METHOD          \
  QVTK_OBJECT_IS_CONNECTION_METHOD                 \
  QVTK_OBJECT_BLOCK_CONNECTION_METHOD              \
  QVTK_OBJECT_BLOCK_CONNECTION_METHOD2             \
  QVTK_OBJECT_UNBLOCK_CONNECTION_METHOD            \
  QVTK_OBJECT_BLOCKALL_CONNECTION_METHOD           \
  QVTK_OBJECT_BLOCKALL_CONNECTION_METHOD2          \
  QVTK_OBJECT_UNBLOCKALL_CONNECTION_METHOD         \
private:                                           \
  ctkVTKObjectEventsObserver MyQVTK;

/// @}

#endif
