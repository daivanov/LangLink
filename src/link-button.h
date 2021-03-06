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

#ifndef LINK_BUTTON_H
#define LINK_BUTTON_H

#include <QGraphicsPolygonItem>
#include <QTransform>
#include <QObject>

class LinkButton : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

public:
    LinkButton(qreal scalingFactor, QGraphicsItem *parent = 0);
    ~LinkButton();

    void setCenterPos(const QPointF &pos);
    void setPolygon(const QPolygonF &polygon);

signals:
    void clicked();

protected:
    bool sceneEvent(QEvent *event);

private:
    QTransform m_scale;
};

#endif // LINK_BUTTON_H
