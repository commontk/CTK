// QT includes
#include <QDebug>
#include <QFileDialog>
#include <QMainWindow>
#include <QTextStream>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

// QtTesting includes
#include "pqTestUtility.h"
#include "pqEventObserver.h"
#include "pqEventSource.h"

// VTK includes
#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkLineSource.h>
#include <vtkPlaneWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSplineWidget2.h>
#include <vtkBoxWidget.h>

// CTK includes
#include "ctkQtTestingMainWindow.h"
#include "ctkXMLEventObserver.h"
#include "ctkXMLEventSource.h"


//-----------------------------------------------------------------------------
ctkQtTestingMainWindow::ctkQtTestingMainWindow()
{
  this->Ui.setupUi(this);

  QObject::connect(Ui.RecordButton, SIGNAL(toggled(bool)), this, SLOT(record(bool)));
  QObject::connect(Ui.PlayBackButton, SIGNAL(clicked()), this, SLOT(play()));

  this->TestUtility = new ctkQtTestingUtility(this);
  this->TestUtility->addEventObserver("xml", new ctkXMLEventObserver(this->TestUtility));
  this->TestUtility->addEventSource("xml", new ctkXMLEventSource(this->TestUtility));

  Ui.renderView->setBackgroundColor(QColor(Qt::gray));
  Ui.renderView->setBackgroundColor2(QColor(Qt::darkBlue));
  Ui.renderView->setGradientBackground(true);
  Ui.renderView->setCornerAnnotationText("ctk Qt test");

  // Create a cube.
  vtkSmartPointer<vtkCubeSource> cubeSource =
    vtkSmartPointer<vtkCubeSource>::New();

  // Create a mapper and actor.
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(cubeSource->GetOutputPort());

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  // Add the actors to the scene
  Ui.renderView->renderer()->AddActor(actor);

//  vtkSmartPointer<vtkBoxWidget> boxWidget =
//    vtkSmartPointer<vtkBoxWidget>::New();
//  boxWidget->SetInteractor(Ui.renderView->interactor());
//  boxWidget->SetPlaceFactor(1.0);
//  boxWidget->PlaceWidget();
//  boxWidget->On();

  Ui.renderView->resetCamera();
}

//-----------------------------------------------------------------------------
ctkQtTestingMainWindow::~ctkQtTestingMainWindow()
{
  if(TestUtility)
    {
    delete this->TestUtility;
    }
}

//-----------------------------------------------------------------------------
void ctkQtTestingMainWindow::record(bool start)
{
  if (start)
    {
    QString filename = QFileDialog::getSaveFileName (this, "Test File Name",
                                                     QString(), "XML Files (*.xml)");
    if (!filename.isEmpty())
      {
      qDebug() << "Start recording";
      QFileInfo fileInfo(filename);
      if (fileInfo.suffix() != "xml")
        {
        filename += ".xml";
        }
      this->TestUtility->recordTests(filename);
      }
    }
  else
    {
    qDebug() << "Stop recording";
    this->TestUtility->stopRecords(1);
    }
}

//-----------------------------------------------------------------------------
void ctkQtTestingMainWindow::play()
{
  qDebug() << "Start Playback";
  QString filename = QFileDialog::getOpenFileName (this, "Test File Name",
    QString(), "XML Files (*.xml)");
  if (!filename.isEmpty())
    {
    this->TestUtility->playTests(filename);
    }
  qDebug() << "End Playback";
}
