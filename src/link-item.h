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

#ifndef LINK_ITEM_H
#define LINK_ITEM_H

#include <QGraphicsSimpleTextItem>
#include <QPointF>

class LinkItem : public QGraphicsSimpleTextItem
{
public:
    LinkItem(const QString & text, QGraphicsItem *parent = 0);
    ~LinkItem();

    enum State {
        Inactive,
        Undefined,
        Correct,
        Wrong
    };

    State state() const;
    void setState(State state);
    void setNextState();

    QPointF center() const;
    void setCenterPos(const QPointF &pos);

private:
    State m_state;
    QPointF m_center;
    static QFont m_font;
    static bool m_fontInitialized;
};

#endif // LINK_ITEM_H
