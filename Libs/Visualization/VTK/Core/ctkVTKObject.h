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

#ifndef __ctkVTKObject_h
#define __ctkVTKObject_h

/// ctkVTK includes
#include "ctkVTKObjectEventsObserver.h"

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_ADD_CONNECTION_METHOD                               \
QString qvtkConnect(vtkObject* vtk_obj, unsigned long vtk_event,        \
                    const QObject* qt_obj, const char* qt_slot,         \
                    float priority = 0.0)                               \
{                                                                       \
  return MyQVTK.addConnection(vtk_obj, vtk_event,                       \
                              qt_obj, qt_slot, priority);               \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_RECONNECT_METHOD                                    \
QString qvtkReconnect(vtkObject* old_vtk_obj, vtkObject* vtk_obj,       \
                      unsigned long vtk_event, const QObject* qt_obj,   \
                      const char* qt_slot, float priority = 0.0)        \
{                                                                       \
  return MyQVTK.addConnection(old_vtk_obj, vtk_obj, vtk_event,          \
                              qt_obj, qt_slot, priority);               \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_RECONNECT_METHOD_2                                  \
QString qvtkReconnect(vtkObject* vtk_obj,                               \
                      unsigned long vtk_event, const QObject* qt_obj,   \
                      const char* qt_slot, float priority = 0.0)        \
{                                                                       \
  return MyQVTK.reconnection(vtk_obj, vtk_event,                        \
                             qt_obj, qt_slot, priority);                \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_REMOVE_CONNECTION_METHOD                            \
int qvtkDisconnect(vtkObject* vtk_obj, unsigned long vtk_event,         \
  const QObject* qt_obj, const char* qt_slot)                           \
{                                                                       \
  return MyQVTK.removeConnection(vtk_obj, vtk_event,                    \
    qt_obj, qt_slot);                                                   \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_REMOVEALL_CONNECTION_METHOD                         \
int qvtkDisconnectAll()                                                 \
{                                                                       \
  return MyQVTK.removeAllConnections();                                 \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_IS_CONNECTION_METHOD                                \
bool qvtkIsConnected(vtkObject* vtk_obj = 0,                            \
                     unsigned long vtk_event = vtkCommand::NoEvent,     \
                     const QObject* qt_obj = 0,                         \
                     const char* qt_slot = 0)                           \
{                                                                       \
  return MyQVTK.containsConnection(vtk_obj, vtk_event,                  \
                                   qt_obj, qt_slot);                    \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_BLOCK_CONNECTION_METHOD                              \
void qvtkBlock(vtkObject* vtk_obj, unsigned long vtk_event,              \
  const QObject* qt_obj)                                                 \
{                                                                        \
  MyQVTK.blockConnection(true, vtk_obj, vtk_event, qt_obj);              \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_BLOCK_CONNECTION_METHOD2                             \
void qvtkBlock(const QString& id, bool blocked)                          \
{                                                                        \
  MyQVTK.blockConnection(id, blocked);                                   \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_BLOCKALL_CONNECTION_METHOD                          \
bool qvtkBlockAll()                                                     \
{                                                                       \
  return MyQVTK.blockAllConnections(true);                              \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_BLOCKALL_CONNECTION_METHOD2                         \
bool qvtkBlockAll(bool block)                                           \
{                                                                       \
  return MyQVTK.blockAllConnections(block);                             \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_UNBLOCK_CONNECTION_METHOD                           \
void qvtkUnblock(vtkObject* vtk_obj, unsigned long vtk_event,           \
  const QObject* qt_obj)                                                \
{                                                                       \
  MyQVTK.blockConnection(false, vtk_obj, vtk_event, qt_obj);            \
}

//-----------------------------------------------------------------------------
#define QVTK_OBJECT_UNBLOCKALL_CONNECTION_METHOD                        \
bool qvtkUnblockAll()                                                   \
{                                                                       \
  return MyQVTK.blockAllConnections(false);                             \
}

//-----------------------------------------------------------------------------
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

#endif
