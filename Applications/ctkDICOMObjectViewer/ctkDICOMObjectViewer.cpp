#include <iostream>

#include "ctkDICOMObjectViewer.h"

#include "ctkDICOMDatasetViewerWidget.h"


/**
 *
 */
ctkDICOMObjectViewer::ctkDICOMObjectViewer( QWidget* _parent,
  const char* _name, bool _modal,
  Qt::WFlags _fl )
:QDialog(_parent)
{
  setupUi(this);
}

/**
 *  Destroys the object and frees any allocated resources
 */
ctkDICOMObjectViewer::~ctkDICOMObjectViewer()
{
}

void ctkDICOMObjectViewer::SetInputImage( const QImage * image )
{
  this->OpenGlWindow->show();
  this->OpenGlWindow->addImage( image );
  std::cout << "Adding image height = " << image->height() << std::endl;
  std::cout << "Adding image width = " << image->width() << std::endl;
  this->OpenGlWindow->update();
}
