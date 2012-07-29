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

#include <QGraphicsScene>
#include <QTimer>
#include <QtCore/qmath.h>
#include <QDebug>

#include "link-progress-indicator.h"
#include "link-item.h"

#define BALL_RELATIVE_SIZE 0.3

LinkProgressIndicator::LinkProgressIndicator(const QString &count,
    qreal scalingFactor, QGraphicsItem *parent) : QGraphicsItemGroup(parent),
    m_timer(new QTimer(this)),
    m_i(0)
{
    m_text = new LinkItem(count);
    QFont font = m_text->font();
    font.setPixelSize(scalingFactor);
    m_text->setFont(font);
    m_text->setBrush(Qt::yellow);
    m_text->setPen(QPen(Qt::yellow));
    m_text->setCenterPos(QPointF(0.0, 0.0));
    addToGroup(m_text);

    QGraphicsEllipseItem *item;
    for (qreal angle = 0.0; angle < 2 * M_PI; angle += M_PI / 6) {
        item = new QGraphicsEllipseItem(
            scalingFactor * (qCos(angle) - BALL_RELATIVE_SIZE / 2),
            scalingFactor * (qSin(angle) - BALL_RELATIVE_SIZE / 2),
            scalingFactor * BALL_RELATIVE_SIZE,
            scalingFactor * BALL_RELATIVE_SIZE);
        item->setBrush(Qt::yellow);
        item->setPen(QPen(Qt::yellow));
        item->setOpacity(angle / (M_PI * 11 / 6));
        m_items.append(item);
        addToGroup(item);
    }

    connect(m_timer, SIGNAL(timeout()),
            SLOT(tick()));
}

LinkProgressIndicator::~LinkProgressIndicator()
{
}

void LinkProgressIndicator::start(int msec)
{
    show();
    m_i = m_items.count() - 1;
    m_timer->start(msec);
}

void LinkProgressIndicator::stop()
{
    m_timer->stop();
    hide();
}

void LinkProgressIndicator::tick()
{
    int cnt = m_items.count();
    for (int i = 0; i < cnt; ++i) {
        m_items.at(i)->setOpacity(((i + m_i) % (cnt - 1)) / (cnt - 1.0));
    }
    m_i--;
    if (m_i < 0)
        m_i = m_items.count() - 1;
}

void LinkProgressIndicator::setCount(const QString &count)
{
    m_text->setText(count);
    m_text->setCenterPos(QPointF(0.0, 0.0));
}

void LinkProgressIndicator::setCenterPos(const QPointF &pos)
{
    setPos(pos - boundingRect().center());
}
