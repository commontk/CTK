

// ctkDICOMWidgets includes
#include "ctkDICOMQueryWidget.h"
#include "ui_ctkDICOMQueryWidget.h"

// STD includes
#include <iostream>


//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMQueryWidget");


//----------------------------------------------------------------------------
class ctkDICOMQueryWidgetPrivate: public Ui_ctkDICOMQueryWidget
{
public:
  ctkDICOMQueryWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMQueryWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMQueryWidget methods

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::ctkDICOMQueryWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMQueryWidgetPrivate)
{
  Q_D(ctkDICOMQueryWidget);
  
  d->setupUi(this);

  connect(d->nameSearch, SIGNAL(textChanged(QString)), this, SIGNAL(nameSearchTextChanged(QString)));
  connect(d->studySearch, SIGNAL(textChanged(QString)), this, SIGNAL(studySearchTextChanged(QString)));
  connect(d->seriesSearch, SIGNAL(textChanged(QString)), this, SIGNAL(seriesSearchTextChanged(QString)));
  connect(d->idSearch, SIGNAL(textChanged(QString)), this, SIGNAL(idSearchTextChanged(QString)));
}

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::~ctkDICOMQueryWidget()
{
    Q_D(ctkDICOMQueryWidget);

    disconnect(d->nameSearch, SIGNAL(textChanged(QString)), this, SIGNAL(nameSearchTextChanged(QString)));
    disconnect(d->studySearch, SIGNAL(textChanged(QString)), this, SIGNAL(studySearchTextChanged(QString)));
    disconnect(d->seriesSearch, SIGNAL(textChanged(QString)), this, SIGNAL(seriesSearchTextChanged(QString)));
    disconnect(d->idSearch, SIGNAL(textChanged(QString)), this, SIGNAL(idSearchTextChanged(QString)));
}


//----------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMQueryWidget::parameters()
{
  Q_D(ctkDICOMQueryWidget);

  QMap<QString,QVariant> parameters;

  parameters["Name"] = d->nameSearch->text();
  parameters["Study"] = d->studySearch->text();
  parameters["Series"] = d->seriesSearch->text();
  parameters["ID"] = d->idSearch->text();

  if ( !d->modalityWidget->areAllModalitiesSelected() )
  { // some PACS (conquest) don't seem to accept list of modalities,
    // so don't include the list at all when all modalities are desired
    // TODO: think about how to fix this for conquest at the query level
    parameters["Modalities"] = d->modalityWidget->selectedModalities();
  }

  if ( !d->dateRangeWidget->isAnyDate() )
  {
    QDate startDate = d->dateRangeWidget->startDateTime().date();
    QDate endDate = d->dateRangeWidget->endDateTime().date();
    parameters["StartDate"] = startDate.toString("yyyyMMdd");
    parameters["EndDate"] = endDate.toString("yyyyMMdd");
  }

  return parameters;
}
