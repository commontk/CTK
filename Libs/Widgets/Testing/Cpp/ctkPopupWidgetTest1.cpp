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
void changeOpacity(void* callData)
{
  ctkPopupWidget* popup = reinterpret_cast<ctkPopupWidget*>(callData);
  popup->setOpacity(qobject_cast<QSlider*>(popup->baseWidget())->value());
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
  //QObject::connect(&base, SIGNAL(clicked()), &popup, SLOT(showPopup()));
  base.show();

  QWidget topLevel;
  QPushButton focusButton("Focus popup");
  QPushButton openButton("Open popup");
  QPushButton toggleButton("Toggle popup");
  toggleButton.setCheckable(true);
  QSlider opacitySlider(Qt::Horizontal);
  opacitySlider.setRange(0, 255);
  QVBoxLayout* vlayout = new QVBoxLayout;
  vlayout->addWidget(&focusButton);
  vlayout->addWidget(&openButton);
  vlayout->addWidget(&toggleButton);
  vlayout->addWidget(&opacitySlider);
  topLevel.setLayout(vlayout);

  ctkPopupWidget focusPopup;
  focusPopup.setFrameStyle(QFrame::Box);
  focusPopup.setLineWidth(1);
  focusPopup.setAutoHide(true);
  QPushButton focusPopupContent("button");
  QVBoxLayout* focusLayout = new QVBoxLayout;
  focusLayout->addWidget(&focusPopupContent);
  focusPopup.setLayout(focusLayout);
  focusPopup.setBaseWidget(&focusButton);

  ctkPopupWidget openPopup;
  openPopup.setAutoHide(false);
  QPushButton openPopupContent("Close popup");
  QVBoxLayout* openLayout = new QVBoxLayout;
  openLayout->addWidget(&openPopupContent);
  openPopup.setLayout(openLayout);
  openPopup.setBaseWidget(&openButton);
  QObject::connect(&openButton, SIGNAL(clicked()),
                   &openPopup, SLOT(showPopup()));
  QObject::connect(&openPopupContent, SIGNAL(clicked()),
                   &openPopup, SLOT(hidePopup()));
                   
  ctkPopupWidget togglePopup;
  togglePopup.setAutoHide(false);
  QPushButton togglePopupContent("button");
  QVBoxLayout* toggleLayout = new QVBoxLayout;
  toggleLayout->addWidget(&togglePopupContent);
  togglePopup.setLayout(toggleLayout);
  togglePopup.setBaseWidget(&toggleButton);
  QObject::connect(&toggleButton, SIGNAL(toggled(bool)),
                   &togglePopup, SLOT(showPopup(bool)));
  togglePopup.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  
  ctkPopupWidget sliderPopup;
  sliderPopup.setAutoHide(false);
  sliderPopup.setOpacity(255);
  QPalette palette = sliderPopup.palette();
  QLinearGradient linearGradient(QPointF(0.f, 0.f), QPointF(0.f, 0.666f));
  linearGradient.setSpread(QGradient::PadSpread);
  linearGradient.setCoordinateMode(QGradient::StretchToDeviceMode);
  linearGradient.setColorAt(0, palette.color(QPalette::Window));
  linearGradient.setColorAt(1, palette.color(QPalette::Dark));
  palette.setBrush(QPalette::Window, linearGradient);
  sliderPopup.setPalette(palette);
  QWidget sliderPopupContent;
  QVBoxLayout* sliderLayout = new QVBoxLayout;
  sliderLayout->addWidget(&sliderPopupContent);
  sliderPopup.setLayout(sliderLayout);
  sliderPopup.setBaseWidget(&opacitySlider);
  ctkCallback callback(changeOpacity);
  callback.setCallbackData(&sliderPopup);
  opacitySlider.setValue(255);
  QObject::connect(&opacitySlider, SIGNAL(valueChanged(int)),
                   &callback, SLOT(invoke()));

  topLevel.show();
  sliderPopup.showPopup();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}

