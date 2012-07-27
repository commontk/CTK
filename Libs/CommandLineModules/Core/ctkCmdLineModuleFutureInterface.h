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

#include "ctkCmdLineModuleResult.h"

#include <QFutureInterface>

class ctkCmdLineModuleFuture;

template <>
class QFutureInterface<ctkCmdLineModuleResult> : public QFutureInterfaceBase
{

public:

  QFutureInterface(State initialState = NoState)
    : QFutureInterfaceBase(initialState),
      CanCancel(false), CanPause(false)
  { }

  QFutureInterface(const QFutureInterface &other)
    : QFutureInterfaceBase(other),
      CanCancel(other.CanCancel), CanPause(other.CanPause)
  { }

  ~QFutureInterface()
  {
    if (referenceCountIsOne())
      resultStore().clear();
  }

  static QFutureInterface canceledResult()
  { return QFutureInterface(State(Started | Finished | Canceled)); }

  QFutureInterface& operator=(const QFutureInterface& other)
  {
    if (referenceCountIsOne())
      resultStore().clear();
    QFutureInterfaceBase::operator=(other);
    CanCancel = other.CanCancel;
    CanPause = other.CanPause;
    return *this;
  }

  inline ctkCmdLineModuleFuture future(); // implemented in ctkCmdLineModuleFuture.h

  inline bool canCancel() const { return CanCancel; }
  inline void setCanCancel(bool canCancel) { CanCancel = canCancel; }
  inline bool canPause() const { return CanPause; }
  inline void setCanPause(bool canPause) { CanPause = canPause; }

  inline void reportResult(const ctkCmdLineModuleResult *result, int index = -1);
  inline void reportResult(const ctkCmdLineModuleResult &result, int index = -1);
  inline void reportResults(const QVector<ctkCmdLineModuleResult> &results, int beginIndex = -1, int count = -1);
  inline void reportFinished(const ctkCmdLineModuleResult *result = 0);

  inline const ctkCmdLineModuleResult &resultReference(int index) const;
  inline const ctkCmdLineModuleResult *resultPointer(int index) const;
  inline QList<ctkCmdLineModuleResult> results();

private:

  QtConcurrent::ResultStore<ctkCmdLineModuleResult> &resultStore()
  { return static_cast<QtConcurrent::ResultStore<ctkCmdLineModuleResult> &>(resultStoreBase()); }
  const QtConcurrent::ResultStore<ctkCmdLineModuleResult> &resultStore() const
  { return static_cast<const QtConcurrent::ResultStore<ctkCmdLineModuleResult> &>(resultStoreBase()); }

  bool CanCancel;
  bool CanPause;
};

inline void QFutureInterface<ctkCmdLineModuleResult>::reportResult(const ctkCmdLineModuleResult *result, int index)
{
    QMutexLocker locker(mutex());
    if (this->queryState(Canceled) || this->queryState(Finished)) {
        return;
    }

    QtConcurrent::ResultStore<ctkCmdLineModuleResult> &store = resultStore();


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

    QtConcurrent::ResultStore<ctkCmdLineModuleResult> &store = resultStore();

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
    return resultStore().resultAt(index).value();
}

inline const ctkCmdLineModuleResult *QFutureInterface<ctkCmdLineModuleResult>::resultPointer(int index) const
{
    QMutexLocker lock(mutex());
    return resultStore().resultAt(index).pointer();
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

    QtConcurrent::ResultIterator<ctkCmdLineModuleResult> it = resultStore().begin();
    while (it != resultStore().end()) {
        res.append(it.value());
        ++it;
    }

    return res;
}

typedef QFutureInterface<ctkCmdLineModuleResult> ctkCmdLineModuleFutureInterface;

#endif // CTKCMDLINEMODULEFUTUREINTERFACE_H
