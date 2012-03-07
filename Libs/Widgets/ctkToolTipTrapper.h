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
/*=========================================================================

   Program: ParaView
   Module:  pqToolTipTrapper.h

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See http://www.paraview.org/paraview/project/license.html for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#ifndef __ctkToolTipTrapper_h
#define __ctkToolTipTrapper_h

// Qt includes
#include <QObject>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkToolTipTrapperPrivate;

/// \ingroup Widgets
/// Filters tooltips, to prevent tooltips from appearing or to word wrap
/// tooltips.
/// If toolTipsTrapped or toolTipsWordWrapped is true, installs an event filter to
/// trap or wrap tooltips.
/// If toolTipsTrapped and toolTipsWordWrapped are false, does not install the event
/// filter.
/// Tooltips are trapped and not word wrapped by default.
class CTK_WIDGETS_EXPORT ctkToolTipTrapper : public QObject
{
  Q_OBJECT
  Q_PROPERTY( bool toolTipsTrapped READ toolTipsTrapped WRITE setToolTipsTrapped)
  Q_PROPERTY( bool toolTipsWordWrapped READ toolTipsWordWrapped WRITE setToolTipsWordWrapped)
public:
  typedef QObject Superclass;
  /// Constructs a ToolTip trapper which is a child of objectParent
  explicit ctkToolTipTrapper(QObject* objectParent = 0);
  explicit ctkToolTipTrapper(bool toolTipsTrapped,
                             bool toolTipsWordWordWrapped,
                             QObject* objectParent = 0);
  virtual ~ctkToolTipTrapper();

  /// Returns true if the tooltips are trapped to prevent them from appearing.
  bool toolTipsTrapped()const;

  /// Returns true if the tooltips are word wrapped.
  bool toolTipsWordWrapped()const;

  /// Automatically called when the tooltips are trapped or word wrapped.
  /// You shouldn't have to call it manually.
  bool eventFilter(QObject* watched, QEvent* event);

public Q_SLOTS:
  /// If true, installs the eventFilter on the application if it isn't already
  /// installed.  Otherwise, removes the eventFilter if tooltips are neither
  /// trapped nor word wrapped.
  void setToolTipsTrapped(bool toolTipsTrapped);
  void setToolTipsWordWrapped(bool toolTipsWordWrapped);

protected:
  QScopedPointer<ctkToolTipTrapperPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkToolTipTrapper);
  Q_DISABLE_COPY(ctkToolTipTrapper);
};

#endif
