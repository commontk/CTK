/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
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

#include "ctkPluginTableModel.h"

#include <ctkPlugin.h>
#include <ctkPluginContext.h>

ctkPluginTableModel::ctkPluginTableModel(ctkPluginContext* pc, QObject* parent)
  : QAbstractTableModel(parent)
{
  plugins = pc->getPlugins();
  pc->connectPluginListener(this, SLOT(pluginChanged(ctkPluginEvent)));
}

QVariant ctkPluginTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) return QVariant();

  QSharedPointer<ctkPlugin> plugin = plugins.at(index.row());
  if (role == Qt::DisplayRole)
  {
    int col = index.column();
    if (col == 0)
    {
      return QVariant(plugin->getSymbolicName());
    }
    else if (col == 1)
    {
      return QVariant(plugin->getVersion().toString());
    }
    else if (col == 2)
    {
      return QVariant(getStringForState(plugin->getState()));
    }
  }
  else if (role == Qt::UserRole)
  {
    return QVariant::fromValue<qlonglong>(plugin->getPluginId());
  }

  return QVariant();
}

QVariant ctkPluginTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    if (section == 0)
    {
      return QVariant("Plugin");
    }
    else if (section == 1)
    {
      return QVariant("Version");
    }
    else if (section == 2)
    {
      return QVariant("State");
    }
  }

  return QVariant();
}

int ctkPluginTableModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)

  return 3;
}

int ctkPluginTableModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)

  return plugins.size();
}

QString ctkPluginTableModel::getStringForState(const ctkPlugin::State state) const
{
  static const QString uninstalled("UNINSTALLED");
  static const QString installed("INSTALLED");
  static const QString resolved("RESOLVED");
  static const QString starting("STARTING");
  static const QString stopping("STOPPING");
  static const QString active("ACTIVE");

  switch(state)
  {
  case ctkPlugin::UNINSTALLED: return uninstalled;
  case ctkPlugin::INSTALLED: return installed;
  case ctkPlugin::RESOLVED: return resolved;
  case ctkPlugin::STARTING: return starting;
  case ctkPlugin::STOPPING: return stopping;
  case ctkPlugin::ACTIVE: return active;
  default: return QString("unknown");
  }
}

void ctkPluginTableModel::pluginChanged(const ctkPluginEvent& event)
{
  QModelIndex topLeftIndex = createIndex(plugins.indexOf(event.getPlugin()), 0);
  QModelIndex bottomRightIndex = createIndex(topLeftIndex.row(), columnCount()-1);
  emit dataChanged(topLeftIndex, bottomRightIndex);
}
