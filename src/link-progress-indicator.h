/*
 * Copyright 2012 Daniil Ivanov <daniil.ivanov@gmail.com>
 *
 * This file is part of LangLink.
 *
 * LangLink is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * LangLink is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with LangLink. If not, see http://www.gnu.org/licenses/.
 */

#ifndef LINK_PROGRESS_INDICATOR_H
#define LINK_PROGRESS_INDICATOR_H

#include <QGraphicsItemGroup>
#include <QObject>

class QTimer;
class QGraphicsItem;
class LinkItem;

class LinkProgressIndicator : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    LinkProgressIndicator(const QString &count, qreal scalingFactor, QGraphicsItem *parent = 0);
    ~LinkProgressIndicator();

    void setCount(const QString &count);
    void setCenterPos(const QPointF &pos);

public slots:
    void start(int msec = 100);
    void stop();

private slots:
    void tick();

private:
    QTimer *m_timer;
    QList<QGraphicsItem*> m_items;
    LinkItem *m_text;
    int m_i;
};

#endif // LINK_PROGRESS_INDICATOR_H
