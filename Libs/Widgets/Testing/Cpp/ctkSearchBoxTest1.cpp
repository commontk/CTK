// Qt includes
#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QTimer>

// CTK includes
#include "ctkSearchBox.h"

// STD includes
#include <cstdlib>
#include <iostream>

// ------------------------------------------------------------------------------
int ctkSearchBoxTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  ctkSearchBox search;
  search.setShowSearchIcon(true);
  search.show();

  QWidget topLevel;
  ctkSearchBox search3;
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(&search3);
  topLevel.setLayout(layout);
  topLevel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

