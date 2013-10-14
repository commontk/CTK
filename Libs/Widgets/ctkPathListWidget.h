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

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

=========================================================================*/

#ifndef __ctkPathListWidget_h
#define __ctkPathListWidget_h

// Qt includes
#include <QListView>

// QtGUI includes
#include "ctkWidgetsExport.h"

class ctkPathListWidgetPrivate;

class QStandardItem;

/// \ingroup Widgets
///
/// \brief The ctkPathListWidget lists files and/or directories.
///
/// The ctkPathListWidget is a QListView sub-class tailored specifically for displaying
/// lists of file and/or directory entries. A \e path denotes either a file or a directory.
/// Paths can be relative or absolute and the range of valid paths can be constrained
/// by setting file and directory options.
/// \sa ctkPathLineEdit, ctkDirectoryButton
///
class CTK_WIDGETS_EXPORT ctkPathListWidget : public QListView
{
  Q_OBJECT

  /// The current list of paths.
  Q_PROPERTY(QStringList paths READ paths WRITE setPaths NOTIFY pathsChanged)

  /// The mode for this ctkPathListWidget.
  Q_PROPERTY(Mode mode READ mode WRITE setMode)

  /// Constraints on the file type.
  Q_PROPERTY(PathOptions fileOptions READ fileOptions WRITE setFileOptions)

  /// Constraints on the directory type.
  Q_PROPERTY(PathOptions directoryOptions READ directoryOptions WRITE setDirectoryOptions)

  /// The icon to be shown for a file entry.
  Q_PROPERTY(QIcon fileIcon READ fileIcon WRITE setFileIcon RESET unsetFileIcon)

  /// The icon to be shown for a directory entry.
  Q_PROPERTY(QIcon directoryIcon READ directoryIcon WRITE setDirectoryIcon RESET unsetDirectoryIcon)

  Q_FLAGS(PathOption PathOptions)
  Q_ENUMS(Mode)

public:

  enum
  {
    /// A role for getting the absolute path from items in this list views model.
    AbsolutePathRole = Qt::UserRole + 1
  };

  /// Describes constraints on paths.
  enum PathOption
  {
    /// No constraints
    None       = 0x00,
    /// The path must exist in the file system.
    Exists     = 0x01,
    /// The path must be readable by the current user.
    Readable   = 0x02,
    /// The path must be writable by the current user.
    Writable   = 0x04,
    /// The path must be executable by the current user.
    Executable = 0x08
  };
  Q_DECLARE_FLAGS(PathOptions, PathOption)

  enum Mode
  {
    /// Allow all paths.
    Any = 0,
    /// Allow only file entries.
    FilesOnly,
    /// Allow only directory entries.
    DirectoriesOnly
  };

  /// Superclass typedef
  typedef QListView Superclass;

  /// Constructor
  explicit ctkPathListWidget(QWidget* parent = 0);

  /// Destructor
  virtual ~ctkPathListWidget();

  /// \return The current widget mode.
  Mode mode() const;

  /// \return The QIcon used for file entries.
  QIcon fileIcon() const;

  /// Sets a QIcon to be used for file entries.
  /// \param icon The new file entry icon.
  void setFileIcon(const QIcon& icon);

  /// Un-set any custom file icon.
  void unsetFileIcon();

  /// \return The QIcon used for directory entries.
  QIcon directoryIcon() const;

  /// Sets a QIcon to be used for directory entries.
  /// \param icon The new directory entry icon.
  void setDirectoryIcon(const QIcon& icon);

  /// Un-set any custom directory icon.
  void unsetDirectoryIcon();

  /// \return The file entry constraints.
  PathOptions fileOptions() const;

  /// Set new file entry constraints.
  /// \param fileOptions The file entry constraints.
  void setFileOptions(PathOptions fileOptions);

  /// \return The directory entry constraints.
  PathOptions directoryOptions() const;

  /// Set new directory entry constraints.
  /// \param directoryOptions The directory entry constraints.
  void setDirectoryOptions(PathOptions directoryOptions);

  /// Checks if an entry with the given \a path already exists.
  /// \return <code>true</code> if the \a path has already been added, <code>false</code> otherwise.
  bool contains(const QString& path)const;

  /// Get all file entries.
  /// \param absolutePath If <code>true</code>, resolve all entries to absolute paths.
  /// \return A list of all file entries.
  QStringList files(bool absolutePath = false) const;

  /// Get all directory entries.
  /// \param absolutePath If <code>true</code>, resolve all entries to absolute paths.
  /// \return A list of all directory entries.
  QStringList directories(bool absolutePath = false) const;

  /// Get all path entries.
  /// \param absolutePath If <code>true</code>, resolve all entries to absolute paths.
  /// \return A list of all entries.
  QStringList paths(bool absolutePath = false) const;

  /// Get all selected path entries.
  /// \param absolutePath If <code>true</code>, resolve all entries to absolute paths.
  /// \return A list of all selected entries.
  QStringList selectedPaths(bool absolutePath = false) const;

  /// Get the currently focused path entry.
  /// \param absolutePath If <code>true</code>, resolve all entries to absolute paths.
  /// \return The focused path entry or a null QString if no entry is focused.
  QString currentPath(bool absolutePath = false) const;

  /// \return The current entry count.
  int count() const;

  /// \return The absolute path for \a row or a null QString if \a row is out of range.
  QString path(int row) const;

  /// \return The item for \a row or NULL if \a row is out of range.
  QStandardItem* item(int row) const;

  /// \return The item for the given absolute path or NULL if the the path is not known.
  QStandardItem* item(const QString& absolutePath) const;

  /// \return The absolute path for the entry located at the point \a point (in the
  ///         widget coordinate system) or a null QString if no entry could be found for \a point.
  QString pathAt(const QPoint& point) const;

  /// \return The item for the entry located at the point \a point (in the widget
  ///         coordinate system) or NULL if no ite could be found for \a point.
  QStandardItem* itemAt(const QPoint& point) const;

  /// \see pathAt(const QPoint&)
  QString pathAt(int x, int y) const { return pathAt(QPoint(x, y)); }

  /// \see itemAt(const QPoint&)
  QStandardItem* itemAt(int x, int y) const { return itemAt(QPoint(x, y)); }

  /// \return The row number for the given \a path or -1 if \a path is not in the list of current
  ///         entries.
  int row(const QString& path) const;

  /// Changes \a oldPath to the new value given by \a newPath. Does nothing if \a oldPath is not
  /// in the list or \a newPath does not fullfill the current path options (constraints).
  /// \param oldPath The path to be edited.
  /// \param newPath The new path replacing \a oldPath.
  /// \return <code>true</code> if the old path was successfully changed, <code>false</code> otherwise.
  bool editPath(const QString& oldPath, const QString& newPath);

  /// Changes the path value of \a index to \a newPath.
  /// \param index The model index for which the path will be changed.
  /// \param newPath The new path replacing the path value of \a index.
  ///
  /// \sa editPath(const QString&, const QString&)
  bool editPath(const QModelIndex& index, const QString& newPath);

  /// \return Returns <code>true</code> if the given path is treated as a file,
  ///         <code>false</code> otherwise.
  bool isFile(const QString& path) const;

  /// \return Returns <code>true</code> if the given path is treated as a directory,
  ///         <code>false</code> otherwise.
  bool isDirectory(const QString& path) const;

public Q_SLOTS:

  /// Set the mode for controlling the path type.
  /// \param mode The path mode.
  void setMode(Mode mode);

  /// Depending on the mode and path constraints, add \a path to the entry list and emit signal pathListChanged().
  /// \param path The path to add.
  /// \return <code>true</code> if the path was added, <code>false</code> otherwise.
  ///
  /// \sa pathListChanged()
  bool addPath(const QString& path);

  /// Depending on the mode and path constraints, add \a paths to the entry list and emit signal pathListChanged().
  /// \param paths The paths to add.
  /// \return The absolute paths of all added entries from \a paths.
  ///
  /// \sa pathListChanged()
  QStringList addPaths(const QStringList& paths);

  /// Remove all entries and set all valid entries in \a paths as the current list.
  /// The signal pathListChanged() is emitted if the old list of paths is
  /// different from the provided one.
  /// \param paths The new path list.
  ///
  /// \sa addPaths(), pathListChanged()
  void setPaths(const QStringList& paths);

  /// Remove \a path from the list.
  /// The signal pathListChanged() is emitted if the path was in the list.
  /// \param path The path to remove.
  /// \return <code>true</code> if \a path was removed, <code>false</code> otherwise.
  ///
  /// \sa pathListChanged()
  bool removePath(const QString& path);

  /// Remove \a paths from the list.
  /// \param paths The paths to remove.
  /// \return The absolute paths of all removed entries from \a paths.
  QStringList removePaths(const QStringList& paths);

  /// Remove all currently selected paths from the list.
  void removeSelectedPaths();

  /// Remove all paths from the list.
  void clear();

Q_SIGNALS:

  /// This signal is emitted when paths are added or removed to the list.
  /// \param added The newly added absolute paths.
  /// \param removed The removed absolute paths.
  void pathsChanged(const QStringList& added, const QStringList& removed);

  /// The user clicked on a path entry.
  void pathClicked(const QString& absolutePath);

  /// The user double-clicked on a path entry.
  void pathDoubleClicked(const QString& absolutePath);

  /// This signal is emitted when the \a absolutePath entry is activated. The entry is activated when the user
  /// clicks or double clicks on it, depending on the system configuration. It is also activated
  /// when the user presses the activation key (on Windows and X11 this is the Return key, on
  /// Mac OS X it is Ctrl+0).
  void pathActivated(const QString& absolutePath);

  /// This signal is emitted whenever the current item changes.
  /// \param currentAbsolutePath The new current path entry.
  /// \param previousAbsolutePath The path entry that previously had the focus.
  void currentPathChanged(const QString& currentAbsolutePath, const QString& previousAbsolutePath);

protected:
  QScopedPointer<ctkPathListWidgetPrivate> d_ptr;

private:

  void setModel(QAbstractItemModel *model);

  Q_DECLARE_PRIVATE(ctkPathListWidget)
  Q_DISABLE_COPY(ctkPathListWidget)

  Q_PRIVATE_SLOT(d_func(), void _q_emitPathClicked(const QModelIndex& index))
  Q_PRIVATE_SLOT(d_func(), void _q_emitPathDoubleClicked(const QModelIndex& index))
  Q_PRIVATE_SLOT(d_func(), void _q_emitPathActivated(const QModelIndex& index))
  Q_PRIVATE_SLOT(d_func(), void _q_emitCurrentPathChanged(const QModelIndex &previous, const QModelIndex &current))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkPathListWidget::PathOptions)

#endif

