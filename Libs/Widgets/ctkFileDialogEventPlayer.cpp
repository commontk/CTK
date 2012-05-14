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

// ----------------------------------------------------------------------------
ctkFileDialogEventPlayer::ctkFileDialogEventPlayer(pqTestUtility* util, QObject *parent)
  : pqNativeFileDialogEventPlayer(util, parent)
{

}

// ----------------------------------------------------------------------------
bool ctkFileDialogEventPlayer::playEvent(QObject *Object,
                                                const QString &Command,
                                                const QString &Arguments,
                                                bool &Error)
{
  if (Command == "FileOpen" || Command == "DirOpen" ||
      Command == "FileSave" || Command == "FilesOpen")
    {
    return this->Superclass::playEvent(Object, Command, Arguments, Error);
    }

  if (Command != "newFile" &&
      Command != "rejected" &&
      Command != "fileSave")
    {
    return false;
    }

  if(ctkFileDialog* const object =
       qobject_cast<ctkFileDialog*>(Object))
    {
    if (Command == "newFile")
      {
      // set the directory
      QStringList files = Arguments.split("#");
      QFileInfo infoFile(files.at(0));
      if(!infoFile.exists())
        {
        qWarning() << "The File doesn't exist, or wasn't finded.";
        return false;
        }
      object->setDirectory(infoFile.absoluteDir().absolutePath());
      if(object->directory() != infoFile.absoluteDir())
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
    if (Command == "rejected")
      {
      object->close();
      return true;
      }
    }

  qCritical() << "calling newFile/rejected on unhandled type " << Object;
  Error = true;
  return true;
}

