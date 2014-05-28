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

#ifndef __ctkVTKConnection_h
#define __ctkVTKConnection_h

// Qt includes
#include <QObject>
#include <QVector>

// CTK includes
#include <ctkPimpl.h>

#include "ctkVisualizationVTKCoreExport.h"

class vtkObject;
class ctkVTKConnectionPrivate;
class vtkCallbackCommand;

/// \ingroup Visualization_VTK_Core
/// Warning the slot must have its signature order:
/// vtkObject*, vtkObject* : sender, callData
/// or
/// vtkObject*, void*, unsigned long, void*: sender, callData, eventId, clientData
/// Of course the slot can contain less parameters, but always the same order
/// though.
class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKConnection : public QObject
{
Q_OBJECT

public:
  typedef QObject Superclass;
  explicit ctkVTKConnection(QObject* parent);
  virtual ~ctkVTKConnection();

  ///
  QString shortDescription();
  static QString shortDescription(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot = 0);

  /// 
  /// Warning the slot must have its signature order:
  /// vtkObject*, vtkObject* : sender, callData
  /// or
  /// vtkObject*, void*, unsigned long, void*: sender, callData, eventId, clientData
  /// Of course the slot can contain less parameters, but always the same order
  /// though.
  /// \sa object(), vtkobject()
  void setup(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot, float priority = 0.f,
    Qt::ConnectionType connectionType = Qt::AutoConnection);

  /// 
  /// Check the validity of the parameters. Parameters must be valid to add 
  /// a connection
  static bool isValid(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, const char* qt_slot);

  /// 
  /// Temporarilly block any signals/slots. If the event is fired, the slot
  /// won't be called. You can restore the connection by calling SetBlocked
  /// with block = false.
  void setBlocked(bool block);
  bool isBlocked()const;

  /// 
  bool isEqual(vtkObject* vtk_obj, unsigned long vtk_event,
               const QObject* qt_obj, const char* qt_slot)const;

  /// 
  /// Return a string uniquely identifying the connection within the current process
  QString  id()const;

  ///
  /// Return the QObject set using setup() method.
  QObject* object()const;

  /// Return the vtkObject set using setup() method.
  vtkObject* vtkobject() const;

  /// false by default, it is slower to observe vtk object deletion
  void observeDeletion(bool enable);
  bool deletionObserved()const;
  
Q_SIGNALS:
  /// 
  /// The qt signal emited by the VTK Callback
  /// The signal corresponding to the slot will be emited
  void emitExecute(vtkObject* caller, vtkObject* call_data);

  /// Note: even if the signal has a signature with 4 args, you can
  /// connect it to a slot with less arguments as long as the types of the 
  /// argument are matching:
  /// connect(obj1,SIGNAL(signalFunc(A,B,C,D)),obj2,SLOT(slotFunc(A)));
  void emitExecute(vtkObject* caller, void* call_data, unsigned long vtk_event, void* client_data);

protected Q_SLOTS:
  void vtkObjectDeleted();
  void qobjectDeleted();

protected:
  QScopedPointer<ctkVTKConnectionPrivate> d_ptr;
  ctkVTKConnection(ctkVTKConnectionPrivate* pimpl, QObject* _parent);

  void disconnect();
  virtual void addObserver(vtkObject* caller, unsigned long vtk_event, vtkCallbackCommand* callback, float priority=0.0f);
  virtual void removeObserver(vtkObject* caller, unsigned long vtk_event, vtkCallbackCommand* callback);

private:
  Q_DECLARE_PRIVATE(ctkVTKConnection);
  Q_DISABLE_COPY(ctkVTKConnection);
  friend QDebug operator<<(QDebug dbg, const ctkVTKConnection& connection);
};

/// \ingroup Visualization_VTK_Core
QDebug operator<<(QDebug dbg, const ctkVTKConnection& connection);

#endif
