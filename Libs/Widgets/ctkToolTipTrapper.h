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
/*=========================================================================

   Program: ParaView
   Module:    pqToolTipTrapper.h

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
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

/// To prevent tooltips from appearing, create an instance of this object.
class CTK_WIDGETS_EXPORT ctkToolTipTrapper : public QObject
{
  Q_OBJECT
  Q_PROPERTY( bool enabled READ isEnabled WRITE setEnabled)
public:
  typedef QObject Superclass;
  /// Constructs a ToolTip trapper which is a child of objectParent
  /// The trapper is enabled by default 
  explicit ctkToolTipTrapper(QObject* objectParent = 0);
  /// Constructs a ToolTip trapper which is a child of objectParent
  /// If enable is false, the trapper doesn't install the event filter
  explicit ctkToolTipTrapper(bool enable, QObject* objectParent = 0);
  virtual ~ctkToolTipTrapper();

  /// Returns true if the eventFilter is installed and ToolTip events are
  /// filtered
  bool isEnabled()const;

  /// Automatically called when the tooltip is enabled. It prevents the
  /// tooltips events from being processed. You shouldn't have to call
  /// it manually.
  bool eventFilter(QObject* watched, QEvent* event);

public slots:
  /// If true, it installs the eventFilter on the application. Otherwise
  /// it removes it.
  void setEnabled(bool enable);

protected:
  QScopedPointer<ctkToolTipTrapperPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkToolTipTrapper);
  Q_DISABLE_COPY(ctkToolTipTrapper);
};

#endif
