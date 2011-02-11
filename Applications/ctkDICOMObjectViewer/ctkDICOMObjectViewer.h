#ifndef __ctkDICOMObjectViewer_h
#define __ctkDICOMObjectViewer_h

#include "ui_ctkDICOMObjectViewerMainWindow.h"

#include <QDialog>

class ctkDICOMObjectViewer : public QDialog, Ui::MainWindow
{
public:

  ctkDICOMObjectViewer( QWidget* parent = 0, const char* name = 0,
    bool modal = FALSE, Qt::WFlags fl = 0 );
  ~ctkDICOMObjectViewer();

  void DisplayPosition(int x,int y ,int z,float value);

  void SetInputImage( const QImage * image );
};

#endif
