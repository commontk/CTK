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
   Module:  pqSettings.cxx

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
#include "ctkSettings.h"

// Qt includes
#include <QDialog>
#include <QMainWindow>
#include <QDesktopWidget>

//-----------------------------------------------------------------------------
ctkSettings::ctkSettings(const QString& organization,
                         const QString& application,
                         QObject* parentObject)
  : ctkCoreSettings(organization, application, parentObject)
{
}

//-----------------------------------------------------------------------------
ctkSettings::ctkSettings(QSettings::Scope scope,
                         const QString& organization,
                         const QString& application,
                         QObject* parentObject)
  : ctkCoreSettings(scope, organization,application, parentObject)
{
}

//-----------------------------------------------------------------------------
ctkSettings::ctkSettings(QSettings::Format format,
                         QSettings::Scope scope,
                         const QString& organization,
                         const QString& application,
                         QObject* parentObject)
  : ctkCoreSettings(format, scope, organization, application, parentObject)
{
}

//-----------------------------------------------------------------------------
ctkSettings::ctkSettings(const QString& fileName, QSettings::Format format, QObject* parentObject)
  : ctkCoreSettings(fileName, format, parentObject)
{
}

//-----------------------------------------------------------------------------
ctkSettings::ctkSettings(QObject* parentObject)
  : ctkCoreSettings(parentObject)
{
}

//-----------------------------------------------------------------------------
void ctkSettings::alertSettingsModified()
{
  emit this->modified();
}

//-----------------------------------------------------------------------------
void ctkSettings::saveState(const QMainWindow& window, const QString& key)
{
  this->beginGroup(key);
  this->setValue("Position", window.pos());
  this->setValue("Size", window.size());
  this->setValue("Layout", window.saveState());
  this->endGroup();
}

//-----------------------------------------------------------------------------
void ctkSettings::saveState(const QDialog& dialog, const QString& key)
{
  this->beginGroup(key);
  this->setValue("Position", dialog.pos());
  this->setValue("Size", dialog.size());
  this->endGroup();
}

//-----------------------------------------------------------------------------
void ctkSettings::restoreState(const QString& key, QMainWindow& window)
{
  this->beginGroup(key);
  
  if(this->contains("Size"))
    {
    window.resize(this->value("Size").toSize());
    }
    
  if(this->contains("Position"))
    {
    QPoint windowTopLeft = this->value("Position").toPoint();
    QRect mwRect(windowTopLeft, window.size());
    
    QDesktopWidget desktop;
    QRect desktopRect = desktop.availableGeometry( desktop.primaryScreen() );
    // try moving it to keep size
    if(!desktopRect.contains(mwRect))
      {
      mwRect = QRect(desktopRect.topLeft(), window.size());
      }
    // still doesn't fit, resize it
    if(!desktopRect.contains(mwRect))
      {
      mwRect = QRect(desktopRect.topLeft(), window.size());
      window.resize(desktopRect.size());
      }
    window.move(mwRect.topLeft());
    }

  if(this->contains("Layout"))
    {
    window.restoreState(this->value("Layout").toByteArray());
    }
  
  this->endGroup();
}

//-----------------------------------------------------------------------------
void ctkSettings::restoreState(const QString& key, QDialog& dialog)
{
  this->beginGroup(key);
  
  if(this->contains("Size"))
    {
    dialog.resize(this->value("Size").toSize());
    }
    
  if(this->contains("Position"))
    {
    dialog.move(this->value("Position").toPoint());
    }

  this->endGroup();
}
