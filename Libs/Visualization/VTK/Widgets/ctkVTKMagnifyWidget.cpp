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

// Qt includes
#include <QEvent>
#include <QMouseEvent>
#include <QPointF>

// CTK includes
#include "ctkVTKMagnifyWidget.h"
#include "ctkLogger.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkRenderWindow.h>
#include <vtkUnsignedCharArray.h>

// STD includes
#include <math.h>

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKMagnifyWidget");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkVTKMagnifyWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKMagnifyWidget);
protected:
  ctkVTKMagnifyWidget* const q_ptr;
public:
  ctkVTKMagnifyWidgetPrivate(ctkVTKMagnifyWidget& object);

  void init();
  void removePixmap();
  void observe(QVTKWidget * widget);
  void remove(QVTKWidget * widget);
  void updatePixmap(QVTKWidget * widget, QPointF pos);

  QList<QVTKWidget *> ObservedQVTKWidgets;
  double Magnification;
};

// --------------------------------------------------------------------------
// ctkVTKMagnifyWidgetPrivate methods

// --------------------------------------------------------------------------
ctkVTKMagnifyWidgetPrivate::ctkVTKMagnifyWidgetPrivate(ctkVTKMagnifyWidget& object)
  :q_ptr(&object)
{
  this->ObservedQVTKWidgets = QList<QVTKWidget *>();
  this->Magnification = 1.0;
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidgetPrivate::init()
{
  this->removePixmap();
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidgetPrivate::removePixmap()
{
  Q_Q(ctkVTKMagnifyWidget);
  QPixmap nullPixmap;
  q->setPixmap(nullPixmap);
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidgetPrivate::observe(QVTKWidget * widget)
{
  Q_ASSERT(widget);

  // If we are not already observing the widget, add it to the list and install
  // the public implementation as the event filter to handle mousing
  if (!this->ObservedQVTKWidgets.contains(widget))
    {
    this->ObservedQVTKWidgets.append(widget);
    Q_Q(ctkVTKMagnifyWidget);
    widget->installEventFilter(q);
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidgetPrivate::remove(QVTKWidget * widget)
{
  Q_ASSERT(widget);

  // If we are observing the widget, remove it from the list and remove the
  // public implementations event filtering
  if (this->ObservedQVTKWidgets.contains(widget))
    {
    Q_ASSERT(this->ObservedQVTKWidgets.count(widget) == 1);
    this->ObservedQVTKWidgets.removeOne(widget);
    Q_Q(ctkVTKMagnifyWidget);
    widget->removeEventFilter(q);
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidgetPrivate::updatePixmap(QVTKWidget * widget, QPointF pos)
{
  Q_ASSERT(widget);
  Q_Q(ctkVTKMagnifyWidget);

  // Retrieve buffer of given QVTKWidget from its render window
  vtkRenderWindow * renderWindow = widget->GetRenderWindow();
  if (!renderWindow)
    {
    this->removePixmap();
    return;
    }

  // Get the window size and mouse position, and do error checking
  int * windowSize = renderWindow->GetSize();
  QPointF mouseWindowPos(pos.x(), static_cast<double>(windowSize[1]-1)-pos.y());
  if (mouseWindowPos.x() < 0 || mouseWindowPos.x() >= windowSize[0] ||
      mouseWindowPos.y() < 0 || mouseWindowPos.y() >= windowSize[1])
    {
    this->removePixmap();
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
  int indexLeft = floor(posLeft);
  int indexRight = ceil(posRight);
  int indexBottom = floor(posBottom);
  int indexTop = ceil(posTop);

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
    this->removePixmap();
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
    this->removePixmap();
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
  int cropIndexLeft = round(errorLeft);
  int cropIndexRight = imageSize.width() - round(errorRight) - 1;
  int cropIndexTop = round(errorTop);
  int cropIndexBottom = imageSize.height() - round(errorBottom) - 1;

  // Handle case when label size and magnification are not both even or odd
  // (errorLeft/errorRight/errorBottom/errorTop will have fractional component,
  // so cropped image wouldn't be the correct size unless we adjust further).
  int requiredWidth = round((posRight - posLeft + 1) * this->Magnification);
  int requiredHeight = round((posTop - posBottom + 1) * this->Magnification);
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


  // Finally, set the pixelmap to the new one we have created
  q->setPixmap(QPixmap::fromImage(image));
}

//---------------------------------------------------------------------------
// ctkVTKMagnifyWidget methods

// --------------------------------------------------------------------------
ctkVTKMagnifyWidget::ctkVTKMagnifyWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkVTKMagnifyWidgetPrivate(*this))
{
  Q_D(ctkVTKMagnifyWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkVTKMagnifyWidget::~ctkVTKMagnifyWidget()
{
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKMagnifyWidget, double, magnification, Magnification);

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidget::setMagnification(double newMagnification)
{
  Q_D(ctkVTKMagnifyWidget);
  if (newMagnification == d->Magnification || newMagnification <= 0)
    {
    return;
    }

  d->Magnification = newMagnification;
  this->update();
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidget::observe(QVTKWidget * widget)
{
  Q_D(ctkVTKMagnifyWidget);
  if (widget)
    {
    d->observe(widget);
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidget::observe(QList<QVTKWidget *> widgets)
{
  foreach(QVTKWidget * widget, widgets)
    {
    this->observe(widget);
    }
}

// --------------------------------------------------------------------------
bool ctkVTKMagnifyWidget::isObserved(QVTKWidget * widget) const
{
  if (!widget)
    {
    return false;
    }
  Q_D(const ctkVTKMagnifyWidget);
  return d->ObservedQVTKWidgets.contains(widget);
}

// --------------------------------------------------------------------------
int ctkVTKMagnifyWidget::numberObserved() const
  {
  Q_D(const ctkVTKMagnifyWidget);
  return d->ObservedQVTKWidgets.length();
  }

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidget::remove(QVTKWidget * widget)
{
  Q_D(ctkVTKMagnifyWidget);
  if (widget)
    {
    d->remove(widget);
    }
}

// --------------------------------------------------------------------------
void ctkVTKMagnifyWidget::remove(QList<QVTKWidget *> widgets)
{
  foreach(QVTKWidget * widget, widgets)
    {
    this->remove(widget);
    }
}

// --------------------------------------------------------------------------
bool ctkVTKMagnifyWidget::eventFilter(QObject * obj, QEvent * event)
{
  // The given object should be a QVTKWidget in our list
  QVTKWidget * widget = static_cast<QVTKWidget *>(obj);
  Q_ASSERT(widget);
  Q_D(ctkVTKMagnifyWidget);
  Q_ASSERT(d->ObservedQVTKWidgets.contains(widget));

  QEvent::Type eventType = event->type();
  // On mouse move, update the pixmap with the zoomed image
  if (eventType == QEvent::MouseMove)
    {
    QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
    Q_ASSERT(mouseEvent);
    d->updatePixmap(widget, mouseEvent->posF());
    this->update();
    return false;
    }
  // On enter, update the pixmap with the zoomed image (required for zooming when widget
  // is created with mouse already within it), and emit signal of enter event
  else if (eventType == QEvent::Enter)
    {
    QPointF posF = widget->mapFromGlobal(QCursor::pos());
    d->updatePixmap(widget, posF);
    this->update();
    emit enteredObservedWidget(widget);
    return false;
    }
  // On leave, fill the pixmap with a solid color and emit signal of leave event
  else if (eventType == QEvent::Leave)
    {
    d->removePixmap();
    this->update();
    emit leftObservedWidget(widget);
    return false;
    }
  // For other event types, use standard event processing
  else
    {
    return QObject::eventFilter(obj, event);
    }
}
