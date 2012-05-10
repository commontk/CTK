/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includse
#include <QApplication>
#include <QDebug>
#include <QPushButton>
#include <QTimer>

// CTK includes
#include "ctkSliderWidget.h"
#include "ctkSimpleLayoutManager.h"

// STD includes
#include <iostream>

QString simpleLayout("<layout><item><view/></item></layout>");
QString vboxLayout("<layout type=\"vertical\"><item><view/></item><item><view/></item></layout>");
QString gridLayout(
"<layout type=\"grid\">"
" <item><view/></item>"
" <item column=\"1\"><view/></item>"
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

/// \ingroup Widgets
struct ctkCachedInstanciator
  : public ctkWidgetInstanciator
{
  int CreateWidgetCount;
  QWidgetList CachedWidgets;
  virtual void beginSetupLayout()
    {
    this->CreateWidgetCount = 0;
    }
  virtual QWidget* createWidget()
    {
    if (this->CreateWidgetCount >= this->CachedWidgets.size())
      {
      QWidget* widget = new ctkSliderWidget;
      this->CachedWidgets.push_back(widget);
      }
    return this->CachedWidgets[this->CreateWidgetCount++];
    }
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
  tab.setWindowTitle("Tab Layout");
  ctkSimpleLayoutManager tabLayoutManager;
  tabLayoutManager.setViewInstanciator(&pButtonInstanciator);
  tabLayoutManager.setLayout(tabLayoutDoc);
  tabLayoutManager.setViewport(&tab);
  tab.show();

  QWidget nested;
  nested.setWindowTitle("Nested Layout");
  ctkSimpleLayoutManager nestedLayoutManager;
  nestedLayoutManager.setViewInstanciator(&pButtonInstanciator);
  nestedLayoutManager.setLayout(nestedLayoutDoc);
  nestedLayoutManager.setViewport(&nested);
  nested.show();

  // TabToGrid
  QWidget tabToGrid;
  tabToGrid.setWindowTitle("Tab to Grid Layout");
  ctkCachedInstanciator tabToGridInstanciator;
  ctkSimpleLayoutManager tabToGridLayoutManager;
  tabToGridLayoutManager.setViewInstanciator(&tabToGridInstanciator);
  tabToGridLayoutManager.setLayout(tabLayoutDoc);
  tabToGridLayoutManager.setViewport(&tabToGrid);
  tabToGrid.show();

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  tabToGridLayoutManager.setLayout(gridLayoutDoc);

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  if (tabToGridInstanciator.CachedWidgets[0]->isHidden() ||
      tabToGridInstanciator.CachedWidgets[1]->isHidden() ||
      tabToGridInstanciator.CachedWidgets[2]->isHidden() ||
      tabToGridInstanciator.CachedWidgets[3]->isHidden())
    {
    std::cout << __LINE__ << " TabToGrid: "
              << "ctkLayoutManager::setupLayout() failed to show/hide widgets"
              << tabToGridInstanciator.CachedWidgets[0]->isHidden() << " "
              << tabToGridInstanciator.CachedWidgets[1]->isHidden() << " "
              << tabToGridInstanciator.CachedWidgets[2]->isHidden() << " "
              << tabToGridInstanciator.CachedWidgets[3]->isHidden() << std::endl;
    return EXIT_FAILURE;
    }

  // TabToSimple
  QWidget tabToSimple;
  tabToSimple.setWindowTitle("Tab to Simple Layout");
  ctkCachedInstanciator tabToSimpleInstanciator;
  ctkSimpleLayoutManager tabToSimpleLayoutManager;
  tabToSimpleLayoutManager.setViewInstanciator(&tabToSimpleInstanciator);
  //tabToSimpleLayoutManager.setLayout(gridLayoutDoc);
  tabToSimpleLayoutManager.setLayout(tabLayoutDoc);
  tabToSimpleLayoutManager.setViewport(&tabToSimple);
  tabToSimple.show();

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  tabToSimpleLayoutManager.setLayout(simpleLayoutDoc);

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  if (tabToSimpleInstanciator.CachedWidgets[0]->isHidden() ||
      tabToSimpleInstanciator.CachedWidgets[1]->isVisible() ||
      tabToSimpleInstanciator.CachedWidgets[2]->isVisible())
    {
    std::cout << __LINE__ << " TabToSimple: "
              << "ctkLayoutManager::setupLayout() failed to show/hide widgets"
              << tabToSimpleInstanciator.CachedWidgets[0]->isHidden() << " "
              << tabToSimpleInstanciator.CachedWidgets[1]->isVisible() << " "
              << tabToSimpleInstanciator.CachedWidgets[2]->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  // NestedToTab
  QWidget nestedToTab;
  nestedToTab.setWindowTitle("Nested to Tab Layout");
  ctkCachedInstanciator nestedToTabInstanciator;
  ctkSimpleLayoutManager nestedToTabLayoutManager;
  nestedToTabLayoutManager.setViewInstanciator(&nestedToTabInstanciator);
  nestedToTabLayoutManager.setLayout(nestedLayoutDoc);
  nestedToTabLayoutManager.setViewport(&nestedToTab);
  nestedToTab.show();

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  nestedToTabLayoutManager.setLayout(tabLayoutDoc);

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  if (nestedToTabInstanciator.CachedWidgets[0]->isHidden() ||
      nestedToTabInstanciator.CachedWidgets[1]->isVisible() ||
      nestedToTabInstanciator.CachedWidgets[2]->isVisible() ||
      nestedToTabInstanciator.CachedWidgets[3]->isVisible())
    {
    std::cout << __LINE__ << " NestedToTab: "
              << "ctkLayoutManager::setupLayout() failed to show/hide widgets"
              << nestedToTabInstanciator.CachedWidgets[0]->isHidden() << " "
              << nestedToTabInstanciator.CachedWidgets[1]->isVisible() << " "
              << nestedToTabInstanciator.CachedWidgets[2]->isVisible() << " "
              << nestedToTabInstanciator.CachedWidgets[3]->isVisible() << std::endl;
    return EXIT_FAILURE;
    }


  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
