
#include <QFileDialog>
#include <QSqlQueryModel>
#include <QSqlQuery>

// ctkDICOMWidgets includes
#include "ctkDICOMDirectoryListWidget.h"
#include "ui_ctkDICOMDirectoryListWidget.h"

#include <iostream>

//----------------------------------------------------------------------------
class ctkDICOMDirectoryListWidgetPrivate: public ctkPrivate<ctkDICOMDirectoryListWidget>,
                                     public Ui_ctkDICOMDirectoryListWidget
{
public:
  ctkDICOMDirectoryListWidgetPrivate(){}
  ctkDICOM* dicom;
  QSqlQueryModel* directoryListModel;
};

//----------------------------------------------------------------------------
// ctkDICOMDirectoryListWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMDirectoryListWidget methods

//----------------------------------------------------------------------------
ctkDICOMDirectoryListWidget::ctkDICOMDirectoryListWidget(QWidget* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDICOMDirectoryListWidget);
  CTK_D(ctkDICOMDirectoryListWidget);

  d->setupUi(this);

  connect(d->addButton, SIGNAL(clicked()), this, SLOT(addDirectoryClicked()));
  connect(d->removeButton, SIGNAL(clicked()), this, SLOT(removeDirectoryClicked()));

  d->directoryListModel = new QSqlQueryModel(this);


}

//----------------------------------------------------------------------------
ctkDICOMDirectoryListWidget::~ctkDICOMDirectoryListWidget()
{
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget::addDirectoryClicked()
{
  CTK_D(ctkDICOMDirectoryListWidget);
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));

//d->directoryListView->setModel(NULL);
// d->tableView->setModel(NULL);
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget::removeDirectoryClicked()
{

}

void ctkDICOMDirectoryListWidget::setDICOM(ctkDICOM* dicom)
{
  CTK_D(ctkDICOMDirectoryListWidget);
  d->dicom = dicom;
  QSqlQuery getDirectoriesQuery(dicom->database());
  getDirectoriesQuery.exec("SELECT Dirname from Directories");
  std::cout << getDirectoriesQuery.size();
  d->directoryListModel->setQuery(getDirectoriesQuery);
  //d->directoryListView->setModel(d->directoryListModel);
  d->directoryListView->setModel(d->directoryListModel);

}
