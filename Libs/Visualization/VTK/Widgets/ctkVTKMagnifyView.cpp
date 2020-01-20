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

// Qt includes
#include <QEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QTimerEvent>

// CTK includes
#include "ctkVTKMagnifyView.h"
#include "ctkVTKMagnifyView_p.h"
#include "ctkVTKOpenGLNativeWidget.h"
#include "ctkLogger.h"

// VTK includes
#include <vtkMath.h>
#include <vtkRenderWindow.h>
#include <vtkUnsignedCharArray.h>

// STD includes
#include <cmath>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKMagnifyView");
//--------------------------------------------------------------------------

// --------------------------------------------------------------------------
// ctkVTKMagnifyViewPrivate methods

// --------------------------------------------------------------------------
ctkVTKMagnifyViewPrivate::ctkVTKMagnifyViewPrivate(ctkVTKMagnifyView& object)
  : QObject(&object), q_ptr(&object)
{
  this->ObservedQVTKWidgets = QList<ctkVTKOpenGLNativeWidget *>();
  this->Magnification = 1.0;
  this->ObserveRenderWindowEvents = true;

  this->EventHandler.EventType = NoEvent;
  this->EventHandler.Position = QPointF(0,0);

  this->EventHandler.UpdateInterval = 20;
  this->EventHandler.TimerId = 0;

}

// --------------------------------------------------------------------------
ctkVTKMagnifyViewPrivate::~ctkVTKMagnifyViewPrivate()
{
  if (this->EventHandler.TimerId != 0)
    {
    this->killTimer(this->EventHandler.TimerId);
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::init()
{
  // Start by removing the pixmap
  this->EventHandler.EventType = RemovePixmapEvent;
  this->removePixmap();

  // Start the timer
  this->restartTimer();
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::restartTimer()
{
  // Kill any old timers
  if (this->EventHandler.TimerId != 0)
    {
    this->killTimer(this->EventHandler.TimerId);
    this->EventHandler.TimerId = 0;
    }

  // Start timer if appropriate
  if (this->EventHandler.UpdateInterval != 0)
    {
    this->EventHandler.TimerId = this->startTimer(this->EventHandler.UpdateInterval);
    Q_ASSERT(this->EventHandler.TimerId);
    }
  // Not using any timers, process events as they come
  else
    {
    this->EventHandler.TimerId = 0;
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::resetEventHandler()
{
  this->EventHandler.EventType = NoEvent;
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::timerEvent(QTimerEvent * event)
{
  Q_UNUSED(event);
  Q_ASSERT(event->timerId() == this->EventHandler.TimerId);

  if (this->EventHandler.EventType == UpdatePixmapEvent)
    {
    this->updatePixmap();
    }
  else if (this->EventHandler.EventType == RemovePixmapEvent)
    {
    this->removePixmap();
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::pushUpdatePixmapEvent()
{
  if (this->EventHandler.Widget.isNull())
    {
    return;
    }
  this->pushUpdatePixmapEvent(
        this->EventHandler.Widget.data()->mapFromGlobal(QCursor::pos()));
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::pushUpdatePixmapEvent(QPointF pos)
{
  if (this->EventHandler.Widget.isNull())
    {
    return;
    }
  // Add this event to the queue
  this->EventHandler.EventType = UpdatePixmapEvent;
  this->EventHandler.Position = pos;

  // Process the event if we handle all events
  if (this->EventHandler.UpdateInterval == 0)
    {
    this->updatePixmap();
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::pushRemovePixmapEvent()
{
  // Add this event to the queue
  this->EventHandler.EventType = RemovePixmapEvent;

  // Process the event if we handle all events
  if (this->EventHandler.UpdateInterval == 0)
    {
    this->removePixmap();
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::connectRenderWindow(ctkVTKOpenGLNativeWidget * widget)
{
  Q_ASSERT(widget);
  Q_ASSERT(this->ObserveRenderWindowEvents);

#if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 90)
  vtkRenderWindow * renderWindow = widget->renderWindow();
#else
  vtkRenderWindow * renderWindow = widget->GetRenderWindow();
#endif
  if (renderWindow)
    {
    this->qvtkConnect(renderWindow, vtkCommand::EndEvent,
                      this, SLOT(pushUpdatePixmapEvent()));
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::disconnectRenderWindow(ctkVTKOpenGLNativeWidget * widget)
{
  Q_ASSERT(widget);

#if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 90)
  vtkRenderWindow * renderWindow = widget->renderWindow();
#else
  vtkRenderWindow * renderWindow = widget->GetRenderWindow();
#endif
  if (renderWindow)
    {
    this->qvtkDisconnect(renderWindow, vtkCommand::EndEvent,
                         this, SLOT(pushUpdatePixmapEvent()));
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::observe(ctkVTKOpenGLNativeWidget * widget)
{
  Q_ASSERT(widget);

  // If we are not already observing the widget, add it to the list and install
  // the public implementation as the event filter to handle mousing
  if (!this->ObservedQVTKWidgets.contains(widget))
    {
    this->ObservedQVTKWidgets.append(widget);
    Q_Q(ctkVTKMagnifyView);
    widget->installEventFilter(q);
    if (this->ObserveRenderWindowEvents)
      {
      this->connectRenderWindow(widget);
      }
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::remove(ctkVTKOpenGLNativeWidget * widget)
{
  Q_ASSERT(widget);

  // If we are observing the widget, remove it from the list and remove the
  // public implementations event filtering
  if (this->ObservedQVTKWidgets.contains(widget))
    {
    Q_ASSERT(this->ObservedQVTKWidgets.count(widget) == 1);
    this->ObservedQVTKWidgets.removeOne(widget);
    Q_Q(ctkVTKMagnifyView);
    widget->removeEventFilter(q);
    if (this->ObserveRenderWindowEvents)
      {
      this->disconnectRenderWindow(widget);
      }
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::removePixmap()
{
  Q_ASSERT(this->EventHandler.EventType == RemovePixmapEvent);
  Q_Q(ctkVTKMagnifyView);
  QPixmap nullPixmap;
  q->setPixmap(nullPixmap);
  q->update();
  this->resetEventHandler();
}

// -------------------------------------------------------------------------
void ctkVTKMagnifyViewPrivate::updatePixmap()
{
  Q_ASSERT(this->EventHandler.EventType == UpdatePixmapEvent);
  Q_ASSERT(!this->EventHandler.Widget.isNull());
  Q_Q(ctkVTKMagnifyView);

  // Retrieve buffer of given QVTKWidget from its render window
#if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 90)
  vtkRenderWindow * renderWindow = this->EventHandler.Widget.data()->renderWindow();
#else
  vtkRenderWindow * renderWindow = this->EventHandler.Widget.data()->GetRenderWindow();
#endif
  if (!renderWindow)
    {
    return;
    }

  // Get the window size and mouse position, and do error checking
  QPointF pos = this->EventHandler.Position;
  int * windowSize = renderWindow->GetSize();
  QPointF mouseWindowPos(pos.x(), static_cast<double>(windowSize[1]-1)-pos.y());
  if (mouseWindowPos.x() < 0 || mouseWindowPos.x() >= windowSize[0] ||
      mouseWindowPos.y() < 0 || mouseWindowPos.y() >= windowSize[1])
    {
    return;
    }

  // Compute indices into the render window's data array
  // Given a magnification and the label's widget size, compute the number of
  // pixels we can show.  We should round to get a larger integer extent, since
  // we will later adjust the pixmap's location in paintEvent().
  // Left-right and up-down are in the render window coordinate frame.
  // (which is different in the y-direction compared to Qt coordinates).
  QSizeF sizeToMagnify = QSizeF(q->size()) / this->Magnification;
  double posLeft = (mouseWindowPos.x() - ((sizeToMagnify.width()-1.0) / 2.0));
  double posRight = (mouseWindowPos.x() + ((sizeToMagnify.width()-1.0) / 2.0));
  double posBottom = (mouseWindowPos.y() - ((sizeToMagnify.height()-1.0) / 2.0));
  double posTop = (mouseWindowPos.y() + ((sizeToMagnify.height()-1.0) / 2.0));

  // Round to ints, for indexing into the pixel array
  int indexLeft = std::floor(posLeft);
  int indexRight = std::ceil(posRight);
  int indexBottom = std::floor(posBottom);
  int indexTop = std::ceil(posTop);

  // Handle when mouse is near the border
  int minLeft = 0;
  int maxRight = windowSize[0]-1;
  int minBottom = 0;
  int maxTop = windowSize[1]-1;

  bool overLeft = indexLeft < minLeft;
  bool overRight = indexRight > maxRight;
  bool overBottom = indexBottom < minBottom;
  bool overTop = indexTop > maxTop;

  // Ensure we don't access nonexistant indices
  if (overLeft)
    {
    indexLeft = minLeft;
    posLeft = minLeft;
    }
  if (overRight)
    {
    indexRight = maxRight;
    posRight = maxRight;
    }
  if (overBottom)
    {
    indexBottom = minBottom;
    posBottom = minBottom;
    }
  if (overTop)
    {
    indexTop = maxTop;
    posTop = maxTop;
    }

  // Error case
  if (indexLeft > indexRight || indexBottom > indexTop)
    {
    return;
    }

  // Setup the pixelmap's position in the label
  Qt::Alignment alignment;
  if (overLeft && !overRight)
    {
    alignment = Qt::AlignRight;
    }
  else if (overRight && !overLeft)
    {
    alignment = Qt::AlignLeft;
    }
  else
    {
    alignment = Qt::AlignLeft;
    }
  if (overBottom && !overTop)
    {
    alignment = alignment | Qt::AlignTop;
    }
  else if (overTop && !overBottom)
    {
    alignment = alignment | Qt::AlignBottom;
    }
  else
    {
    alignment = alignment | Qt::AlignTop;
    }
  q->setAlignment(alignment);

  // Retrieve the pixel data into a QImage (flip vertically to move from render
  // window coordinates to Qt coordinates)
  QSize actualSize(indexRight-indexLeft+1, indexTop-indexBottom+1);
  QImage image(actualSize.width(), actualSize.height(), QImage::Format_RGB32);
  vtkUnsignedCharArray * pixelData = vtkUnsignedCharArray::New();
  pixelData->SetArray(image.bits(), actualSize.width() * actualSize.height() * 4, 1);
  int front = renderWindow->GetDoubleBuffer();
  int success = renderWindow->GetRGBACharPixelData(
      indexLeft, indexBottom, indexRight, indexTop, front, pixelData);
  if (!success)
    {
    return;
    }
  pixelData->Delete();
  image = image.rgbSwapped();
  image = image.mirrored();

  // Scale the image to zoom, using FastTransformation to prevent smoothing
  QSize imageSize = actualSize * this->Magnification;
  image = image.scaled(imageSize, Qt::KeepAspectRatioByExpanding,
                       Qt::FastTransformation);

  // Crop the magnified image to solve the problem of magnified partial pixels
  double errorLeft
      = (posLeft - static_cast<double>(indexLeft)) * this->Magnification;
  double errorRight
      = (static_cast<double>(indexRight) - posRight) * this->Magnification;
  double errorBottom
      = (posBottom - static_cast<double>(indexBottom)) * this->Magnification;
  double errorTop
      = (static_cast<double>(indexTop) - posTop) * this->Magnification;

  // When cropping the Qt image, the 'adjust' variables are in Qt coordinates,
  // not render window coordinates (bottom and top switch).
  int cropIndexLeft = vtkMath::Round(errorLeft);
  int cropIndexRight = imageSize.width() - vtkMath::Round(errorRight) - 1;
  int cropIndexTop = vtkMath::Round(errorTop);
  int cropIndexBottom = imageSize.height() - vtkMath::Round(errorBottom) - 1;

  // Handle case when label size and magnification are not both even or odd
  // (errorLeft/errorRight/errorBottom/errorTop will have fractional component,
  // so cropped image wouldn't be the correct size unless we adjust further).
  int requiredWidth = vtkMath::Round((posRight - posLeft + 1) * this->Magnification);
  int requiredHeight = vtkMath::Round((posTop - posBottom + 1) * this->Magnification);
  int actualWidth = cropIndexRight - cropIndexLeft + 1;
  int actualHeight = cropIndexBottom - cropIndexTop + 1;
  int diffWidth = requiredWidth - actualWidth;
  int diffHeight = requiredHeight - actualHeight;
  // Too wide
  if (diffWidth < 0 && cropIndexRight != imageSize.width()-1)
    {
    Q_ASSERT(actualWidth - requiredWidth <= 1);
    cropIndexRight += diffWidth;
    }
  // Too narrow
  else if (diffWidth > 0 && cropIndexLeft != 0)
    {
    Q_ASSERT(requiredWidth - actualWidth <= 1);
    cropIndexLeft -= diffWidth;
    }
  // Too tall
  if (diffHeight < 0 && cropIndexBottom != imageSize.height()-1)
    {
    Q_ASSERT(actualHeight - requiredHeight <= 1);
    cropIndexBottom += diffHeight;
    }
  // Too short
  else if (diffHeight > 0 && cropIndexTop != 0)
    {
    Q_ASSERT(requiredHeight - actualHeight <= 1);
    cropIndexTop -= diffHeight;
    }

  // Finally crop the QImage for display
  QRect cropRect(QPoint(cropIndexLeft, cropIndexTop),
                 QPoint(cropIndexRight, cropIndexBottom));
  image = image.copy(cropRect);


  // Finally, set the pixelmap to the new one we have created and update
  q->setPixmap(QPixmap::fromImage(image));
  q->update();
  this->resetEventHandler();
}

//---------------------------------------------------------------------------
// ctkVTKMagnifyView methods

// --------------------------------------------------------------------------
ctkVTKMagnifyView::ctkVTKMagnifyView(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkVTKMagnifyViewPrivate(*this))
{
  Q_D(ctkVTKMagnifyView);
  d->init();
}

// --------------------------------------------------------------------------
ctkVTKMagnifyView::~ctkVTKMagnifyView()
{
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKMagnifyView, double, magnification, Magnification)

// --------------------------------------------------------------------------
void ctkVTKMagnifyView::setMagnification(double newMagnification)
{
  Q_D(ctkVTKMagnifyView);
  if (newMagnification == d->Magnification || newMagnification <= 0)
    {
    return;
    }

  d->Magnification = newMagnification;
  this->update();
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKMagnifyView, bool,
            observeRenderWindowEvents, ObserveRenderWindowEvents)

// --------------------------------------------------------------------------
void ctkVTKMagnifyView::setObserveRenderWindowEvents(bool newObserve)
{
  Q_D(ctkVTKMagnifyView);
  if (newObserve == d->ObserveRenderWindowEvents)
    {
    return;
    }

  d->ObserveRenderWindowEvents = newObserve;

  // Connect/disconnect observations on vtkRenderWindow EndEvents, depending
  // on whether we are switching from not-observing to observing or from
  // observing to not-observing
  QList<ctkVTKOpenGLNativeWidget *>::iterator it = d->ObservedQVTKWidgets.begin();
  while (it != d->ObservedQVTKWidgets.end())
    {
    if (newObserve)
      {
      d->connectRenderWindow(*it);
      }
    else
      {
      d->disconnectRenderWindow(*it);
      }
    ++it;
    }
}

// --------------------------------------------------------------------------
int ctkVTKMagnifyView::updateInterval() const
  {
  Q_D(const ctkVTKMagnifyView);
  return d->EventHandler.UpdateInterval;
  }

// --------------------------------------------------------------------------
void ctkVTKMagnifyView::setUpdateInterval(int newInterval)
{
  Q_D(ctkVTKMagnifyView);
  if (newInterval == d->EventHandler.UpdateInterval || newInterval < 0)
    {
    return;
    }

  d->EventHandler.UpdateInterval = newInterval;
  d->restartTimer();
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyView::observe(ctkVTKOpenGLNativeWidget * widget)
{
  Q_D(ctkVTKMagnifyView);
  if (widget)
    {
    d->observe(widget);
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyView::observe(QList<ctkVTKOpenGLNativeWidget *> widgets)
{
  foreach(ctkVTKOpenGLNativeWidget * widget, widgets)
    {
    this->observe(widget);
    }
}

// --------------------------------------------------------------------------
bool ctkVTKMagnifyView::isObserved(ctkVTKOpenGLNativeWidget * widget) const
{
  if (!widget)
    {
    return false;
    }
  Q_D(const ctkVTKMagnifyView);
  return d->ObservedQVTKWidgets.contains(widget);
}

// --------------------------------------------------------------------------
int ctkVTKMagnifyView::numberObserved() const
{
  Q_D(const ctkVTKMagnifyView);
  return d->ObservedQVTKWidgets.length();
}

// --------------------------------------------------------------------------
bool ctkVTKMagnifyView::hasCursorInObservedWidget()const
{
  Q_D(const ctkVTKMagnifyView);
  // checking underMouse is faster than 
  // QApplication::widgetAt(QCursor::pos())
  foreach(const ctkVTKOpenGLNativeWidget* widget, d->ObservedQVTKWidgets)
    {
    if (widget->underMouse())
      {
      return true;
      }
    }
  return false;
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyView::remove(ctkVTKOpenGLNativeWidget * widget)
{
  Q_D(ctkVTKMagnifyView);
  if (widget)
    {
    d->remove(widget);
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyView::remove(QList<ctkVTKOpenGLNativeWidget *> widgets)
{
  foreach(ctkVTKOpenGLNativeWidget * widget, widgets)
    {
    this->remove(widget);
    }
}

// --------------------------------------------------------------------------
bool ctkVTKMagnifyView::eventFilter(QObject * obj, QEvent * event)
{
  // The given object should be a ctkVTKOpenGLNativeWidget in our list
  ctkVTKOpenGLNativeWidget * widget = static_cast<ctkVTKOpenGLNativeWidget *>(obj);
  Q_ASSERT(widget);
  Q_D(ctkVTKMagnifyView);
  Q_ASSERT(d->ObservedQVTKWidgets.contains(widget));
  d->EventHandler.Widget = widget;

  QEvent::Type eventType = event->type();

  // On mouse move, update the pixmap with the zoomed image
  if (eventType == QEvent::MouseMove)
    {
    QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
    Q_ASSERT(mouseEvent);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    d->pushUpdatePixmapEvent(mouseEvent->localPos());
#else
    d->pushUpdatePixmapEvent(mouseEvent->posF());
#endif
    }
  // On enter, update the pixmap with the zoomed image (required for zooming when
  // widget is created with mouse already within it), and emit signal of enter event
  else if (eventType == QEvent::Enter)
    {
    d->pushUpdatePixmapEvent();
    emit enteredObservedWidget(widget);
    }
  // On leave, fill the pixmap with a solid color and emit signal of leave event
  else if (eventType == QEvent::Leave)
    {
    d->pushRemovePixmapEvent();
    emit leftObservedWidget(widget);
    }
  return this->Superclass::eventFilter(obj, event);
}
