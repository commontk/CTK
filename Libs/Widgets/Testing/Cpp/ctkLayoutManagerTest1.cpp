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

// Qt includse
#include <QApplication>
#include <QPushButton>
#include <QTimer>

// CTK includes
#include "ctkSimpleLayoutManager.h"

// STD includes
#include <iostream>

QString simpleLayout("<layout><item><view/></item></layout>");
QString vboxLayout("<layout type=\"vertical\"><item><view/></item><item><view/></item></layout>");
QString gridLayout(
"<layout type=\"grid\">"
" <item><view/></item>"
" <item col=\"1\"><view/></item>"
" <item row=\"1\"><view/></item>"
" <item row=\"1\" column=\"1\"><view/></item>"
" <item row=\"2\" colspan=\"2\"><view/></item>"
" <item row=\"3\" rowspan=\"2\"><view/></item>"
"</layout>");
QString tabLayout(
"<layout type=\"tab\">"
" <item><view name=\"tab1\"/></item>"
" <item><view name=\"tab2\"/></item>"
" <item><view name=\"tab3\"/></item>"
"</layout>");
QString nestedLayout(
"<layout type=\"tab\">"
" <item>"
"  <layout type=\"horizontal\">"
"   <item><view/></item>"
"   <item>"
"    <layout type=\"vertical\">"
"     <item><view/></item>"
"     <item><view/></item>"
"     <item>"
"      <layout type=\"grid\">"
"       <item row=\"0\" column=\"1\"><view/></item>"
"       <item row=\"1\" column=\"0\"><view/></item>"
"      </layout>"
"     </item>"
"    </layout>"
"   </item>"
"   <item><view/></item>"
"  </layout>"
" </item>"
" <item><view name=\"tab2\"/></item>"
" <item><view name=\"tab3\"/></item>"
"</layout>");

class ctkPushButton: public QPushButton
{
  Q_OBJECT
public:
  Q_INVOKABLE ctkPushButton(): QPushButton(0){}
};

//-----------------------------------------------------------------------------
int ctkLayoutManagerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget viewport;
  viewport.setWindowTitle("Simple layout");
  ctkSimpleLayoutManager layoutManager;

  layoutManager.setViewport(&viewport);
  if (layoutManager.viewport() != &viewport)
    {
    std::cerr << __LINE__ << ": ctkLayoutManager::setViewport() failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  //layoutManager.setViewMetaObject(QPushButton::staticMetaObject);
  ctkTemplateInstanciator<QPushButton> pButtonInstanciator;
  layoutManager.setViewInstanciator(&pButtonInstanciator);

  QDomDocument simpleLayoutDoc("simplelayout");
  bool res = simpleLayoutDoc.setContent(simpleLayout);
  Q_ASSERT(res);
  QDomDocument vboxLayoutDoc("vboxlayout");
  res = vboxLayoutDoc.setContent(vboxLayout);
  Q_ASSERT(res);
  QDomDocument gridLayoutDoc("gridlayout");
  res = gridLayoutDoc.setContent(gridLayout);
  Q_ASSERT(res);
  QDomDocument tabLayoutDoc("tablayout");
  res = tabLayoutDoc.setContent(tabLayout);
  Q_ASSERT(res);
  QDomDocument nestedLayoutDoc("nestedlayout");
  res = nestedLayoutDoc.setContent(nestedLayout);
  Q_ASSERT(res);

  layoutManager.setLayout(simpleLayoutDoc);
  if (layoutManager.layout() != simpleLayoutDoc)
    {
    std::cerr << __LINE__ << ": ctkSimpleLayoutManager::setLayout() failed."
              << std::endl;
    return EXIT_FAILURE;
    }
  viewport.show();


  QWidget vbox;
  vbox.setWindowTitle("Vertical Box Layout");
  ctkSimpleLayoutManager vboxLayoutManager;
  vboxLayoutManager.setViewInstanciator(&pButtonInstanciator);
  vboxLayoutManager.setLayout(vboxLayoutDoc);
  vboxLayoutManager.setViewport(&vbox);
  vbox.show();

  QWidget grid;
  grid.setWindowTitle("Grid Layout");
  ctkSimpleLayoutManager gridLayoutManager;
  gridLayoutManager.setViewInstanciator(&pButtonInstanciator);
  gridLayoutManager.setLayout(gridLayoutDoc);
  gridLayoutManager.setViewport(&grid);
  grid.show();

  QWidget tab;
  grid.setWindowTitle("Tab Layout");
  ctkSimpleLayoutManager tabLayoutManager;
  tabLayoutManager.setViewInstanciator(&pButtonInstanciator);
  tabLayoutManager.setLayout(tabLayoutDoc);
  tabLayoutManager.setViewport(&tab);
  tab.show();

  QWidget nested;
  grid.setWindowTitle("Nested Layout");
  ctkSimpleLayoutManager nestedLayoutManager;
  nestedLayoutManager.setViewInstanciator(&pButtonInstanciator);
  nestedLayoutManager.setLayout(nestedLayoutDoc);
  nestedLayoutManager.setViewport(&nested);
  nested.show();

  QWidget tabToSimple;
  tabToSimple.setWindowTitle("Tab to Simple Layout");
  ctkSimpleLayoutManager tabToSimpleLayoutManager;
  tabToSimpleLayoutManager.setViewInstanciator(&pButtonInstanciator);
  tabToSimpleLayoutManager.setLayout(tabLayoutDoc);
  tabToSimpleLayoutManager.setViewport(&tabToSimple);
  tabToSimple.show();
  tabToSimpleLayoutManager.setLayout(simpleLayoutDoc);

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
