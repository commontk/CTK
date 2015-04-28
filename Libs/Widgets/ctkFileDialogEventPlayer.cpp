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

//QT includes
#include <QDebug>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>

// CTK includes
#include "ctkFileDialog.h"
#include "ctkFileDialogEventPlayer.h"

// QtTesting includes
#include <pqTestUtility.h>

// ----------------------------------------------------------------------------
ctkFileDialogEventPlayer::ctkFileDialogEventPlayer(pqTestUtility* util, QObject *parent)
  : pqNativeFileDialogEventPlayer(util, parent)
{

}

// ----------------------------------------------------------------------------
bool ctkFileDialogEventPlayer::playEvent(QObject *object,
                                         const QString &command,
                                         const QString &arguments,
                                         bool &error)
{
  if (command == "FileOpen" || command == "DirOpen" ||
      command == "FileSave" || command == "FilesOpen")
    {
    return this->Superclass::playEvent(object, command, arguments, error);
    }

  if (command != "newFile" &&
      command != "rejected" &&
      command != "fileSave")
    {
    return false;
    }

  if(ctkFileDialog* const fileDialog = qobject_cast<ctkFileDialog*>(object))
    {
    if (command == "newFile")
      {
      // set the directory
      QStringList files;
	  foreach(const QString& file, arguments.split("#"))
	  {
        files.append(mUtil->convertFromDataDirectory(file));
	  }
      QFileInfo infoFile(files.at(0));
      if(!infoFile.exists())
        {
        qWarning() << "File does not exist or can't be found.";
        return false;
        }
	  fileDialog->setDirectory(infoFile.absoluteDir().absolutePath());
	  if (fileDialog->directory() != infoFile.absoluteDir())
        {
        qWarning() << "The Directory wasn't selected.";
        }
      // select the file
      QList<QLineEdit*> line = object->findChildren<QLineEdit*>();
      if (line.count() > 0)
        {
        QStringList text;
        foreach (QString file, files)
          {
          QFileInfo info(file);
          text << info.fileName();
          }
        QString lineText = "\"" + text.join("\" \"") + "\"";
        line[0]->setText(lineText);
//        qDebug() << lineText;
        }
      else
        {
        qWarning() << "Files wasn't set in the line edit";
        }

      QList<QPushButton*> buttons = object->findChildren<QPushButton*>();
      if(buttons.count() > 0)
        {
        foreach(QPushButton* button, buttons)
          {
          if(button->text().contains("Open") ||
             button->text().contains("Ok") ||
             button->text().contains("Choose"))
            {
            button->setEnabled(true);
            button->setChecked(true);
            button->click();
            }
          }
        }
      return true;
      }
    if (command == "rejected")
      {
      fileDialog->close();
      return true;
      }
    }

  qCritical() << "calling newFile/rejected on unhandled type " << object;
  error = true;
  return true;
}

