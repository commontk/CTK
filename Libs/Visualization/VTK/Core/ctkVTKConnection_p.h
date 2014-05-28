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

#ifndef __ctkVTKConnection_p_h
#define __ctkVTKConnection_p_h

// Qt includes
#include <QString>
class QObject;

// CTK includes
#include "ctkVTKConnection.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
class vtkObject;
class vtkCallbackCommand;

#include "ctkVisualizationVTKCoreExport.h"

//-----------------------------------------------------------------------------
/// \ingroup Visualization_VTK_Core
class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKConnectionPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKConnection);
protected:
  ctkVTKConnection* const q_ptr;
public:
  enum
    {
    ARG_UNKNOWN = 0,
    ARG_VTKOBJECT_AND_VTKOBJECT,
    ARG_VTKOBJECT_VOID_ULONG_VOID
    };

  typedef ctkVTKConnectionPrivate Self;
  ctkVTKConnectionPrivate(ctkVTKConnection& object);
  ~ctkVTKConnectionPrivate();

  void connect();
  void disconnectSlots();
  void disconnectVTKObject();

  bool IsSameQtSlot(const char* qt_slot)const;

  /// 
  /// VTK Callback
  static void DoCallback(vtkObject* vtk_obj, unsigned long event,
                         void* client_data, void* call_data);

  /// 
  /// Called by 'DoCallback' to emit signal
  void execute(vtkObject* vtk_obj, unsigned long vtk_event, void* client_data, void* call_data);

  vtkSmartPointer<vtkCallbackCommand> Callback;
  vtkWeakPointer<vtkObject>           VTKObject;
  const QObject*                      QtObject;
  unsigned long                       VTKEvent;
  QString                             QtSlot;
  float                               Priority;
  Qt::ConnectionType                  ConnectionType;
  int                                 SlotType;
  bool                                Connected;
  bool                                Blocked;
  QString                             Id;
  bool                                ObserveDeletion;
};

#endif
