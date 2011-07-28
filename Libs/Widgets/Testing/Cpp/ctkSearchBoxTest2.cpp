// Qt includes
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QTimer>
#include <QVBoxLayout>

//CTK includes
#include "ctkSearchBox.h"

// STD includes
#include <cstdlib>
#include <iostream>

// ------------------------------------------------------------------------------
int ctkSearchBoxTest2(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList stringList;
  stringList<<"totoa"<<"TOTOaa"<<"tic"<<"tac"<<"nice"<<"slicer"<<"monday"<<"july"<<"phone";

  ctkSearchBox search3;
  search3.show();

  search3.setText("phone");

  QRegExp regExp(search3.text(),Qt::CaseInsensitive, QRegExp::Wildcard);

  //QStringList testFilter = stringList.filter(search3.text());
  QStringList testFilter = stringList.filter(regExp);
  qDebug() << "Result of Test Filter : " << testFilter;

  if (testFilter.size() >= stringList.size())
    {
    qDebug() << "Line : " << __LINE__<< " error with the filter : " << search3.text();
    }

  QStringListModel listModel(stringList);

  QSortFilterProxyModel filterModel;
  filterModel.setSourceModel(&listModel);
  filterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
  filterModel.setFilterWildcard(search3.text());

  QObject::connect(&search3, SIGNAL(textChanged(QString)),
                   &filterModel, SLOT(setFilterWildcard(QString)));

  QListView listView;
  listView.setModel(&filterModel);
  listView.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

