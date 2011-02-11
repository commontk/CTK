

// ctkDICOMWidgets includes
#include "ctkDICOMQueryWidget.h"
#include "ui_ctkDICOMQueryWidget.h"

// STD includes
#include <iostream>

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
}

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::~ctkDICOMQueryWidget()
{
}


//----------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMQueryWidget::parameters()
{
  Q_D(ctkDICOMQueryWidget);

  QMap<QString,QVariant> parameters;

  parameters["Name"] = d->NameSearch->text();
  parameters["Study"] = d->StudySearch->text();
  parameters["Series"] = d->SeriesSearch->text();
  parameters["ID"] = d->IdSearch->text();

  if ( !d->ModalityWidget->areAllModalitiesSelected() )
  { // some PACS (conquest) don't seem to accept list of modalities,
    // so don't include the list at all when all modalities are desired
    // TODO: think about how to fix this for conquest at the query level
    parameters["Modalities"] = d->ModalityWidget->selectedModalities();
  }

  if ( !d->DateRangeWidget->isAnyDate() )
  {
    QDate startDate = d->DateRangeWidget->startDateTime().date();
    QDate endDate = d->DateRangeWidget->endDateTime().date();
    parameters["StartDate"] = startDate.toString("yyyyMMdd");
    parameters["EndDate"] = endDate.toString("yyyyMMdd");
  }

  return parameters;
}
