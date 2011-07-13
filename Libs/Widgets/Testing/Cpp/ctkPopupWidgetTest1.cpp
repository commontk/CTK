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

// Qt includes
#include <QApplication>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QTimer>

// CTK includes
#include "ctkCallback.h"
#include "ctkPopupWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
QWidget* createPanel(const QString& title, QList<ctkPopupWidget*>& popups)
{
  QWidget* topLevel = new QWidget(0);
  topLevel->setWindowTitle(title);
  
  QComboBox* focusComboBox = new QComboBox;
  focusComboBox->addItem("Focus popup");
  focusComboBox->addItem("Focus popup");
  focusComboBox->addItem("Focus popup");
  focusComboBox->addItem("Focus popup");
  QPushButton* openButton = new QPushButton("Open popup");
  QPushButton* toggleButton = new QPushButton("Toggle popup");
  toggleButton->setCheckable(true);

  QVBoxLayout* vlayout = new QVBoxLayout;
  vlayout->addWidget(focusComboBox);
  vlayout->addWidget(openButton);
  vlayout->addWidget(toggleButton);
  topLevel->setLayout(vlayout);

  ctkPopupWidget* focusPopup = new ctkPopupWidget;
  focusPopup->setAutoShow(true);
  focusPopup->setAutoHide(true);
  QPushButton* focusPopupContent = new QPushButton("button");
  QVBoxLayout* focusLayout = new QVBoxLayout;
  focusLayout->addWidget(focusPopupContent);
  focusPopup->setLayout(focusLayout);
  focusPopup->setBaseWidget(focusComboBox);

  QPalette palette = focusPopup->palette();
  QLinearGradient linearGradient(QPointF(0.f, 0.f), QPointF(0.f, 0.666f));
  linearGradient.setSpread(QGradient::PadSpread);
  linearGradient.setCoordinateMode(QGradient::StretchToDeviceMode);
  linearGradient.setColorAt(0, palette.color(QPalette::Window));
  linearGradient.setColorAt(1, palette.color(QPalette::Dark));
  palette.setBrush(QPalette::Window, linearGradient);
  focusPopup->setPalette(palette);

  ctkPopupWidget* openPopup = new ctkPopupWidget;
  openPopup->setFrameStyle(QFrame::Box);
  openPopup->setLineWidth(1);
  openPopup->setAutoShow(false);
  openPopup->setAutoHide(false);
  openPopup->setWindowOpacity(0.7);
  QPushButton* openPopupContent = new QPushButton("Close popup");
  QVBoxLayout* openLayout = new QVBoxLayout;
  openLayout->addWidget(openPopupContent);
  openPopup->setLayout(openLayout);
  openPopup->setBaseWidget(openButton);
  QObject::connect(openButton, SIGNAL(clicked()),
                   openPopup, SLOT(showPopup()));
  QObject::connect(openPopupContent, SIGNAL(clicked()),
                   openPopup, SLOT(hidePopup()));
                   
  ctkPopupWidget* togglePopup = new ctkPopupWidget;
  togglePopup->setAutoShow(false);
  togglePopup->setAutoHide(false);
  QPushButton* togglePopupContent = new QPushButton("useless button");
  QVBoxLayout* toggleLayout = new QVBoxLayout;
  toggleLayout->addWidget(togglePopupContent);
  togglePopup->setLayout(toggleLayout);
  togglePopup->setBaseWidget(toggleButton);
  QObject::connect(toggleButton, SIGNAL(toggled(bool)),
                   togglePopup, SLOT(showPopup(bool)));
  togglePopup->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  
  popups << focusPopup << openPopup << togglePopup;
  return topLevel;
}

//-----------------------------------------------------------------------------
int ctkPopupWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QPushButton base("Top level push button");
  
  ctkPopupWidget popup;
  QPushButton popupContent("popup");
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(&popupContent);
  popup.setLayout(layout);

  popup.setBaseWidget(&base);
  base.show();
  
  QList<ctkPopupWidget*> popups;
  QWidget* hiddenPanel = createPanel("Hidden", popups);
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

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}

