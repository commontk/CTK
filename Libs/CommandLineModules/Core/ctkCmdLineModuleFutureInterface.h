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

#ifndef CTKCMDLINEMODULEFUTUREINTERFACE_H
#define CTKCMDLINEMODULEFUTUREINTERFACE_H

#include <ctkCommandLineModulesCoreExport.h>

#include "ctkCmdLineModuleResult.h"

#include <QFutureInterface>
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtCore>
#else
#include <QtConcurrent>
#include <qresultstore.h>
#endif


class ctkCmdLineModuleFuture;
class ctkCmdLineModuleFutureInterfacePrivate;

/**
 * \ingroup CommandLineModulesCore_API
 *
 * \brief A QFutureInterface specialization.
 *
 * This QFutureInterface must be used by custom backend implementations to retrieve
 * a suitable QFuture object and to report state changes to it via this interface.
 */
template <>
class CTK_CMDLINEMODULECORE_EXPORT QFutureInterface<ctkCmdLineModuleResult> : public QFutureInterfaceBase
{

public:

  QFutureInterface(State initialState = NoState);

  QFutureInterface(const QFutureInterface &other);

  ~QFutureInterface();

  static QFutureInterface canceledResult();

  QFutureInterface& operator=(const QFutureInterface& other);

  inline ctkCmdLineModuleFuture future(); // implemented in ctkCmdLineModuleFuture.h

  bool canCancel() const;
  void setCanCancel(bool canCancel);
  bool canPause() const;
  void setCanPause(bool canPause);

  inline void reportResult(const ctkCmdLineModuleResult *result, int index = -1);
  inline void reportResult(const ctkCmdLineModuleResult &result, int index = -1);
  inline void reportResults(const QVector<ctkCmdLineModuleResult> &results, int beginIndex = -1, int count = -1);
  inline void reportFinished(const ctkCmdLineModuleResult *result = 0);

  void reportOutputData(const QByteArray& outputData);
  void reportErrorData(const QByteArray& errorData);

  inline const ctkCmdLineModuleResult &resultReference(int index) const;
  inline const ctkCmdLineModuleResult *resultPointer(int index) const;
  inline QList<ctkCmdLineModuleResult> results();

  QByteArray outputData(int position = 0, int size = -1) const;
  QByteArray errorData(int position = 0, int size = -1) const;

private:

  friend struct ctkCmdLineModuleFutureWatcherPrivate;

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  QtConcurrent::ResultStore<ctkCmdLineModuleResult> &resultStore()
  { return static_cast<QtConcurrent::ResultStore<ctkCmdLineModuleResult> &>(resultStoreBase()); }
  const QtConcurrent::ResultStore<ctkCmdLineModuleResult> &resultStore() const
  { return static_cast<const QtConcurrent::ResultStore<ctkCmdLineModuleResult> &>(resultStoreBase()); }
#elif (QT_VERSION < QT_VERSION_CHECK(5, 9, 0))
  QtPrivate::ResultStore<ctkCmdLineModuleResult> &resultStore()
  { return static_cast<QtPrivate::ResultStore<ctkCmdLineModuleResult> &>(resultStoreBase()); }
  const QtPrivate::ResultStore<ctkCmdLineModuleResult> &resultStore() const
  { return static_cast<const QtPrivate::ResultStore<ctkCmdLineModuleResult> &>(resultStoreBase()); }
#else
  QtPrivate::ResultStoreBase &resultStore()
  { return static_cast<QtPrivate::ResultStoreBase &>(resultStoreBase()); }
  const QtPrivate::ResultStoreBase &resultStore() const
  { return static_cast<const QtPrivate::ResultStoreBase &>(resultStoreBase()); }
#endif

  ctkCmdLineModuleFutureInterfacePrivate* d;
};

inline void QFutureInterface<ctkCmdLineModuleResult>::reportResult(const ctkCmdLineModuleResult *result, int index)
{
    QMutexLocker locker(mutex());
    if (this->queryState(Canceled) || this->queryState(Finished)) {
        return;
    }

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QtConcurrent::ResultStore<ctkCmdLineModuleResult> &store = resultStore();
#elif (QT_VERSION < QT_VERSION_CHECK(5, 9, 0))
    QtPrivate::ResultStore<ctkCmdLineModuleResult> &store = resultStore();
#else
    QtPrivate::ResultStoreBase &store = resultStore();
#endif

    if (store.filterMode()) {
        const int resultCountBefore = store.count();
        store.addResult(index, result);
        this->reportResultsReady(resultCountBefore, resultCountBefore + store.count());
    } else {
        const int insertIndex = store.addResult(index, result);
        this->reportResultsReady(insertIndex, insertIndex + 1);
    }
}

inline void QFutureInterface<ctkCmdLineModuleResult>::reportResult(const ctkCmdLineModuleResult &result, int index)
{
    reportResult(&result, index);
}

inline void QFutureInterface<ctkCmdLineModuleResult>::reportResults(const QVector<ctkCmdLineModuleResult> &_results, int beginIndex, int count)
{
    QMutexLocker locker(mutex());
    if (this->queryState(Canceled) || this->queryState(Finished)) {
        return;
    }

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QtConcurrent::ResultStore<ctkCmdLineModuleResult> &store = resultStore();
#elif (QT_VERSION < QT_VERSION_CHECK(5, 9, 0))
    QtPrivate::ResultStore<ctkCmdLineModuleResult> &store = resultStore();
#else
    QtPrivate::ResultStoreBase &store = resultStore();
#endif

    if (store.filterMode()) {
        const int resultCountBefore = store.count();
        store.addResults(beginIndex, &_results, count);
        this->reportResultsReady(resultCountBefore, store.count());
    } else {
        const int insertIndex = store.addResults(beginIndex, &_results, count);
        this->reportResultsReady(insertIndex, insertIndex + _results.count());
    }
}

inline void QFutureInterface<ctkCmdLineModuleResult>::reportFinished(const ctkCmdLineModuleResult *result)
{
    if (result)
        reportResult(result);
    QFutureInterfaceBase::reportFinished();
}

inline const ctkCmdLineModuleResult &QFutureInterface<ctkCmdLineModuleResult>::resultReference(int index) const
{
    QMutexLocker lock(mutex());
#if (QT_VERSION < QT_VERSION_CHECK(5, 9, 0))
    return resultStore().resultAt(index).value();
#else
    return resultStore().resultAt(index).value<ctkCmdLineModuleResult>();
#endif
}

inline const ctkCmdLineModuleResult *QFutureInterface<ctkCmdLineModuleResult>::resultPointer(int index) const
{
    QMutexLocker lock(mutex());
#if (QT_VERSION < QT_VERSION_CHECK(5, 9, 0))
    return resultStore().resultAt(index).pointer();
#else
    return resultStore().resultAt(index).pointer<ctkCmdLineModuleResult>();
#endif
}

inline QList<ctkCmdLineModuleResult> QFutureInterface<ctkCmdLineModuleResult>::results()
{
    if (this->isCanceled()) {
        exceptionStore().throwPossibleException();
        return QList<ctkCmdLineModuleResult>();
    }
    QFutureInterfaceBase::waitForResult(-1);

    QList<ctkCmdLineModuleResult> res;
    QMutexLocker lock(mutex());

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QtConcurrent::ResultIterator<ctkCmdLineModuleResult> it = resultStore().begin();
#elif (QT_VERSION < QT_VERSION_CHECK(5, 9, 0))
    QtPrivate::ResultIterator<ctkCmdLineModuleResult> it = resultStore().begin();
#else
    QtPrivate::ResultIteratorBase it = resultStore().begin();
#endif
    while (it != resultStore().end()) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 9, 0))
        res.append(it.value());
#else
        res.append(it.value<ctkCmdLineModuleResult>());
#endif
        ++it;
    }

    return res;
}

typedef QFutureInterface<ctkCmdLineModuleResult> ctkCmdLineModuleFutureInterface;

#endif // CTKCMDLINEMODULEFUTUREINTERFACE_H
