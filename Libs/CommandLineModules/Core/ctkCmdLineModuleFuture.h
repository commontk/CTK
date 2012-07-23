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

#ifndef CTKCMDLINEMODULEFUTURE_H
#define CTKCMDLINEMODULEFUTURE_H

#include "ctkCmdLineModuleFutureInterface.h"

#include <QString>
#include <QVariant>
#include <QFuture>

template <typename ctkCmdLineModuleResult>
class QFutureWatcher;
template <>
class QFutureWatcher<void>;

/**
 * QFuture specialization with two additional methods:
 *
 *   - bool canCancel()
 *   - bool canPause()
 */
template<>
class QFuture<ctkCmdLineModuleResult>
{
public:

  QFuture()
    : d(QFutureInterface<ctkCmdLineModuleResult>::canceledResult())
  { }
  explicit QFuture(QFutureInterface<ctkCmdLineModuleResult> *p) // internal
    : d(*p)
  { }
  QFuture(const QFuture &other)
    : d(other.d)
  { }
  ~QFuture()
  { }

  inline QFuture &operator=(const QFuture &other);
  bool operator==(const QFuture &other) const { return (d == other.d); }
  bool operator!=(const QFuture &other) const { return (d != other.d); }

  // additional methods
  bool canCancel() const { return  d.canCancel(); }
  bool canPause() const { return d.canPause(); }

  void cancel() { d.cancel(); }
  bool isCanceled() const { return d.isCanceled(); }

  void setPaused(bool paused) { d.setPaused(paused); }
  bool isPaused() const { return d.isPaused(); }
  void pause() { setPaused(true); }
  void resume() { setPaused(false); }
  void togglePaused() { d.togglePaused(); }

  bool isStarted() const { return d.isStarted(); }
  bool isFinished() const { return d.isFinished(); }
  bool isRunning() const { return d.isRunning(); }

  int resultCount() const { return d.resultCount(); }
  int progressValue() const { return d.progressValue(); }
  int progressMinimum() const { return d.progressMinimum(); }
  int progressMaximum() const { return d.progressMaximum(); }
  QString progressText() const { return d.progressText(); }
  void waitForFinished() { d.waitForFinished(); }

  inline ctkCmdLineModuleResult result() const;
  inline ctkCmdLineModuleResult resultAt(int index) const;
  bool isResultReadyAt(int resultIndex) const { return d.isResultReadyAt(resultIndex); }

  operator ctkCmdLineModuleResult() const { return result(); }
  QList<ctkCmdLineModuleResult> results() const { return d.results(); }

  class const_iterator
  {
  public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef qptrdiff difference_type;
    typedef ctkCmdLineModuleResult value_type;
    typedef const ctkCmdLineModuleResult *pointer;
    typedef const ctkCmdLineModuleResult &reference;

    inline const_iterator() {}
    inline const_iterator(QFuture const * const _future, int _index) : future(_future), index(_index) {}
    inline const_iterator(const const_iterator &o) : future(o.future), index(o.index)  {}
    inline const_iterator &operator=(const const_iterator &o)
    { future = o.future; index = o.index; return *this; }
    inline const ctkCmdLineModuleResult &operator*() const { return future->d.resultReference(index); }
    inline const ctkCmdLineModuleResult *operator->() const { return future->d.resultPointer(index); }

    inline bool operator!=(const const_iterator &other) const
    {
      if (index == -1 && other.index == -1) // comparing end != end?
        return false;
      if (other.index == -1)
        return (future->isRunning() || (index < future->resultCount()));
      return (index != other.index);
    }

    inline bool operator==(const const_iterator &o) const { return !operator!=(o); }
    inline const_iterator &operator++() { ++index; return *this; }
    inline const_iterator operator++(int) { const_iterator r = *this; ++index; return r; }
    inline const_iterator &operator--() { --index; return *this; }
    inline const_iterator operator--(int) { const_iterator r = *this; --index; return r; }
    inline const_iterator operator+(int j) const { return const_iterator(future, index + j); }
    inline const_iterator operator-(int j) const { return const_iterator(future, index - j); }
    inline const_iterator &operator+=(int j) { index += j; return *this; }
    inline const_iterator &operator-=(int j) { index -= j; return *this; }
  private:
    QFuture const * future;
    int index;
  };
  friend class const_iterator;
  typedef const_iterator ConstIterator;

  const_iterator begin() const { return  const_iterator(this, 0); }
  const_iterator constBegin() const { return  const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, -1); }
  const_iterator constEnd() const { return const_iterator(this, -1); }

private:
  friend class QFutureWatcher<ctkCmdLineModuleResult>;

public: // Warning: the d pointer is not documented and is considered private.
  mutable QFutureInterface<ctkCmdLineModuleResult> d;
};

typedef QFuture<ctkCmdLineModuleResult> ctkCmdLineModuleFuture;

inline ctkCmdLineModuleFuture& ctkCmdLineModuleFuture::operator=(const ctkCmdLineModuleFuture& other)
{
  d = other.d;
  return *this;
}

inline ctkCmdLineModuleResult ctkCmdLineModuleFuture::result() const
{
  d.waitForResult(0);
  return d.resultReference(0);
}

inline ctkCmdLineModuleResult ctkCmdLineModuleFuture::resultAt(int index) const
{
  d.waitForResult(index);
  return d.resultReference(index);
}

inline ctkCmdLineModuleFuture ctkCmdLineModuleFutureInterface::future()
{
  return ctkCmdLineModuleFuture(this);
}

#endif // CTKCMDLINEMODULEFUTURE_H
