/*===================================================================
  
BlueBerry Platform

Copyright (c) German Cancer Research Center, 
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without 
even the implied warranty of MERCHANTABILITY or FITNESS FOR 
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef CTKCMDLINEMODULEXMLPROGRESSWATCHER_H
#define CTKCMDLINEMODULEXMLPROGRESSWATCHER_H

#include "ctkCommandLineModulesCoreExport.h"

#include <QObject>

class ctkCmdLineModuleXmlProgressWatcherPrivate;

class QIODevice;

class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleXmlProgressWatcher : public QObject
{
  Q_OBJECT

public:

  ctkCmdLineModuleXmlProgressWatcher(QIODevice* input);
  ~ctkCmdLineModuleXmlProgressWatcher();

Q_SIGNALS:

  void filterStarted(const QString& name, const QString& comment);
  void filterProgress(float progress);
  void filterFinished(const QString& name);
  void filterXmlError(const QString& error);

private:

  friend class ctkCmdLineModuleXmlProgressWatcherPrivate;

  Q_PRIVATE_SLOT(d, void _q_readyRead())

  QScopedPointer<ctkCmdLineModuleXmlProgressWatcherPrivate> d;
};

#endif // CTKCMDLINEMODULEXMLPROGRESSWATCHER_H
