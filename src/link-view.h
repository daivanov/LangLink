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

#ifndef LINK_VIEW_H
#define LINK_VIEW_H

#include <QObject>
#include <QPointF>
#include <QTransform>

#include "link-item.h"

class QGraphicsScene;
class QGraphicsView;
class QGraphicsItem;
class QGraphicsLineItem;
class LinkButton;
class LinkProgressIndicator;
class QWebPage;

class LinkView : public QObject
{
    Q_OBJECT

public:
    LinkView(int capacity, QObject *parent = 0);
    ~LinkView();
    void appendOriginal(const QString &item);
    void appendTranslation(const QString &item, int pos);
    void setOverallAssessment(int correct);

public slots:
    void show();
    void clear();
    void captcha(const QWebPage *page);

private slots:
    void evaluateLine();

signals:
    void result(const QList<QPair<int,QString> > &translations);
    void solved();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    bool mouseEvent(QObject *obj, QGraphicsSceneMouseEvent *mouseEvent);

private:
    int mapToPos(const QPointF &point) const;
    QPointF mapFromPos(qreal pos, qreal levelShift = 0) const;
    void adjustItems(const QList<QGraphicsItem*> &items, qreal dx, qreal dy);
    void updateWidth();
    void updateOriginalWidth(const QString &item);
    void updateTranslatedWidth(const QString &item);

    QGraphicsScene *m_scene;
    QGraphicsView *m_view;

    /* Data */
    int m_capacity;
    QList<QGraphicsItem*> m_originalItems;
    QList<QGraphicsItem*> m_translatedItems;
    QHash<QString,LinkItem::State> m_savedStates;

    /* UI items */
    QGraphicsLineItem *m_hSeparator;
    QGraphicsLineItem *m_vSeparator;
    LinkProgressIndicator *m_progressIndicator;
    LinkButton *m_button;
    LinkButton *m_closeButton;

    /* Item motion handling */
    QGraphicsItem *m_movingItem;
    int m_originPos, m_gapPos;
    QPointF m_translation;
    qreal m_originalWidth, m_width, m_height;
    QTransform m_transform;
    qreal m_activeLines;

    /* Gesture handling */
    QPointF m_gestureOrigin;
    bool m_possibleGesture;
    bool m_gesture;
};

#endif // LINK_VIEW_H
