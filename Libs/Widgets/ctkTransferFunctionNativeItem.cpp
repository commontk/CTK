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

/// Qt includes
#include <QColor>
#include <QDebug>
#include <QPainter>
#include <QtGlobal>
#include <QVariant>
#include <QtOpenGL>

/// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionNativeItem.h"
#include "ctkTransferFunctionScene.h"

#include <windows.h>
#include <gl\gl.h>							// Header File For The OpenGL32 Library
#include <gl\glu.h>							// Header File For The GLu32 Library

//----------------------------------------------------------------------------
class ctkTransferFunctionNativeItemPrivate
{
public:
  ctkTransferFunctionNativeItemPrivate();
  virtual ~ctkTransferFunctionNativeItemPrivate();
  void initTexture();
  GLuint		Texture[1];
};

//----------------------------------------------------------------------------
ctkTransferFunctionNativeItemPrivate::ctkTransferFunctionNativeItemPrivate()
{
  this->Texture[0] = GL_INVALID_VALUE;
}

//----------------------------------------------------------------------------
void ctkTransferFunctionNativeItemPrivate::initTexture()
{
  glGenTextures(1, &this->Texture[0]);

  glBindTexture(GL_TEXTURE_2D, this->Texture[0]);
  if (!glIsTexture(this->Texture[0]))
    {
    qDebug() << "pb texture";
    }
  float transferFunction[12] = {0.,0.,0.,1.,0.,0.,0.,1.,0.,0.,0.,1.};
  glTexImage2D(GL_TEXTURE_2D, 0, 3, 4, 1, 0, GL_RGB, GL_FLOAT, transferFunction);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionNativeItemPrivate::~ctkTransferFunctionNativeItemPrivate()
{
  glDeleteTextures(1, &this->Texture[0]);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionNativeItem::ctkTransferFunctionNativeItem(QGraphicsItem* parentGraphicsItem)
  :ctkTransferFunctionItem(parentGraphicsItem), d_ptr(new ctkTransferFunctionNativeItemPrivate)
{
}

//-----------------------------------------------------------------------------
ctkTransferFunctionNativeItem::ctkTransferFunctionNativeItem(
  ctkTransferFunction* transferFunction, QGraphicsItem* parentItem)
  :ctkTransferFunctionItem(transferFunction, parentItem), 
  d_ptr(new ctkTransferFunctionNativeItemPrivate)
{
}

//-----------------------------------------------------------------------------
ctkTransferFunctionNativeItem::~ctkTransferFunctionNativeItem()
{  
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionNativeItem::paint(
  QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  Q_D(ctkTransferFunctionNativeItem);
  painter->beginNativePainting();

  if (d->Texture[0] == GL_INVALID_VALUE)
    {
    d->initTexture();
    }

  glEnable(GL_TEXTURE_2D);
  //glDisable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LEQUAL);
  glBindTexture(GL_TEXTURE_2D, d->Texture[0]);
  glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 0.0f);
  glEnd();
  painter->endNativePainting();
}
