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

#include <QEvent>
#include <QGraphicsScene>
#include <QDebug>

#include "link-button.h"

LinkButton::LinkButton(qreal scalingFactor, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    setBrush(Qt::yellow);
    setPen(QPen(Qt::yellow));
    m_scale.scale(scalingFactor, scalingFactor);
}

LinkButton::~LinkButton()
{
}

void LinkButton::setPolygon(const QPolygonF &polygon)
{
    QPolygonF scaled = m_scale.map(polygon);
    QGraphicsPolygonItem::setPolygon(scaled);
}

bool LinkButton::sceneEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:
        emit clicked();
        break;
    default:
        break;
    }
    return QGraphicsItem::sceneEvent(event);
}

void LinkButton::setCenterPos(const QPointF &pos)
{
    setPos(pos - boundingRect().center());
}
