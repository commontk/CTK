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
#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

// CTK includes
#include "ctkCallback.h"
#include "ctkCollapsibleButton.h"
#include "ctkPopupWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
QWidget* createPanel(const QString& title, QList<ctkPopupWidget*>& popups)
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

//-----------------------------------------------------------------------------
int ctkPopupWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QPushButton base("Top level push button");
  
  ctkPopupWidget popup(&base);
  QPushButton popupContent("popup");
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(&popupContent);
  popup.setLayout(layout);

  popup.setAlignment(Qt::AlignVCenter | Qt::AlignRight);
  popup.setHorizontalDirection(Qt::RightToLeft);
  popup.setOrientation(Qt::Horizontal);
  base.show();
  
  QList<ctkPopupWidget*> popups;
  createPanel("Hidden", popups); //create a panel that is hidden (never shown)
  QWidget* scrollPanel = createPanel("Scroll", popups);
  foreach(ctkPopupWidget* popup, popups)
    {
    popup->setVerticalDirection(ctkPopupWidget::TopToBottom);
    popup->setHorizontalDirection(Qt::LeftToRight);
    popup->setAlignment( Qt::AlignBottom | Qt::AlignJustify);
    popup->setEasingCurve(QEasingCurve::OutElastic);
    QPalette p = popup->palette();
    p.setColor(QPalette::Window, QColor(255, 0, 0, 128));
    popup->setPalette(p);
    popup->setAttribute(Qt::WA_TranslucentBackground);
    }
  popups.clear();
  QWidget* fadePanel = createPanel("Window opacity", popups);
  foreach(ctkPopupWidget* popup, popups)
    {
    popup->setAnimationEffect(ctkPopupWidget::WindowOpacityFadeEffect);
    }

  scrollPanel->show();
  fadePanel->show();

  ctkPopupWidget screenPopup;
  screenPopup.setAutoHide(false);
  screenPopup.setOrientation(Qt::Horizontal | Qt::Vertical);

  QFrame screenPopupContents;
  screenPopupContents.setFixedSize(200, 200);
  QVBoxLayout* screenLayout = new QVBoxLayout;
  screenLayout->addWidget(&screenPopupContents);
  screenPopup.setLayout(screenLayout);
  screenPopup.move(0,0);
  QTimer::singleShot(200, &screenPopup, SLOT(showPopup()));

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}

