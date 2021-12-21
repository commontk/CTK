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
#include <QHBoxLayout>
#include <QSignalSpy>
#include <QStyle>
#include <QTimer>

#if (QT_VERSION < 0x50000)
#include <QCleanlooksStyle>
#endif

// CTK includes
#include "ctkPushButton.h"
#include "ctkTest.h"

//-----------------------------------------------------------------------------
class ctkPushButtonTester : public QObject
{
  Q_OBJECT

private slots:

  void testDefaults();
};

//-----------------------------------------------------------------------------
class ctkButtonModeSwitcher : public QObject
{
  Q_OBJECT

public:
  ctkButtonModeSwitcher(ctkPushButton* aButton)
    : button(aButton)
    , mode(0)
  {
  }

public slots:

  void nextMode()
  {
    switch (mode)
    {
    case 0:
      button->setText("text on the right side of button, icon is on the left side of the text (0)");
      button->setIconAlignment(Qt::AlignHCenter);
      button->setButtonTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      break;
    case 1:
      button->setText("text on the right side of button, icon is on the left of the button (1)");
      button->setIconAlignment(Qt::AlignLeft);
      button->setButtonTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      break;
    case 2:
      button->setText("text on the right side of button, icon is on the right of the button (2)");
      button->setIconAlignment(Qt::AlignRight);
      button->setButtonTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      break;

    case 3:
      button->setText("text is the left side of button, icon is on the right side of the text (3)"); //!!!! moves! bad
      button->setIconAlignment(Qt::AlignHCenter);
      button->setButtonTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      break;
    case 4:
      button->setText("text is the left side of button, icon is left of the button (4)");
      button->setIconAlignment(Qt::AlignLeft);
      button->setButtonTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      break;
    case 5:
      button->setText("text is the left side of button, icon is right of the button (5)");
      button->setIconAlignment(Qt::AlignRight);
      button->setButtonTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      break;

    case 6:
      button->setText("text is centered, icon is left of the text (6)");
      button->setIconAlignment(Qt::AlignHCenter);
      button->setButtonTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      break;
    case 7:
      button->setText("text is centered, icon is left of the button (7)");
      button->setIconAlignment(Qt::AlignLeft);
      button->setButtonTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      break;
    case 8:
      button->setText("text is centered, icon is right of the button (8)");
      button->setIconAlignment(Qt::AlignRight);
      button->setButtonTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      break;
    default:
      mode = 0;
      button->setElideMode(button->elideMode() == Qt::ElideMiddle ? Qt::ElideNone : Qt::ElideMiddle);
      nextMode();
      return;
    }
    this->mode++;
  }
private:
  ctkPushButton* button;
  int mode;
};


// ----------------------------------------------------------------------------
void ctkPushButtonTester::testDefaults()
{
#if (QT_VERSION < 0x50000)
  QApplication::setStyle( new QCleanlooksStyle );
#endif

  ctkPushButton button("This is a long text. Click to to see more alignment modes.");

  QCOMPARE(button.buttonTextAlignment(), Qt::AlignHCenter | Qt::AlignVCenter);
  QCOMPARE(button.iconAlignment(), Qt::AlignLeft|Qt::AlignVCenter);

  button.show();
#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&button);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&button);
#endif

  button.setElideMode(Qt::ElideMiddle);

  button.setIcon(qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning));
  button.show();

  ctkButtonModeSwitcher switcher(&button);
  QObject::connect(&button, SIGNAL(clicked()), &switcher, SLOT(nextMode()));

  // Uncomment the next line for interactive testing.
  //qApp->exec();
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkPushButtonTest)
#include "moc_ctkPushButtonTest.cpp"
