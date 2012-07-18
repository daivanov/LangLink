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
#include <QDebug>

#include "link-item.h"

LinkItem::LinkItem(const QString & text, QGraphicsItem *parent)
    : QGraphicsSimpleTextItem(text, parent),
    m_state(Inactive)
{
    setBrush(Qt::white);
    QFont currFont = font();
    currFont.setBold(true);
    setFont(currFont);
    m_center = boundingRect().center();
}

LinkItem::~LinkItem()
{
}

LinkItem::State LinkItem::state() const
{
    return m_state;
}

void LinkItem::setState(State state)
{
    m_state = state;
    switch(m_state) {
    case Inactive:
        setBrush(Qt::white);
        break;
    case Undefined:
        setBrush(Qt::yellow);
        break;
    case Wrong:
        setBrush(Qt::red);
        break;
    case Correct:
        setBrush(Qt::green);
        break;
    }
}

void LinkItem::setNextState()
{
    switch(m_state) {
    case Undefined:
        setState(Correct);
        break;
    case Correct:
        setState(Wrong);
        break;
    case Wrong:
        setState(Undefined);
        break;
    default:
        break;
    }
}

QPointF LinkItem::center() const
{
    return pos() + m_center;
}

void LinkItem::setCenterPos(const QPointF &pos)
{
    setPos(pos - m_center);
}
