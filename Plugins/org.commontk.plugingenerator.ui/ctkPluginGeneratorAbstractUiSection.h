/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#ifndef CTKPLUGINGENERATORUISECTION_H
#define CTKPLUGINGENERATORUISECTION_H

#include <QObject>
#include <QIcon>

class QWidget;

class ctkPluginGeneratorAbstractUiSection : public QObject
{

  Q_OBJECT

public:

  ctkPluginGeneratorAbstractUiSection();
  virtual ~ctkPluginGeneratorAbstractUiSection();

  QWidget* getWidget();

  virtual QWidget* createWidget(QWidget* parent) = 0;

  QString getDescription() const;
  QString getTitle() const;

  QString getErrorMessage() const;
  QString getMessage() const;

  QIcon getIcon() const;

signals:

  void descriptionChanged(const QString&);
  void titleChanged(const QString&);
  void errorMessageChanged(const QString&);
  void messageChanged(const QString&);
  void iconChanged(const QIcon&);

protected:

  void setDescription(const QString& description);
  void setTitle(const QString& title);

  void setErrorMessage(const QString& errorMsg);
  void setMessage(const QString& msg);

  void setIcon(const QIcon& icon);

private:

  QWidget* sectionWidget;

  QString description;
  QString title;
  QString errorMessage;
  QString message;
  QIcon icon;

};

#endif // CTKPLUGINGENERATORUISECTION_H
