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
#include <QFontDatabase>
#include <QFontMetrics>
#include <QCoreApplication>
#include <QDebug>

#include "link-item.h"

QFont LinkItem::m_font;
QFontMetrics LinkItem::m_fontMetrics(LinkItem::m_font);
bool LinkItem::m_fontInitialized = false;

LinkItem::LinkItem(const QString & text, QGraphicsItem *parent)
    : QGraphicsSimpleTextItem(text, parent),
    m_state(Inactive)
{
    if (!m_fontInitialized)
        initFont();

    setBrush(Qt::white);
    setFont(m_font);
    m_center = boundingRect().center();
    setTransformOriginPoint(m_center);
}

LinkItem::~LinkItem()
{
}

void LinkItem::setFont(const QFont &font)
{
    QGraphicsSimpleTextItem::setFont(font);
    m_center = boundingRect().center();
}

void LinkItem::setText(const QString &text)
{
    QGraphicsSimpleTextItem::setText(text);
    m_center = boundingRect().center();
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

QPointF LinkItem::centerPos() const
{
    return pos() + transform().map(m_center);
}

void LinkItem::setCenterPos(const QPointF &pos)
{
    setPos(pos - transform().map(m_center));
}

void LinkItem::initFont()
{
    QString fontFile = QCoreApplication::applicationDirPath() +
        "/../fonts/foo.ttf";
    if (QFontDatabase::addApplicationFont(fontFile) != -1) {
        QFontDatabase database;
        m_font = database.font("Foo", "Regular", 25);
    } else {
        qCritical("Cannot find the font file %s", qPrintable(fontFile));
        m_font.setPixelSize(20);
    }

    m_font.setBold(true);
    m_fontMetrics = QFontMetrics(m_font);

    m_fontInitialized = true;
}

int LinkItem::width(const QString &text, int len)
{
    if (!m_fontInitialized)
        initFont();

    return m_fontMetrics.width(text, len);
}
