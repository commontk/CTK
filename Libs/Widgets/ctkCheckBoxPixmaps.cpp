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
   Module:  pqCheckBoxPixMaps.cxx

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

// Qt includes
#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QStyleOptionButton>
#include <QWidget>

// CTK includes
#include "ctkCheckBoxPixmaps.h"

class ctkCheckBoxPixmapsPrivate
{
public:
  
  enum PixmapStateIndex
    {
    Checked                 = 0,
    PartiallyChecked        = 1,
    UnChecked               = 2,
    
    // All active states in lower half
    Checked_Active          = 3,
    PartiallyChecked_Active = 4,
    UnChecked_Active        = 5,
   
    PixmapCount             = 6
    };
  QPixmap Pixmaps[7];
};

//-----------------------------------------------------------------------------
ctkCheckBoxPixmaps::ctkCheckBoxPixmaps(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkCheckBoxPixmapsPrivate)
{
  Q_D(ctkCheckBoxPixmaps);
  // Initialize the pixmaps. The following style array should
  // correspond to the PixmapStateIndex enum.
  const QStyle::State PixmapStyle[] =
    {
    QStyle::State_On | QStyle::State_Enabled,
    QStyle::State_NoChange | QStyle::State_Enabled,
    QStyle::State_Off | QStyle::State_Enabled,
    QStyle::State_On | QStyle::State_Enabled | QStyle::State_Active,
    QStyle::State_NoChange | QStyle::State_Enabled | QStyle::State_Active,
    QStyle::State_Off | QStyle::State_Enabled | QStyle::State_Active
    };

  QStyleOptionButton option;
  QStyle* style = parentWidget ? parentWidget->style() : qApp->style();
  QRect r = style->subElementRect(
      QStyle::SE_CheckBoxIndicator, &option, parentWidget);
  option.rect = QRect(QPoint(0,0), r.size());
  for(int i = 0; i < ctkCheckBoxPixmapsPrivate::PixmapCount; i++)
    {
    d->Pixmaps[i] = QPixmap(r.size());
    d->Pixmaps[i].fill(QColor(0, 0, 0, 0));
    QPainter painter(&d->Pixmaps[i]);
    option.state = PixmapStyle[i];
    style->drawPrimitive(
        QStyle::PE_IndicatorCheckBox, &option, &painter, parentWidget);
    }
}

//-----------------------------------------------------------------------------
ctkCheckBoxPixmaps::~ctkCheckBoxPixmaps()
{
}

//-----------------------------------------------------------------------------
const QPixmap& ctkCheckBoxPixmaps::pixmap(Qt::CheckState state, bool active) const
{
  Q_D(const ctkCheckBoxPixmaps);
  int offset = active ? 3 : 0;
  switch (state)
    {
    case Qt::Checked:
      return d->Pixmaps[offset + ctkCheckBoxPixmapsPrivate::Checked];

    case Qt::Unchecked:
      return d->Pixmaps[offset + ctkCheckBoxPixmapsPrivate::UnChecked];

    case Qt::PartiallyChecked:
      return d->Pixmaps[offset + ctkCheckBoxPixmapsPrivate::PartiallyChecked];

    default:
      return d->Pixmaps[ctkCheckBoxPixmapsPrivate::PixmapCount];
    }

  return d->Pixmaps[ctkCheckBoxPixmapsPrivate::PixmapCount];
}


