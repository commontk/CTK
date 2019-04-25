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
#include "ctkPushButton.h"
#include "ctkLayoutFactory.h"
#include "ctkLayoutViewFactory.h"

// STD includes
#include <iostream>

QString simpleLayout("<layout><item><view/></item></layout>");
QString vboxLayout("<layout type=\"vertical\"><item><view/></item><item><view/></item></layout>");
QString gridLayout(
"<layout type=\"grid\">"
" <item><view/></item>"
" <item column=\"1\"><view/></item>"
" <item row=\"1\"><view horizontalStretch=\"1\"/></item>"
" <item row=\"1\" column=\"1\"><view horizontalStretch=\"2\"/></item>"
" <item row=\"2\" colspan=\"2\"><view/></item>"
" <item row=\"3\" rowspan=\"2\"><view/></item>"
"</layout>");
QString tabLayout(
"<layout type=\"tab\">"
" <item><view name=\"tab1\"/></item>"
" <item><view name=\"tab2\"/></item>"
" <item><view name=\"tab3\"/></item>"
"</layout>");
QString tabMultipleLayout(
  "<layout type=\"tab\">"
  " <item multiple=\"true\"><view name=\"tab1\"/></item>"
  "</layout>");
QString nestedLayout(
"<layout type=\"tab\">"
" <item>"
"  <layout type=\"horizontal\" split=\"true\">"
"   <item splitSize=\"400\"><view/></item>"
"   <item splitSize=\"100\">"
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
"   <item splitSize=\"100\"><view/></item>"
"  </layout>"
" </item>"
" <item><view name=\"tab2\"/></item>"
" <item><view name=\"tab3\"/></item>"
"</layout>");

namespace
{
class ctkNullViewFactory: public ctkLayoutViewFactory
{
public:
  ctkNullViewFactory(QObject* parent = 0) : ctkLayoutViewFactory(parent)
  {
    this->setUseCachedViews(true);
  }
  virtual QWidget* createViewFromXML(QDomElement layoutElement){
    Q_UNUSED(layoutElement);
    return 0;
    }
};
}

/// \ingroup Widgets

//-----------------------------------------------------------------------------
int ctkLayoutManagerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget viewport;
  viewport.setWindowTitle("Simple layout");
  ctkLayoutFactory layoutManager;

  layoutManager.setViewport(&viewport);
  if (layoutManager.viewport() != &viewport)
    {
    std::cerr << __LINE__ << ": ctkLayoutManager::setViewport() failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  ctkTemplateLayoutViewFactory<QPushButton>* pButtonInstanciator=
    new ctkTemplateLayoutViewFactory<QPushButton>(&viewport);
  layoutManager.registerViewFactory(pButtonInstanciator);

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
  QDomDocument tabMultipleLayoutDoc("tabMultiplelayout");
  res = tabMultipleLayoutDoc.setContent(tabMultipleLayout);
  Q_ASSERT(res);
  QDomDocument nestedLayoutDoc("nestedlayout");
  res = nestedLayoutDoc.setContent(nestedLayout);
  Q_ASSERT(res);
  Q_UNUSED(res);

  layoutManager.setLayout(simpleLayoutDoc);
  if (layoutManager.layout() != simpleLayoutDoc)
    {
    std::cerr << __LINE__ << ": ctkLayoutFactory::setLayout() failed."
              << std::endl;
    return EXIT_FAILURE;
    }
  viewport.show();

  QWidget vbox;
  vbox.setWindowTitle("Vertical Box Layout");
  ctkLayoutFactory vboxLayoutManager;
  vboxLayoutManager.registerViewFactory(pButtonInstanciator);
  vboxLayoutManager.setLayout(vboxLayoutDoc);
  vboxLayoutManager.setViewport(&vbox);
  vbox.show();

  QWidget grid;
  grid.setWindowTitle("Grid Layout");
  ctkLayoutFactory gridLayoutManager;
  gridLayoutManager.registerViewFactory(pButtonInstanciator);
  gridLayoutManager.setLayout(gridLayoutDoc);
  gridLayoutManager.setViewport(&grid);
  grid.show();

  QWidget tab;
  tab.setWindowTitle("Tab Layout");
  ctkLayoutFactory tabLayoutManager;
  tabLayoutManager.registerViewFactory(pButtonInstanciator);
  tabLayoutManager.setLayout(tabLayoutDoc);
  tabLayoutManager.setViewport(&tab);
  tab.show();

  QWidget nested;
  nested.setWindowTitle("Nested Layout");
  ctkLayoutFactory nestedLayoutManager;
  nestedLayoutManager.registerViewFactory(pButtonInstanciator);
  nestedLayoutManager.setLayout(nestedLayoutDoc);
  nestedLayoutManager.setViewport(&nested);
  nested.show();

  // TabToGrid
  QWidget tabToGrid;
  tabToGrid.setWindowTitle("Tab to Grid Layout");
  ctkTemplateLayoutViewFactory<ctkPushButton>* tabToGridInstanciator =
    new ctkTemplateLayoutViewFactory<ctkPushButton>(&viewport);
  ctkLayoutFactory tabToGridLayoutManager;
  tabToGridLayoutManager.registerViewFactory(tabToGridInstanciator);
  tabToGridLayoutManager.setLayout(tabLayoutDoc);
  tabToGridLayoutManager.setViewport(&tabToGrid);
  tabToGrid.show();

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  tabToGridLayoutManager.setLayout(gridLayoutDoc);

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  if (tabToGridInstanciator->registeredViews().count() != 6 ||
      tabToGridInstanciator->registeredViews()[0]->isHidden() ||
      tabToGridInstanciator->registeredViews()[1]->isHidden() ||
      tabToGridInstanciator->registeredViews()[2]->isHidden() ||
      tabToGridInstanciator->registeredViews()[3]->isHidden())
    {
    std::cout << __LINE__ << " TabToGrid: "
              << "ctkLayoutManager::setupLayout() failed to show/hide widgets"
              << tabToGridInstanciator->registeredViews().count() << " "
              << tabToGridInstanciator->registeredViews()[0]->isHidden() << " "
              << tabToGridInstanciator->registeredViews()[1]->isHidden() << " "
              << tabToGridInstanciator->registeredViews()[2]->isHidden() << " "
              << tabToGridInstanciator->registeredViews()[3]->isHidden() << std::endl;
    return EXIT_FAILURE;
    }

  // TabToSimple
  QWidget tabToSimple;
  tabToSimple.setWindowTitle("Tab to Simple Layout");
  ctkTemplateLayoutViewFactory<ctkPushButton>* tabToSimpleInstanciator =
    new ctkTemplateLayoutViewFactory<ctkPushButton>(&viewport);
  ctkLayoutFactory tabToSimpleLayoutManager;
  tabToSimpleLayoutManager.registerViewFactory(tabToSimpleInstanciator);
  //tabToSimpleLayoutManager.setLayout(gridLayoutDoc);
  tabToSimpleLayoutManager.setLayout(tabLayoutDoc);
  tabToSimpleLayoutManager.setViewport(&tabToSimple);
  tabToSimple.show();

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();
  tabToSimpleLayoutManager.setLayout(simpleLayoutDoc);

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  if (tabToSimpleInstanciator->registeredViews().count() != 3 ||
      tabToSimpleInstanciator->registeredViews()[0]->isHidden() ||
      tabToSimpleInstanciator->registeredViews()[1]->isVisible() ||
      tabToSimpleInstanciator->registeredViews()[2]->isVisible())
    {
    std::cout << __LINE__ << " TabToSimple: "
              << "ctkLayoutManager::setupLayout() failed to show/hide widgets"
              << tabToSimpleInstanciator->registeredViews().count() << " "
              << tabToSimpleInstanciator->registeredViews()[0]->isHidden() << " "
              << tabToSimpleInstanciator->registeredViews()[1]->isVisible() << " "
              << tabToSimpleInstanciator->registeredViews()[2]->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  // NestedToTab
  QWidget nestedToTab;
  nestedToTab.setWindowTitle("Nested to Tab Layout");
  ctkTemplateLayoutViewFactory<ctkPushButton>* nestedToTabInstanciator =
    new ctkTemplateLayoutViewFactory<ctkPushButton>(&viewport);
  ctkLayoutFactory nestedToTabLayoutManager;
  nestedToTabLayoutManager.registerViewFactory(nestedToTabInstanciator);
  nestedToTabLayoutManager.setLayout(nestedLayoutDoc);
  nestedToTabLayoutManager.setViewport(&nestedToTab);
  nestedToTab.show();

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  nestedToTabLayoutManager.setLayout(tabLayoutDoc);

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  if (nestedToTabInstanciator->registeredViews()[0]->isHidden() ||
      nestedToTabInstanciator->registeredViews()[1]->isVisible() ||
      nestedToTabInstanciator->registeredViews()[2]->isVisible() ||
      nestedToTabInstanciator->registeredViews()[3]->isVisible())
    {
    std::cout << __LINE__ << " NestedToTab: "
              << "ctkLayoutManager::setupLayout() failed to show/hide widgets"
              << nestedToTabInstanciator->registeredViews()[0]->isHidden() << " "
              << nestedToTabInstanciator->registeredViews()[1]->isVisible() << " "
              << nestedToTabInstanciator->registeredViews()[2]->isVisible() << " "
              << nestedToTabInstanciator->registeredViews()[3]->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  // Switch back to nested layout
  nestedToTabLayoutManager.setLayout(nestedLayoutDoc);
  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  // Test that multiple="true" makes all cached views shown,
  // even if cached views are disabled.
  nestedToTabInstanciator->setUseCachedViews(false);
  nestedToTabLayoutManager.setLayout(tabMultipleLayoutDoc);
  if (nestedToTabInstanciator->registeredViews().count() != 2 * 4 ||
    nestedToTabInstanciator->registeredViews()[0]->isHidden() ||
    !nestedToTabInstanciator->registeredViews()[1]->isHidden() ||
    !nestedToTabInstanciator->registeredViews()[2]->isHidden())
    {
    std::cout << __LINE__ << " tabMultiple: "
      << "ctkLayoutManager::setupLayout() failed to show/hide widgets "
      << nestedToTabInstanciator->registeredViews().count();
    for (int i = 0; i < nestedToTabInstanciator->registeredViews().count(); i++)
      {
      std::cout << " " << nestedToTabInstanciator->registeredViews()[i]->isHidden();
      }
    std::cout << std::endl;
    return EXIT_FAILURE;
    }

  // Test that factories failing to instantiate view do not lead to a "crash"
  ctkNullViewFactory* nullViewInstanciator = new ctkNullViewFactory(&viewport);
  QWidget nullView;
  nullView.setWindowTitle("Layout with Invalid View");
  ctkLayoutFactory nullViewLayoutManager;
  nullViewLayoutManager.registerViewFactory(nullViewInstanciator);
  nullViewLayoutManager.setLayout(simpleLayoutDoc);
  nullViewLayoutManager.setViewport(&nullView);
  nestedToTab.show();

  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
