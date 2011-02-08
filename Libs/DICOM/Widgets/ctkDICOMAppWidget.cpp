#include <QDebug>
#include <QTreeView>
#include <QTabBar>
#include <QSettings>
#include <QAction>

// ctkDICOMWidgets includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"
#include "ctkDICOMAppWidget.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ui_ctkDICOMAppWidget.h"
#include "ctkDirectoryButton.h"
#include "ctkDICOMQueryRetrieveWidget.h"

//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMAppWidget");

//----------------------------------------------------------------------------
class ctkDICOMAppWidgetPrivate: public Ui_ctkDICOMAppWidget
{
public:
  ctkDICOMQueryRetrieveWidget* queryRetrieveWidget;

  ctkDICOMAppWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMAppWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMAppWidget methods

//----------------------------------------------------------------------------
ctkDICOMAppWidget::ctkDICOMAppWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMAppWidgetPrivate)
{
  Q_D(ctkDICOMAppWidget);  

  d->setupUi(this);

  d->queryRetrieveWidget = new ctkDICOMQueryRetrieveWidget();

  connect(d->directoryButton, SIGNAL(directoryChanged(const QString&)), this, SLOT(onDatabaseDirectoryChanged(const QString&)));
}

//----------------------------------------------------------------------------
ctkDICOMAppWidget::~ctkDICOMAppWidget()
{
  Q_D(ctkDICOMAppWidget);  

  d->queryRetrieveWidget->deleteLater();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onDatabaseDirectoryChanged(const QString& directory)
{
  Q_D(ctkDICOMAppWidget);  

  QSettings settings;
  settings.setValue("DatabaseDirectory", directory);
  settings.sync();

  
}

void ctkDICOMAppWidget::onAddToDatabase()
{
  Q_D(ctkDICOMAppWidget);

  //d->
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onImport(){

}

void ctkDICOMAppWidget::onExport(){

}

void ctkDICOMAppWidget::onQuery(){
  Q_D(ctkDICOMAppWidget);

  d->queryRetrieveWidget->show();
  d->queryRetrieveWidget->raise();
}
