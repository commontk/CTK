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

// Qt includes
#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QMenu>
#include <QPushButton>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTimer>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

// CTK includes
#include "ctkCallback.h"
#include "ctkCollapsibleButton.h"
#include "ctkConfig.h"
#include "ctkPopupWidget.h"
#include "ctkPopupWidgetEventPlayer.h"
#include "ctkPopupWidgetEventTranslator.h"
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"

// STD includes
#include <cstdlib>
#include <iostream>

QWidget* createPanelTest(const QString& title, QList<ctkPopupWidget*>& popups);

namespace
{
QSignalSpy *Spy1;
QSignalSpy *Spy2;
QSignalSpy *Spy3;
QSignalSpy *Spy4;

//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkPopupWidget*> widget = parentWidget->findChildren<ctkPopupWidget*>();
  QList<QComboBox*> widget2 = parentWidget->findChildren<QComboBox*>();
  if(widget.count())
    {
    CTKCOMPARE(Spy1->count(), 1);
    CTKCOMPARE(Spy2->count(), 1);
    CTKCOMPARE(Spy3->count(), 1);
    CTKCOMPARE(Spy4->count(), 1);
    }
  else if(widget2.count())
    {
    CTKCOMPARE(widget2[0]->currentIndex(), 2);
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
}

//-----------------------------------------------------------------------------
int ctkPopupWidgetEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventTranslator(new ctkPopupWidgetEventTranslator);
  etpWidget.addWidgetEventPlayer(new ctkPopupWidgetEventPlayer);

  // Test case 1
  QList<ctkPopupWidget*> popups;
  QWidget* widget = createPanelTest("Window opacity", popups);
  foreach(ctkPopupWidget* popup, popups)
    {
    popup->setAnimationEffect(ctkPopupWidget::WindowOpacityFadeEffect);
    }

  QSignalSpy spy1(popups[0], SIGNAL(popupOpened(bool)));
  QSignalSpy spy2(popups[1], SIGNAL(popupOpened(bool)));
  QSignalSpy spy3(popups[2], SIGNAL(popupOpened(bool)));
  QSignalSpy spy4(popups[3], SIGNAL(popupOpened(bool)));

  Spy1 = &spy1;
  Spy2 = &spy2;
  Spy3 = &spy3;
  Spy4 = &spy4;

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkPopupWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

//-----------------------------------------------------------------------------
QWidget* createPanelTest(const QString& title, QList<ctkPopupWidget*>& popups)
{
  QWidget* topLevel = new QWidget(0);
  topLevel->setObjectName("topLevelWidget");
  topLevel->setWindowTitle(title);
  ctkCollapsibleButton* button = new ctkCollapsibleButton;

  QComboBox* focusComboBox = new QComboBox;
  focusComboBox->setObjectName("focusComboBox");
  focusComboBox->addItem("Focus popup");
  focusComboBox->addItem("Focus popup");
  focusComboBox->addItem("Focus popup");
  focusComboBox->addItem("Focus popup");
  QPushButton* openButton = new QPushButton("Open popup");
  openButton->setObjectName("openButton");
  QPushButton* toggleButton = new QPushButton("Toggle popup");
  toggleButton->setObjectName("toggleButton");
  toggleButton->setCheckable(true);
  QToolButton* pinButton = new QToolButton(0);
  pinButton->setCheckable(true);

  QVBoxLayout* collapsibleLayout = new QVBoxLayout;
  collapsibleLayout->addWidget(focusComboBox);
  button->setLayout(collapsibleLayout);

  QVBoxLayout* vlayout = new QVBoxLayout;
  vlayout->addWidget(button);
  vlayout->addWidget(openButton);
  vlayout->addWidget(toggleButton);
  vlayout->addWidget(pinButton);
  topLevel->setLayout(vlayout);

  ctkPopupWidget* focusPopup = new ctkPopupWidget(focusComboBox);
  focusPopup->setObjectName("focusPopup");
  focusPopup->setAutoShow(true);
  focusPopup->setAutoHide(true);
  QPushButton* focusPopupContent = new QPushButton("button");
  focusPopupContent->setObjectName("focusPopupContent");
  QToolButton* popupToolButton = new QToolButton;
  popupToolButton->setObjectName("popupToolButton");
  QMenu* menu = new QMenu(popupToolButton);
  menu->setObjectName("menu");
  menu->addAction("first menu item");
  menu->addAction("second menu item");
  menu->addAction("third menu item");
  menu->addAction("fourth menu item");
  popupToolButton->setPopupMode(QToolButton::InstantPopup);
  popupToolButton->setMenu(menu);

  QHBoxLayout* focusLayout = new QHBoxLayout;
  focusLayout->addWidget(focusPopupContent);
  focusLayout->addWidget(popupToolButton);
  focusPopup->setLayout(focusLayout);
  focusLayout->setContentsMargins(0,0,0,0);

  QPalette palette = focusPopup->palette();
  QLinearGradient linearGradient(QPointF(0.f, 0.f), QPointF(0.f, 0.666f));
  linearGradient.setSpread(QGradient::PadSpread);
  linearGradient.setCoordinateMode(QGradient::StretchToDeviceMode);
  linearGradient.setColorAt(0, palette.color(QPalette::Window));
  linearGradient.setColorAt(1, palette.color(QPalette::Dark));
  palette.setBrush(QPalette::Window, linearGradient);
  focusPopup->setPalette(palette);

  ctkPopupWidget* openPopup = new ctkPopupWidget(openButton);
  openPopup->setObjectName("openPopup");
  openPopup->setFrameStyle(QFrame::Box);
  openPopup->setLineWidth(1);
  openPopup->setAutoShow(false);
  openPopup->setAutoHide(false);
  openPopup->setWindowOpacity(0.7);
  QPushButton* openPopupContent = new QPushButton("Close popup");
  openPopupContent->setObjectName("openPopupContent");
  QVBoxLayout* openLayout = new QVBoxLayout;
  openLayout->addWidget(openPopupContent);
  openPopup->setLayout(openLayout);
  QObject::connect(openButton, SIGNAL(clicked()),
                   openPopup, SLOT(showPopup()));
  QObject::connect(openPopupContent, SIGNAL(clicked()),
                   openPopup, SLOT(hidePopup()));

  ctkPopupWidget* togglePopup = new ctkPopupWidget(toggleButton);
  togglePopup->setObjectName("togglePopup");
  togglePopup->setAutoShow(false);
  togglePopup->setAutoHide(false);
  QPushButton* togglePopupContent = new QPushButton("tooltip button");
  togglePopupContent->setObjectName("togglePopupContent");
  togglePopupContent->setToolTip("tooltip");
  QVBoxLayout* toggleLayout = new QVBoxLayout;
  toggleLayout->addWidget(togglePopupContent);
  togglePopup->setLayout(toggleLayout);
  QObject::connect(toggleButton, SIGNAL(toggled(bool)),
                   togglePopup, SLOT(showPopup(bool)));
  togglePopup->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  ctkPopupWidget* pinPopup = new ctkPopupWidget(pinButton);
  pinPopup->setObjectName("pinPopup");
  QPushButton* pinPopupContent = new QPushButton("pin button");
  pinPopupContent->setCheckable(true);
  QObject::connect(pinPopupContent, SIGNAL(toggled(bool)),
                   pinButton, SLOT(setChecked(bool)));
  QObject::connect(pinButton, SIGNAL(toggled(bool)),
                   pinPopupContent, SLOT(setChecked(bool)));
  pinPopupContent->setObjectName("pinPopupContent");
  QVBoxLayout* pinLayout = new QVBoxLayout;
  pinLayout->addWidget(pinPopupContent);
  pinPopup->setLayout(pinLayout);
  QObject::connect(pinButton, SIGNAL(toggled(bool)),
                   pinPopup, SLOT(pinPopup(bool)));

  popups << focusPopup << openPopup << togglePopup << pinPopup;
  return topLevel;
}

