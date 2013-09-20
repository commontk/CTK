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
// ctkPixmapIconEngine is based on QPixmapIconEngine located in qicon_p.h/cpp

// Qt includes
#include <QApplication>
#include <QFileInfo>
#include <QPainter>
#include <QPixmapCache>
#include <QStyleOption>

#include "ctkPixmapIconEngine.h"

ctkPixmapIconEngine::ctkPixmapIconEngine()
{
}

ctkPixmapIconEngine::ctkPixmapIconEngine(const ctkPixmapIconEngine &other)
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
  : QIconEngine(other)
#else
  : QIconEngineV2(other)
#endif
  , pixmaps(other.pixmaps)
{
}

ctkPixmapIconEngine::~ctkPixmapIconEngine()
{
}

void ctkPixmapIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    QSize pixmapSize = rect.size();
#if defined(Q_WS_MAC)
    //pixmapSize *= qt_mac_get_scalefactor();
#endif
    painter->drawPixmap(rect, pixmap(pixmapSize, mode, state));
}

static inline int area(const QSize &s) { return s.width() * s.height(); }

// returns the smallest of the two that is still larger than or equal to size.
static ctkPixmapIconEngineEntry *bestSizeMatch( const QSize &size, ctkPixmapIconEngineEntry *pa, ctkPixmapIconEngineEntry *pb)
{
    int s = area(size);
    if (pa->size == QSize() && pa->pixmap.isNull()) {
        pa->pixmap = QPixmap(pa->fileName);
        pa->size = pa->pixmap.size();
    }
    int a = area(pa->size);
    if (pb->size == QSize() && pb->pixmap.isNull()) {
        pb->pixmap = QPixmap(pb->fileName);
        pb->size = pb->pixmap.size();
    }
    int b = area(pb->size);
    int res = a;
    if (qMin(a,b) >= s)
        res = qMin(a,b);
    else
        res = qMax(a,b);
    if (res == a)
        return pa;
    return pb;
}

ctkPixmapIconEngineEntry *ctkPixmapIconEngine::tryMatch(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    ctkPixmapIconEngineEntry *pe = 0;
    for (int i = 0; i < pixmaps.count(); ++i)
        if (pixmaps.at(i).mode == mode && pixmaps.at(i).state == state) {
            if (pe)
                pe = bestSizeMatch(size, &pixmaps[i], pe);
            else
                pe = &pixmaps[i];
        }
    return pe;
}


ctkPixmapIconEngineEntry *ctkPixmapIconEngine::bestMatch(const QSize &size, QIcon::Mode mode, QIcon::State state, bool sizeOnly)
{
    ctkPixmapIconEngineEntry *pe = tryMatch(size, mode, state);
    while (!pe){
        QIcon::State oppositeState = (state == QIcon::On) ? QIcon::Off : QIcon::On;
        if (mode == QIcon::Disabled || mode == QIcon::Selected) {
            QIcon::Mode oppositeMode = (mode == QIcon::Disabled) ? QIcon::Selected : QIcon::Disabled;
            if ((pe = tryMatch(size, QIcon::Normal, state)))
                break;
            if ((pe = tryMatch(size, QIcon::Active, state)))
                break;
            if ((pe = tryMatch(size, mode, oppositeState)))
                break;
            if ((pe = tryMatch(size, QIcon::Normal, oppositeState)))
                break;
            if ((pe = tryMatch(size, QIcon::Active, oppositeState)))
                break;
            if ((pe = tryMatch(size, oppositeMode, state)))
                break;
            if ((pe = tryMatch(size, oppositeMode, oppositeState)))
                break;
        } else {
            QIcon::Mode oppositeMode = (mode == QIcon::Normal) ? QIcon::Active : QIcon::Normal;
            if ((pe = tryMatch(size, oppositeMode, state)))
                break;
            if ((pe = tryMatch(size, mode, oppositeState)))
                break;
            if ((pe = tryMatch(size, oppositeMode, oppositeState)))
                break;
            if ((pe = tryMatch(size, QIcon::Disabled, state)))
                break;
            if ((pe = tryMatch(size, QIcon::Selected, state)))
                break;
            if ((pe = tryMatch(size, QIcon::Disabled, oppositeState)))
                break;
            if ((pe = tryMatch(size, QIcon::Selected, oppositeState)))
                break;
        }

        if (!pe)
            return pe;
    }

    if (sizeOnly ? (pe->size.isNull() || !pe->size.isValid()) : pe->pixmap.isNull()) {
        pe->pixmap = QPixmap(pe->fileName);
        if (!pe->pixmap.isNull())
            pe->size = pe->pixmap.size();
    }

    return pe;
}

QPixmap ctkPixmapIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    QPixmap pm;
    ctkPixmapIconEngineEntry *pe = bestMatch(size, mode, state, false);
    if (pe)
        pm = pe->pixmap;

    if (pm.isNull()) {
        int idx = pixmaps.count();
        while (--idx >= 0) {
            if (pe == &pixmaps[idx]) {
                pixmaps.remove(idx);
                break;
            }
        }
        if (pixmaps.isEmpty())
            return pm;
        else
            return pixmap(size, mode, state);
    }

    QSize actualSize = pm.size();
    if (!actualSize.isNull() && (actualSize.width() > size.width() || actualSize.height() > size.height()))
        actualSize.scale(size, Qt::KeepAspectRatio);

    QString key = QLatin1String("$qt_icon_")
                  + QString::number(pm.cacheKey())
                  + QString::number(static_cast<int>(pe->mode))
                  + QString::number(QApplication::palette().cacheKey())
                  + QLatin1Char('_')
                  + QString::number(actualSize.width())
                  + QLatin1Char('_')
                  + QString::number(actualSize.height())
                  + QLatin1Char('_');


    if (mode == QIcon::Active) {
        if (QPixmapCache::find(key + QString::number(static_cast<int>(mode)), pm))
            return pm; // horray
        if (QPixmapCache::find(key + QString::number(static_cast<int>(QIcon::Normal)), pm)) {
            QStyleOption opt(0);
            opt.palette = QApplication::palette();
            QPixmap active = QApplication::style()->generatedIconPixmap(QIcon::Active, pm, &opt);
            if (pm.cacheKey() == active.cacheKey())
                return pm;
        }
    }

    if (!QPixmapCache::find(key + QString::number(static_cast<int>(mode)), pm)) {
        if (pm.size() != actualSize)
            pm = pm.scaled(actualSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if (pe->mode != mode && mode != QIcon::Normal) {
            QStyleOption opt(0);
            opt.palette = QApplication::palette();
            QPixmap generated = QApplication::style()->generatedIconPixmap(mode, pm, &opt);
            if (!generated.isNull())
                pm = generated;
        }
        QPixmapCache::insert(key + QString::number(static_cast<int>(mode)), pm);
    }
    return pm;
}

QSize ctkPixmapIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    QSize actualSize;
    if (ctkPixmapIconEngineEntry *pe = bestMatch(size, mode, state, true))
        actualSize = pe->size;

    if (actualSize.isNull())
        return actualSize;

    if (!actualSize.isNull() && (actualSize.width() > size.width() || actualSize.height() > size.height()))
        actualSize.scale(size, Qt::KeepAspectRatio);
    return actualSize;
}

void ctkPixmapIconEngine::addPixmap(const QPixmap &pixmap, QIcon::Mode mode, QIcon::State state)
{
    if (!pixmap.isNull()) {
        ctkPixmapIconEngineEntry *pe = tryMatch(pixmap.size(), mode, state);
        if(pe && pe->size == pixmap.size()) {
            pe->pixmap = pixmap;
            pe->fileName.clear();
        } else {
            pixmaps += ctkPixmapIconEngineEntry(pixmap, mode, state);
        }
    }
}

void ctkPixmapIconEngine::addFile(const QString &fileName, const QSize &_size, QIcon::Mode mode, QIcon::State state)
{
    if (!fileName.isEmpty()) {
        QSize size = _size;
        QPixmap pixmap;

        QString abs = fileName;
        if (fileName.at(0) != QLatin1Char(':'))
            abs = QFileInfo(fileName).absoluteFilePath();

        for (int i = 0; i < pixmaps.count(); ++i) {
            if (pixmaps.at(i).mode == mode && pixmaps.at(i).state == state) {
                ctkPixmapIconEngineEntry *pe = &pixmaps[i];
                if(size == QSize()) {
                    pixmap = QPixmap(abs);
                    size = pixmap.size();
                }
                if (pe->size == QSize() && pe->pixmap.isNull()) {
                    pe->pixmap = QPixmap(pe->fileName);
                    pe->size = pe->pixmap.size();
                }
                if(pe->size == size) {
                    pe->pixmap = pixmap;
                    pe->fileName = abs;
                    return;
                }
            }
        }
        ctkPixmapIconEngineEntry e(abs, size, mode, state);
        e.pixmap = pixmap;
        pixmaps += e;
    }
}

QString ctkPixmapIconEngine::key() const
{
    return QLatin1String("ctkPixmapIconEngine");
}

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
QIconEngine *ctkPixmapIconEngine::clone() const
#else
QIconEngineV2 *ctkPixmapIconEngine::clone() const
#endif
{
    return new ctkPixmapIconEngine(*this);
}

bool ctkPixmapIconEngine::read(QDataStream &in)
{
    int num_entries;
    QPixmap pm;
    QString fileName;
    QSize sz;
    uint mode;
    uint state;

    in >> num_entries;
    for (int i=0; i < num_entries; ++i) {
        if (in.atEnd()) {
            pixmaps.clear();
            return false;
        }
        in >> pm;
        in >> fileName;
        in >> sz;
        in >> mode;
        in >> state;
        if (pm.isNull()) {
            addFile(fileName, sz, QIcon::Mode(mode), QIcon::State(state));
        } else {
            ctkPixmapIconEngineEntry pe(fileName, sz, QIcon::Mode(mode), QIcon::State(state));
            pe.pixmap = pm;
            pixmaps += pe;
        }
    }
    return true;
}

bool ctkPixmapIconEngine::write(QDataStream &out) const
{
    int num_entries = pixmaps.size();
    out << num_entries;
    for (int i=0; i < num_entries; ++i) {
        if (pixmaps.at(i).pixmap.isNull())
            out << QPixmap(pixmaps.at(i).fileName);
        else
            out << pixmaps.at(i).pixmap;
        out << pixmaps.at(i).fileName;
        out << pixmaps.at(i).size;
        out << static_cast<uint>(pixmaps.at(i).mode);
        out << static_cast<uint>(pixmaps.at(i).state);
    }
    return true;
}

void ctkPixmapIconEngine::virtual_hook(int id, void *data)
{
  switch (id) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    case QIconEngine::AvailableSizesHook: {
        QIconEngine::AvailableSizesArgument &arg =
            *reinterpret_cast<QIconEngine::AvailableSizesArgument*>(data);
#else
    case QIconEngineV2::AvailableSizesHook: {
        QIconEngineV2::AvailableSizesArgument &arg =
            *reinterpret_cast<QIconEngineV2::AvailableSizesArgument*>(data);
#endif
        arg.sizes.clear();
        for (int i = 0; i < pixmaps.size(); ++i) {
            ctkPixmapIconEngineEntry &pe = pixmaps[i];
            if (pe.size == QSize() && pe.pixmap.isNull()) {
                pe.pixmap = QPixmap(pe.fileName);
                pe.size = pe.pixmap.size();
            }
            if (pe.mode == arg.mode && pe.state == arg.state && !pe.size.isEmpty())
                arg.sizes.push_back(pe.size);
        }
        break;
    }
    default:
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
      QIconEngine::virtual_hook(id, data);
#else
      QIconEngineV2::virtual_hook(id, data);
#endif
    }
}
