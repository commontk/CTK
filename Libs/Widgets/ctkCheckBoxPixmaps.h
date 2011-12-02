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
   Module:  pqCheckBoxPixMaps.h

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
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

========================================================================*/
#ifndef __ctkCheckBoxPixmaps_h
#define __ctkCheckBoxPixmaps_h

/// Qt includes
#include <QObject>
#include <QPixmap>

/// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"

class QPixmap;
class QWidget;
class ctkCheckBoxPixmapsPrivate;

/// \ingroup Widgets
///
/// ctkCheckBoxPixmaps is a helper class that can used to create pixmaps for
/// checkboxs in various states. This is useful for showing checkboxes in qt-views.
class CTK_WIDGETS_EXPORT ctkCheckBoxPixmaps : public QObject
{
  Q_OBJECT
  typedef QObject Superclass;

public:
  ///
  /// The widget is used to retrieve the style of the checkboxes
  /// If the widget is 0 (not recommended) use the QApplication style.
  ctkCheckBoxPixmaps(QWidget* parent = 0);
  virtual ~ctkCheckBoxPixmaps();

  ///
  /// Returns a pixmap for the given state .
  /// The pixmaps have been cached so the cost of the function is minimum.
  const QPixmap& pixmap(Qt::CheckState state, bool active) const;
  ///
  /// Utility function that can take an int for the state.
  /// Best to be avoided.
  inline const QPixmap& pixmap(int state, bool active) const;

protected:
  QScopedPointer<ctkCheckBoxPixmapsPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkCheckBoxPixmaps);
  Q_DISABLE_COPY(ctkCheckBoxPixmaps);
};

const QPixmap& ctkCheckBoxPixmaps::pixmap(int state, bool active) const
{
  return this->pixmap(static_cast<Qt::CheckState>(state), active);
}

#endif
